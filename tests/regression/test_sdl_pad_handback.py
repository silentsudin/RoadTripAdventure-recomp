"""Live mode: after a test-socket pad/press/step the socket holds the pads and `sdlpad` input is
ignored; release_to_sdl hands them back so scripts can drive the app's real-player input path."""

from rtharness import seconds
from rtharness.adventure import boot_to_main_menu
from rtharness.driver import CARS_BASE
import struct

LIVE = {"RT_TIME": "real", "RT_TEST_LIVE": "1", "RT_HEADLESS": "0", "RT_BACKGROUND_INPUT": "1", "RT_CONTROL_SCHEME": "modern", "RT_ANALOG_TRIGGERS": "1"}
CAR = CARS_BASE - 0x90  # the car object: speed +0x1D8


def speed(game) -> int:
    return struct.unpack("<I", game.read(CAR + 0x1D8, 4))[0]


def test_release_to_sdl_hands_input_back(game_factory):
    game = game_factory(env=LIVE)
    boot_to_main_menu(game)
    game.press("down")
    game.run(seconds(2))
    game.run(seconds(2))
    for _ in range(8):  # into a Quick Race (real time: presses are timed by the host clock)
        game.press("cross")
        game.run(seconds(4))
    game.run(seconds(4))  # the countdown
    # The test socket holds the pad (neutral): the SDL pad's gas is ignored.
    game.pad()
    game.sdl(rt=1.0, hold=2.5)
    assert speed(game) < 150, "pad() should keep overriding SDL input"
    # Handed back: the same SDL gas now drives (Modern: right trigger).
    game.release_to_sdl()
    game.sdl(rt=1.0, hold=3.0)
    assert speed(game) > 500, "sdlpad should drive once input is released to SDL"
