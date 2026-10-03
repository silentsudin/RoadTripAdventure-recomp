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
    """A recorded movie. Version 2 lines carry a pad port ("<vblank> <port> <buttons> ..."), and
    "# connect" lines plug/unplug pads; version 1 lines drive both ports."""
    version: int = 1
    changes: int = 0
    end: int | None = None
    markers: list[Marker] = field(default_factory=list)
    ports: set[int] = field(default_factory=set)  # ports with their own input (version 2)

    @classmethod
    def load(cls, path: Path) -> "Movie":
        movie = cls()
        for line in Path(path).read_text().splitlines():
            if line.startswith("# roadtrip-movie "):
                movie.version = int(line.split()[2])
            elif line.startswith("# marker "):
                parts = line.split(" ", 4)
                movie.markers.append(Marker(int(parts[2]), parts[3], parts[4] if len(parts) > 4 else ""))
            elif line.startswith("# end "):
                movie.end = int(line.split()[2])
            elif line and not line.startswith("#"):
                movie.changes += 1
                if movie.version >= 2:
                    movie.ports.add(int(line.split()[1]))
        return movie

    def goldens(self) -> list[Marker]:
        return [m for m in self.markers if m.kind == "golden"]
