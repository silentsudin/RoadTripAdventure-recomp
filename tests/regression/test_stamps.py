"""Notebook stamps: each test plays the task that earns a stamp and checks the game recorded it
(the stamp's bit in the earned set) and the notebook shows it."""

from __future__ import annotations

from pathlib import Path

import pytest

from rtharness import seconds
from rtharness.adventure import FIELDS, GAME_MAP, PROGRESS, continue_to_factory
from rtharness.town import (TownDriver, TownMap, car_pose, door_quads, finish_dialogue, park_traffic,
                            speaker, stamps_earned)

CLOUD_HILL = 8
TOWNS = GAME_MAP["towns"]["names"]


@pytest.fixture
def town(game_factory, options, test_data):
    """town(n, **edits) -> (game, TownDriver): a game driving around town n."""

    def start(number: int, edits: dict[int, bytes] | None = None, licence: int = 2, quiet: bool = False,
              no_doors: bool = False):
        """quiet: no street traffic and every door parked (for tests that warp into buildings,
        where NPC cars and stray doors only get in the way)."""
        progress = {FIELDS["location"]["offset"]: bytes([number]), FIELDS["licence"]["offset"]: bytes([licence])}
        progress.update(edits or {})
        game = game_factory(checkpoint="adventure_first_save", progress_edits=progress)
        continue_to_factory(game)
        if quiet:
            park_traffic(game, number)
        for _ in range(5 if licence == 3 else 4):  # ... / Quit game / Drive around town
            game.press("down")
        game.press("cross")
        game.run(seconds(2))
        game.press("cross")  # "Come again!"
        game.run(seconds(12))
        finish_dialogue(game)  # some towns greet you on the way out
        town_map = TownMap.load(number, Path(options.base_data) / "disc", test_data / "maps")
        driver = TownDriver(game, town_map, door_quads(game, number), town=number)
        if quiet or no_doors:
            driver.park_doors()
        return game, driver

    return start


@pytest.mark.xfail(reason="the Dust chase regressed with the door/dialogue rework; being fixed", strict=False)
def test_stamp_86_angels_wings(town):
    """Dust, driving around Cloud Hill, gives a pure heart his Angel's Wings."""
    game, driver = town(CLOUD_HILL, no_doors=True)  # keep the chase out of the shops
    assert driver.chase(2, "Dust"), "met Dust"
    finish_dialogue(game)
    assert 86 in stamps_earned(game)


HOUSES = GAME_MAP["houses"]


# Towns whose run does not pass yet (the bot gets stuck leaving or entering particular
# buildings). Kept as expected failures so the suite reports when they start passing.
HOUSES_NOT_YET = {"Sandpolis", "Chestnut Canyon", "White Mountain", "Papaya Island", "Cloud Hill",
                  "My City"}


@pytest.mark.parametrize("number", [
    pytest.param(n, marks=pytest.mark.xfail(reason="town bot cannot finish this town's buildings yet",
                                             strict=False) if TOWNS[n] in HOUSES_NOT_YET else [])
    for n in range(1, len(TOWNS))], ids=[t.replace(" ", "_") for t in TOWNS[1:]])
def test_visited_all_houses(town, number):
    """"Visited all the houses in <town>": go through every door the town counts, one by one;
    each visit must be recorded (its bit in the town's unvisited-doors mask clears), then the
    town's stamp. Shortcut: buildings are entered by warping their door to the car
    (TownDriver.warp_into); the visits themselves run for real."""
    game, driver = town(number, quiet=True)
    mask_at = PROGRESS + HOUSES["unvisited_doors"] + 4 * number
    stamp = game.u8(HOUSES["stamp_table"] + number)
    mask = game.u32(mask_at)
    assert mask, "the save has doors still to visit"
    spawn = car_pose(game)[:2]  # outside Q's Factory: open, driveable ground in every town
    for door in range(32):
        if not mask >> door & 1:
            continue
        who = ""
        for _ in range(2):
            # From known ground (leaving a building can leave us in an awkward corner).
            driver.drive_to(spawn, radius=12, timeout_vblanks=seconds(60))
            who = driver.warp_into(door)
            if who:
                break
        assert who, f"door {door} opened"
        driver.leave_building()
        assert not game.u32(mask_at) >> door & 1, f"the visit to door {door} ({who}) was recorded"
    assert game.u32(mask_at) == 0
    assert stamp in stamps_earned(game), f"stamp {stamp} for visiting every house"
