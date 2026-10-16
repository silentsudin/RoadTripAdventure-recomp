"""Shared, dependency-free parsing/derivation helpers for the native UI-parity suite.

This module is the *ground-truth reader* for `docs/native-reference/ui-parity.md`
(the implementation contract for `re/native-complete`). It is deliberately plain
`python3` + stdlib: no GPU, no network, no build, no third-party imports.

Everything here is read from the recomp sources on disk and the spec document
itself, so the suite is deterministic and offline. Nothing in this module writes
to `src/`, the existing tests, or the docs.

Contracts implemented
---------------------
* String inventory (ui-parity.md "Text-parity contract"): the declared source
  ranges are read, C++ comments are stripped (so a quoted literal inside a
  comment is never mistaken for UI copy), C escapes are decoded (``\\u2026``,
  ``\\xE2\\x80\\x94``, ...), and adjacent string literals are joined, because C++
  concatenates them into one template.
* FONTE slot formula, ported from `src/native/assets/glyph_atlas.h`
  (`fonteSlotForChar`) and the corrected `hud-layout.md` §2 ordering.
* The recomp design scale ``S = max(.5, H/1080 * handheldBoost)`` and the
  ``x' = x*W/640`` / ``y' = (y+16)*H/480`` frame mapping.
* Finding registry used by every check. ``DEFECT`` findings are the only ones
  that make a check fail; ``SPEC_DRIFT`` and ``KNOWN_GAP`` are reported.
"""

from __future__ import annotations

import json
import pathlib
import re
from dataclasses import dataclass

# ---------------------------------------------------------------------------
# Locations
# ---------------------------------------------------------------------------

REPO_ROOT = pathlib.Path(__file__).resolve().parents[2]
SPEC_DOC = REPO_ROOT / "docs" / "native-reference" / "ui-parity.md"

# The Text-parity contract inventory table (ui-parity.md:92-100). These are the
# exact source ranges the spec says the ~184-template manifest must be generated
# from. Ranges are 1-based, inclusive.
STRING_SOURCES: dict[str, list[tuple[int, int]]] = {
    "src/ui/PauseMenu.cpp": [(293, 303), (324, 365), (397, 768), (813, 1048), (1796, 2089)],
    "src/ui/SetupScreen.cpp": [(264, 405)],
    "src/ui/SecondScreen.cpp": [(174, 245), (323, 623), (626, 824)],
    "src/ui/MapView.cpp": [(40, 191)],
    "src/ui/PerfOverlay.cpp": [(19, 96)],
    "src/game/GameOptions.cpp": [(59, 67)],
    "src/platform/Controllers.h": [(3, 78)],
}

# Short file name -> repo-relative path, for resolving the spec's inline
# citations (which mix `src/ui/PauseMenu.cpp:293` and `PauseMenu.cpp:293`).
CITATION_FILES: dict[str, str] = {
    "PauseMenu.cpp": "src/ui/PauseMenu.cpp",
    "PauseMenu.h": "src/ui/PauseMenu.h",
    "SetupScreen.cpp": "src/ui/SetupScreen.cpp",
    "SecondScreen.cpp": "src/ui/SecondScreen.cpp",
    "MapView.cpp": "src/ui/MapView.cpp",
    "PerfOverlay.cpp": "src/ui/PerfOverlay.cpp",
    "Theme.cpp": "src/ui/Theme.cpp",
    "Theme.h": "src/ui/Theme.h",
    "GameOptions.cpp": "src/game/GameOptions.cpp",
    "Controllers.h": "src/platform/Controllers.h",
}

# Files the spec cites only as *native* ground truth; a citation to one of these
# on a doc line means the literals on that line belong to the native model, not
# the recomp manifest, so drift is not asserted for them.
NATIVE_CITATION_FILES = {"main.cpp", "settings.cpp", "race_hud.h", "glyphs.cpp", "glyphs.h"}

SPEC_DECLARED_STRING_COUNT = 184  # ui-parity.md:90

# ---------------------------------------------------------------------------
# Findings registry
# ---------------------------------------------------------------------------

CATEGORIES = ("SPEC_DRIFT", "SPEC_GAP", "KNOWN_GAP", "DEFECT", "INFO")

FINDINGS: list[dict] = []


def reset_findings() -> None:
    FINDINGS.clear()


