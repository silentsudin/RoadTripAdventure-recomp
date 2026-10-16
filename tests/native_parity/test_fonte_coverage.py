"""Check 4 - FONTE coverage (protocol item 4).

Ports the atlas slot formula from `src/native/assets/glyph_atlas.h`
(`fonteSlotForChar`) and asserts every character of every user-visible manifest
string is either renderable by the FONTE atlas or is one of the four permitted
punctuation substitutions of the deviation ledger (ui-parity.md:102, ledger 5).

An unrenderable, un-ledgered byte is a DEFECT. A permitted substitution actually
used is a ledgered KNOWN_GAP.
"""

from __future__ import annotations

import parity_spec as ps

CHECK = "check4-fonte-coverage"

# Source pins the ledger gives for each substitution (ui-parity.md:102).
SUBSTITUTION_SOURCE_PINS = {
    "\u2026": [("src/ui/PauseMenu.cpp", 2002, 2002), ("src/ui/PauseMenu.cpp", 686, 693)],
    "\u2014": [("src/ui/SecondScreen.cpp", 491, 491)],
    "\u00b0": [("src/ui/PerfOverlay.cpp", 59, 59)],
}


def test_slot_formula_matches_glyph_atlas_spot_values():
    # Cross-checked against the verified orderings in glyph_atlas.h:27-37.
    spot = {"!": 0, "A": 33, "^": 62, "_": 64, "`": 65, "~": 96}
    for ch, slot in spot.items():
        assert ps.fonte_slot_for_char(ch) == slot, f"{ch!r} -> {ps.fonte_slot_for_char(ch)} != {slot}"
    for ch in (" ", "\n", "\t", " "):
        assert ps.fonte_slot_for_char(ch) == -1
    assert ps.fonte_slot_for_char(chr(0x20)) == -1
    assert ps.fonte_slot_for_char(chr(0x7F)) == -1


def test_slot_mapping_is_total_and_injective():
    slots = [ps.fonte_slot_for_char(chr(c)) for c in range(0x21, 0x7F)]
    assert all(0 <= s < ps.GLYPH_SLOT_COUNT for s in slots)
    assert len(set(slots)) == len(slots), "FONTE slot collision"
    # each row's col-31 slot is reserved for a coloured icon, never a glyph
    assert all(s % ps.GLYPH_GRID_COLUMNS != 31 for s in slots)


def test_uv_rects_stay_inside_the_atlas():
    for slot in range(ps.GLYPH_SLOT_COUNT):
        u0, v0, u1, v1 = ps.fonte_slot_uv(slot)
        assert 0.0 < u0 < u1 < 1.0 and 0.0 < v0 < v1 < 1.0


def test_every_manifest_character_is_renderable_or_ledgered(visible_manifest, defect_baseline):
    offenders: dict[str, list[str]] = {}
    substitutions: dict[str, list[str]] = {}
    for s in visible_manifest:
        for ch in s.value:
            if ps.is_renderable(ch):
                continue
            if ch in ps.PERMITTED_SUBSTITUTIONS:
                substitutions.setdefault(ch, []).append(s.citation)
                continue
            offenders.setdefault(ch, []).append(s.citation)

    for ch, cites in sorted(offenders.items()):
        ps.record("DEFECT", CHECK,
                  f"unrenderable, un-ledgered character U+{ord(ch):04X} in the manifest",
                  char=ch, code=hex(ord(ch)), citations=sorted(set(cites))[:5])
    for ch, cites in sorted(substitutions.items()):
        ps.record("KNOWN_GAP", CHECK,
                  f"manifest uses {ch!r} (U+{ord(ch):04X}); native must render "
                  f"{ps.PERMITTED_SUBSTITUTIONS[ch]!r}",
                  char=ch, substitution=ps.PERMITTED_SUBSTITUTIONS[ch],
                  citations=sorted(set(cites))[:5], ledger="ui-parity.md:102 (ledger 5)")
    assert ps.defect_count() == defect_baseline


def test_substitutions_are_pinned_to_their_source_lines(defect_baseline):
    for ch, pins in SUBSTITUTION_SOURCE_PINS.items():
        for path, a, b in pins:
            values = [s.value for s in ps.extract_literals(path, [(a, b)])]
            if not any(ch in v for v in values):
                ps.record("DEFECT", CHECK,
                          f"ledgered substitution {ch!r} not found at {path}:{a}-{b}",
                          char=ch, file=path, start=a, end=b)
    assert ps.defect_count() == defect_baseline
