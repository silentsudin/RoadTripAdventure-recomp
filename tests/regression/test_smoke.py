"""Smoke tests: boot, a Quick Race, and an Adventure save that later tests continue from.

Everything runs in guest time (vblanks), so the same inputs always reach the same frames.
"""

import pytest

from rtharness import seconds


def boot_to_main_menu(game):
    """Boot -> title -> Start -> main menu (Adventure / Quick Race / 2 Player / Options / Results)."""
    game.run(seconds(10))
    game.press("start")
    game.run(seconds(3))


def test_boot_to_title(game_factory, golden):
    game = game_factory()
    game.run(seconds(12))
    golden("title", game.frame())
    stats = game.stats()
    assert stats["flips"] > 300, "the game should have been presenting frames"


def test_quick_race(game_factory, golden):
    game = game_factory()
    boot_to_main_menu(game)
    game.press("down")
    game.run(seconds(2))
    # Course/car selection screens: accept the defaults.
    for _ in range(8):
        game.press("cross")
        game.run(seconds(4))
    golden("quick_race_start", game.frame())
    game.pad("cross")  # accelerate
    game.run(seconds(20))
    before = game.frame()
    game.run(seconds(1))
    after = game.frame()
    assert before.rgba != after.rgba, "the race should be moving"
    golden("quick_race_20s", after)


@pytest.mark.xdist_group("adventure")
def test_adventure_new_game_save(game_factory, checkpoint_dir):
    game = game_factory()
    boot_to_main_menu(game)
    game.press("cross")  # Adventure
    game.run(seconds(3))
    # New Game, name and currency entry, then the intro dialogue up to Q's Factory, where the
    # remaining presses go into Change parts -> Tires.
    for _ in range(48):
        game.press("cross")
        game.run(seconds(4) - 16)
    # Back out to the factory menu and save to slot 1.
    for step in ("triangle", "triangle"):
        game.press(step)
        game.run(seconds(3))
    game.press("down")
    game.press("down")
    game.press("cross")  # Save data
    for _ in range(4):  # Slot 1, create new saved data: Yes, confirmations
        game.run(seconds(4))
        game.press("cross")
    game.run(seconds(4))
    save = game.saves_dir / "mc0" / "BASLUS-20398"
    assert (save / "icon.sys").read_bytes()[:4] == b"PS2D"
    assert (save / "BASLUS-20398").stat().st_size == 13384
    game.snapshot_card(checkpoint_dir("adventure_first_save"))


@pytest.mark.xdist_group("adventure")
def test_adventure_continue(game_factory, golden):
    game = game_factory(checkpoint="adventure_first_save")
    boot_to_main_menu(game)
    game.press("cross")  # Adventure
    game.run(seconds(2))
    game.press("down")  # Continue
    game.press("cross")
    game.run(seconds(3))
    game.press("cross")  # Slot 1
    game.run(seconds(3))
    golden("adventure_continue_summary", game.frame())
    game.press("cross")  # Load this saved data: Yes
    game.run(seconds(12))
    golden("adventure_continue_factory", game.frame())
