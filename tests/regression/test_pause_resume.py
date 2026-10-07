"""Pausing (the app's in-game menu, Android's background) and resuming in real time.

The rest of the suite runs in guest time, where a pause costs nothing. In real time the guest's
vblanks follow the host clock: a pause used to leave every vblank it held back due at once, and
the game ran fast afterwards until it had caught up (seconds after a long pause, and for good
after many: the sound then sat at the output queue's cap, dropping, and crackled). Here the game
runs live (RT_TIME=real, RT_TEST_LIVE=1) and the socket's `pause` holds it as the menu does.
"""

import time

from rtharness import seconds
from rtharness.adventure import boot_to_main_menu

LIVE = {"RT_TIME": "real", "RT_TEST_LIVE": "1"}
RATE = 48000  # SPU2 output frames per second


def vblank_now(game) -> int:
    return game.run(1)  # (live: sleeps one vblank, returns the game's vblank)


def test_pause_resume_real_time(game_factory):
    game = game_factory(env=LIVE)
    # Into a Quick Race (real time: the presses are timed by the host clock).
    boot_to_main_menu(game)
    game.press("down")
    game.run(seconds(2))
    for _ in range(8):
        game.press("cross")
        game.run(seconds(4))
    game.pad("cross")  # accelerate
    game.run(seconds(5))
    game.audio()
    game.run(seconds(3))
    before = game.audio()
    assert before["rms"] > 500, f"the race should be audible before pausing: {before}"

    for cycle in range(20):
        game._call("pause", on=1)
        time.sleep(0.3)  # parks at the next vblank
        held = vblank_now(game)
        time.sleep(1.0 if cycle % 5 else 3.0)
        assert vblank_now(game) - held <= 1, "the game should stand still while paused"
        game._call("pause", on=0)
        start, t0 = vblank_now(game), time.monotonic()
        time.sleep(1.0)
        ran, took = vblank_now(game) - start, time.monotonic() - t0
        rate = ran / took
        assert rate < 75, f"cycle {cycle}: {rate:.0f} vblanks/s after resuming (catching up the pause)"
        assert rate > 30, f"cycle {cycle}: {rate:.0f} vblanks/s after resuming (stalled)"

    game.audio()
    t0 = time.monotonic()
    game.run(seconds(3))
    after = game.audio()
    per_second = after["frames"] / (time.monotonic() - t0)
    assert 0.8 * RATE < per_second < 1.2 * RATE, f"sound should come at the real-time rate: {per_second:.0f}/s"
    assert after["rms"] > 500, f"the race should still be audible after 20 pauses: {after}"
    assert after["rms"] < 4 * before["rms"], f"nothing should be blaring after the pauses: {before} -> {after}"
