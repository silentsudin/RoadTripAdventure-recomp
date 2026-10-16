"""Shared fixtures for the native UI-parity suite.

The suite is plain python3 + pytest, offline and headless. It reads the recomp
ground truth (src/ui, src/game/GameOptions.cpp) and the spec document
(docs/native-reference/ui-parity.md) and never mutates the tree.

Findings are accumulated in `parity_spec.FINDINGS` during the run and written to
`tests/native_parity/findings.json` in `pytest_sessionfinish`, so a machine
`readable record exists even when a check fails.
"""

from __future__ import annotations

import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parent))

import parity_spec  # noqa: E402


@pytest.fixture(scope="session", autouse=True)
def _fresh_findings():
    parity_spec.reset_findings()
    yield


@pytest.fixture
def defect_baseline() -> int:
    """DEFECT count before the test body, so a later test ignores earlier defects."""
    return parity_spec.defect_count()


@pytest.fixture(scope="session")
def spec_text() -> str:
    return parity_spec.load_spec_text()


@pytest.fixture(scope="session")
def manifest():
    return parity_spec.manifest()


@pytest.fixture(scope="session")
def visible_manifest():
    return parity_spec.manifest(only_visible=True)


@pytest.fixture(scope="session")
def native_options():
    return parity_spec.parse_native_options()


def pytest_sessionfinish(session, exitstatus):  # noqa: ARG001
    try:
        path = parity_spec.write_findings_json()
        counts = parity_spec.summary()
        print(f"\n[native_parity] findings.json -> {path}")
        print(f"[native_parity] findings: {counts}")
    except Exception as exc:  # pragma: no cover - never mask the test result
        print(f"[native_parity] could not write findings.json: {exc}")
