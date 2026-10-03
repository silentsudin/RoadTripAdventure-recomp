"""Notebook stamps: each test plays the task that earns a stamp and checks the game recorded it
(the stamp's bit in the earned set) and the notebook shows it."""

from __future__ import annotations

from pathlib import Path

import pytest

from rtharness import seconds
from rtharness.adventure import FIELDS, continue_to_factory
from rtharness.town import TownDriver, TownMap, finish_dialogue, stamps_earned

CLOUD_HILL = 8


@pytest.fixture
def town(game_factory, options, test_data):
    """town(n, **edits) -> (game, TownDriver): a game driving around town n."""

    def start(number: int, edits: dict[int, bytes] | None = None, licence: int = 2):
        progress = {FIELDS["location"]["offset"]: bytes([number]), FIELDS["licence"]["offset"]: bytes([licence])}
        progress.update(edits or {})
        game = game_factory(checkpoint="adventure_first_save", progress_edits=progress)
        continue_to_factory(game)
        for _ in range(5 if licence == 3 else 4):  # ... / Quit game / Drive around town
            game.press("down")
        game.press("cross")
        game.run(seconds(2))
        game.press("cross")  # "Come again!"
        game.run(seconds(12))
        town_map = TownMap.load(number, Path(options.base_data) / "disc", test_data / "maps")
        return game, TownDriver(game, town_map)

    return start


def test_stamp_86_angels_wings(town):
    """Dust, driving around Cloud Hill, gives a pure heart his Angel's Wings."""
    game, driver = town(CLOUD_HILL)
    assert driver.chase(2, "Dust"), "met Dust"
    finish_dialogue(game)
    assert 86 in stamps_earned(game)
