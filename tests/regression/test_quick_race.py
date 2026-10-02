"""Every Quick Race course: race it with the driving bot from the course card and back. Checks the
card, the start (frame and sound) and the card afterwards, whose records (attempts, best times,
best result) show the game stored the race."""

from __future__ import annotations

import pytest

from rtharness import seconds
from rtharness.quick_race import COURSES, drive_quick_race, open_course, start_race


@pytest.mark.parametrize("index", range(len(COURSES)), ids=[c.replace(" ", "_") for c in COURSES])
def test_quick_race_course(game_factory, golden, golden_audio, index):
    game = game_factory()
    open_course(game, index)
    key = "quick_" + COURSES[index].replace(" ", "_")
    golden(key + "_card", game.frame())
    start_race(game)
    game.audio()

    def check_start():
        golden(key + "_start", game.frame())
        golden_audio(key + "_start", game.audio())

    place = drive_quick_race(game, at={15: check_start})
    assert 1 <= place <= 24
    # The race ends back at the carousel, on this course; its card now lists the race.
    game.run(seconds(2))
    game.press("cross")
    game.run(seconds(3))
    golden(key + "_records", game.frame())
