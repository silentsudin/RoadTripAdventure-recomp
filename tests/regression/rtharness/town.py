"""Driving around towns: maps from the disc, path planning, dialogue and stamps.

Each town's collision geometry is on the disc (config/game_state.toml [towns] maps): FLD tiles
and ACTION files hold, per cell, triangle strips with per-triangle normals, in the town's own
coordinates (0..1600). Ground triangles give a height map; the car can move between
neighbouring cells whose heights differ a little, so building walls, cliffs and water are
obstacles. Maps are cached in the test-data directory (they are game data: never committed).
"""

from __future__ import annotations

import heapq
import math
import struct
from pathlib import Path
from typing import TYPE_CHECKING

import numpy as np

from .adventure import GAME_MAP, PROGRESS
from .driver import CAR_STRIDE, CARS_BASE

if TYPE_CHECKING:
    from . import Game

CELL = 2.0           # map resolution (town units per cell)
SIZE = 1600          # a town map is SIZE x SIZE units
N = int(SIZE / CELL)
DIALOGUE = GAME_MAP["dialogue"]
STAMPS = GAME_MAP["stamps"]


# ---------------------------------------------------------------- map data
def collision_triangles(path: Path):
    """Triangles (a, b, c, normal) of a map file's collision section (the third section)."""
    data = path.read_bytes()
    base, end = struct.unpack_from("<5I", data, 0)[2:4]
    cells = struct.unpack_from("<256I", data, base)
    for c in range(256):
        p, stop = base + cells[c], (base + cells[c + 1] if c < 255 else end)
        while p + 16 <= stop:
            n = struct.unpack_from("<I", data, p)[0] & 0x7FFF
            if not 3 <= n <= 64:
                break
            vs = [struct.unpack_from("<3f", data, p + 16 + 16 * i) for i in range(n)]
            ns = [struct.unpack_from("<3f", data, p + 16 + 16 * n + 16 * i) for i in range(n - 2)]
            for i in range(n - 2):
                yield vs[i], vs[i + 1], vs[i + 2], ns[i]
            p += 16 + 16 * n + 16 * (n - 2)


def height_map(path: Path) -> np.ndarray:
    """Highest ground height per cell (NaN where there is no ground)."""
    H = np.full((N, N), np.nan, np.float32)
    centres = (np.arange(N) + 0.5) * CELL
    for a, b, c, n in collision_triangles(path):
        if n[1] <= 0.7:
            continue
        xs, zs = (a[0], b[0], c[0]), (a[2], b[2], c[2])
        i0, i1 = max(0, int(min(xs) / CELL)), min(N - 1, int(max(xs) / CELL))
        j0, j1 = max(0, int(min(zs) / CELL)), min(N - 1, int(max(zs) / CELL))
        d = (b[2] - c[2]) * (a[0] - c[0]) + (c[0] - b[0]) * (a[2] - c[2])
        if abs(d) < 1e-9:
            continue
        x = centres[i0:i1 + 1, None]
        z = centres[None, j0:j1 + 1]
        l1 = ((b[2] - c[2]) * (x - c[0]) + (c[0] - b[0]) * (z - c[2])) / d
        l2 = ((c[2] - a[2]) * (x - c[0]) + (a[0] - c[0]) * (z - c[2])) / d
        l3 = 1 - l1 - l2
        inside = (l1 >= -0.05) & (l2 >= -0.05) & (l3 >= -0.05)
        y = (l1 * a[1] + l2 * b[1] + l3 * c[1]).astype(np.float32)
        block = H[i0:i1 + 1, j0:j1 + 1]
        np.copyto(block, np.where(inside, np.fmax(block, y), block))
    return H


