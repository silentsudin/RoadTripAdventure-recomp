"""Notebook stamps: each test plays the task that earns a stamp and checks the game recorded it
(the stamp's bit in the earned set) and the notebook shows it."""

from __future__ import annotations

from pathlib import Path

import pytest

from rtharness import seconds
from rtharness.adventure import FIELDS, GAME_MAP, PROGRESS, continue_to_factory
from rtharness.minigames import drive_minigame, play_roulette
from rtharness.town import (TownDriver, TownMap, car_pose, door_quads, finish_dialogue, park_traffic,
                            speaker, stamps_earned)

SANDPOLIS = 3
WHITE_MOUNTAIN = 6
CLOUD_HILL = 8
TOWNS = GAME_MAP["towns"]["names"]


@pytest.fixture
def town(game_factory, options, test_data, request):
    """town(n, **edits) -> (game, TownDriver): a game driving around town n."""

    def start(number: int, edits: dict[int, bytes] | None = None, licence: int = 2, quiet: bool = False,
              no_doors: bool = False, name: str | None = None):
        """quiet: no street traffic and every door parked (for tests that warp into buildings,
        where NPC cars and stray doors only get in the way)."""
        progress = {FIELDS["location"]["offset"]: bytes([number]), FIELDS["licence"]["offset"]: bytes([licence])}
        progress.update(edits or {})
        game = game_factory(checkpoint="adventure_first_save", progress_edits=progress,
                            name=f"{request.node.name}_{name}" if name else None)
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
        driver = TownDriver(game, town_map, door_quads(game, number), town=number, licence=licence)
        if quiet or no_doors:
            driver.park_doors()
        return game, driver

    return start


def test_stamp_86_angels_wings(town):
    """Dust, driving around Cloud Hill, gives a pure heart his Angel's Wings."""
    game, driver = town(CLOUD_HILL, no_doors=True)  # keep the chase out of the shops
    assert driver.chase(2, "Dust"), "met Dust"
    finish_dialogue(game)
    assert 86 in stamps_earned(game)


HOUSES = GAME_MAP["houses"]


# Towns whose run cannot pass from this checkpoint. Kept as expected failures so the suite reports
# when they start passing.
HOUSES_NOT_YET = {"My City": "My City's houses come with the story"}
# Doors whose resident is never in at this checkpoint: the building turns you away (you are put
# back at its door and nothing is recorded) at every hour of a game day. Every other door of the
# town is still tested; only the town's stamp, which needs these too, is an expected failure.
NOBODY_HOME = {
    # Shirley lives here but drives around White Mountain (met there by the bot).
    "Papaya Island": {10: "Shirley"},
    # Not among any town's drivers; why he is out is not known yet (to check against PCSX2).
    "White Mountain": {17: "Bigfoot Joe"},
}
# Residents who are in at some hours and not others (found by trying every hour). Only these doors
# are tried again, an hour of game time later, for up to a day; any other door that turns you away
# fails at once, so a building the recomp breaks some of the time is not hidden by retries.
KEEPS_HOURS = {"White Mountain": {10: "Santa Claus"}}
RESIDENT_RETRY = int(60 * GAME_MAP["clock"]["vblanks_per_minute"])


@pytest.mark.parametrize("number", [
    pytest.param(n, marks=pytest.mark.xfail(reason=HOUSES_NOT_YET[TOWNS[n]], strict=False)
                 if TOWNS[n] in HOUSES_NOT_YET else [])
    for n in range(1, len(TOWNS))], ids=[t.replace(" ", "_") for t in TOWNS[1:]])
