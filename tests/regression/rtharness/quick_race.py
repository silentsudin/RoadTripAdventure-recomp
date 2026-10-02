"""Quick Race flows: pick a course from the carousel, race it with the driving bot."""

from __future__ import annotations

from typing import TYPE_CHECKING

from . import seconds
from .adventure import GAME_MAP, boot_to_main_menu
from .driver import CAR_COUNT, CAR_STRIDE, CARS_BASE, Driver, DriverConfig

if TYPE_CHECKING:
    from . import Game

COURSES = GAME_MAP["quick_race"]["courses"]


def open_course(game: "Game", index: int):
    """Boot -> main menu -> Quick Race -> the course carousel's `index`th entry -> its card (track
    length, laps, entries and this course's records)."""
    boot_to_main_menu(game)
    game.press("down")
    game.run(seconds(2))
    game.press("cross")
    game.run(seconds(3))
    for _ in range(index):
        game.press("right")
        game.run(seconds(1.5))
    game.press("cross")
    game.run(seconds(3))


def start_race(game: "Game"):
    """From the course card: race (default car)."""
    game.press("cross")
    game.run(seconds(3))


def drive_quick_race(game: "Game", timeout_seconds: float = 1500, config: DriverConfig | None = None,
                     at: dict | None = None) -> int:
    """Lets the driving bot race until the game is back at the course carousel; returns our place.
    The card has no flag we have mapped, so the end is recognised by the field standing still
    (in a race the AI cars never stop once they have started)."""
    driver = Driver(game, config or DriverConfig())
    start = game.vblank
    pending = sorted((start + seconds(t), fn) for t, fn in (at or {}).items())
    place, moved, last = None, False, None
    while game.vblank < start + seconds(timeout_seconds):
        step = seconds(0.5)
        if pending:
            step = max(1, min(step, pending[0][0] - game.vblank))
        driver.drive(step)
        while pending and game.vblank >= pending[0][0]:
            pending.pop(0)[1]()
        cars = game.read(CARS_BASE, CAR_COUNT * CAR_STRIDE)
        if last is not None and cars == last:
            if moved:
                game.release()
                print(f"[race] finished {place}th, {driver.recoveries} recoveries")
                return place
        elif last is not None:
            moved = True
        last = cars
        if moved:
            place = game.u32(GAME_MAP["race"]["place_address"]) + 1
    raise AssertionError(f"the race did not finish within {timeout_seconds} s (place {place}, "
                         f"{driver.recoveries} recoveries)")
