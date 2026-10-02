"""Every Adventure race: load each town's Q's Factory (by editing the save point) and race every
event in its menu with the driving bot. Checks the start of the race (frame and sound), that the
race finishes, and that the game records the result."""

from __future__ import annotations

import pytest

from rtharness import seconds
from rtharness.adventure import FIELDS, GAME_MAP, PROGRESS, continue_to_factory, drive_race, enter_race

TOWNS = GAME_MAP["towns"]["names"]
CASES = [(town, i, name) for town, races in enumerate(GAME_MAP["towns"]["races"]) for i, name in enumerate(races)]
LICENCE_A = 2


def results(game) -> bytes:
    f = FIELDS["race_results"]
    return game.read(PROGRESS + f["offset"], f["length"])


@pytest.mark.parametrize("town,index,name", CASES, ids=[f"{TOWNS[t]}-{n}".replace(" ", "_") for t, _, n in CASES])
def test_adventure_race(game_factory, golden, golden_audio, town, index, name):
    edits = {FIELDS["location"]["offset"]: bytes([town]), FIELDS["licence"]["offset"]: bytes([LICENCE_A])}
    game = game_factory(checkpoint="adventure_first_save", progress_edits=edits)
    continue_to_factory(game)
    before = results(game)
    enter_race(game, index)
    game.audio()
    key = f"race_{TOWNS[town]}_{name}".replace(" ", "_")

    def check_start():
        golden(key, game.frame())
        golden_audio(key, game.audio())

    place = drive_race(game, at={15: check_start})
    after = results(game)
    changed = [i for i in range(len(after)) if after[i] != before[i]]
    assert len(changed) == 1, f"one race result should be recorded, got entries {changed}"
    assert after[changed[0]] + 1 == place, "the recorded place matches the place at the finish"