def test_visited_all_houses(town, number):
    """"Visited all the houses in <town>": go through every door the town counts, one by one;
    each visit must be recorded (its bit in the town's unvisited-doors mask clears), then the
    town's stamp is awarded.

    Shortcuts (COVERAGE.md): buildings are entered by warping their door to the car; the visits
    run for real. Some buildings are destinations reached through something else (Fuji City's
    maze guard sits at the end of the underwater Treasure Hunting Maze), and leaving one puts the
    car where that route leads. So when a visit fails or leaves the car stranded, the test goes on
    in a fresh game that starts with the visits made so far (carried in the unvisited-doors mask,
    as a save would). A door fails the test only if it also fails as a fresh game's first visit."""
    mask_offset = HOUSES["unvisited_doors"] + 4 * number
    mask_at = PROGRESS + mask_offset
    carried = None
    for round_ in range(40):
        edits = {} if carried is None else {mask_offset: carried.to_bytes(4, "little")}
        game, driver = town(number, quiet=True, edits=edits, name=f"game{round_}")
        stamp = game.u8(HOUSES["stamp_table"] + number)
        spawn = car_pose(game)[:2]
        away = NOBODY_HOME.get(TOWNS[number], {})
        remaining = [d for d in range(32) if game.u32(mask_at) >> d & 1 and d not in away]
        assert remaining or carried is not None, "the save has doors still to visit"
        hours = KEEPS_HOURS.get(TOWNS[number], {})
        for n, door in enumerate(remaining):
            for attempt in range(24 if door in hours else 1):
                driver.drive_to(spawn, radius=12, timeout_vblanks=seconds(60))
                driver.warp_into(door)
                driver.leave_building()
                driver.settle()
                visited = not game.u32(mask_at) >> door & 1
                if visited or driver.stranded():
                    break
                if door in hours:
                    game.run(RESIDENT_RETRY)  # not in yet: come back in an hour
            if not visited:
                # Some buildings have nobody in them: the game records the visit all the same.
                assert n > 0, f"the visit to door {door} was recorded (in a fresh game)"
                break      # most likely the previous building left us somewhere: start afresh
            if driver.stranded():
                break      # a destination building: go on in a fresh game
        carried = game.u32(mask_at)
        if carried & ~sum(1 << d for d in away) == 0:
            if carried:
                pytest.xfail(f"every other door visited; nobody home at door(s) "
                             f"{', '.join(f'{d} ({who})' for d, who in away.items())}")
            assert stamp in stamps_earned(game), f"stamp {stamp} for visiting every house"
            return
        game.close()
    raise AssertionError(f"doors left unvisited: mask {carried:#x}")


def test_shop_open_at_night(town):
    """At 21:00 the town is dark and a shop still serves you (Peach Town's Parts Shop)."""
    game, driver = town(1, quiet=True)
    driver.set_clock(21)
    game.run(seconds(2))
    assert driver.warp_into(1) == "Parts Shop"
    driver.leave_building()


# Stamps given by talking to someone at home and taking what they offer: (stamp, town, door,
# resident). Found by surveying every door of every town (accepting each first offer). Stamps a
# warp hands out for merely reaching a place (Grandpa Tal's Barrel Dodging, the maze guard's
# Treasure Hunting Maze) are not here: those need their real task.
TALK_STAMPS = [
    (2, 1, 7, "Kinsera"),            # Got a local Peach Wine!
    (11, 2, 7, "Princess Nanaha"),   # Got a Gold Ornament!
    (17, 2, 10, "Otomi"),            # Saved Otomi of Dumpling Cake Shop!
    (18, 2, 14, "Iwasuke"),          # Met Iwasuke!
    (59, 4, 3, "Gene"),              # Came up with a new greeting! (a text entry)
    (73, 7, 7, "Luke"),              # Got an Unbabo Doll!
    (83, 7, 11, "Casa"),             # Woke sleepy Casa up!
]


@pytest.mark.parametrize("stamp,number,door,who", TALK_STAMPS,
                         ids=[f"{s}_{w.replace(' ', '_')}" for s, _, _, w in TALK_STAMPS])
def test_talk_stamp(town, stamp, number, door, who):
    """Visit the resident, say yes to their offer: the stamp is earned (shown in the notebook)."""
    game, driver = town(number, quiet=True)
    assert stamp not in stamps_earned(game)
    assert driver.warp_into(door) == who
    driver.leave_building(accept=True)
    driver.settle()
    assert stamp in stamps_earned(game)


def test_stamp_29_played_roulette(town):
    """Sandpolis's Roulette: register, bet, roll the car-ball into a pocket, leave the table."""
    game, driver = town(SANDPOLIS, quiet=True)
    assert driver.warp_into(8) == "Roulette Registration"
    finish_dialogue(game)  # Cross: "Play!"
    play_roulette(game)
    driver.leave_building()
    driver.settle()
    assert 29 in stamps_earned(game)


# Driving mini-games finished by plain driving: (stamp, town, door, desk).
DRIVE_STAMPS = [
    (69, WHITE_MOUNTAIN, 18, "Curling Registration"),  # Played Curling!
]


@pytest.mark.parametrize("stamp,number,door,desk", DRIVE_STAMPS,
                         ids=[f"{s}_{d.split()[0]}" for s, _, _, d in DRIVE_STAMPS])
def test_minigame_stamp(town, stamp, number, door, desk):
    """Register at the desk, play the mini-game through, back in town: the stamp is earned."""
    game, driver = town(number, quiet=True)
    assert driver.warp_into(door) == desk
    finish_dialogue(game)  # Cross: take part
    drive_minigame(game)
    driver.settle()
    assert stamp in stamps_earned(game)