class TownMap:
    """Free space and A* paths for one town."""

    def __init__(self, heights: np.ndarray, step: float = 0.8, clearance: int = 2, water: float = -2.0):
        self.H = heights
        Hn = np.nan_to_num(heights, nan=-99.0)
        blocked = Hn <= water
        for di, dj in ((1, 0), (0, 1), (1, 1), (1, -1)):
            limit = step * (1.42 if di and dj else 1.0)
            cliff = np.abs(Hn - np.roll(np.roll(Hn, di, 0), dj, 1)) > limit
            blocked |= cliff | np.roll(np.roll(cliff, -di, 0), -dj, 1)
        free = ~blocked
        for di in range(-clearance, clearance + 1):
            for dj in range(-clearance, clearance + 1):
                if di * di + dj * dj <= clearance * clearance:
                    free &= ~np.roll(np.roll(blocked, di, 0), dj, 1)
        self.free = free
        # Walls the ground map does not show (fences, building sides): learned when the car gets
        # stuck against them, for the rest of the run.
        self.learned = np.zeros_like(free)

    def learn_obstacle(self, x: float, z: float, radius: float = 5.0) -> None:
        ci, cj = self.cell(x, z)
        r = int(radius / CELL) + 1
        for di in range(-r, r + 1):
            for dj in range(-r, r + 1):
                a, b = ci + di, cj + dj
                if di * di + dj * dj <= r * r and 0 <= a < N and 0 <= b < N:
                    self.learned[a, b] = True

    @classmethod
    def load(cls, town: int, disc: Path, cache: Path, clearance: int = 2) -> "TownMap":
        name = GAME_MAP["towns"]["maps"][town]
        cache.mkdir(parents=True, exist_ok=True)
        cached = cache / (name.replace("/", "_") + ".npy")
        if cached.exists():
            heights = np.load(cached)
        else:
            heights = height_map(disc / (name + ".BIN"))
            np.save(cached, heights)
        return cls(heights, clearance=clearance)

    @staticmethod
    def cell(x: float, z: float) -> tuple[int, int]:
        return min(N - 1, max(0, int(x / CELL))), min(N - 1, max(0, int(z / CELL)))

    def nearest_free(self, c: tuple[int, int]) -> tuple[int, int]:
        for r in range(60):
            for di in range(-r, r + 1):
                for dj in ((-r, r) if abs(di) != r else range(-r, r + 1)):
                    a, b = c[0] + di, c[1] + dj
                    if 0 <= a < N and 0 <= b < N and self.free[a, b]:
                        return a, b
        return c

    def path(self, start, goal, avoid=()) -> list[tuple[float, float]] | None:
        """Cell-centre waypoints from start to goal ((x, z) pairs), or None if unreachable.
        `avoid` holds (x, z, radius) discs to keep out of (other cars)."""
        free = self.free & ~self.learned
        if avoid:
            ii, jj = np.meshgrid(np.arange(N), np.arange(N), indexing="ij")
            for ax, az, r in avoid:
                ci, cj = self.cell(ax, az)
                rc = int(r / CELL) + 1
                sl = (slice(max(0, ci - rc), ci + rc + 1), slice(max(0, cj - rc), cj + rc + 1))
                near = (ii[sl] - ci) ** 2 + (jj[sl] - cj) ** 2 <= rc * rc
                free[sl] &= ~near
        saved, self.free = self.free, free
        try:
            return self._path(start, goal)
        finally:
            self.free = saved

    def _path(self, start, goal) -> list[tuple[float, float]] | None:
        s, g = self.nearest_free(self.cell(*start)), self.nearest_free(self.cell(*goal))
        frontier = [(0.0, s)]
        came: dict = {s: None}
        cost = {s: 0.0}
        while frontier:
            _, c = heapq.heappop(frontier)
            if c == g:
                break
            for di in (-1, 0, 1):
                for dj in (-1, 0, 1):
                    if not (di or dj):
                        continue
                    n = (c[0] + di, c[1] + dj)
                    if not (0 <= n[0] < N and 0 <= n[1] < N) or not self.free[n]:
                        continue
                    nc = cost[c] + (1.414 if di and dj else 1.0)
                    if nc < cost.get(n, math.inf):
                        cost[n] = nc
                        came[n] = c
                        heapq.heappush(frontier, (nc + math.dist(n, g), n))
        if g not in came:
            return None
        out, c = [], g
        while c is not None:
            out.append(((c[0] + 0.5) * CELL, (c[1] + 0.5) * CELL))
            c = came[c]
        return out[::-1]


