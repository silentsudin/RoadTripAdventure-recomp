"""Driving controls and feel (src/game/Driving.cpp): control schemes written into the game's own
button setup, analogue gas and brake, Modern's reverse on the brake, and dynamic vibration.

Lockstep runs are deterministic: without RT_CONTROL_SCHEME / RT_ANALOG_TRIGGERS the game keeps its
own layout and reads buttons only. The analogue values here come from the test socket
({"cmd":"driving","analog":1,"gas":..}), standing in for a trigger."""

from __future__ import annotations

import struct

from rtharness import seconds
from rtharness.driver import CARS_BASE
from rtharness.quick_race import open_course, start_race

PEACH_RACEWAY = 0
CAR = CARS_BASE - 0x90  # the car object (0x21A820's a2): speed +0x1D8, brake ramp +0x1FE, gear +0x1FF
MODERN = {"RT_CONTROL_SCHEME": "modern", "RT_ANALOG_TRIGGERS": "1"}


def speed(game) -> int:
    return struct.unpack("<I", game.read(CAR + 0x1D8, 4))[0]


def driving(game, **args) -> dict:
    return game._call("driving", **args)


def to_start(game):
    open_course(game, PEACH_RACEWAY)
    start_race(game)
    game.run(seconds(4))  # the countdown


def stop(game):
    """Brakes to a standstill and lets go (before Modern's reverse starts)."""
    for _ in range(100):
        if speed(game) < 150:
            break
        game.step(6, ["l2"])
    game.step(10, [])


def accelerate(game, seconds_: float, buttons=(), gas: float | None = None) -> int:
    """Holds `buttons` (and an analogue gas) from a standstill; returns the speed reached."""
    if gas is not None:
        driving(game, analog=1, gas=gas, brake=0)
    game.step(seconds(seconds_), list(buttons))
    if gas is not None:
        driving(game, analog=0)
    return speed(game)


def test_driving_defaults_are_the_games(game_factory):
    """No scheme and no analogue under lockstep: the game's own layout, Cross accelerates."""
    game = game_factory()
    to_start(game)
    d = driving(game)
    assert d["scheme"] == -1 and not d["analog_active"]
    assert d["ports"][0]["actions"]["gas"] == "cross" and d["ports"][0]["actions"]["jet"] == "r2"
    assert accelerate(game, 3, ["cross"]) > 5000
    d = driving(game)
    assert d["ports"][0]["actions"]["gas"] == "cross"  # untouched while driving
    assert d["ports"][0]["driving"]


def test_modern_scheme_analogue_and_reverse(game_factory):
    game = game_factory(env=MODERN)
    to_start(game)
    game.step(2, [])
    d = driving(game)
    a = d["ports"][0]["actions"]
    # Modern, written into the game's setup while driving.
    assert (a["gas"], a["brake"], a["reverse"], a["jet"]) == ("r2", "l2", "square", "cross"), a
    assert (a["wing_up"], a["wing_down"], a["horn"], a["view"], a["navigator"]) == ("r1", "l1", "circle", "triangle", "select")
    # Analogue gas: a partly pressed trigger accelerates less than a full one, more than none.
    half = accelerate(game, 2.5, gas=0.4)
    assert half > 500
    # Brake to a stop, holding the ramp short (40% -> at most 13 of 32 frames).
    driving(game, analog=1, gas=0, brake=0.4)
    ramps = [game.step(1, [], reads=[(CAR + 0x1FE, 1)])[0][0] for _ in range(40)]
    driving(game, analog=0)
    assert 0 < max(ramps) <= 13, ramps
    stop(game)
    # Full analogue gas = the button.
    full = accelerate(game, 2.5, gas=1.0)
    stop(game)
    assert speed(game) < 150
    button = accelerate(game, 2.5, ["r2"])
    assert full > half * 1.3, (full, half)
    assert abs(full - button) < button * 0.15, (full, button)
    # Modern: holding the brake at a standstill reverses.
    stop(game)
    game.step(seconds(2.5), ["l2"])
    d = driving(game)
    assert d["ports"][0]["reversing"]
    assert speed(game) > 500
    game.step(seconds(1), [])
    assert not driving(game)["ports"][0]["reversing"]


def test_dynamic_vibration(game_factory):
    """Dynamic vibration follows the car: quiet on the grid, engine and road at speed, graded."""
    game = game_factory()
    to_start(game)
    still = driving(game)["ports"][0]["rumble"]
    assert max(still) < 0.05, still
    game.step(seconds(3), ["cross"])
    d = driving(game)["ports"][0]
    low, high, left, right = d["rumble"]
    assert d["driving"]
    assert 0 < high < 0.5 and right > 0.05 and low < 0.5, d["rumble"]  # graded, not the game's on/off
