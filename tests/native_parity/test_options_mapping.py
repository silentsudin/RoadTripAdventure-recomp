"""Check 3 - adapter-to-OptionsModel mapping (protocol item 3).

Maps the recomp settings rows (ui-parity.md Display/Sound/Graphics/Controllers/
Save sections) onto the 40-row native `OptionsModel`
(src/native/gameplay/settings.{h,cpp}). Rows present in the recomp catalog but
absent from the native model are the *known model gaps*; the gap list is pinned
explicitly so a newly appearing gap fails the check, while the ledgered gaps pass
with their ui-parity.md annotation.
"""

from __future__ import annotations

import re

import parity_spec as ps

CHECK = "check3-options-mapping"

LEDGER = "ui-parity.md:144-146 (Model-layer gaps)"

# Recomp settings row -> native OptionSpec id (or None when the native model has
# no backing row yet). `screen` mirrors the recomp page; `ledger` marks the gap.
RECOMP_ROWS: list[dict] = [
    # Display (PauseMenu.cpp:397-525)
    {"screen": "Display", "row": "Display mode", "native": None,
     "ledger": "no native display-mode row"},
    {"screen": "Display", "row": "Window size", "native": None,
     "ledger": "no native window-size row"},
    {"screen": "Display", "row": "Aspect ratio", "native": None,
     "ledger": "no native aspect-ratio row (widescreen policy, ledger 3)"},
    {"screen": "Display", "row": "HUD position", "native": None,
     "ledger": "no native HUD-position row"},
    {"screen": "Display", "row": "Map", "native": "display.map_mode",
     "ledger": None},
    {"screen": "Display", "row": "Frame rate", "native": None,
     "ledger": "no native frame-rate row"},
    {"screen": "Display", "row": "Frame generation", "native": None,
     "ledger": "no native frame-generation row"},
    {"screen": "Display", "row": "Frame skip", "native": None,
     "ledger": "no native frame-skip row"},
    {"screen": "Display", "row": "Performance overlay", "native": "display.debug_overlay",
     "ledger": None},
    {"screen": "Display", "row": "Second screen", "native": "display.second_screen",
     "ledger": None},
    # Sound (PauseMenu.cpp:526-542)
    {"screen": "Sound", "row": "Volume", "native": "sound.audio", "ledger": None},
    {"screen": "Sound", "row": "Speaker", "native": None,
     "ledger": "no native speaker (mono/stereo) row"},
    {"screen": "Sound", "row": "Radio station", "native": "sound.radio_station", "ledger": None},
    # Graphics (PauseMenu.cpp:543-766)
    {"screen": "Graphics", "row": "Renderer", "native": None,
     "ledger": "no native renderer-choice row"},
    {"screen": "Graphics", "row": "Supersampling (SSAA)", "native": None,
     "ledger": "no native SSAA row"},
    {"screen": "Graphics", "row": "Mipmaps", "native": None,
     "ledger": "no native mipmap row"},
    {"screen": "Graphics", "row": "Fix steering bias", "native": None,
     "ledger": "no native steering-bias row"},
    {"screen": "Graphics", "row": "High detail cars", "native": None,
     "ledger": "no native high-detail-cars row"},
    {"screen": "Graphics", "row": "Draw distance", "native": None,
     "ledger": "no native draw-distance row"},
    {"screen": "Graphics", "row": "Race car shadows", "native": None,
     "ledger": "no native race-car-shadow row"},
    {"screen": "Graphics", "row": "Texture pack", "native": None,
     "ledger": "no native HD-texture-pack row (Field textures is not the pack)"},
    {"screen": "Graphics", "row": "Interlacing", "native": None,
     "ledger": "native is progressive-only (ledger 2)"},
    {"screen": "Graphics", "row": "Anti-aliasing", "native": "display.aa", "ledger": None},
    {"screen": "Graphics", "row": "Upscaling", "native": "display.upscaler", "ledger": None},
    {"screen": "Graphics", "row": "Sharpening", "native": None,
     "ledger": "no native sharpening row"},
    # Controllers (PauseMenu.cpp:813-1048)
    {"screen": "Controllers", "row": "Controls", "native": None,
     "ledger": "no native scheme row (Modern/Classic/Custom)"},
    {"screen": "Controllers", "row": "Vibration", "native": "controllers.rumble", "ledger": None},
    {"screen": "Controllers", "row": "Strength", "native": "controllers.rumble_strength",
     "ledger": None},
    {"screen": "Controllers", "row": "Plays as", "native": None,
     "ledger": "no native device-assignment row"},
    {"screen": "Controllers", "row": "Buttons", "native": None,
     "ledger": "no native rebinding row"},
    {"screen": "Controllers", "row": "Left stick deadzone", "native": "controllers.deadzone",
     "ledger": None},
    {"screen": "Controllers", "row": "Right stick deadzone", "native": None,
     "ledger": "native Stick deadzone covers one stick only"},
    {"screen": "Controllers", "row": "Test vibration", "native": None,
     "ledger": "no native test-vibration row"},
    {"screen": "Controllers", "row": "Analogue triggers", "native": "controllers.triggers",
     "ledger": None},
    # Save states (PauseMenu.cpp:324-365)
    {"screen": "Save", "row": "Save state", "native": "save.save_state", "ledger": None},
    {"screen": "Save", "row": "Load state", "native": "save.load_state", "ledger": None},
    {"screen": "Save", "row": "Save slot (four cards)", "native": None,
     "ledger": "native has two slot<N>.sav slots, not four thumbnail cards"},
]