# ---------------------------------------------------------------- the car in town
def car_pose(game: "Game", slot: int = 0) -> tuple[float, float, float]:
    """(x, z, heading) of a car; slot 0 is ours, the others are town traffic (NPCs)."""
    raw = game.read(CARS_BASE + slot * CAR_STRIDE, 0xE0)
    x, _, z = struct.unpack_from("<3f", raw, 0)
    fx, _, fz = struct.unpack_from("<3f", raw, 0xB0)
    rx, _, rz = struct.unpack_from("<3f", raw, 0xD0)
    return x, z, math.atan2(fz - rz, fx - rx)


def steer_towards(x, z, heading, tx, tz, gain: float = 2.0) -> int:
    err = (math.atan2(tz - z, tx - x) - heading + math.pi) % (2 * math.pi) - math.pi
    return int(max(0, min(255, 128 - 127 * max(-1.0, min(1.0, err * gain)))))


class TownDriver:
    """Drives our car to places in town along planned paths."""

    def __init__(self, game: "Game", town_map: TownMap):
        self.game = game
        self.map = town_map
        self.keep_away: dict[int, int] = {}  # slot -> vblank until which we keep clear of it
        self.stuck_count = 0
        self.direct_last: tuple[int, tuple[float, float]] | None = None

    def other_cars(self, exclude: int | None = None, radius: float = 14.0):
        """(x, z, radius) of the town traffic, to plan around (wider around cars we just
        talked to, which otherwise stop us again)."""
        out = []
        for slot in range(1, 12):
            if slot == exclude:
                continue
            x, z, _ = car_pose(self.game, slot)
            if x or z:
                wide = self.game.vblank < self.keep_away.get(slot, 0)
                out.append((x, z, 30.0 if wide else radius))
        return out

    def nearest_car(self) -> int:
        x, z, _ = car_pose(self.game)
        return min(range(1, 12), key=lambda k: math.dist((x, z), car_pose(self.game, k)[:2]))

    def leave_conversation(self) -> None:
        """After talking to someone we were not looking for: back away and give them room."""
        slot = self.nearest_car()
        finish_dialogue(self.game)
        self.keep_away[slot] = self.game.vblank + 600
        x, z, heading = car_pose(self.game)
        ox, oz, _ = car_pose(self.game, slot)
        self.game.step(45, ["circle"], 255 - steer_towards(x, z, heading, ox, oz))

    def drive_to(self, goal, radius: float = 10.0, timeout_vblanks: int = 60 * 300,
                 stop_on_dialogue: bool = True, target_slot: int | None = None) -> bool:
        """Drives to `goal` (x, z). Returns True on arrival; False if a dialogue opened on the
        way (with stop_on_dialogue) or the time ran out."""
        game = self.game
        end = game.vblank + timeout_vblanks
        x, z, _ = car_pose(game)
        path = self.map.path((x, z), goal, self.other_cars(target_slot)) or [goal]
        index, last, last_check, planned = 0, (x, z), game.vblank, game.vblank
        while game.vblank < end:
            if stop_on_dialogue and dialogue_open(game):
                game.release()
                return False
            x, z, heading = car_pose(game)
            if math.dist((x, z), goal) < radius:
                game.release()
                return True
            while index < len(path) - 1 and math.dist(path[index], (x, z)) < 12:
                index += 1
            if game.vblank - last_check >= 90:
                # (A car starting from rest covers only a few units in the first second.)
                if math.dist(last, (x, z)) < 3.0:
                    # Stuck against something: remember it, back off, plan again from here.
                    self.map.learn_obstacle(x + 7 * math.cos(heading), z + 7 * math.sin(heading))
                    self.stuck_count += 1
                    # Reverse on full lock, longer and to alternating sides on repeats.
                    lock = 0 if self.stuck_count % 2 else 255
                    game.step(60 + 30 * min(4, self.stuck_count), ["circle"], lock)
                    x, z, _ = car_pose(game)
                    path = self.map.path((x, z), goal, self.other_cars(target_slot)) or [goal]
                    index, planned = 0, game.vblank
                last, last_check = (x, z), game.vblank
            if game.vblank - planned > 120:
                # Traffic moves: plan again every 2 s.
                path = self.map.path((x, z), goal, self.other_cars(target_slot)) or [goal]
                index, planned = 0, game.vblank
            tx, tz = path[index]
            game.step(10, ["cross"], steer_towards(x, z, heading, tx, tz))
        game.release()
        return False

    def drive_direct(self, goal, vblanks: int) -> None:
        """Steers straight at `goal` for `vblanks`, backing off if stuck."""
        game = self.game
        x, z, heading = car_pose(game)
        if self.direct_last and game.vblank - self.direct_last[0] >= 90:
            if math.dist(self.direct_last[1], (x, z)) < 3.0:
                self.stuck_count += 1
                game.step(60 + 30 * min(4, self.stuck_count), ["circle"], 0 if self.stuck_count % 2 else 255)
                x, z, heading = car_pose(game)
            self.direct_last = None
        if self.direct_last is None:
            self.direct_last = (game.vblank, (x, z))
        game.step(vblanks, ["cross"], steer_towards(x, z, heading, *goal))

    def chase(self, slot: int, name: str | None = None, timeout_vblanks: int = 60 * 180) -> bool:
        """Drives into town car `slot` (an NPC) until its dialogue opens. With `name`, other
        NPCs met on the way are talked through and the chase goes on."""
        game = self.game
        end = game.vblank + timeout_vblanks
        history: list[tuple[int, float, float]] = []
        while game.vblank < end:
            if dialogue_open(game):
                game.release()
                if name is None or speaker(game) == name:
                    return True
                self.leave_conversation()
                continue
            x, z, heading = car_pose(game)
            tx, tz, _ = car_pose(game, slot)
            history.append((game.vblank, tx, tz))
            history[:] = [h for h in history if h[0] >= game.vblank - 60]
            # Aim where they will be when we get there (NPCs keep driving their rounds).
            t0, x0, z0 = history[0]
            dt = max(1, game.vblank - t0)
            vx, vz = (tx - x0) / dt, (tz - z0) / dt
            eta = min(240.0, math.dist((x, z), (tx, tz)) / 1.2)
            px, pz = tx + vx * eta, tz + vz * eta
            if math.dist((x, z), (tx, tz)) > 60:
                self.drive_to((px, pz), radius=50, timeout_vblanks=60 * 2, target_slot=slot)
                continue
            # Close: straight at them (the planner's detours lose them).
            self.drive_direct((px, pz), 10)
        game.release()
        return False


