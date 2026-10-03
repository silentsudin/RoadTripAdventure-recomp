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
FREE_DRIVING = 127  # talk state while driving around with no conversation
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

    def __init__(self, heights: np.ndarray, step: float = 1.2, clearance: int = 1, water: float = -2.0):
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
    def load(cls, town: int, disc: Path, cache: Path, clearance: int = 1) -> "TownMap":
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
        for ax, az, r in avoid:
            ci, cj = self.cell(ax, az)
            rc = int(r / CELL) + 1
            i0, j0 = max(0, ci - rc), max(0, cj - rc)
            ii = np.arange(i0, min(N, ci + rc + 1))[:, None]
            jj = np.arange(j0, min(N, cj + rc + 1))[None, :]
            free[i0:i0 + ii.shape[0], j0:j0 + jj.shape[1]] &= (ii - ci) ** 2 + (jj - cj) ** 2 > rc * rc
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


# ---------------------------------------------------------------- doors
def door_quads(game: "Game", town: int) -> list[list[tuple[float, float]]]:
    """The building doors of a town (quads of (x, z) points), read from the game in RAM."""
    doors = GAME_MAP["doors"]
    table = struct.unpack(f"<{doors['locations']}I", game.read(doors["table_address"], 4 * doors["locations"]))
    start = table[town]
    end = min(p for p in table if p > start)
    raw = game.read(start, end - start)
    return [[struct.unpack_from("<2f", raw, q + 8 * k) for k in range(4)] for q in range(0, len(raw) - 31, 32)]


def in_quad(quad, x: float, z: float, margin: float = 2.0) -> bool:
    """Whether (x, z) lies in a (convex) door quad, give or take `margin`."""
    (cx, cz) = (sum(p[0] for p in quad) / 4, sum(p[1] for p in quad) / 4)
    sign = None
    for k in range(4):
        (ax, az), (bx, bz) = quad[k], quad[(k + 1) % 4]
        cross = (bx - ax) * (z - az) - (bz - az) * (x - ax)
        inward = (bx - ax) * (cz - az) - (bz - az) * (cx - ax)
        if cross * inward < 0 and abs(cross) / (math.dist((ax, az), (bx, bz)) or 1) > margin:
            return False
    return True


def door_geometry(quad):
    """(centre, normal): the door's middle and a unit vector across its long edge."""
    cx = sum(p[0] for p in quad) / 4
    cz = sum(p[1] for p in quad) / 4
    a, b = max(((quad[k], quad[(k + 1) % 4]) for k in range(4)), key=lambda e: math.dist(*e))
    ex, ez = b[0] - a[0], b[1] - a[1]
    length = math.hypot(ex, ez) or 1.0
    return (cx, cz), (-ez / length, ex / length)


