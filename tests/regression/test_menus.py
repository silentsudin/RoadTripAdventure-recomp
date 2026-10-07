"""Title menus: Options (vibration, speaker, sound volume, each checked for its effect), Results,
and the attract demo; the town's Start menu's Settings."""

from __future__ import annotations

import pytest

from rtharness import seconds
from rtharness.adventure import FIELDS, GAME_MAP, SCENE_TOWN, boot_to_main_menu, continue_to_factory, scene

# Main menu: Adventure / Quick Race / 2 Player / Options / Results.
OPTIONS, RESULTS = 3, 4


def open_main_item(game, index: int):
    for _ in range(index):
        game.press("down")
    game.press("cross")
    game.run(seconds(2))


def set_option(game, item: int, choice: int):
    """Options menu (Vibration / Speaker / Sound Volume / Exit): pick `choice` in `item`'s list."""
    for _ in range(item):
        game.press("down")
    game.press("cross")
    game.run(seconds(1))
    for _ in range(choice):
        game.press("down")
    game.press("cross")  # apply
    game.run(seconds(1))


def quick_race_sound(game, seconds_of_race: float = 12) -> dict:
    """From the main menu (cursor on Options): Quick Race with the defaults, then race sound."""
    game.press("up")
    game.press("up")
    game.press("cross")
    game.run(seconds(2))
    for _ in range(3):
        game.press("cross")
        game.run(seconds(3))
    game.run(seconds(6))
    game.pad("cross")
    game.audio()
    game.run(seconds(seconds_of_race))
    return game.audio()


def test_options_screens(game_factory, golden):
    game = game_factory()
    boot_to_main_menu(game)
    open_main_item(game, OPTIONS)
    golden("options_menu", game.frame())
    for item, name in enumerate(("vibration", "speaker", "sound_volume")):
        for _ in range(item):
            game.press("down")
        game.press("cross")
        game.run(seconds(1))
        golden(f"options_{name}", game.frame())
        game.press("triangle")
        game.run(seconds(1))
        for _ in range(item):
            game.press("up")


def test_vibration_test_button(game_factory):
    """Vibration -> TEST drives the pad motors (no crash, the menu stays responsive)."""
    game = game_factory()
    boot_to_main_menu(game)
    open_main_item(game, OPTIONS)
    set_option(game, 0, 3)
    game.run(seconds(3))
    assert game.stats()["flips"] > 0


def test_sound_volume_mute(game_factory):
    game = game_factory()
    boot_to_main_menu(game)
    open_main_item(game, OPTIONS)
    set_option(game, 2, 5)  # 0 (mute)
    game.press("triangle")
    game.run(seconds(2))
    sound = quick_race_sound(game)
    assert sound["peak"] <= 8, f"sound volume 0 should mute the race (peak {sound['peak']})"


def test_speaker_mono(game_factory):
    game = game_factory()
    boot_to_main_menu(game)
    open_main_item(game, OPTIONS)
    set_option(game, 1, 1)  # Mono
    game.press("triangle")
    game.run(seconds(2))
    sound = quick_race_sound(game)
    assert sound["rms"] > 300, "the race should be audible"
    # The output itself stays different left and right (the reverb is stereo), so check the
    # voices: mono drives every one at the same volume on both sides.
    panned = [v for v in game.voice_volumes() if v[2] != v[3]]
    assert not panned, f"mono: voices panned (core, voice, VOLL, VOLR): {panned}"


def test_speaker_stereo_differs(game_factory):
    """Control for test_speaker_mono: the default stereo output does differ between channels."""
    game = game_factory()
    boot_to_main_menu(game)
    for _ in range(OPTIONS):
        game.press("down")
    sound = quick_race_sound(game)
    assert sound["lr_diff"] > 100
    assert any(v[2] != v[3] for v in game.voice_volumes()), "stereo: some voices are panned"


