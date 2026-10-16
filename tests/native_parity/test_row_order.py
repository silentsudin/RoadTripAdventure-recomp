"""Check 2 - ground-truth menu row order / visibility (protocol item 2).

Parses the recomp PauseMenu sections into an ordered fixture per screen and
asserts the ground-truth source still presents those rows in that order. Rows
whose order is chosen at runtime (the deadzone pair is built inside a loop) are
asserted for presence only. A label that is missing or out of order is a DEFECT.
"""

from __future__ import annotations

import parity_spec as ps

CHECK = "check2-row-order"

# Ground-truth order per recomp screen, from ui-parity.md §"Pause root",
# §"Pause Options", §"Controllers and children". `ordered` must appear in source
# order; `members` must appear in the cited range in any order.
SCREENS: dict[str, dict] = {
    "pause_root": {
        "ranges": [(293, 303)],
        "ordered": ["Resume", "Save state", "Load state", "Options", "Controllers", "Quit"],
        "visibility": {
            "Save state": "only when the save-state predicate passes",
            "Load state": "same predicate as Save state",
        },
    },
    "display": {
        "ranges": [(397, 525)],
        "ordered": ["Display mode", "Window size", "Aspect ratio", "HUD position", "Map",
                    "Frame rate", "Frame generation", "Frame skip", "Performance overlay",
                    "Second screen"],
        "visibility": {
            "Display mode": "not Android",
            "Window size": "non-Android + Window",
            "Aspect ratio": "16:9 capability",
            "HUD position": "Aspect row visible",
            "Map": "hardware GS, not second display",
            "Frame rate": "120 Hz capability",
            "Frame generation": ">60 fps and RT_FRAME_GEN=1",
            "Second screen": "presentation display exists",
        },
    },
    "sound": {
        "ranges": [(526, 542)],
        "ordered": ["Volume", "Speaker", "Radio station"],
        "visibility": {"Radio station": "Adventure only"},
    },
    "graphics": {
        "ranges": [(543, 766)],
        "ordered": ["Renderer", "Supersampling (SSAA)", "Mipmaps", "Fix steering bias",
                    "High detail cars", "Draw distance", "Race car shadows", "Texture pack",
                    "Interlacing", "Anti-aliasing", "Upscaling", "Sharpening"],
        "visibility": {
            "Renderer": "GPU GS + reference renderers",
            "Mipmaps": "accurate renderer only",
            "Interlacing": "hidden until native matches (ledger 2)",
            "Sharpening": "FSR1/GSR2/ASR/FSR3/DLSS/XeSS only",
        },
    },
    "controllers": {
        "ranges": [(813, 872)],
        "ordered": ["Devices", "Driving", "Vibration", "Strength"],
        "visibility": {
            "Vibration": "gamepad present",
            "Strength": "disables when Vibration is Off",
        },
    },
    "device": {
        "ranges": [(875, 908)],
        "ordered": ["Plays as", "Buttons"],
        "members": ["Left stick deadzone", "Right stick deadzone", "Vibration", "Test vibration"],
        "visibility": {"Left stick deadzone": "pads only",
                       "Right stick deadzone": "pads only"},
    },
    "buttons": {
        "ranges": [(911, 934)],
        "ordered": ["Up", "Down", "Left", "Right", "Cross", "Circle", "Square", "Triangle",
                    "L1", "R1", "L2", "R2", "Start", "Select", "L3", "R3"],
    },
    "driving": {
        "ranges": [(979, 1048)],
        "ordered": ["Layout", "Action", "Your control", "Feel", "Analogue triggers"],
        # The `Controls` row is produced by schemeRow() (shared with the Controllers
        # page) whose literal sits at line 960, outside this page's cited range.
        "shared": {"Controls": 960},
    },
}

PAUSE = "src/ui/PauseMenu.cpp"


def _in_any_range(line: int, ranges) -> bool:
    return any(a <= line <= b for a, b in ranges)


def _section(manifest, ranges):
    return [s for s in manifest if s.file == PAUSE and _in_any_range(s.line, ranges)]


def _greedy_order(section, labels):
    """Return (ok, detail) for a strictly increasing greedy match of `labels`."""
    cursor = 0
    positions = []
    for label in labels:
        found = -1
        for i in range(cursor, len(section)):
            if section[i].value == label:
                found = i
                break
        if found < 0:
            return False, f"row {label!r} not present in section"
        positions.append((label, section[found].line))
        cursor = found + 1
    return True, positions


def test_screen_inventory_matches_spec_sections(defect_baseline):
    # The checks below only mean something if the fixtures cover the PauseMenu
    # string ranges the spec says hold root/options/controllers copy.
    declared = set(ps.STRING_SOURCES[PAUSE])
    missing = []
    for screen in SCREENS.values():
        for r in screen["ranges"]:
            if not any(d0 <= r[0] and r[1] <= d1 for d0, d1 in declared):
                missing.append(r)
    for r in missing:
        ps.record("DEFECT", CHECK,
                  f"row-order fixture cites a range outside the declared PauseMenu sources: {r}",
                  range=list(r))
    assert not missing


def test_shared_rows_are_traced_to_their_source(manifest, defect_baseline):
    """A row reused across pages must be traceable, and its literal cited."""
    for name, screen in SCREENS.items():
        for label, line in screen.get("shared", {}).items():
            if not any(s.file == PAUSE and s.line == line and s.value == label for s in manifest):
                ps.record("DEFECT", CHECK,
                          f"{name}: shared row {label!r} not found at {PAUSE}:{line}",
                          screen=name, row=label)
                continue
            if not any(a <= line <= b for a, b in screen["ranges"]):
                ps.record("SPEC_DRIFT", CHECK,
                          f"{name}: row {label!r} is drawn on this page but its literal "
                          f"({PAUSE}:{line}) is outside the spec's cited range {screen['ranges']}",
                          screen=name, row=label, literal_line=line)
    assert ps.defect_count() == defect_baseline


def test_row_order_per_screen(manifest, defect_baseline):
    for name, screen in SCREENS.items():
        section = _section(manifest, screen["ranges"])
        ok, detail = _greedy_order(section, screen["ordered"])
        if not ok:
            ps.record("DEFECT", CHECK, f"{name}: {detail}", screen=name)
            continue
        for label in screen.get("members", []):
            if not any(s.value == label for s in section):
                ps.record("DEFECT", CHECK,
                          f"{name}: required row {label!r} absent from {screen['ranges']}",
                          screen=name)
    assert ps.defect_count() == defect_baseline


def test_visibility_predicates_are_documented(defect_baseline):
    # Every conditional row must carry a documented predicate in the spec so the
    # native adapter can replicate it; a row with unknown visibility is drift.
    for name, screen in SCREENS.items():
        for label, predicate in screen.get("visibility", {}).items():
            if not predicate:
                ps.record("SPEC_DRIFT", CHECK,
                          f"{name}: row {label!r} has no documented visibility predicate",
                          screen=name, row=label)
    assert ps.defect_count() == defect_baseline