# ---------------------------------------------------------------- street traffic
def park_traffic(game: "Game", town: int) -> int:
    """Moves the town's street NPC spawn points off the map, so the street stays empty (call
    before the town loads, e.g. at Q's Factory). For tests that warp into buildings, where
    street NPCs only get in the way; returns how many spawn points were moved."""
    table = GAME_MAP["traffic"]["spawn_table"]
    pointers = struct.unpack("<40I", game.read(table, 160))
    start = pointers[town]
    if not start:
        return 0
    end = min(p for p in pointers if start < p < 0x400000)
    raw = bytearray(game.read(start, end - start))
    count = len(raw) // 16
    for k in range(count):
        struct.pack_into("<3f", raw, 16 * k, -4000.0 - 20 * k, 0.0, -4000.0)
    game.write(start, bytes(raw))
    return count


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

    def __init__(self, game: "Game", town_map: TownMap, doors=(), town: int = 0):
        self.game = game
        self.town = town
        self.map = town_map
        # Doors enter buildings: keep out of all but the one we are going through.
        self.door_quads = list(doors)
        self.trace: list | None = None  # set to a list to record (vblank, x, z, event)
        self._speed = 0.0
        self.last_pose = car_pose(game)
        self.entered_pose: tuple[float, float] | None = None
        self.entered: int | None = None   # door we went in by (None: a street NPC)
        self._throttle_vblanks = 0  # vblanks on the throttle since the last stuck check
        self.doors = [door_geometry(q) for q in doors]
        self.door_target: int | None = None
        self._parked: dict[int, bytes] = {}  # original quads of doors moved away
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
        for i, ((dx, dz), _) in enumerate(self.doors):
            if i != self.door_target:
                out.append((dx, dz, 12.0))
        return out

    def nearest_car(self) -> int:
        x, z, _ = car_pose(self.game)
        return min(range(1, 12), key=lambda k: math.dist((x, z), car_pose(self.game, k)[:2]))

    def leave_conversation(self) -> None:
        """After talking to someone we were not looking for: back away and give them room."""
        slot = self.nearest_car()
        finish_dialogue(self.game, decline=True)
        self.keep_away[slot] = self.game.vblank + 600
        x, z, heading = car_pose(self.game)
        ox, oz, _ = car_pose(self.game, slot)
        self.game.step(45, ["circle"], 255 - steer_towards(x, z, heading, ox, oz))

    def speed(self) -> float:
        """Our speed in units per vblank, from the last two steps."""
        return self._speed

    def opened(self) -> bool:
        """Whether a dialogue (or a building's loading) has started; records which door we were
        in at that moment (None on the street)."""
        if dialogue_open(self.game):
            if self.entered_pose is None:
                x, z, _ = self.last_pose
                self.entered_pose = (x, z)
                self.entered = self.door_at(x, z)
            return True
        return False

    def steer(self, tx: float, tz: float, vblanks: int = 10, slow_within: float = 0.0) -> None:
        """One control step towards (tx, tz) with speed control: the car cannot turn sharply at
        speed, so it brakes for big heading errors, coasts for medium ones and creeps near a
        target (within `slow_within`) instead of circling it."""
        game = self.game
        x, z, heading = car_pose(game)
        err = (math.atan2(tz - z, tx - x) - heading + math.pi) % (2 * math.pi) - math.pi
        lx = steer_towards(x, z, heading, tx, tz)
        near = slow_within and math.dist((x, z), (tx, tz)) < slow_within
        # The car only turns while moving: below crawling speed it always gets throttle.
        if self._speed < 0.25:
            buttons = ["cross"]
        elif abs(err) > 1.0 and self._speed > 0.5:
            buttons = ["square"]          # brake, then turn
        elif abs(err) > 0.5 or (near and self._speed > 0.35):
            buttons = []                  # coast
        else:
            buttons = ["cross"]
        game.step(vblanks, buttons, lx)
        pose = car_pose(game)
        if not dialogue_open(game):
            self.last_pose = pose   # (inside a building the pose is the interior's)
        nx, nz, _ = pose
        self._speed = math.dist((x, z), (nx, nz)) / vblanks
        if buttons == ["cross"]:
            self._throttle_vblanks += vblanks

    def note(self, event: str = "") -> None:
        if self.trace is not None:
            x, z, _ = car_pose(self.game)
            self.trace.append((self.game.vblank, x, z, event))

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
            if stop_on_dialogue and self.opened():
                game.release()
                return False
            x, z, heading = car_pose(game)
            if math.dist((x, z), goal) < radius:
                game.release()
                return True
            while index < len(path) - 1 and math.dist(path[index], (x, z)) < 12:
                index += 1
            if game.vblank - last_check >= 90:
                # Stuck: on the throttle most of the time and still hardly moving (not merely
                # slowing down for a turn or a target).
                trying = self._throttle_vblanks >= 60
                self._throttle_vblanks = 0
                if trying and math.dist(last, (x, z)) < 1.5:
                    # Stuck against something: remember it, back off, plan again from here.
                    self.map.learn_obstacle(x + 7 * math.cos(heading), z + 7 * math.sin(heading))
                    self.note("stuck")
                    self.stuck_count += 1
                    # Reverse on full lock, longer and to alternating sides on repeats.
                    lock = 0 if self.stuck_count % 2 else 255
                    game.step(40 + 15 * min(4, self.stuck_count), ["circle"], lock)
                    x, z, _ = car_pose(game)
                    path = self.map.path((x, z), goal, self.other_cars(target_slot)) or [goal]
                    index, planned = 0, game.vblank
                last, last_check = (x, z), game.vblank
            if game.vblank - planned > 120:
                # Traffic moves: plan again every 2 s.
                path = self.map.path((x, z), goal, self.other_cars(target_slot)) or [goal]
                index, planned = 0, game.vblank
            tx, tz = path[index]
            self.steer(tx, tz, slow_within=radius + 20 if index >= len(path) - 3 else 0.0)
            self.note()
        game.release()
        self.note("timeout")
        return False

    def enter_door(self, index: int, timeout_vblanks: int = 60 * 90, attempts: int = 4) -> str:
        """Drives in through door `index`; returns who greets us inside ('' if we did not get
        in). NPCs met on the street on the way are talked through and the door tried again."""
        for _ in range(attempts):
            self._enter_door(index, timeout_vblanks)
            if not dialogue_open(self.game):
                return ""
            who = self.wait_for_speaker()
            if self.entered == index:
                return who
            if self.entered is None:
                self.leave_conversation()   # a street NPC stopped us
            else:
                self.leave_building()       # we drove through another building's door
        return ""

    def door_at(self, x: float, z: float) -> int | None:
        for i, quad in enumerate(self.door_quads):
            if in_quad(quad, x, z, margin=4.0):
                return i
        return None

    def wait_for_speaker(self, vblanks: int = 600) -> str:
        """After a door: the inside loads ("NOW LOADING") before anyone speaks."""
        game = self.game
        for _ in range(vblanks // 30):
            who = speaker(game)
            if who:
                return who
            game.run(30)
        return speaker(game)

    def _enter_door(self, index: int, timeout_vblanks: int) -> str:
        game = self.game
        self.entered_pose, self.entered = None, None
        (cx, cz), (nx, nz) = self.doors[index]
        self.door_target = index
        try:
            for side in (1, -1):
                far = (cx + side * nx * 32, cz + side * nz * 32)
                near = (cx + side * nx * 12, cz + side * nz * 12)
                if not self.map.free[self.map.cell(*near)]:
                    continue
                # Line up from further out when there is room (narrow streets often have none).
                stage = far if self.map.free[self.map.cell(*far)] else near
                if not self.drive_to(stage, radius=6, timeout_vblanks=timeout_vblanks):
                    if self.opened():
                        return speaker(game)
                    continue
                # The last stretch is short and straight: steer, don't plan (the map's step
                # limits can rule out a kerb the car crosses easily).
                for _ in range(60):
                    x, z, heading = car_pose(game)
                    if math.dist((x, z), near) < 4 or self.opened():
                        break
                    self.steer(*near, slow_within=20)
                if self.opened():
                    game.release()
                    return speaker(game)
                tx, tz = cx - side * nx * 8, cz - side * nz * 8
                for _ in range(30):
                    self.steer(tx, tz)
                    if self.opened():
                        game.release()
                        game.run(60)
                        return speaker(game)
                game.release()
                for _ in range(5):  # entering loads the inside first ("NOW LOADING")
                    game.run(60)
                    if self.opened():
                        return speaker(game)
                game.step(40, ["circle"], 128)
            return ""
        finally:
            self.door_target = None

    def warp_into(self, index: int) -> str:
        """Enters building `index` without driving there: its door quad is moved to just ahead
        of the car (wound like the game's quads), the car rolls into it, and the real door is
        put back. A coverage-first edit: the inside, its people and their scripts run for real;
        the drive there is skipped (drive_to/enter_door cover driving). Returns the greeter."""
        game = self.game
        if dialogue_open(game):
            finish_dialogue(game, decline=True)  # something stopped us first (a passer-by)
        address = self.door_table_address(index)
        original = game.read(address, 32)
        try:
            # A square just ahead of the car; if a wall stops us, one just behind, in reverse.
            for direction, button in ((1, "cross"), (-1, "circle")):
                x, z, heading = car_pose(game)
                ax = x + direction * 7 * math.cos(heading)
                az = z + direction * 7 * math.sin(heading)
                square = [(ax - 6, az + 6), (ax + 6, az + 6), (ax + 6, az - 6), (ax - 6, az - 6)]
                game.write(address, b"".join(struct.pack("<2f", *p) for p in square))
                for _ in range(20):
                    game.step(10, [button], 128)
                    if dialogue_open(game):
                        break
                game.release()
                if dialogue_open(game):
                    break
            self.entered = index if dialogue_open(game) else None
            return self.wait_for_speaker() if dialogue_open(game) else ""
        finally:
            # Put the real door back: leaving the building places the car at its door. (Parked
            # doors are moved away again by leave_building.)
            game.write(address, self._parked.get(index, original))

    def park_doors(self, keep: int | None = None) -> None:
        """Moves every door of the town far off the map except `keep`, so driving around cannot
        enter a building by accident (for tests that warp into buildings; restore_doors puts
        them back)."""
        game = self.game
        if not self._parked:
            self._parked = {i: game.read(self.door_table_address(i), 32) for i in range(len(self.doors))}
        away = b"".join(struct.pack("<2f", -5000.0 - 10 * k, -5000.0) for k in range(4))
        for i in range(len(self.doors)):
            game.write(self.door_table_address(i), self._parked[i] if i == keep else away)

    def restore_doors(self) -> None:
        for i, original in self._parked.items():
            self.game.write(self.door_table_address(i), original)
        self._parked = {}

    def door_table_address(self, index: int) -> int:
        doors = GAME_MAP["doors"]
        table = struct.unpack(f"<{doors['locations']}I", self.game.read(doors["table_address"], 4 * doors["locations"]))
        return table[self.town] + 32 * index

    def leave_building(self) -> None:
        """Talks through the greeting and backs out of menus (Triangle) until we are driving
        again, then moves out of the doorway."""
        game = self.game
        closed = 0
        seen: list[str] = []
        escapes = silent = 0
        for n in range(80):
            if not dialogue_open(game):
                if game.u8(DIALOGUE["talk_state_address"]) == 1:
                    # Still in a building's menus: some screens (the Paint Shop's colours) have
                    # no message window. Back out of them.
                    game.press("triangle")
                    game.run(60)
                    closed = 0
                    continue
                # Leaving shows NOW LOADING, which raises the flag again: done once we are
                # driving again (talk state 127) and the window stays down.
                closed += 1
                if closed >= 3 and game.u8(DIALOGUE["talk_state_address"]) == FREE_DRIVING:
                    break
                game.run(60)
                continue
            closed = 0
            if not speaker(game) and not text(game).strip() and silent < 3:
                silent += 1
                game.run(60)  # loading: wait, don't press (a few seconds at most)
                continue
            silent = 0
            if not text(game).strip() and not is_choice(game):
                game.run(60)  # a message may be about to start typing
            current = text(game)
            if is_choice(game) or not current.strip():
                # A menu or question, or a screen with no message (a shop's catalogue): back
                # out. (Cross there would pick or buy something.)
                game.press("triangle")
            elif seen.count(current) >= 2:
                # The same message keeps coming back: a menu drawn by the building itself
                # ("<name> / Quit"), where Cross picks the first entry and we return here. Its
                # last entry is the way out (Quit, No, ...). Menus wrap around and differ in
                # size, so each attempt goes one entry further down.
                escapes += 1
                for _ in range(escapes):
                    game.press("down")
                game.press("cross")
                seen.clear()
            else:
                game.press("cross")   # a message: read on
            seen.append(current)
            game.run(60)
        self.clear_doorway()
        if self._parked:
            self.park_doors()  # the door we used was put back for the exit

    def clear_doorway(self) -> None:
        """Out of a building we stand in its doorway, sometimes facing it: move away from the
        nearest door (backwards if it is ahead of us) so we do not roll straight back in."""
        game = self.game
        x, z, heading = car_pose(game)
        if not self.doors:
            game.step(60, ["cross"], 128)
            game.release()
            return
        (dx, dz), _ = min(self.doors, key=lambda d: math.dist(d[0], (x, z)))
        ahead = (dx - x) * math.cos(heading) + (dz - z) * math.sin(heading) > 0
        button = "circle" if ahead else "cross"
        for _ in range(30):  # until 20 units clear (a car from rest is slow to get going)
            if math.dist((dx, dz), car_pose(game)[:2]) > 20 or dialogue_open(game):
                break
            game.step(10, [button], 128)
        game.release()
        game.run(30)

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
                who = self.wait_for_speaker(120)
                self.note(f"met {who}")
                if name is None or who == name:
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
    """A message window is up: the window pointer points into the conversation slots."""
    lo, hi = DIALOGUE["window_range"]
    return lo <= game.u32(DIALOGUE["window_address"]) < hi


def c_string(game: "Game", address: int, limit: int = 160) -> str:
    raw = game.read(address, limit)
    return raw.split(b"\0", 1)[0].decode("latin-1")


def speaker(game: "Game") -> str:
    """Name of the NPC or place talking, or '' if none. The message window points at the
    conversation slot of whoever is talking (a slot per car slot; buildings use their own); the
    slot holds their NPC entry, whose first word points at the name."""
    window = game.u32(DIALOGUE["window_address"])
    if not 0x200000 <= window < 0x2000000:
        return ""
    entry = game.u32(window + DIALOGUE["window_entry"])
    if not 0x200000 <= entry < 0x2000000:
        return ""
    name = game.u32(entry)
    if not 0x200000 <= name < 0x2000000:
        return ""
    return c_string(game, name, 24)


def text(game: "Game") -> str:
    """The script text at the dialogue cursor (printable part)."""
    cursor = game.u32(DIALOGUE["cursor_address"])
    if not 0x200000 <= cursor < 0x2000000:
        return ""
    raw = game.read(cursor, 200)
    return "".join(chr(b) for b in raw.split(b"\0", 1)[0] if 32 <= b < 127 or b == 10)


def is_choice(game: "Game") -> bool:
    """The message on screen offers choices (a menu or a question): its script text holds the
    0x09 choice marker."""
    cursor = game.u32(DIALOGUE["cursor_address"])
    if not 0x200000 <= cursor < 0x2000000:
        return False
    # The cursor can sit anywhere in the current message: look back to its start too.
    raw = game.read(cursor - 0x80, 0x180)
    start = raw.rfind(b"\0", 0, 0x80) + 1
    message = raw[start:].split(b"\0", 1)[0]
    return b"\x09" in message


def finish_dialogue(game: "Game", presses: int = 40, answer=None, decline: bool = False) -> None:
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
        if decline and is_choice(game):
            game.press("triangle")  # say no / back out: we are only passing by
            game.run(60)
            continue
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
