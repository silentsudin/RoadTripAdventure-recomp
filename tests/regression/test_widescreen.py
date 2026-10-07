"""Widescreen (Options -> Aspect ratio 16:9): 2D-backed screens are shown 4:3 from their first
frame, and the HUD keeps its shape (gs_wide_layout.h). Frames here are the GS output, 640 units
wide: the presenter stretches them to 16:9, so HUD that keeps its shape is drawn 0.75 as wide."""

from __future__ import annotations

from pathlib import Path

import numpy as np
import pytest

from rtharness import seconds
from rtharness.adventure import FIELDS, continue_to_factory
from rtharness.town import TownDriver, TownMap, dialogue_open, door_quads, finish_dialogue

CLOUD_HILL = 8


def wide_settings(hud: str = "4:3") -> str:
    return f'[display]\naspect = "16:9"\nhud = "{hud}"\n[general]\nmenu_hint_shown = true'


def white(frame) -> bool:
    """The Takara logo: red letters on white."""
    a = frame.array()
    return float((a.min(axis=2) > 230).mean()) > 0.6


def title_picture(frame) -> bool:
    """The title's picture (faded in): sky in the top-left corner."""
    a = frame.array().astype(int)
    corner = a[: a.shape[0] // 8, : a.shape[1] // 8]
    return float(((corner[..., 2] > 150) & (corner[..., 2] > corner[..., 0] + 60)).mean()) > 0.5


def test_title_at_4_3_from_its_first_frame(game_factory):
    """The Takara logo is 3D (shown wide); the title after it is a 2D picture, shown 4:3 from its
    first frame (it took ten frames, the title showing stretched in the meantime)."""
    game = game_factory(settings=wide_settings(), render=True)
    end = game.vblank + seconds(30)
    while game.vblank < end and not (game.wide()["driving"] and white(game.frame())):
        game.run(10)
    assert game.wide()["driving"], "the Takara logo is shown wide"
    # The title fades in from black right after the logo: every frame from the logo's end on
    # (the fade's dark frames included) is the title's.
    while game.vblank < end and white(game.frame()):
        game.run(1)
    stretched, seen_title = [], 0
    for _ in range(40):
        frame = game.frame()
        seen_title += title_picture(frame)
        if not game.wide()["frame_2d"]:
            stretched.append(game.vblank)
        game.run(1)
    assert seen_title, "reached the title"
    assert not stretched, f"title frames shown wide (stretched) at vblanks {stretched}"


# The message window's frame at 4:3 (1280-wide GS output, 4x): x 64..1207.
WINDOW_4_3 = (64, 1207)


def window_extent(frame) -> list[tuple[int, int]]:
    """Runs of the window's orange frame across the bottom of the picture (x, 1280-wide)."""
    a = frame.array().astype(int)
    r, g, b = a[..., 0], a[..., 1], a[..., 2]
    orange = (r > 200) & (g > 80) & (g < 170) & (b < 90)
    xs = np.nonzero(orange[int(a.shape[0] * 0.70):].any(axis=0))[0] * (1280 / a.shape[1])
    runs: list[tuple[int, int]] = []
    for x in xs:
        if runs and x <= runs[-1][1] + 8:
            runs[-1] = (runs[-1][0], int(x))
        else:
            runs.append((int(x), int(x)))
    return runs


@pytest.mark.parametrize("hud", ["edges", "4:3"])
def test_town_message_window_keeps_its_shape(game_factory, options, test_data, hud):
    """Talking to someone driving around town (Dust, Cloud Hill): the message window is one HUD
    element, narrowed whole around the centre. With HUD at the screen edges its tiles and text
    were placed one by one by their own thirds of the screen, which tore it across the screen."""
    edits = {FIELDS["location"]["offset"]: bytes([CLOUD_HILL]), FIELDS["licence"]["offset"]: bytes([2])}
    game = game_factory(checkpoint="adventure_first_save", progress_edits=edits, settings=wide_settings(hud))
    continue_to_factory(game)
    for _ in range(4):  # Change parts / Race / Save data / Quit game / Drive around town
        game.press("down")
    game.press("cross")
    game.run(seconds(2))
    game.press("cross")  # "Come again!"
    game.run(seconds(12))
    finish_dialogue(game)
    town_map = TownMap.load(CLOUD_HILL, Path(options.base_data) / "disc", test_data / "maps")
    driver = TownDriver(game, town_map, door_quads(game, CLOUD_HILL), town=CLOUD_HILL, licence=2)
    driver.park_doors()
    assert driver.chase(2), "met someone"
    game.run(30)
    assert dialogue_open(game)
    runs = window_extent(game.frame())
    k = 0.75
    left, right = (640 + (x - 640) * k for x in WINDOW_4_3)
    assert len(runs) == 1, f"the window is in one piece: {runs}"
    assert abs(runs[0][0] - left) <= 6 and abs(runs[0][1] - right) <= 6, \
        f"the window spans {runs[0]}, expected {left:.0f}..{right:.0f} (4:3 shape, centred)"