def record(category: str, check: str, detail: str, **evidence) -> dict:
    """Append one finding. Returns it (handy for tests)."""
    assert category in CATEGORIES, category
    finding = {"category": category, "check": check, "detail": detail}
    if evidence:
        finding["evidence"] = evidence
    FINDINGS.append(finding)
    return finding


def findings_by_category() -> dict[str, list[dict]]:
    out: dict[str, list[dict]] = {c: [] for c in CATEGORIES}
    for f in FINDINGS:
        out.setdefault(f["category"], []).append(f)
    return out


def defect_count() -> int:
    return sum(1 for f in FINDINGS if f["category"] == "DEFECT")


def summary() -> dict:
    by = findings_by_category()
    return {c: len(by.get(c, [])) for c in CATEGORIES}


# ---------------------------------------------------------------------------
# C++ string-literal extraction
# ---------------------------------------------------------------------------

def strip_cpp_comments(text: str) -> str:
    """Blank out // and /* */ comments while preserving string/char literals.

    Line structure and offsets are preserved (comments become spaces, newlines
    are kept) so line numbers stay exact and literals inside comments are not
    mistaken for code.
    """
    out: list[str] = []
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c in '"\'':  # string / char literal
            quote = c
            out.append(c)
            i += 1
            while i < n:
                out.append(text[i])
                if text[i] == "\\":
                    i += 1
                    if i < n:
                        out.append(text[i])
                elif text[i] == quote:
                    i += 1
                    break
                i += 1
            continue
        if c == "/" and i + 1 < n and text[i + 1] == "/":
            while i < n and text[i] != "\n":
                out.append(" ")
                i += 1
            continue
        if c == "/" and i + 1 < n and text[i + 1] == "*":
            while i < n and not (text[i] == "*" and i + 1 < n and text[i + 1] == "/"):
                out.append("\n" if text[i] == "\n" else " ")
                i += 1
            if i < n:
                out.append("  ")
                i += 2
            continue
        out.append(c)
        i += 1
    return "".join(out)


_SIMPLE_ESCAPES = {
    "n": "\n", "t": "\t", "r": "\r", "0": "\0", "a": "\a", "b": "\b",
    "f": "\f", "v": "\v", "\\": "\\", '"': '"', "'": "'", "?": "?",
}


def decode_c_string(raw: str) -> str:
    """Decode a raw C++ string body (without quotes) into Python text."""
    buf = bytearray()
    i, n = 0, len(raw)
    while i < n:
        c = raw[i]
        if c != "\\":
            buf += c.encode("utf-8")
            i += 1
            continue
        i += 1
        if i >= n:
            break
        e = raw[i]
        if e == "x":  # \xNN (arbitrary hex digits)
            i += 1
            j = i
            while j < n and raw[j] in "0123456789abcdefABCDEF":
                j += 1
            buf.append(int(raw[i:j], 16) & 0xFF)
            i = j
        elif e == "u":  # \uNNNN
            buf += chr(int(raw[i + 1:i + 5], 16)).encode("utf-8")
            i += 5
        elif e == "U":  # \UNNNNNNNN
            buf += chr(int(raw[i + 1:i + 9], 16)).encode("utf-8")
            i += 9
        elif e in "01234567":  # octal, up to 3 digits
            j = i
            while j < n and j < i + 3 and raw[j] in "01234567":
                j += 1
            buf.append(int(raw[i:j], 8) & 0xFF)
            i = j
        else:
            buf += _SIMPLE_ESCAPES.get(e, e).encode("utf-8")
            i += 1
    try:
        return buf.decode("utf-8")
    except UnicodeDecodeError:
        return buf.decode("latin-1")


_STRING_LITERAL = re.compile(r'"((?:[^"\\]|\\.)*)"')


@dataclass(frozen=True)
class ManifestString:
    """One decoded UI string template and its source citation."""
    file: str
    line: int
    value: str
    order: int  # position in the decoded manifest (source order)

    @property
    def citation(self) -> str:
        return f"{self.file}:{self.line}"


def _split_lines(text: str) -> list[str]:
    return text.split("\n")


