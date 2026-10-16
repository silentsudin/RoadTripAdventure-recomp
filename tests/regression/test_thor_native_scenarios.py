"""Small protocol-level checks for the Thor native scenario runner."""

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "scripts"))

import thor_native_scenarios as scenarios


def test_position_distance_reports_motion_only_when_coordinates_change():
    assert scenarios.position_distance((1.0, 2.0), (1.0, 2.0)) == 0.0
    assert scenarios.position_distance((1.0, 2.0), (4.0, 6.0)) == 5.0