# Label-only aliases used for the independent (label-match) computation. Keys and
# values are already normalized (lower-case alphanumerics only).
ALIASES = {
    "map": "mapmode",
    "performanceoverlay": "debugoverlay",
    "volume": "audiooutput",
    "vibration": "rumble",
    "strength": "rumblestrength",
    "leftstickdeadzone": "stickdeadzone",
    "analoguetriggers": "analogtriggerdrive",
    "upscaling": "upscaler",
}

# Rows the spec says must be Hidden in retail UI before their effects work
# (ui-parity.md:54) plus the remaining native-only diagnostics. Pinned so a new
# native-only row is noticed.
EXPECTED_PENDING = {
    "display.aa", "display.upscaler", "display.map_mode", "display.second_screen",
    "controllers.triggers", "controllers.rumble", "controllers.rumble_strength",
    "controllers.deadzone", "sound.radio_station",
}


def _norm(text: str) -> str:
    return re.sub(r"[^a-z0-9]", "", text.lower())


def test_native_catalog_is_forty_rows(native_options):
    assert len(native_options) == 40, f"expected 40 OptionsModel rows, parsed {len(native_options)}"
    ids = [o.id for o in native_options]
    assert len(set(ids)) == len(ids), "duplicate OptionSpec ids"


def test_declared_native_mappings_exist(native_options, defect_baseline):
    by_id = {o.id: o for o in native_options}
    for row in RECOMP_ROWS:
        if row["native"] is None:
            continue
        if row["native"] not in by_id:
            ps.record("DEFECT", CHECK,
                      f"recomp row {row['row']!r} maps to unknown native id {row['native']!r}",
                      row=row["row"], native_id=row["native"])
        elif _norm(by_id[row["native"]].label) != ALIASES.get(_norm(row["row"]), _norm(row["row"])):
            ps.record("DEFECT", CHECK,
                      f"recomp row {row['row']!r} declared to native {row['native']!r} "
                      f"(label {by_id[row['native']].label!r}) but labels do not correspond",
                      row=row["row"], native_id=row["native"])
    assert ps.defect_count() == defect_baseline


def test_gap_list_is_explicit_and_stable(native_options, defect_baseline):
    declared_gaps = {row["row"] for row in RECOMP_ROWS if row["native"] is None}

    # Independent label-match: a recomp row is a gap unless its (aliased) label
    # matches a native label.
    native_norms = {_norm(o.label) for o in native_options}
    label_gaps = set()
    for row in RECOMP_ROWS:
        target = ALIASES.get(_norm(row["row"]), _norm(row["row"]))
        if target not in native_norms:
            label_gaps.add(row["row"])

    if label_gaps != declared_gaps:
        ps.record("DEFECT", CHECK,
                  "computed model gaps differ from the pinned gap list",
                  only_declared=sorted(declared_gaps - label_gaps),
                  only_computed=sorted(label_gaps - declared_gaps))

    # Every gap must carry a ledger annotation (documented gaps pass).
    for row in RECOMP_ROWS:
        if row["native"] is None and not row["ledger"]:
            ps.record("DEFECT", CHECK,
                      f"model gap {row['row']!r} has no ledger annotation", row=row["row"])

    for row in sorted(declared_gaps):
        gap = next(r for r in RECOMP_ROWS if r["row"] == row)
        ps.record("KNOWN_GAP", CHECK,
                  f"recomp row {row!r} absent from the native OptionsModel: {gap['ledger']}",
                  screen=gap["screen"], ledger=LEDGER)
    assert ps.defect_count() == defect_baseline


def test_pending_gpu_rows_are_pinned(native_options, defect_baseline):
    pending = {o.id for o in native_options if o.status == "GpuWavePending"}
    if pending != EXPECTED_PENDING:
        ps.record("DEFECT", CHECK,
                  "native GpuWavePending row set changed",
                  only_expected=sorted(EXPECTED_PENDING - pending),
                  only_parsed=sorted(pending - EXPECTED_PENDING))
    # These are the rows the recomp renderer must hide until their effects work.
    for oid in sorted(pending):
        ps.record("KNOWN_GAP", CHECK,
                  f"native row {oid!r} is GpuWavePending and must be hidden in retail UI",
                  native_id=oid, ledger="ui-parity.md:54")
    assert ps.defect_count() == defect_baseline