def extract_literals(path: str, ranges: list[tuple[int, int]] | None = None,
                     *, decode: bool = True, join_adjacent: bool = True) -> list[ManifestString]:
    """Extract decoded string literals from `path`, optionally restricted to ranges.

    Comments are stripped first. Adjacent literals separated only by whitespace
    (including newlines) are joined, matching C++ concatenation; a literal's
    citation is the line of its first fragment.
    """
    text = (REPO_ROOT / path).read_text(encoding="utf-8", errors="replace")
    stripped = strip_cpp_comments(text)
    lines = _split_lines(stripped)
    newline_idx = [0]  # cumulative newlines before each char, lazily via prefix counts
    for ln in lines:
        newline_idx.append(newline_idx[-1] + len(ln) + 1)

    def line_of(pos: int) -> int:
        import bisect
        return bisect.bisect_right(newline_idx, pos)

    def in_range(line: int) -> bool:
        if ranges is None:
            return True
        return any(a <= line <= b for a, b in ranges)

    matches = list(_STRING_LITERAL.finditer(stripped))
    out: list[ManifestString] = []
    k = 0
    order = 0
    while k < len(matches):
        m = matches[k]
        line = line_of(m.start())
        # join following literals while only whitespace separates them. Each
        # fragment is decoded *before* concatenation: C++ processes escapes per
        # literal, so `"\xC2\xB0" "C"` is "°C", never the bogus `\xB0C`.
        fragments = [m.group(1)]
        while (k + 1 < len(matches)
               and stripped[m.end():matches[k + 1].start()].strip() == ""):
            k += 1
            fragments.append(matches[k].group(1))
        if in_range(line):
            value = "".join(decode_c_string(f) for f in fragments) if decode \
                else "".join(fragments)
            if value:
                out.append(ManifestString(path, line, value, order))
                order += 1
        k += 1
    return out


# ---------------------------------------------------------------------------
# "Is this user-visible copy?" classification
# ---------------------------------------------------------------------------

_LOG_OR_INTERNAL = re.compile(r"^\[|chosen %d|here %d|^\s*RT_[A-Z0-9_]+\s*$")
_INTERNAL_TOKENS = {"keyboard", "ps", "xbox", "nintendo", "text", "ready", "packs"}


def is_user_visible(value: str) -> bool:
    """Conservative user-visible classification for a manifest literal.

    Keeps row labels, values, headings and hint/copy sentences; drops ImGui ids,
    env-var names, log formats and internal enum spellings. Punctuation-only
    fragments are kept (the spec enumerates some, e.g. ` (missing)`).
    """
    if value.strip() == "" and not value.count("\n"):
        return False
    if "##" in value:
        return False
    if "RT_" in value and re.fullmatch(r"RT_[A-Z0-9_]+", value):
        return False
    if _LOG_OR_INTERNAL.search(value):
        return False
    if value in _INTERNAL_TOKENS:
        return False
    return True


def manifest(only_visible: bool = False) -> list[ManifestString]:
    """The ground-truth string manifest across all declared source ranges."""
    out: list[ManifestString] = []
    for path, ranges in STRING_SOURCES.items():
        for s in extract_literals(path, ranges):
            if only_visible and not is_user_visible(s.value):
                continue
            out.append(ManifestString(s.file, s.line, s.value, len(out)))
    return out


# ---------------------------------------------------------------------------
# Spec document parsing
# ---------------------------------------------------------------------------

def load_spec_text() -> str:
    return SPEC_DOC.read_text(encoding="utf-8")


_SPEC_CITATION = re.compile(
    r"`?((?:src/|docs/|[A-Za-z0-9_./]+/)?)([A-Za-z0-9_]+\.(?:cpp|h|md|toml|py))`?:"
    r"(\d+(?:-\d+)?(?:,\d+(?:-\d+)?)*)"
)
# A bare, backticked line-range list (the doc's shorthand), e.g. `174-245,775-824`.
_SPEC_BARE_RANGE = re.compile(r"`(\d+(?:-\d+)(?:,\d+(?:-\d+)?)*)`")
_SPEC_TOKEN = re.compile(r"`([^`]+)`")


def _parse_range_list(text: str) -> list[tuple[int, int]]:
    out: list[tuple[int, int]] = []
    for part in text.split(","):
        part = part.strip()
        if not part:
            continue
        if "-" in part:
            a, b = part.split("-", 1)
            out.append((int(a), int(b)))
        else:
            out.append((int(part), int(part)))
    return out


