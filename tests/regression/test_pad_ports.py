"""Two pads: each port gets its own input, a pad can be unplugged and plugged back in, the game's
vibration reaches the host, and movies record and replay both ports."""

from __future__ import annotations

import math
import struct

from rtharness import seconds
from rtharness.adventure import boot_to_main_menu
from rtharness.driver import CAR_STRIDE, CARS_BASE
from rtharness.movie import Movie
from rtharness import quick_race

TWO_PLAYER = 2  # main menu index
DESERT_RACEWAY = 1  # 2 Player > Race Right-Away carousel index
ALIGN = "0001ffffffff"  # the game's actuator table: byte 0 = small motor, byte 1 = large motor


def car(game, slot: int) -> tuple[float, float]:
    x, _, z = struct.unpack("<3f", game.read(CARS_BASE + slot * CAR_STRIDE, 12))
    return x, z


def start_two_player_race(game):
    """Main menu -> 2 Player -> Race Right-Away -> Desert Raceway, to the start (P1 = car 0, P2 = car 1)."""
    boot_to_main_menu(game)
    for _ in range(TWO_PLAYER):
        game.press("down")
    game.press("cross")
    game.run(seconds(3))
    game.press("cross")
    game.run(seconds(3))
    for _ in range(DESERT_RACEWAY):
        game.press("right")
        game.run(seconds(1.5))
    game.press("cross")
    game.run(seconds(3))
    game.press("cross")
    game.run(seconds(6))
    game.release()


def test_ports_are_independent(game_factory):
    """In a 2-player race, accelerating on pad 2 moves only player 2's car, and then pad 1 only
    player 1's (before per-port input, pad 2 mirrored pad 1)."""
    game = game_factory()
    start_two_player_race(game)
    p1, p2 = car(game, 0), car(game, 1)
    game.pad("cross", port=1)
    game.run(seconds(8))
    assert math.dist(car(game, 1), p2) > 50, "player 2 drives on pad 2's input"
    assert math.dist(car(game, 0), p1) < 2, "player 1 does not move without input on pad 1"
    game.release(port=1)
    p1 = car(game, 0)
    game.pad("cross", port=0)
    game.run(seconds(8))
    assert math.dist(car(game, 0), p1) > 50, "player 1 drives on pad 1's input"


def test_unplug_and_replug(game_factory):
    """Unplugging pad 2 mid-race: the game sees no pad there and races on; plugged back in, it
    finds the pad again and sets up its DualShock mode and motors as at the start."""
    game = game_factory()
    start_two_player_race(game)
    assert [p["align"] for p in game.pad_info()] == [ALIGN, ALIGN]
    game.pad(port=1, connected=False)
    game.run(seconds(2))
    port2 = game.pad_info()[1]
    assert not port2["connected"] and not port2["analog"], port2
    before = game.frame()
    game.pad("cross", port=0)
    game.run(seconds(2))
    assert game.frame().rgba != before.rgba, "the race goes on"
    game.pad(port=1, connected=True)
    game.run(seconds(2))
    port2 = game.pad_info()[1]
    assert port2["connected"] and port2["analog"] and port2["align"] == ALIGN, port2


def test_rumble_when_crashing(game_factory):
    """Driving into the walls of a Quick Race makes pad 1 vibrate (the game drives both motors
    through its actuator table); pad 2's motors stay still."""
    game = game_factory()
    quick_race.open_course(game, 0)
    quick_race.start_race(game)
    game.run(seconds(8))
    assert [p["align"] for p in game.pad_info()] == [ALIGN, ALIGN]
    for k in range(30):  # full lock one way, then the other: into the walls
        game.step(60, ["cross"], 0 if (k // 5) % 2 == 0 else 255)
    pads = game.actuators()
    assert pads[0]["changes"] > 10, pads
    assert pads[1]["changes"] == 0, pads


def test_movie_records_both_ports(game_factory, test_data):
    """A recorded movie (version 2) keeps each pad's input; replaying it puts both cars exactly
    where they were."""
    movie = test_data / "runs" / "pad_ports_movie.txt"
    movie.parent.mkdir(parents=True, exist_ok=True)
    game = game_factory(name="pad_ports_record", env={"RT_MOVIE_RECORD": str(movie)})
    start_two_player_race(game)
    game.pad("cross", port=1)
    game.run(seconds(4))
    game.pad("cross", lx=0, port=0)
    game.run(seconds(4))
    end = game.vblank
    expected = car(game, 0), car(game, 1)
    game.close()
    recorded = Movie.load(movie)
    assert recorded.version == 2 and recorded.ports == {0, 1}

    replay = game_factory(name="pad_ports_replay", env={"RT_MOVIE_PLAY": str(movie)})
    replay.run(end - replay.vblank)
    assert (car(replay, 0), car(replay, 1)) == expected
