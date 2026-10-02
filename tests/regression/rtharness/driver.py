"""A driving bot for races: follows the racing line of the game's own AI cars.

It records the trail of an AI car (their live positions are in RAM, see config/game_state.toml) and
steers our car towards a point a little further along that trail (pure pursuit), holding the
accelerator and lifting/braking for sharp turns. No knowledge of the course is needed.
"""

from __future__ import annotations

import math
import struct
from dataclasses import dataclass, field
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from . import Game

CARS_BASE = 0x0177ACE0  # position (x, y, z) of car 0 (ours); cars follow at CAR_STRIDE
CAR_STRIDE = 0x270
CAR_COUNT = 24


@dataclass
class DriverConfig:
    step: int = 2               # vblanks between control updates
    lookahead: float = 14.0     # distance along the trail to aim at
    steer_gain: float = 2.2     # stick deflection per radian of heading error
    brake_angle: float = 0.9    # radians of heading error above which we brake
    lift_angle: float = 0.5     # ... and above which we lift off the accelerator
    trail_spacing: float = 2.0  # record a trail point every this many units
    # Corner speeds (units per vblank; ~1 unit/vblank is about 100 mph) by how far the racing line
    # turns (radians) over the stretch ahead.
    corner_speeds: tuple = ((0.35, 9.0), (0.8, 0.85), (1.3, 0.62), (2.0, 0.48), (9.0, 0.36))
    preview_vblanks: int = 50   # look this far ahead (at the current speed) for corners
    leader_speed_factor: float = 0.92  # drive at this fraction of the AI leader's speed
    stuck_vblanks: int = 45     # no progress for this long -> back out
    reverse_vblanks: int = 60   # how long to reverse when stuck