@dataclass(frozen=True)
class SpecCitation:
    line: int          # doc line number
    file: str          # short file name, e.g. "PauseMenu.cpp"
    start: int         # cited first source line
    end: int           # cited last source line
    resolved: str | None  # repo-relative path when resolvable
    pos: int = 0       # character offset of the citation within the doc line


def _resolve_citation_file(name: str) -> str | None:
    if name in CITATION_FILES:
        return CITATION_FILES[name]
    for p, short in CITATION_FILES.items():
        if p.endswith(name):
            return short
    return None


def parse_spec_citations() -> list[SpecCitation]:
    """Resolve the spec's inline `File.cpp:NN-MM` citations.

    A bare ``NN-MM`` range (the doc's shorthand) inherits the most recently named
    file when that file actually has literals in the range, otherwise the
    ground-truth source with the most literals there. The file memory persists
    across doc lines, matching the section-scoped shorthand.
    """
    out: list[SpecCitation] = []
    # Most-recently-mentioned resolved file first; a bare range binds to the first
    # file in this history that actually has literals in the range.
    history: list[str] = []
    for line_no, line in enumerate(load_spec_text().split("\n"), 1):
        for m in _SPEC_CITATION.finditer(line):
            name = m.group(2)
            resolved = _resolve_citation_file(name)
            if resolved:
                if name in history:
                    history.remove(name)
                history.insert(0, name)
            for a, b in _parse_range_list(m.group(3)):
                out.append(SpecCitation(line_no, name, a, b, resolved, m.start()))
        for m in _SPEC_BARE_RANGE.finditer(line):
            ranges = _parse_range_list(m.group(1))
            chosen = None
            for name in history:
                path = _resolve_citation_file(name)
                if path and any(extract_literals(path, [r]) for r in ranges):
                    chosen = name
                    break
            if chosen:
                for a, b in ranges:
                    out.append(SpecCitation(line_no, chosen, a, b,
                                            _resolve_citation_file(chosen), m.start()))
    return out


def spec_token_is_literal(token: str) -> bool:
    """A spec backticked token meant as quoted UI copy (not code/hex/path/range)."""
    if not re.search(r"[a-z]", token):
        return False
    if token != token.strip():
        return False
    if re.search(r"\.(cpp|h|md|toml|py)(:|$)", token):
        return False
    if re.fullmatch(r"\d+([,-]\d+)*", token):
        return False
    if re.fullmatch(r"[0-9A-Fa-f]{6,8}", token):
        return False
    if "<" in token or ">" in token or "..." in token:
        return False
    if re.search(r"\bNN?\b", token):
        return False
    if re.search(r"[=(){}\[\];|]", token):
        return False
    return True


def _arrow_before(text: str, open_tick: int) -> bool:
    j = open_tick - 1
    while j >= 0 and text[j] in " \t":
        j -= 1
    return j >= 0 and text[j] == "\u2192"


def spec_literal_drifts(max_distance: int = 25) -> list[dict]:
    """Spec literals anchored to a ground-truth citation where they are absent.

    A token is anchored to its nearest citation by character distance, within
    `max_distance`, ignoring the replacement side of a ``->`` substitution.
    """
    lines = load_spec_text().split("\n")
    by_line: dict[int, list[SpecCitation]] = {}
    for c in parse_spec_citations():
        by_line.setdefault(c.line, []).append(c)

    gt_sources = set(STRING_SOURCES)
    drifts: list[dict] = []
    for line_no, cites in by_line.items():
        line = lines[line_no - 1]
        for m in _SPEC_TOKEN.finditer(line):
            if _arrow_before(line, m.start()):
                continue
            token = m.group(1)
            if not spec_token_is_literal(token):
                continue
            best, best_d = None, None
            for c in cites:
                d = c.pos - m.end() if c.pos >= m.end() else m.start() - c.pos
                if best_d is None or d < best_d:
                    best, best_d = c, d
            if best is None or best.resolved not in gt_sources or best_d > max_distance:
                continue
            values = {s.value for s in extract_literals(best.resolved, [(best.start, best.end)])}
            if token not in values:
                drifts.append({
                    "doc_line": line_no,
                    "literal": token,
                    "citation": f"{best.resolved}:{best.start}-{best.end}",
                    "distance": best_d,
                })
    seen = set()
    unique = []
    for d in drifts:
        key = (d["literal"], d["citation"])
        if key not in seen:
            seen.add(key)
            unique.append(d)
    return unique


