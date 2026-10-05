"""Play-through sections: recorded input replayed from a checkpoint, checked at its markers.

Each section is sections/<name>.toml + sections/<name>.movie (see sections/README.md). Sections run
in name order on one worker, since each may start from the checkpoint an earlier one produced.
"""

from __future__ import annotations

import tomllib
from pathlib import Path

import pytest

from rtharness.movie import Movie

SECTIONS = Path(__file__).parent / "sections"


def section_files():
    params = []
    for spec in sorted(SECTIONS.glob("*.toml")):
        produces = tomllib.loads(spec.read_text()).get("produces")
        params.append(pytest.param(spec, id=spec.stem, marks=[pytest.mark.produces(produces)] if produces else []))
    return params


@pytest.mark.xdist_group("sections")
@pytest.mark.parametrize("spec", section_files())
def test_section(spec, game_factory, golden, golden_audio, new_checkpoint):
    meta = tomllib.loads(spec.read_text())
    movie_path = SECTIONS / meta["movie"]
    movie = Movie.load(movie_path)
    produces = new_checkpoint(meta["produces"]) if meta.get("produces") else None
    start = meta.get("start", "boot")
    game = game_factory(checkpoint=None if start == "boot" else start, name=f"section_{spec.stem}",
                        env={"RT_MOVIE_PLAY": str(movie_path)})
    game.audio()
    for i, marker in enumerate(movie.goldens()):
        game.run(marker.vblank - game.vblank)
        golden(f"{spec.stem}:{i}", game.frame())
    end = meta.get("end_vblank", movie.end)
    game.run(end - game.vblank)
    golden_audio(spec.stem, game.audio())
    for name, value in meta.get("expect", {}).items():
        assert game.progress()[name] == value, f"{name} at the end of {spec.stem}"
    if produces:
        game.snapshot_card(produces)
