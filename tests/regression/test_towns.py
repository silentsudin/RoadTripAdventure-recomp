"""Every town's open world: load the town's Q's Factory, choose "Drive around town" and drive for
a while. Covers each map's streaming, rendering and sound."""

from __future__ import annotations

import pytest

from rtharness import seconds
from rtharness.adventure import FIELDS, GAME_MAP, continue_to_factory, scene

TOWNS = GAME_MAP["towns"]["names"]
SCENE_TOWN = 2


@pytest.mark.parametrize("town", range(1, len(TOWNS)), ids=[t.replace(" ", "_") for t in TOWNS[1:]])
def test_drive_around_town(game_factory, golden, golden_audio, town):
    edits = {FIELDS["location"]["offset"]: bytes([town]), FIELDS["licence"]["offset"]: bytes([2])}
    game = game_factory(checkpoint="adventure_first_save", progress_edits=edits)
    continue_to_factory(game)
    golden(f"factory_{TOWNS[town]}".replace(" ", "_"), game.frame())
    for _ in range(4):  # Change parts / Race / Save data / Quit game / Drive around town
        game.press("down")
    game.press("cross")
    game.run(seconds(2))
    game.press("cross")  # dismiss "Come again!"
    game.run(seconds(10))
    assert scene(game) == SCENE_TOWN, "driving in town"
    key = f"town_{TOWNS[town]}".replace(" ", "_")
    golden(key, game.frame())
    game.audio()
    game.pad("cross")  # drive off
    game.run(seconds(8))
    game.release()
    game.run(seconds(2))
    golden(key + "_driven", game.frame())
    sound = game.audio()
    assert sound["rms"] > 100, "the town has sound"
    golden_audio(key, sound)
