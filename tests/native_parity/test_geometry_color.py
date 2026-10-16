"""Check 5 - geometry / colour constants (protocol item 5).

Implements the recomp design scale ``S = max(.5, H/1080 * handheldBoost)`` and the
``x'=x*W/640`` / ``y'=(y+16)*H/480`` frame mapping, asserts the spec's listed
design-pixel constants at 1080p (S=1) and 2160p (S=2), and verifies the spec's
cited Theme colours against the source-parsed Theme.h/Theme.cpp values (T0 exact).

A source/Theme mismatch is a DEFECT. A stale numeric claim in the spec prose is a
SPEC_DRIFT finding.
"""

from __future__ import annotations

import parity_spec as ps

CHECK = "check5-geometry-color"


# --- scale formula -----------------------------------------------------------

def test_scale_formula():
    assert ps.scale(1080) == 1.0
    assert ps.scale(2160) == 2.0
    assert ps.scale(720) == 720 / 1080
    assert ps.scale(540) == 0.5
    assert ps.scale(400) == 0.5          # floor holds (max(.5, .370))
    assert ps.scale(1080, boost=1.7) == 1.7
    assert ps.scale(1080, boost=0.5) == 0.5


def test_pause_geometry_at_1080p():
    g = ps.pause_geometry(1080)
    assert g["pause.root_panel_w"] == 520.0
    assert g["pause.subpage_panel_w"] == 880.0
    assert g["pause.row_h"] == 80.0
    assert g["pause.state_card_h"] == 136.0
    assert g["pause.panel_pad"] == 30.0
    assert g["pause.inner_side_inset"] == 30.0
    assert g["pause.safe_margin"] == 24.0
    assert g["pause.name_tab_h"] == 44.0
    assert g["pause.prompt_zone_h"] == 90.0


def test_pause_geometry_at_2160p():
    g = ps.pause_geometry(2160)
    assert g["pause.root_panel_w"] == 1040.0
    assert g["pause.subpage_panel_w"] == 1760.0
    assert g["pause.row_h"] == 160.0
    assert g["pause.state_card_h"] == 272.0
    assert g["pause.panel_pad"] == 60.0
    assert g["pause.safe_margin"] == 48.0
    assert g["pause.name_tab_h"] == 88.0
    assert g["pause.prompt_zone_h"] == 180.0


def test_secondary_geometry_scales():
    for height, s in ((1080, 1.0), (2160, 2.0)):
        g = ps.pause_geometry(height)
        assert g["second.panel_margin"] == 24.0 * s
        assert g["second.tab_row_h"] == 84.0 * s
        assert g["second.content_inset"] == 30.0 * s
        assert g["second.tab_max_w"] == 240.0 * s
        assert g["second.menu_pill_w"] == 168.0 * s
        assert g["setup.dialog_pad"] == 56.0 * s
        assert g["setup.progress_h"] == 44.0 * s
        assert g["perf.x"] == 16.0 * s
        assert g["perf.bottom"] == 12.0 * s
        assert g["perf.rim_w"] == 2.0 * s


def test_subpage_panel_respects_viewport_cap():
    # "subpages 880*S, each capped at 92% viewport" (ui-parity.md:29).
    for width in (1920, 2560, 3840):
        height = width * 9 / 16
        g = ps.pause_geometry(height)
        assert g["pause.subpage_panel_w"] <= ps.VIEWPORT_CAP * width + 1e-9


# --- frame mapping -----------------------------------------------------------

def test_display_frame_mapping_factors():
    assert ps.display_frame_factors(1920, 1080) == (3.0, 2.25)
    assert ps.display_frame_factors(3840, 2160) == (6.0, 4.5)
    assert ps.native_uniform_frame_factor(1920, 1080) == 2.25   # output-modes.md:156
    assert ps.native_uniform_frame_factor(1280, 720) == 1.5
    assert ps.native_uniform_frame_factor(3840, 2160) == 4.5
    # The mapping never stretches: 448-line field is centred inside the 480 frame.
    assert ps.GEOMETRY_1080["perf.x"] * ps.native_uniform_frame_factor(1920, 1080) < 640 * 2.25


def test_spec_1080p_factor_claim_is_stale(defect_baseline):
    spec = ps.load_spec_text()
    if "1.6875" in spec:
        doc_claim = 1.6875
        source_factor = ps.native_uniform_frame_factor(1920, 1080)  # 2.25, uniform & non-stretch
        if doc_claim != source_factor:
            ps.record("SPEC_DRIFT", CHECK,
                      "ui-parity.md:9 states a 1080p x-factor of 1.6875x but the source "
                      "mapping (race_hud.h / hud_layout.h, output-modes.md:156) is a uniform "
                      f"{source_factor}x per axis",
                      doc_claim=doc_claim, source_factor=source_factor,
                      native_uniform=source_factor)
    assert ps.defect_count() == defect_baseline


# --- theme colours / strokes -------------------------------------------------

def test_theme_colours_match_spec(defect_baseline):
    colours = ps.parse_theme_colors()
    for spec_hex, name in ps.THEME_COLOR_EXPECT.items():
        if name not in colours:
            ps.record("DEFECT", CHECK, f"Theme.h colour {name} not found", name=name)
            continue
        source = colours[name]
        # The spec quotes the colours with alpha where it matters (e.g. 485800F2,
        # 000A2899) and RGB-only where the source is opaque (C05B1E ... FF).
        matches = source == spec_hex if len(spec_hex) == 8 else source[:6] == spec_hex
        if not matches:
            ps.record("DEFECT", CHECK,
                      f"Theme.h {name} = {source} but spec cites {spec_hex}",
                      name=name, source=source, spec=spec_hex)
    assert ps.defect_count() == defect_baseline


def test_theme_strokes_and_roundings():
    parsed = ps.parse_theme_strokes()
    assert parsed["frame_stroke_widths"] == [3.0, 6.0, 3.0], parsed["frame_stroke_widths"]
    assert parsed["roundings"].get("window") == 18.0
    assert parsed["roundings"].get("frame") == 14.0
    assert parsed["roundings"].get("popup") == 12.0