def spec_backtick_tokens() -> list[tuple[int, str]]:
    out: list[tuple[int, str]] = []
    for line_no, line in enumerate(load_spec_text().split("\n"), 1):
        for m in _SPEC_TOKEN.finditer(line):
            out.append((line_no, m.group(1)))
    return out


def classify_literal(value: str) -> str:
    """Rough kind for a manifest literal, for triaging SPEC_GAP findings."""
    if "%" in value and re.search(r"%[-+ #0-9.*]*[diufegscxX%]", value):
        return "format"
    if value != value.strip():
        return "fragment"
    if len(value.strip()) <= 3:
        return "fragment"
    if len(value) >= 40 or re.search(r"[.!?]$", value):
        return "hint"
    return "label/value"


def spec_gap_kinds() -> dict[str, int]:
    kinds: dict[str, int] = {}
    for f in FINDINGS:
        if f["category"] == "SPEC_GAP":
            k = f.get("evidence", {}).get("kind", "unknown")
            kinds[k] = kinds.get(k, 0) + 1
    return kinds


def spec_gap_uncited() -> list[dict]:
    """SPEC_GAP findings whose source line no spec citation covers."""
    return [f for f in FINDINGS
            if f["category"] == "SPEC_GAP"
            and not f.get("evidence", {}).get("line_cited", False)]


def normalize_for_spec(value: str) -> str:
    """Digit-runs -> 'N' so source literals compare against spec placeholders."""
    return re.sub(r"\d+", "N", value)


# ---------------------------------------------------------------------------
# FONTE glyph coverage
# ---------------------------------------------------------------------------

# glyph_atlas.h geometry (512x384 atlas, 16x24 cells, 32 columns x 16 rows).
GLYPH_ATLAS_WIDTH = 512
GLYPH_ATLAS_HEIGHT = 384
GLYPH_CELL_WIDTH = 16
GLYPH_CELL_HEIGHT = 24
GLYPH_GRID_COLUMNS = GLYPH_ATLAS_WIDTH // GLYPH_CELL_WIDTH   # 32
GLYPH_GRID_ROWS = GLYPH_ATLAS_HEIGHT // GLYPH_CELL_HEIGHT    # 16
GLYPH_SLOT_COUNT = GLYPH_GRID_COLUMNS * GLYPH_GRID_ROWS     # 512
GLYPHS_PER_ROW = 31                                          # col 31 is a coloured icon


def fonte_slot_for_char(c: str) -> int:
    """Port of glyph_atlas.h::fonteSlotForChar (returns -1 for no proven glyph).

    ASCII c -> g = c - 0x21 -> slot = (g/31)*32 + (g%31); '!'(0x21)..'~'(0x7E)
    only; space and other bytes have no glyph.
    """
    u = ord(c)
    if u < 0x21 or u > 0x7E:
        return -1
    g = u - 0x21
    row, col = divmod(g, GLYPHS_PER_ROW)
    if row >= GLYPH_GRID_ROWS:
        return -1
    return row * GLYPH_GRID_COLUMNS + col


def fonte_slot_uv(slot: int) -> tuple[float, float, float, float]:
    col = slot % GLYPH_GRID_COLUMNS
    row = slot // GLYPH_GRID_COLUMNS
    eu = 0.5 / GLYPH_ATLAS_WIDTH
    ev = 0.5 / GLYPH_ATLAS_HEIGHT
    u0 = col * GLYPH_CELL_WIDTH / GLYPH_ATLAS_WIDTH
    v0 = row * GLYPH_CELL_HEIGHT / GLYPH_ATLAS_HEIGHT
    du = GLYPH_CELL_WIDTH / GLYPH_ATLAS_WIDTH
    dv = GLYPH_CELL_HEIGHT / GLYPH_ATLAS_HEIGHT
    return (u0 + eu, v0 + ev, u0 + du - eu, v0 + dv - ev)


# The four permitted FONTE punctuation substitutions (ui-parity.md:102, ledger
# item 5). Keys are the recomp characters that have no proven glyph.
PERMITTED_SUBSTITUTIONS: dict[str, str] = {
    "\u2026": "...",   # … horizontal ellipsis     -> ...   (PauseMenu.cpp:2002, 686-693)
    "\u2014": " - ",   # — em dash                 -> " - " (SecondScreen.cpp:491)
    "\u00b0": "deg",   # ° degree                  -> deg   (PerfOverlay.cpp:59)
}

