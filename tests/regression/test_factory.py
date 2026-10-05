"""Q's Factory: Change parts shows the fitted part of each category (Tires, Engine, Chassis,
Transmission, Steering, Brake) for the car."""

from __future__ import annotations

from rtharness import seconds
from rtharness.adventure import continue_to_factory

CATEGORIES = ["tires", "engine", "chassis", "transmission", "steering", "brake"]


def test_change_parts_categories(game_factory, golden):
    game = game_factory(checkpoint="adventure_first_save")
    continue_to_factory(game)
    game.press("cross")  # Change parts
    game.run(seconds(2))
    golden("factory_parts_whose", game.frame())
    game.press("cross")  # our car
    game.run(seconds(2))
    for i, name in enumerate(CATEGORIES):
        if i:
            game.press("down")
            game.run(seconds(1))
        golden(f"factory_parts_{name}", game.frame())