# ---------------------------------------------------------------- dialogue
def dialogue_open(game: "Game") -> bool:
    return game.u8(DIALOGUE["open_address"]) != 0


def c_string(game: "Game", address: int, limit: int = 160) -> str:
    raw = game.read(address, limit)
    return raw.split(b"\0", 1)[0].decode("latin-1")


def speaker(game: "Game") -> str:
    """Name of the NPC talking (from their NPC entry), or '' if none."""
    entry = game.u32(DIALOGUE["speaker_address"])
    if not 0x200000 <= entry < 0x2000000:
        return ""
    return c_string(game, game.u32(entry), 24)


def text(game: "Game") -> str:
    """The script text at the dialogue cursor (printable part)."""
    cursor = game.u32(DIALOGUE["cursor_address"])
    if not 0x200000 <= cursor < 0x2000000:
        return ""
    raw = game.read(cursor, 200)
    return "".join(chr(b) for b in raw.split(b"\0", 1)[0] if 32 <= b < 127 or b == 10)


def finish_dialogue(game: "Game", presses: int = 40, answer=None) -> None:
    """Presses through a conversation until the window closes. `answer(text)` may return
    "down" to pick the second choice of a question (the default is the first)."""
    closed = 0
    for _ in range(presses):
        if not dialogue_open(game):
            # The window closes between some messages, and for item pop-ups ("Get ..."), which
            # wait for a press: done only once it stays closed through a few presses.
            closed += 1
            if closed >= 3:
                return
            game.press("cross")
            game.run(60)
            continue
        closed = 0
        if answer and answer(text(game)) == "down":
            game.press("down")
        game.press("cross")
        game.run(60)


# ---------------------------------------------------------------- stamps
def stamps_earned(game: "Game") -> set[int]:
    raw = game.read(PROGRESS + STAMPS["earned_offset"], 16)
    lo, hi = struct.unpack("<QQ", raw)
    bits = lo | (hi << 64)
    return {n for n in range(1, 101) if bits >> (n - 1) & 1}
