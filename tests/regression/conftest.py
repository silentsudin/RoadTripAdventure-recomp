"""pytest configuration for the Road Trip regression suite (see README, "Regression suite")."""

from __future__ import annotations

import os
import shutil
import time
import uuid
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
    group.addoption("--perf", action="store_true", help="also run real-time performance checks (needs a quiet machine)")


def pytest_configure(config):
    config.addinivalue_line("markers", "perf: real-time performance check; runs only with --perf")
    config.addinivalue_line("markers", "produces(name): the test writes checkpoint `name` for later tests")
    # One id per run, shared with the xdist workers: checkpoints made in this run are stamped
    # with it, so tests that start from them can wait for this run's copy.
    worker = getattr(config, "workerinput", None)
    config.rt_session = worker["rt_session"] if worker else uuid.uuid4().hex


@pytest.hookimpl(optionalhook=True)
def pytest_configure_node(node):
    node.workerinput["rt_session"] = node.config.rt_session


def produced_checkpoint(item) -> str | None:
    marker = item.get_closest_marker("produces")
    return marker.args[0] if marker and marker.args else None


def pytest_collection_modifyitems(config, items):
    # Checkpoint producers first, so with -n they are scheduled before the tests waiting on them.
    config.rt_producing = {name for item in items if (name := produced_checkpoint(item))}
    items.sort(key=lambda item: produced_checkpoint(item) is None)
    if config.getoption("--perf"):
        return
    skip = pytest.mark.skip(reason="performance checks run with --perf")
    for item in items:
        if "perf" in item.keywords:
            item.add_marker(skip)


@pytest.hookimpl(wrapper=True)
def pytest_runtest_makereport(item, call):
    report = yield
    if report.when == "call":
        item.rt_call_report = report
    return report


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
            if checkpoint in request.config.rt_producing and produced_checkpoint(request.node) != checkpoint:
                wait_for_checkpoint(request.config, cp)
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
def golden_audio(options):
    def check(name, audio):
        goldens.check_audio(name, audio, update=options.update_goldens)

    return check


def wait_for_checkpoint(config, cp: Path, timeout: float = 3600):
    """Waits until this run's producer has written `cp` (or failed: then the test is skipped
    rather than run from a stale copy)."""
    stamp, failed = cp.with_name(cp.name + ".session"), cp.with_name(cp.name + ".failed")
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        if stamp.is_file() and stamp.read_text() == config.rt_session:
            return
        if failed.is_file() and failed.read_text() == config.rt_session:
            pytest.skip(f"the test producing checkpoint '{cp.name}' failed in this run")
        time.sleep(1)
    pytest.fail(f"checkpoint '{cp.name}' was not produced within {timeout:.0f} s")


@pytest.fixture
def new_checkpoint(test_data, request):
    """new_checkpoint(name) -> path for a checkpoint this test produces (mark the test with
    @pytest.mark.produces(name)). It is written to a scratch path and replaces the old checkpoint
    only when the test passes; if it fails, tests waiting on it in this run are skipped."""
    made = []

    def path(name: str) -> Path:
        scratch = test_data / "checkpoints" / (name + ".new")
        shutil.rmtree(scratch, ignore_errors=True)
        made.append((name, scratch))
        return scratch

    yield path
    report = getattr(request.node, "rt_call_report", None)
    for name, scratch in made:
        cp = test_data / "checkpoints" / name
        if report is not None and report.passed and scratch.is_dir():
            old = cp.with_name(name + ".old")
            shutil.rmtree(old, ignore_errors=True)
            if cp.exists():
                cp.rename(old)
            scratch.rename(cp)
            shutil.rmtree(old, ignore_errors=True)
            cp.with_name(name + ".session").write_text(request.config.rt_session)
        else:
            shutil.rmtree(scratch, ignore_errors=True)
            cp.with_name(name + ".failed").write_text(request.config.rt_session)
