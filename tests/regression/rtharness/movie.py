"""Movie files (pad input by vblank) written by RT_MOVIE_RECORD; see ps2_test_harness.h."""

from __future__ import annotations

from dataclasses import dataclass, field
from pathlib import Path


@dataclass
class Marker:
    vblank: int
    kind: str
    text: str = ""


@dataclass
class Movie:
    changes: int = 0
    end: int | None = None
    markers: list[Marker] = field(default_factory=list)

    @classmethod
    def load(cls, path: Path) -> "Movie":
        movie = cls()
        for line in Path(path).read_text().splitlines():
            if line.startswith("# marker "):
                parts = line.split(" ", 4)
                movie.markers.append(Marker(int(parts[2]), parts[3], parts[4] if len(parts) > 4 else ""))
            elif line.startswith("# end "):
                movie.end = int(line.split()[2])
            elif line and not line.startswith("#"):
                movie.changes += 1
        return movie

    def goldens(self) -> list[Marker]:
        return [m for m in self.markers if m.kind == "golden"]
