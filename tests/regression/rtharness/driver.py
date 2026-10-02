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
    lookahead: float = 14.0     # longest distance along the trail to aim at (at speed)
    tight_bend: float = 0.6     # radians of turn within the look-ahead above which it shrinks
    min_lookahead: float = 6.0
    backtrack: bool = False         # back out of wrong branches (it can hurt on wide tracks)
    off_line: float = 30.0          # this far from the line ...
    off_line_vblanks: int = 180     # ... for this long -> back up along our own path
    off_line_progress: int = 3      # ... unless we moved this many line points on meanwhile
    back_on_line: float = 7.0       # stop backing up once this close to the line again
    backtrack_vblanks: int = 600
    backtrack_lead: int = 6         # aim at the breadcrumb this many back
    steer_gain: float = 2.2     # stick deflection per radian of heading error
    brake_angle: float = 0.9    # radians of heading error above which we brake
    lift_angle: float = 0.5     # ... and above which we lift off the accelerator
    trail_spacing: float = 2.0  # record a trail point every this many units
    # Corner speeds (units per vblank; ~1 unit/vblank is about 100 mph) by how far the racing line
    # turns (radians) over the stretch ahead.
    corner_speeds: tuple = ((0.35, 9.0), (0.8, 0.85), (1.3, 0.62), (2.0, 0.48), (9.0, 0.36))
    preview_vblanks: int = 50   # look this far ahead (at the current speed) for corners
    leader_speed_factor: float = 0.95  # corner at this fraction of the AI leader's speed
    follow: str = "near"
    wait_for_line: bool = True  # sit out the AI's first lap to learn the whole line first
    start_grace: int = 120      # no stuck detection this soon after starting to drive
    crawl_speed: float = 0.15   # below this (units/vblank) always accelerate
    overtake: bool = False      # read all cars every step to steer around ones ahead (slower)        # which AI car's line to learn: "near" (slowest, safest) or "far"
    braking_zone: float = 0.85  # the leader below this fraction of its top speed = a corner
    pass_offset: float = 3.0    # aim this far beside a car blocking our line
    pass_distance: float = 18.0 # ... when it is this close ahead
    stuck_vblanks: int = 45     # no progress for this long -> back out
    reverse_vblanks: int = 60   # how long to reverse when stuck
    nudge_after: int = 99       # (nudging doesn't work: the game rebuilds the car's position)
    stanley: bool = False       # steer along the line (Stanley) instead of at a point ahead (pure pursuit)
    stanley_gain: float = 2.5
    preview_steer: float = 12.0  # line direction taken this many vblanks ahead at our speed
    nudge_distance: float = 20.0


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
    line_distance: float = 0.0
    off_line_since: int | None = None
    off_line_index: int = 0
    backtracking_until: int = 0
    breadcrumbs: list = field(default_factory=list)  # our recent positions, for backing out
    backtracks: int = 0
    last_stuck: tuple | None = None
    stuck_streak: int = 0
    nudges: int = 0
    loop_closed: bool = False  # the trail is one full lap; follow it round and round
    waited: bool = False
    drive_start: int | None = None
    times: list = field(default_factory=list)  # vblank at which the leader passed each trail point
    lap_vblanks: int = 0
    top_speed: float = 0.0

    def _read_self(self):
        """Our position and facing (from the front and rear wheel positions, car +0xB0 and
        +0xD0) in one read."""
        raw = self.game.read(CARS_BASE, 0xE0)
        x, _, z = struct.unpack_from("<3f", raw, 0)
        fx, _, fz = struct.unpack_from("<3f", raw, 0xB0)
        rx, _, rz = struct.unpack_from("<3f", raw, 0xD0)
        return (x, z), math.atan2(fz - rz, fx - rx)

    def _car_heading(self) -> float:
        return self._read_self()[1]

    def _act(self, buttons: list[str], lx: int = 128):
        """Set the pad, run one control step and read the state for the next one, in one round
        trip (Game.step)."""
        reads = [(CARS_BASE, 0xE0)]
        if self.leader is not None:
            reads.append((CARS_BASE + self.leader * CAR_STRIDE, 12))
        data = self.game.step(self.config.step, buttons, lx, reads)
        self._cached = data

    def _state(self):
        """Our (position, heading) and the leader's position, from the last step's reads."""
        data = getattr(self, "_cached", None)
        if not data or (self.leader is not None and len(data) < 2):
            me, heading = self._read_self()
            return me, heading, self._leader_position() if self.leader is not None else None
        raw = data[0]
        x, _, z = struct.unpack_from("<3f", raw, 0)
        fx, _, fz = struct.unpack_from("<3f", raw, 0xB0)
        rx, _, rz = struct.unpack_from("<3f", raw, 0xD0)
        leader = None
        if self.leader is not None:
            lx_, _, lz = struct.unpack("<3f", data[1])
            leader = (lx_, lz)
        return (x, z), math.atan2(fz - rz, fx - rx), leader

    def _leader_position(self):
        x, _, z = struct.unpack("<3f", self.game.read(CARS_BASE + self.leader * CAR_STRIDE, 12))
        return (x, z)

    def positions(self) -> list[tuple[float, float]]:
        raw = self.game.read(CARS_BASE, CAR_STRIDE * CAR_COUNT)
        out = []
        for k in range(CAR_COUNT):
            x, _, z = struct.unpack_from("<3f", raw, k * CAR_STRIDE)
            out.append((x, z))
        return out

    def _record_trail(self, cars):
        if self.leader is None:
            # Learn the line from an AI car: by default the one starting closest to us (the
            # slowest of the field, whose corner speeds our car can match); "far" = the pole car.
            me = cars[0]
            pick = max if self.config.follow == "far" else min
            self.leader = pick(range(1, CAR_COUNT), key=lambda k: math.dist(cars[k], me))
        if self.loop_closed:
            return
        p = cars[self.leader]
        if not self.trail or math.dist(self.trail[-1], p) >= self.config.trail_spacing:
            self.trail.append(p)
            self.times.append(self.game.vblank)
            if len(self.trail) > 5:
                self.top_speed = max(self.top_speed, self._leader_speed(len(self.trail) - 6))
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

    def _target(self, me, speed: float = 0.5):
        # Advance our place on the trail (closest point, searching forward only).
        n = len(self.trail)
        wrap = self.loop_closed
        best, best_d = self.index, math.inf
        for j in range(self.index, self.index + 60 if wrap else min(n, self.index + 60)):
            d = math.dist(self.trail[j % n], me)
            if d < best_d:
                best, best_d = j, d
        self.index = best % n if wrap else best
        self.line_distance = best_d
        # Aim along the trail: further at speed, close in when slow or off the line, so the car
        # follows the AI's line around obstacles instead of cutting across them.
        lookahead = self.config.lookahead
        # Tight sections (the line turns a lot within the look-ahead): aim closer, so we follow
        # the corner instead of cutting into its wall.
        self.index = self.index if not wrap else self.index % n
        bend = self._corner_angle(lookahead)
        if bend > self.config.tight_bend:
            lookahead = max(self.config.min_lookahead, lookahead * self.config.tight_bend / bend)
        dist, i = 0.0, self.index
        while (wrap or i + 1 < n) and dist < lookahead:
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
        v = math.dist(self.trail[i % n], self.trail[j % n]) / max(1, dt)
        # The leader's standing start (and any crawl) is not a corner: ignore it.
        if self.top_speed and v < 0.25 * self.top_speed:
            return self.top_speed
        return v

    def _target_speed(self, speed: float) -> float:
        """Speed to hold: the slowest the leader drove over the stretch we will reach within the
        preview, but only in braking zones (where the leader was well below its top speed);
        elsewhere flat out."""
        end = self._path_point(self.index, max(20.0, speed * self.config.preview_vblanks))
        slowest = min((self._leader_speed(i) for i in range(self.index, max(self.index + 1, end))), default=9.0)
        if self.top_speed and slowest > self.config.braking_zone * self.top_speed:
            return 9.0
        return slowest * self.config.leader_speed_factor

    def _stanley_target(self, me, speed: float):
        """Stanley-style steering expressed as a target point: follow the line's direction at a
        short preview, plus a correction towards the line proportional to how far off it we are."""
        n = len(self.trail)
        i = self.index
        j = self._path_point(i, max(3.0, speed * self.config.preview_steer))
        a, b = self.trail[i % n], self.trail[(i + 2) % n]
        c, d = self.trail[j % n], self.trail[(j + 2) % n]
        path_heading = math.atan2(d[1] - c[1], d[0] - c[0])
        tx, tz = b[0] - a[0], b[1] - a[1]
        tl = math.hypot(tx, tz) or 1.0
        # Signed distance from the line (positive = line is to our left in x/z).
        cross = ((me[0] - a[0]) * tz - (me[1] - a[1]) * tx) / tl
        correction = math.atan2(self.config.stanley_gain * cross, speed * 60.0 + 1.0)
        want = path_heading + correction
        return (me[0] + 10.0 * math.cos(want), me[1] + 10.0 * math.sin(want))

    def _backtrack(self, me, now) -> bool:
        """Far off the line for a while (a wrong branch, a dead end): reverse back along our own
        breadcrumbs until we are near the line again. Returns True while backing up."""
        if len(self.trail) < 20 or not self.loop_closed:
            return False
        self._target(me)  # refreshes index and line_distance
        if now < self.backtracking_until:
            # The forward-only index search above can have jumped ahead (we cut a corner, so a
            # later stretch of the line came nearest); measure against the line behind us too and
            # re-anchor there once we are back on it.
            back = self._nearest_behind(me)
            if math.dist(self.trail[back], me) < self.config.back_on_line:
                self.index = back
                self.line_distance = math.dist(self.trail[self.index], me)
                self.backtracking_until = 0
                self.off_line_since = None
                self.history.clear()
                return False
            # Steer so the car's rear points at a breadcrumb a little way back.
            crumb = self.breadcrumbs[max(0, len(self.breadcrumbs) - 1 - self.config.backtrack_lead)]
            if len(self.breadcrumbs) > self.config.backtrack_lead and math.dist(crumb, me) < 2.0:
                del self.breadcrumbs[-self.config.backtrack_lead:]
            want = math.atan2(crumb[1] - me[1], crumb[0] - me[0])
            err = (want - (self.heading + math.pi) + math.pi) % (2 * math.pi) - math.pi
            lx = int(max(0, min(255, 128 + 127 * max(-1.0, min(1.0, err * self.config.steer_gain)))))
            self._act(["circle"], lx)
            return True
        if self.line_distance > self.config.off_line:
            # Often we are on the road but the forward-only search jumped ahead (we cut a
            # corner, so a later stretch came nearest): look behind too before backing up.
            back = self._nearest_behind(me)
            if math.dist(self.trail[back], me) < self.config.off_line:
                self.index = back
                self.line_distance = math.dist(self.trail[back], me)
                self.off_line_since = None
                return False
            # Off the line on its own is fine on wide roads; a wrong branch also stops our
            # progress along the line.
            if not self.config.backtrack:
                pass
            elif self.off_line_since is None or self._line_advance(self.off_line_index) > self.config.off_line_progress:
                self.off_line_since = now
                self.off_line_index = self.index
            elif now - self.off_line_since > self.config.off_line_vblanks:
                self.backtracks += 1
                self.backtracking_until = now + self.config.backtrack_vblanks
                self.off_line_since = None
        else:
            self.off_line_since = None
        return False

    def _nearest_behind(self, me) -> int:
        """Nearest line point from a stretch behind our index to a little ahead of it."""
        n = len(self.trail)
        return min(range(self.index - 250, self.index + 60), key=lambda j: math.dist(self.trail[j % n], me)) % n

    def _line_advance(self, since: int) -> int:
        """Points moved forward along the (looped) line since index `since`."""
        n = len(self.trail)
        return (self.index - since) % n if self.loop_closed else self.index - since

    def _nudge_forward(self):
        """Last resort when the car is pinned: put it a little further along the AI's line
        (counted in `nudges`; the race itself is then completed normally)."""
        n = len(self.trail)
        i = self._path_point(self.index, self.config.nudge_distance)
        a, b = self.trail[i % n], self.trail[(i + 2) % n]
        y = struct.unpack("<f", self.game.read(CARS_BASE + 4, 4))[0]
        self.game.write(CARS_BASE, struct.pack("<3f", a[0], y + 0.5, a[1]))
        self.index = i % n if self.loop_closed else min(i, n - 1)
        self.heading = math.atan2(b[1] - a[1], b[0] - a[0])
        self.nudges += 1
        self.stuck_streak = 0
        self.reversing_until = 0
        self.history.clear()

    def step(self):
        """One control update: read state, set the pad, run `step` vblanks."""
        me, heading, leader_pos = self._state()
        # All cars are only needed to choose the leader and to overtake; otherwise ours and the
        # leader's come back with each step.
        cars = self.positions() if self.leader is None or self.config.overtake else None
        if cars is None:
            cars = {0: me, self.leader: leader_pos}
        self._record_trail(cars)
        now = self.game.vblank
        if self.config.wait_for_line and not self.loop_closed:
            # Stay on the grid while the AI car completes a lap: the line is then complete and
            # the field is gone, so we drive the race alone and out of start traffic.
            self._act([])
            self.waited = True
            return me
        if self.waited:
            # Start from the trail point nearest the grid position.
            self.waited = False
            self.index = min(range(len(self.trail)), key=lambda i: math.dist(self.trail[i], me))
            self.history.clear()
            self.drive_start = now
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
            if self.stuck_streak % 2 == 0:
                # The last back-out at this spot failed (often: we reversed into something), so
                # this time drive forwards on full lock towards the line.
                self._act(["cross"], 0 if lx == 255 else 255 if lx == 0 else lx)
            else:
                self._act(["circle"], lx)
            self.last_pos = None
            return me
        if self.drive_start is None:
            self.drive_start = now
        if not self.breadcrumbs or math.dist(self.breadcrumbs[-1], me) > 1.5:
            self.breadcrumbs.append(me)
            del self.breadcrumbs[:-400]
        if self._backtrack(me, now):
            return me
        if not self.history:
            self.history.append((now, me))
        span = now - self.history[0][0]
        if (len(self.trail) > 10 and now - self.drive_start > self.config.start_grace and span >= self.config.stuck_vblanks - self.config.step
                and math.dist(self.history[0][1], me) < 1.0):
            self.recoveries += 1
            # Repeated failures at the same spot: back out further each time.
            again = self.last_stuck is not None and math.dist(self.last_stuck, me) < 15.0
            self.stuck_streak = self.stuck_streak + 1 if again else 1
            self.last_stuck = me
            self.reversing_until = now + self.config.reverse_vblanks * min(4, self.stuck_streak)
            if self.stuck_streak >= self.config.nudge_after and len(self.trail) > 20:
                self._nudge_forward()
            self.history.clear()
        self.heading = heading
        self.last_pos = me
        buttons = ["cross"]
        lx = 128
        if self.heading is not None and len(self.trail) > 2:
            speed = math.dist(self.history[-2][1], me) / max(1, now - self.history[-2][0]) if len(self.history) > 1 else 0.0
            tx, tz = self._target(me, speed)
            if self.config.stanley:
                tx, tz = self._stanley_target(me, speed)
            dx, dz = tx - me[0], tz - me[1]
            length = math.hypot(dx, dz) or 1.0
            ux, uz = dx / length, dz / length
            for k, c in (enumerate(cars[1:], 1) if isinstance(cars, list) else []):
                ax, az = c[0] - me[0], c[1] - me[1]
                ahead = ax * ux + az * uz
                side = -ax * uz + az * ux
                if 0 < ahead < self.config.pass_distance and abs(side) < 2.5:
                    # Pass on the side away from it.
                    shift = self.config.pass_offset * (-1 if side > 0 else 1)
                    tx, tz = tx - uz * shift, tz + ux * shift
                    break
            want = math.atan2(tz - me[1], tx - me[0])
            err = (want - self.heading + math.pi) % (2 * math.pi) - math.pi
            # Stick right turns towards smaller angles in (x, z).
            lx = int(max(0, min(255, 128 - 127 * max(-1.0, min(1.0, err * self.config.steer_gain)))))
            speed = math.dist(self.history[-2][1], me) / max(1, now - self.history[-2][0]) if len(self.history) > 1 else 0.0
            target = self._target_speed(speed)
            if speed < self.config.crawl_speed:
                buttons = ["cross"]  # lifting or braking only makes sense when moving
            elif abs(err) > self.config.brake_angle or speed > target * 1.12:
                buttons = ["square"]
            elif abs(err) > self.config.lift_angle or speed > target:
                buttons = []
        self._act(buttons, lx)
        return me

    def drive(self, vblanks: int):
        end = self.game.vblank + vblanks
        while self.game.vblank < end:
            self.step()
