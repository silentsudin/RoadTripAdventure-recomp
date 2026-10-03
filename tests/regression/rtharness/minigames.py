"""Mini-games played by the bot (entered from a town's registration desk)."""

from __future__ import annotations

import math
from typing import TYPE_CHECKING

from .adventure import SCENE_RACING, SCENE_TOWN, scene
from .town import dialogue_open

if TYPE_CHECKING:
    from . import Game

SCENE_ROULETTE = SCENE_RACING  # the roulette is a driving scene: the car is the ball


def _blue(a, fx: float, fy: float) -> bool:
    h, w = a.shape[:2]
    r, _, b = (int(v) for v in a[int(h * fy), int(w * fx)][:3])
    return b > 110 and b > r + 60


def _bet_screen(game: "Game") -> tuple[bool, bool]:
    """(bet window up, message over it): the roulette's "How much do you wish to bet?" window
    and a message on top of it ("Please make a bet." after a bet of 0), both blue."""
    a = game.frame().array()
    return _blue(a, 0.25, 0.40), _blue(a, 0.40, 0.60)


def play_roulette(game: "Game", rounds_seconds: int = 25) -> None:
    """One round of Sandpolis's Roulette, then back to the desk: bet 10 on the square the chip
    starts on (BLACK), start, and drive the car (it is the ball) until it lands in a pocket.
    Ends when the game is back in town."""
    for _ in range(30):  # Cross on the table opens the bet window
        window, message = _bet_screen(game)
        if window and not message:
            break
        game.press("cross")  # (also clears "Please make a bet.")
        game.run(30)
    else:
        raise AssertionError("the roulette's bet window never opened")
    game.press("up")     # the cursor starts on the tens: bet 10
    game.run(20)
    game.press("cross")
    game.run(30)
    for _ in range(3):   # "Start?" Yes
        game.press("cross")
        game.run(30)
    for _ in range(rounds_seconds):  # accelerate: the ball rolls round into a pocket
        if scene(game) != SCENE_ROULETTE:
            break
        game.step(60, ["cross"], 128)
    game.release()
    for _ in range(30):  # the result, then Triangle leaves the table ("Quit?" Yes)
        if scene(game) != SCENE_ROULETTE:
            return
        game.press("triangle")
        game.run(40)
        game.press("cross")
        game.run(40)
    raise AssertionError("still at the roulette table")


def drive_minigame(game: "Game", timeout_seconds: int = 300) -> None:
    """Plays a driving mini-game the plain way: accelerate, weaving the steering, until the game
    is back in town (results and messages are pressed through). Enough to take part and finish
    (e.g. Curling's three slides); not to score well."""
    for t in range(timeout_seconds):
        if scene(game) == SCENE_TOWN and not dialogue_open(game):
            return
        if dialogue_open(game):
            game.press("cross")
            game.run(52)
        else:
            game.step(60, ["cross"], 128 + int(100 * math.sin(t / 3)))
    game.release()
    raise AssertionError("the mini-game did not finish")