# Characters that advance without a glyph quad (whitespace/newlines in strings).
NON_GLYPH_WHITESPACE = {"\n", "\t", " "}


def is_renderable(c: str) -> bool:
    """True when the FONTE atlas can draw `c` (space advances; !..~ has a slot)."""
    if c in NON_GLYPH_WHITESPACE:
        return True
    return fonte_slot_for_char(c) >= 0


# ---------------------------------------------------------------------------
# Recomp design scale + frame mapping
# ---------------------------------------------------------------------------

def handheld_boost() -> float:
    """Desktop boost is always 1.0 (Theme.cpp:68-86; Android clamps 1.0..1.7)."""
    return 1.0


def scale(height: float, boost: float = 1.0) -> float:
    """S = max(.5, height/1080 * handheldBoost) (Theme.cpp:88)."""
    return max(0.5, height / 1080.0 * boost)


# Design-pixel constants at 1080p (S == 1). Values are cited from ui-parity.md
# §"Color / geometry contract" and the per-screen sections.
GEOMETRY_1080: dict[str, float] = {
    "pause.root_panel_w": 520.0,      # PauseMenu.cpp:1838-1887
    "pause.subpage_panel_w": 880.0,
    "pause.row_h": 80.0,
    "pause.state_card_h": 136.0,
    "pause.panel_pad": 30.0,
    "pause.inner_side_inset": 30.0,
    "pause.safe_margin": 24.0,
    "pause.name_tab_h": 44.0,
    "pause.prompt_zone_h": 90.0,
    "second.panel_margin": 24.0,      # SecondScreen.cpp:731-765
    "second.tab_row_h": 84.0,
    "second.content_inset": 30.0,
    "second.tab_max_w": 240.0,
    "second.menu_pill_w": 168.0,
    "setup.dialog_pad": 56.0,         # SetupScreen.cpp:36-62,164-212
    "setup.progress_h": 44.0,
    "perf.x": 16.0,                   # PerfOverlay.cpp:19-96
    "perf.bottom": 12.0,
    "perf.rim_w": 2.0,
}

VIEWPORT_CAP = 0.92  # root/subpage each capped at 92% viewport (ui-parity.md:29)


def pause_geometry(height: float, boost: float = 1.0) -> dict[str, float]:
    """Every design-pixel geometry constant scaled by S(height)."""
    s = scale(height, boost)
    return {k: v * s for k, v in GEOMETRY_1080.items()}


def display_frame_factors(width: float, height: float) -> tuple[float, float]:
    """The spec's mapping factors x'=x*W/640, y'=(y+16)*H/480 (ui-parity.md:9)."""
    return width / 640.0, height / 480.0


def native_uniform_frame_factor(width: float, height: float) -> float:
    """The native source mapping: one uniform factor min(W/640,H/480), centred.

    `race_hud.h:94-113` (fonteTextQuads) and the 640x480 display frame
    (`hud_layout.h:37-41`). Never stretches.
    """
    return min(width / 640.0, height / 480.0)


# ---------------------------------------------------------------------------
# Theme colours / strokes (source-parsed)
# ---------------------------------------------------------------------------

_THEME_COLOR = re.compile(
    r"constexpr\s+ImU32\s+(\w+)\s*=\s*IM_COL32\(\s*0x([0-9A-Fa-f]{2})\s*,\s*"
    r"0x([0-9A-Fa-f]{2})\s*,\s*0x([0-9A-Fa-f]{2})\s*,\s*0x([0-9A-Fa-f]{2})\s*\)"
)


def parse_theme_colors() -> dict[str, str]:
    """Map Theme.h colour name -> 8-hex RGBA (IM_COL32 order)."""
    text = (REPO_ROOT / "src/ui/Theme.h").read_text(encoding="utf-8", errors="replace")
    out: dict[str, str] = {}
    for name, r, g, b, a in _THEME_COLOR.findall(text):
        out[name] = (r + g + b + a).upper()
    return out