def test_options_replaced(game_factory, golden):
    """The app's menu stands in for the game's Options: choosing it leaves the title menu as it
    was (no Options screen) and hands over to the app (src/game/overrides.cpp)."""
    game = game_factory(env={"RT_GAME_OPTIONS": "0"})
    boot_to_main_menu(game)
    open_main_item(game, OPTIONS)
    golden("options_replaced", game.frame())
    assert "Title > Options: the app's menu" in game.log_text(), "the Options hook didn't run"
    # The title menu still works: Quick Race starts from it.
    sound = quick_race_sound(game)
    assert sound["rms"] > 300, "the title menu should carry on after Options"


def test_results_screen(game_factory, golden):
    game = game_factory(checkpoint="adventure_first_save")
    boot_to_main_menu(game)
    open_main_item(game, RESULTS)
    golden("results_slots", game.frame())
    game.press("cross")  # slot 1
    game.run(seconds(3))
    golden("results_slot1", game.frame())


def test_results_empty_card(game_factory, golden):
    game = game_factory()
    boot_to_main_menu(game)
    open_main_item(game, RESULTS)
    game.press("cross")
    game.run(seconds(3))
    golden("results_no_data", game.frame())


def test_attract_demo(game_factory, golden, golden_audio):
    """Left alone, the title runs the attract demo: a full computer race."""
    game = game_factory()
    game.run(seconds(45))
    golden("attract_demo_45s", game.frame())
    game.audio()
    game.run(seconds(15))
    sound = game.audio()
    assert sound["rms"] > 300, "the demo race is audible"
    golden_audio("attract_demo_15s", sound)


# The town's Start menu (config/game_state.toml [options] pause_*): Warp / Notebook / Radio /
# Items / Settings / Map; its state block's +0 is the page shown (0 the list, 5 Settings).
PAUSE_STATE = GAME_MAP["options"]["pause_state"]
PAUSE_SETTINGS = 4  # the list's 5th entry


@pytest.mark.parametrize("own", [False, True], ids=["replaced", "game"])
def test_pause_settings_replaced(game_factory, own):
    """In town, Start > Settings (the game's button setup) opens the app's menu instead
    (src/game/overrides.cpp): the game stays on its Pause list, and leaving it the drive goes on.
    RT_GAME_OPTIONS=1 keeps the game's own Settings page (the control for the addresses)."""
    edits = {FIELDS["location"]["offset"]: bytes([1]), FIELDS["licence"]["offset"]: bytes([2])}
    game = game_factory(checkpoint="adventure_first_save", progress_edits=edits,
                        env={"RT_GAME_OPTIONS": "1" if own else "0"})
    continue_to_factory(game)
    for _ in range(4):  # Change parts / Race / Save data / Quit game / Drive around town
        game.press("down")
    game.press("cross")
    game.run(seconds(2))
    game.press("cross")  # dismiss "Come again!"
    game.run(seconds(10))
    assert scene(game) == SCENE_TOWN, "driving in town"
    game.press("start")
    game.run(seconds(2))
    assert game.u32(PAUSE_STATE) == 0, "the Pause list"
    for _ in range(PAUSE_SETTINGS):
        game.press("down")
    game.press("cross")
    game.run(seconds(3))
    hooked = game.log_text().count("Pause > Settings: the app's menu")
    if own:
        assert game.u32(PAUSE_STATE) == 5, "RT_GAME_OPTIONS=1: the game's Settings page"
        assert hooked == 0
        game.press("triangle")  # the button setup's Exit
        game.run(seconds(2))
        assert game.u32(PAUSE_STATE) == 0, "back on the Pause list"
    else:
        assert hooked == 1, "the Settings hook should run once"
        assert game.u32(PAUSE_STATE) == 0, "the game stays on its Pause list"
    # Leave the Pause list and drive on.
    game.press("triangle")
    game.run(seconds(2))
    assert scene(game) == SCENE_TOWN
    before = game.frame().array()
    game.pad("cross")
    game.run(seconds(4))
    game.release()
    after = game.frame().array()
    moved = (abs(before.astype(int) - after.astype(int)).sum(axis=2) > 48).mean()
    assert moved > 0.2, f"the drive should go on after the Pause menu ({moved:.0%} of the picture changed)"
    assert game.log_text().count("Pause > Settings: the app's menu") == hooked, "opened once"
