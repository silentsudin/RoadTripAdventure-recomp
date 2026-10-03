"""Memory card cases: overwriting a save and loading it back, saving to the second card, and a
damaged save (the game keeps no checksum: it loads what it finds)."""

from __future__ import annotations

import shutil

import pytest

from rtharness import seconds
from rtharness.adventure import FIELDS, PROGRESS, continue_to_factory, save_game

MONEY = PROGRESS + FIELDS["money"]["offset"]


@pytest.mark.xdist_group("memcard")
def test_overwrite_save_and_load_it(game_factory, golden, test_data):
    game = game_factory(checkpoint="adventure_first_save")
    continue_to_factory(game)
    game.write(MONEY, (4321).to_bytes(4, "little"))  # live state differs from the save (1000)
    save_game(game, slot=1)
    assert game.saved_progress()["money"] == 4321, "the save holds the live state"
    card = test_data / "runs" / "memcard_overwritten"
    game.snapshot_card(card)
    game.close()

    again = game_factory(checkpoint=str(card), name="memcard_overwritten_load")
    continue_to_factory(again)
    golden("memcard_overwritten_factory", again.frame())
    assert again.progress()["money"] == 4321


def test_save_to_card_2(game_factory, golden):
    game = game_factory(checkpoint="adventure_first_save")
    continue_to_factory(game)
    save_game(game, slot=2)
    assert game.saved_progress(card=2) == game.saved_progress(card=1)
    # Quit to the title and load from card 2.
    game.press("down")
    game.press("down")
    game.press("down")
    game.press("cross")       # Quit game: "Do you want to quit for today?" (No preselected)
    game.run(seconds(3))
    game.press("up")
    game.press("cross")       # Yes
    game.run(seconds(3))
    game.press("cross")       # "Okay, see you later!"
    game.run(seconds(10))
    game.press("start")
    game.run(seconds(3))
    golden("memcard_quit_to_title", game.frame())
    game.write(MONEY, (0).to_bytes(4, "little"))  # so the load below must restore it
    continue_to_factory(game, slot=2, boot=False)
    golden("memcard_card2_factory", game.frame())
    assert game.progress()["money"] == 1000, "loaded from card 2"


def test_damaged_save_loads(game_factory, golden, test_data):
    src = test_data / "checkpoints" / "adventure_first_save"
    if not src.is_dir():
        pytest.skip("checkpoint 'adventure_first_save' missing")
    damaged = test_data / "runs" / "memcard_damaged_card"
    shutil.rmtree(damaged, ignore_errors=True)
    shutil.copytree(src, damaged)
    save = damaged / "BASLUS-20398" / "BASLUS-20398"
    data = bytearray(save.read_bytes())
    for offset in range(0x600, 0x700):  # player name, currency, money
        data[offset] ^= 0x5A
    save.write_bytes(bytes(data))
    game = game_factory(checkpoint=str(damaged))
    continue_to_factory(game)
    golden("memcard_damaged_factory", game.frame())
    assert game.stats()["flips"] > 0