# ui-parity.md §"Color / geometry contract" (lines 106-114): spec value -> Theme.h name.
THEME_COLOR_EXPECT: dict[str, str] = {
    "485800F2": "Olive",
    "C05B1E": "FrameOuter",
    "F86018": "Frame",
    "F8BF84": "FrameLight",
    "0098C8": "ListBody",
    "8ED4F0": "ListBevelLight",
    "0F5E92": "ListBevelDark",
    "F6E68F": "ListText",
    "ECECEC": "ListSelected",
    "2C6CE8": "BarTop",
    "0838B0": "BarBottom",
    "F8C838": "BarCapGold",
    "F2C230": "Horn",
    "304090F2": "Prompt",
    "91D2E8": "PromptRimOuter",
    "1CA1E6": "PromptRimInner",
    "1A1A10": "Outline",
    "000820": "OutlineBlue",
    "000A2899": "Scrim",
}


def parse_theme_strokes() -> dict:
    """Source-parsed frame stroke widths and standard roundings from Theme.cpp."""
    text = (REPO_ROOT / "src/ui/Theme.cpp").read_text(encoding="utf-8", errors="replace")
    widths = re.search(r"static\s+const\s+float\s+widths\[\]\s*=\s*\{([^}]*)\}", text)
    roundings = {}
    for key, var in (("window", "WindowRounding"), ("frame", "FrameRounding"),
                     ("popup", "PopupRounding")):
        m = re.search(rf"s\.{var}\s*=\s*([0-9.]+)f?\s*;", text)
        if m:
            roundings[key] = float(m.group(1))
    return {
        "frame_stroke_widths": [float(x) for x in re.findall(r"[0-9.]+", widths.group(1))]
        if widths else [],
        "roundings": roundings,
    }


# ---------------------------------------------------------------------------
# Native OptionsModel parsing
# ---------------------------------------------------------------------------

_OPTION_CALL = re.compile(
    r"(toggle|enumOpt|integer|real|stringOpt|action)\(\s*\"([^\"]*)\"\s*,\s*"
    r"\"((?:[^\"\\]|\\.)*)\"\s*,\s*P::(\w+)"
)
_STATUS = re.compile(r"S::(WiredNow|GpuWavePending)")


@dataclass(frozen=True)
class NativeOption:
    builder: str
    id: str
    label: str
    page: str
    status: str


def parse_native_options() -> list[NativeOption]:
    """Parse the 40-row OptionsModel catalog from src/native/gameplay/settings.cpp."""
    text = (REPO_ROOT / "src/native/gameplay/settings.cpp").read_text(encoding="utf-8", errors="replace")
    out: list[NativeOption] = []
    for m in _OPTION_CALL.finditer(text):
        # Status appears inside the same push_back(...) call; search forward to the
        # call's closing `);` while tracking parenthesis depth.
        depth = 1
        j = m.end()
        while j < len(text) and depth > 0:
            if text[j] == "(":
                depth += 1
            elif text[j] == ")":
                depth -= 1
            j += 1
        block = text[m.start():j]
        st = _STATUS.search(block)
        out.append(NativeOption(m.group(1), m.group(2), m.group(3), m.group(4),
                                st.group(1) if st else "WiredNow"))
    return out


# ---------------------------------------------------------------------------
# findings.json writer
# ---------------------------------------------------------------------------

def findings_payload() -> dict:
    m = manifest()
    return {
        "spec": str(SPEC_DOC.relative_to(REPO_ROOT)),
        "generated_by": "tests/native_parity",
        "summary": summary(),
        "spec_gap_breakdown": {
            "by_kind": spec_gap_kinds(),
            "uncited": len(spec_gap_uncited()),
        },
        "string_manifest": {
            "declared_source_ranges": {k: v for k, v in STRING_SOURCES.items()},
            "occurrences_extracted": len(m),
            "distinct_templates": len({s.value for s in m}),
            "visible_templates": len({s.value for s in m if is_user_visible(s.value)}),
            "spec_declared_count": SPEC_DECLARED_STRING_COUNT,
        },
        "findings": FINDINGS,
    }


def write_findings_json(path: pathlib.Path | None = None) -> pathlib.Path:
    path = path or pathlib.Path(__file__).resolve().parent / "findings.json"
    path.write_text(json.dumps(findings_payload(), indent=2, ensure_ascii=False) + "\n",
                    encoding="utf-8")
    return path