@dataclass
class Driver:
    game: "Game"
    config: DriverConfig = field(default_factory=DriverConfig)
    leader: int | None = None
    trail: list = field(default_factory=list)
    index: int = 0
    last_pos: tuple | None = None
    heading: float | None = None
    history: list = field(default_factory=list)  # (vblank, position) of recent steps
    reversing_until: int = 0
    recoveries: int = 0
    last_stuck: tuple | None = None
    stuck_streak: int = 0
    loop_closed: bool = False  # the trail is one full lap; follow it round and round
    times: list = field(default_factory=list)  # vblank at which the leader passed each trail point
    lap_vblanks: int = 0

    def positions(self) -> list[tuple[float, float]]:
        raw = self.game.read(CARS_BASE, CAR_STRIDE * CAR_COUNT)
        out = []
        for k in range(CAR_COUNT):
            x, _, z = struct.unpack_from("<3f", raw, k * CAR_STRIDE)
            out.append((x, z))
        return out

    def _record_trail(self, cars):
        if self.leader is None:
            # Follow the car that starts furthest ahead of us.
            me = cars[0]
            self.leader = max(range(1, CAR_COUNT), key=lambda k: math.dist(cars[k], me))
        if self.loop_closed:
            return
        p = cars[self.leader]
        if not self.trail or math.dist(self.trail[-1], p) >= self.config.trail_spacing:
            self.trail.append(p)
            self.times.append(self.game.vblank)
            # One lap done when the leader comes back to where its trail began. Later laps (and
            # its cool-down after the finish, off the racing line) are not recorded.
            if len(self.trail) > 200:
                head = self.trail[:100]
                j = min(range(len(head)), key=lambda i: math.dist(head[i], p))
                if math.dist(head[j], p) < 4 * self.config.trail_spacing:
                    # Drop the run-up from the grid; the rest is one lap of racing line.
                    self.trail = self.trail[j:-1]
                    self.times = self.times[j:-1]
                    self.lap_vblanks = self.game.vblank - self.times[0]
                    self.index = max(0, self.index - j)
                    self.loop_closed = True

    def _target(self, me):
        # Advance our place on the trail (closest point, searching forward only).
        n = len(self.trail)
        wrap = self.loop_closed
        best, best_d = self.index, math.inf
        for j in range(self.index, self.index + 60 if wrap else min(n, self.index + 60)):
            d = math.dist(self.trail[j % n], me)
            if d < best_d:
                best, best_d = j, d
        self.index = best % n if wrap else best
        # Aim `lookahead` along the trail.
        dist, i = 0.0, self.index
        while (wrap or i + 1 < n) and dist < self.config.lookahead:
            dist += math.dist(self.trail[i % n], self.trail[(i + 1) % n])
            i += 1
        return self.trail[i % n]

    def _path_point(self, start: int, distance: float) -> int:
        n = len(self.trail)
        dist, i = 0.0, start
        while (self.loop_closed or i + 1 < n) and dist < distance:
            dist += math.dist(self.trail[i % n], self.trail[(i + 1) % n])
            i += 1
        return i

    def _corner_angle(self, distance: float) -> float:
        """Total turning (radians) of the racing line from here to `distance` ahead."""
        n = len(self.trail)
        end = self._path_point(self.index, distance)
        total, prev = 0.0, None
        for i in range(self.index, end - 2, 3):
            if not self.loop_closed and i + 3 >= n:
                break
            a, b = self.trail[i % n], self.trail[(i + 3) % n]
            if a == b:
                continue
            h = math.atan2(b[1] - a[1], b[0] - a[0])
            if prev is not None:
                total += abs((h - prev + math.pi) % (2 * math.pi) - math.pi)
            prev = h
        return total

    def _leader_speed(self, i: int) -> float:
        """The leader's speed (units per vblank) around trail point i."""
        n = len(self.trail)
        j = i + 4
        if not self.loop_closed and j >= n:
            return 9.0
        dt = self.times[j % n] - self.times[i % n]
        if dt <= 0:
            dt += self.lap_vblanks
        return math.dist(self.trail[i % n], self.trail[j % n]) / max(1, dt)

    def _target_speed(self, speed: float) -> float:
        """Slowest speed the leader drove over the stretch we will reach within the preview."""
        end = self._path_point(self.index, max(20.0, speed * self.config.preview_vblanks))
        slowest = min((self._leader_speed(i) for i in range(self.index, max(self.index + 1, end))), default=9.0)
        return slowest * self.config.leader_speed_factor

    def step(self):
        """One control update: read state, set the pad, run `step` vblanks."""
        cars = self.positions()
        me = cars[0]
        self._record_trail(cars)
        now = self.game.vblank
        self.history.append((now, me))
        while self.history and self.history[0][0] < now - self.config.stuck_vblanks:
            self.history.pop(0)
        if now < self.reversing_until:
            # Back out with the wheels turned away from where we want to go.
            lx = 128
            halfway = self.reversing_until - self.config.reverse_vblanks // 2
            if now >= halfway and self.heading is not None and len(self.trail) > 2:
                tx, tz = self._target(me)
                err = (math.atan2(tz - me[1], tx - me[0]) - self.heading + math.pi) % (2 * math.pi) - math.pi
                lx = 255 if err > 0 else 0
            self.game.pad("circle", lx=lx)
            self.game.run(self.config.step)
            self.last_pos = None
            return me
        span = now - self.history[0][0]
        if (len(self.trail) > 10 and span >= self.config.stuck_vblanks - self.config.step
                and math.dist(self.history[0][1], me) < 1.0):
            self.recoveries += 1
            # Repeated failures at the same spot: back out further each time.
            again = self.last_stuck is not None and math.dist(self.last_stuck, me) < 15.0
            self.stuck_streak = self.stuck_streak + 1 if again else 1
            self.last_stuck = me
            self.reversing_until = now + self.config.reverse_vblanks * min(4, self.stuck_streak)
            self.history.clear()
        if self.last_pos and math.dist(self.last_pos, me) > 0.05:
            self.heading = math.atan2(me[1] - self.last_pos[1], me[0] - self.last_pos[0])
        self.last_pos = me
        buttons = ["cross"]
        lx = 128
        if self.heading is not None and len(self.trail) > 2:
            tx, tz = self._target(me)
            want = math.atan2(tz - me[1], tx - me[0])
            err = (want - self.heading + math.pi) % (2 * math.pi) - math.pi
            # Stick right turns towards smaller angles in (x, z).
            lx = int(max(0, min(255, 128 - 127 * max(-1.0, min(1.0, err * self.config.steer_gain)))))
            speed = math.dist(self.history[-2][1], me) / max(1, now - self.history[-2][0]) if len(self.history) > 1 else 0.0
            target = self._target_speed(speed)
            if abs(err) > self.config.brake_angle or speed > target * 1.12:
                buttons = ["square"]
            elif abs(err) > self.config.lift_angle or speed > target:
                buttons = []
        self.game.pad(*buttons, lx=lx)
        self.game.run(self.config.step)
        return me

    def drive(self, vblanks: int):
        end = self.game.vblank + vblanks
        while self.game.vblank < end:
            self.step()
