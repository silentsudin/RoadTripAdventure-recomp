"""pytest configuration for the Road Trip regression suite (see README, "Regression suite")."""

from __future__ import annotations

import os
from pathlib import Path

import pytest

import sys

sys.path.insert(0, str(Path(__file__).parent))

from rtharness import DEFAULT_APP, Game  # noqa: E402
from rtharness import goldens  # noqa: E402

REPO = Path(__file__).resolve().parents[2]


def pytest_addoption(parser):
    group = parser.getgroup("roadtrip")
    group.addoption("--base-data", default=os.environ.get("RT_TEST_BASE",
                    str(Path.home() / "Library/Application Support/RoadTripRecomp")),
                    help="installed data directory (extracted disc + built game) to run from")
    group.addoption("--test-data", default=os.environ.get("RT_TEST_DATA", str(REPO / "build/regression")),
                    help="where runs, checkpoints, golden images and failures go (never committed)")
    group.addoption("--app", default=str(DEFAULT_APP))
    group.addoption("--speed", default="max", help="RT_SPEED for the game (max = as fast as possible)")
    group.addoption("--update-goldens", action="store_true", help="accept current frames as goldens")


@pytest.fixture(scope="session")
def options(request):
    o = request.config.option
    base = Path(o.base_data)
    if not (base / "disc").is_dir() or not (base / "game").is_dir():
        pytest.skip(f"no installed game in {base} (run the app once with your disc, or pass --base-data)")
    return o


@pytest.fixture(scope="session")
def test_data(options) -> Path:
    path = Path(options.test_data)
    path.mkdir(parents=True, exist_ok=True)
    return path


@pytest.fixture
def game_factory(options, test_data, request):
    """game_factory(checkpoint=None, name=None) -> started Game; closed after the test."""
    games = []

    def make(checkpoint: str | None = None, name: str | None = None, **kwargs) -> Game:
        work = test_data / "runs" / (name or request.node.name)
        cp = None
        if checkpoint:
            cp = test_data / "checkpoints" / checkpoint
            if not cp.is_dir():
                pytest.skip(f"checkpoint '{checkpoint}' missing; run the test that creates it first")
        game = Game(Path(options.base_data), work, checkpoint=cp, speed=options.speed,
                    app=Path(options.app), **kwargs).start()
        games.append(game)
        return game

    yield make
    for g in games:
        g.close()


@pytest.fixture
def golden(options, test_data):
    def check(name, frame):
        goldens.check(name, frame, test_data, update=options.update_goldens)

    return check


@pytest.fixture
def checkpoint_dir(test_data):
    def path(name: str) -> Path:
        return test_data / "checkpoints" / name

    return path
