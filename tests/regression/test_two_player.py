"""2 Player (split screen): every event of Race Right-Away, including the 2P-only games (Highway,
Tunnel, Sliding Door, Obstacle Course, Soccer). Player 1 drives flat out; player 2 has no pad
input. Checks each event's card, its start and 20 s of play (pictures and sound)."""

from __future__ import annotations

import pytest

from rtharness import seconds
from rtharness.adventure import GAME_MAP, boot_to_main_menu

EVENTS = GAME_MAP["two_player"]["events"]
TWO_PLAYER = 2  # main menu index


def open_two_player(game):
    boot_to_main_menu(game)
    for _ in range(TWO_PLAYER):
        game.press("down")
    game.press("cross")
    game.run(seconds(3))


@pytest.mark.parametrize("index", range(len(EVENTS)), ids=[e.replace(" ", "_") for e in EVENTS])
def test_two_player_event(game_factory, golden, golden_audio, index):
    game = game_factory()
    open_two_player(game)
    game.press("cross")  # Race Right-Away
    game.run(seconds(3))
    for _ in range(index):
        game.press("right")
        game.run(seconds(1.5))
    game.press("cross")
    game.run(seconds(3))
    key = "2p_" + EVENTS[index].replace(" ", "_")
    golden(key + "_card", game.frame())
    game.press("cross")
    game.run(seconds(6))
    golden(key + "_start", game.frame())
    game.pad("cross")
    game.audio()
    game.run(seconds(20))
    sound = game.audio()
    assert sound["rms"] > 300, "the event is audible"
    golden_audio(key, sound)
    before = game.frame()
    game.run(seconds(1))
    after = game.frame()
    assert before.rgba != after.rgba, "the picture moves"
    golden(key + "_20s", after)


@pytest.mark.parametrize("item,name", [(1, "random"), (2, "custom")], ids=["Random_Race", "Custom_Race"])
def test_two_player_saved_cars(game_factory, golden, item, name):
    """Random Race and Custom Race race the two players' saved cars (memory cards 1 and 2):
    the card prompt, both saves loaded, the carousel (with Trade Items, Change Parts, Save),
    the first event's card and its start."""
    game = game_factory(checkpoint="adventure_first_save", card2="adventure_first_save")
    open_two_player(game)
    for _ in range(item):
        game.press("down")
    for step in ("prompt", "loaded", "carousel", "card"):
        game.press("cross")
        game.run(seconds(4))
        golden(f"2p_{name}_{step}", game.frame())
    game.press("cross")
    game.run(seconds(6))
    golden(f"2p_{name}_start", game.frame())
    game.pad("cross")
    game.run(seconds(10))
    golden(f"2p_{name}_10s", game.frame())
