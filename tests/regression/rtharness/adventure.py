"""Adventure-mode flows built on Game: Continue to Q's Factory, enter a race, drive it."""

from __future__ import annotations

import os
import sys
from typing import TYPE_CHECKING

from . import REPO, seconds
from .driver import Driver, DriverConfig

if TYPE_CHECKING:
    from . import Game

sys.path.insert(0, str(REPO / "tools"))
import save_parser  # noqa: E402

GAME_MAP = save_parser.load_map()
PROGRESS = GAME_MAP["progress"]["ram_address"]
FIELDS = GAME_MAP["progress"]["fields"]
SCENE_RACING = 10
SCENE_RACE_OVER = 3


def scene(game: "Game") -> int:
    return game.u32(PROGRESS + FIELDS["scene"]["offset"])


def boot_to_main_menu(game: "Game"):
    """Boot -> title -> Start -> main menu (Adventure / Quick Race / 2 Player / Options / Results)."""
    game.run(seconds(10))
    game.press("start")
    game.run(seconds(3))


def continue_to_factory(game: "Game"):
    """Main menu -> Adventure -> Continue -> slot 1 -> load, ending at Q's Factory's menu."""
    boot_to_main_menu(game)
    game.press("cross")       # Adventure
    game.run(seconds(2))
    game.press("down")        # Continue
    game.press("cross")
    game.run(seconds(3))
    game.press("cross")       # slot 1
    game.run(seconds(3))
    game.press("cross")       # load this saved data: Yes
    game.run(seconds(12))
    game.press("cross")       # dismiss the greeting
    game.run(seconds(2))


def enter_race(game: "Game", menu_index: int):
    """Q's Factory menu -> Race -> the race at `menu_index`, through the countdown."""
    game.press("down")        # Race
    game.press("cross")
    game.run(seconds(3))
    for _ in range(menu_index):
        game.press("down")
    game.press("cross")
    game.run(seconds(10))
    for _ in range(4):        # Adventure races wait for a press before the countdown
        game.press("cross")
        game.run(seconds(2))


class Retired(AssertionError):
    """The bot was stuck and retired from the race through the pause menu."""


def retire(game: "Game"):
    """Pause -> Retire."""
    game.release()
    game.press("start")
    game.run(seconds(1))
    game.press("down")
    game.press("cross")
    game.run(seconds(5))


def drive_race(game: "Game", timeout_seconds: float = 1500, config: DriverConfig | None = None,
               at: dict | None = None) -> int:
    """Lets the driving bot race until the game reports the race over; returns our place.
    `at` maps seconds into the race to callbacks (e.g. frame checks) run at that moment."""
    driver = Driver(game, config or DriverConfig())
    start = game.vblank
    end = start + seconds(timeout_seconds)
    pending = sorted((start + seconds(t), fn) for t, fn in (at or {}).items())
    place = None
    progress_at, progress_index = game.vblank, -1
    while scene(game) != SCENE_RACING:
        if game.vblank - start > seconds(60):
            raise AssertionError(f"the race did not start (scene {scene(game)})")
        game.run(seconds(1))
    while game.vblank < end:
        step = seconds(2)
        if pending:
            step = max(1, min(step, pending[0][0] - game.vblank))
        driver.drive(step)
        if os.environ.get("RT_BOT_DEBUG") and (game.vblank - start) % 300 < step:
            print(game.vblank, 'me', tuple(round(v) for v in driver.positions()[0]), 'trail', len(driver.trail),
                  'idx', driver.index, 'rec', driver.recoveries, flush=True)
        while pending and game.vblank >= pending[0][0]:
            pending.pop(0)[1]()
        # No progress along the line for 90 s: the bot is stuck for good.
        if driver.index != progress_index:
            progress_at, progress_index = game.vblank, driver.index
        elif driver.loop_closed and game.vblank - progress_at > seconds(90):
            retire(game)
            raise Retired(f"the bot was stuck at {driver.positions()[0]} (line point {driver.index}/"
                          f"{len(driver.trail)}, {driver.recoveries} recoveries) and retired")
        if scene(game) == SCENE_RACE_OVER:
            print(f"[race] finished {place}th, {driver.recoveries} recoveries, {driver.nudges} nudges")
            return place
        place = game.u32(GAME_MAP["race"]["place_address"]) + 1
    raise AssertionError(f"the race did not finish within {timeout_seconds} s (place {place}, "
                         f"{driver.recoveries} recoveries, {driver.nudges} nudges)")
