#!/usr/bin/env python3
"""Lockstep TOWN/RACE probes for the Android native facade (development APK only)."""

from __future__ import annotations

import argparse
import json
import math
import socket
import struct
import subprocess
import sys
import time
from pathlib import Path

TILE_WARP = 0x00335910
CARS = 0x0177ACE0
CLOCK = 0x0021BD20 + 0x14
RACE_PLACE = 0x015B0088
CROSS = 0x4000


def position_distance(a: tuple[float, float], b: tuple[float, float]) -> float:
    return math.dist(a, b)


class NativeSocket:
    def __init__(self, host: str, port: int) -> None:
        self.sock = socket.create_connection((host, port), timeout=10)
        self.file = self.sock.makefile("rwb", buffering=0)

    def command(self, cmd: str, timeout: float | None = None, **values):
        # The first step/step-like command after attach triggers lazy scene
        # construction on device (measured ~16 s on the Thor: index + five
        # textured tiles); the default 10 s socket timeout killed the town and
        # race scenarios while the build was legitimately in flight.
        payload = {"cmd": cmd, **values}
        if timeout is not None:
            self.sock.settimeout(timeout)
        self.file.write((json.dumps(payload, separators=(",", ":")) + "\n").encode())
        line = self.file.readline()
        if timeout is not None:
            self.sock.settimeout(10)
        if not line:
            raise RuntimeError(f"socket closed awaiting {cmd}")
        answer = json.loads(line)
        if not answer.get("ok"):
            raise RuntimeError(f"{cmd}: {answer.get('error', 'unknown error')}")
        return answer

    def read(self, address: int, length: int) -> bytes:
        return bytes.fromhex(self.command("read", addr=address, len=length)["data"])

    def write(self, address: int, data: bytes) -> bool:
        payload = {"cmd": "write", "addr": address, "data": data.hex()}
        self.file.write((json.dumps(payload, separators=(",", ":")) + "\n").encode())
        line = self.file.readline()
        if not line:
            raise RuntimeError("socket closed awaiting write")
        return bool(json.loads(line).get("ok"))

    def step(self, ticks: int) -> None:
        self.command("step", timeout=180, vblanks=ticks)

    def pad(self, *, buttons: int = 0xFFFF, lx: int = 128) -> None:
        self.command("pad", buttons=buttons, lx=lx, ly=128, rx=128, ry=128, connected=1)

    def release(self) -> None:
        self.command("release_pad")

    def tile(self) -> int:
        return int(self.command("map")["tile"])

    def position(self) -> tuple[float, float]:
        x, _, z = struct.unpack("<3f", self.read(CARS, 12))
        return x, z

    def clock(self) -> int:
        return struct.unpack("<I", self.read(CLOCK, 4))[0]


def capture(serial: str, destination: Path, name: str) -> None:
    destination.mkdir(parents=True, exist_ok=True)
    output = destination / name
    with output.open("wb") as png:
        subprocess.run(["adb", "-s", serial, "exec-out", "screencap", "-p"], check=True, stdout=png)


def native_gap(result: dict, message: str) -> None:
    result["gaps"].append(message)
    print(f"NATIVE-MODEL GAP: {message}")


def town(sock: NativeSocket, result: dict, serial: str, screenshots: Path) -> None:
    item = result["town"] = {"status": "FAIL"}
    sock.step(120)
    current = sock.tile()
    if current != 43:
        if not sock.write(TILE_WARP, struct.pack("<I", 43)):
            native_gap(result, "Town warp rejected; no route-to-town facade is exposed.")
        sock.step(120)
        current = sock.tile()
    if current != 43:
        raise RuntimeError(f"town tile expected 43, got {current}")
    capture(serial, screenshots, "town-ready.png")
    before = sock.position()
    sock.pad(buttons=0xFFFF & ~CROSS)
    sock.step(180)
    after = sock.position()
    sock.release()
    sock.step(2)
    moved = position_distance(before, after)
    # The facade intentionally exposes position, not an authoritative car-speed field.
    if moved <= 0.0:
        raise RuntimeError(f"town gas did not move car: {before} -> {after}")
    native_gap(result, f"No speed facade; town movement asserted from position delta {moved:.3f} over 180 ticks.")
    native_gap(result, "Town drive v1 physics runs (tickPlayer free-drive); Peach-Town-local spawn anchor pending.")
    native_gap(result, "Doors/interiors/traffic are not implemented by the native model.")
    item.update(status="PASS", tile=current, position_before=before, position_after=after,
                speed_estimate=moved * 60.0 / 180.0)


def race(sock: NativeSocket, result: dict, serial: str, screenshots: Path) -> None:
    item = result["race"] = {"status": "GAP"}
    # kRacePlace is a read-only rank mirror, not a race-state control. Try it only
    # to document the facade boundary; then make the native menu attempt.
    if not sock.write(RACE_PLACE, struct.pack("<I", 1)):
        native_gap(result, "Race start facade is unavailable (kRacePlace is read-only rank).")
    for _ in range(2):
        sock.pad(buttons=0xFFFF & ~CROSS)
        sock.step(2)
        sock.release()
        sock.step(60)
    start_clock, before = sock.clock(), sock.position()
    for tick in range(0, 300, 15):
        sock.pad(buttons=0xFFFF & ~CROSS, lx=80 if (tick // 15) % 2 else 176)
        sock.step(15)
    sock.release()
    end_clock, after = sock.clock(), sock.position()
    capture(serial, screenshots, "race-probe.png")
    moved = position_distance(before, after)
    clock_advanced = end_clock != start_clock
    # Rank is observable but does not distinguish a live race from the native
    # PLAYING scene, so this remains a GAP rather than a fabricated race pass.
    rank = struct.unpack("<I", sock.read(RACE_PLACE, 4))[0]
    native_gap(result, "Race state/timer facade is unavailable; rank does not prove a race started.")
    if not clock_advanced:
        native_gap(result, "Clock did not advance during race probe.")
    if moved <= 0.0:
        native_gap(result, "Car did not move during race probe.")
    item.update(clock_before=start_clock, clock_after=end_clock, clock_advanced=clock_advanced,
                position_before=before, position_after=after, speed_estimate=moved / 5.0,
                rank=rank)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", type=int, default=9876)
    parser.add_argument("--serial", required=True)
    parser.add_argument("--screenshots", type=Path, required=True)
    parser.add_argument("--result", type=Path, required=True)
    args = parser.parse_args()
    result: dict = {"title": {"status": "FAIL"}, "town": {"status": "FAIL"},
                    "race": {"status": "FAIL"}, "gaps": [], "socket_closed": False}
    sock = None
    try:
        # The setup activity emits "assets: ready" before SDL's native thread
        # has created the listener.  adb forwards an early connection and then
        # closes it, so retry the hello handshake rather than treating that
        # expected launch window as a scenario failure.  Cold scene startup on
        # device (terrain build + shader compile) measured >18 s on desktop CPU
        # and longer on Thor, so allow a 3-minute window with progress echoes.
        deadline = time.monotonic() + 180
        last_error = None
        next_echo = 0.0
        while time.monotonic() < deadline:
            try:
                sock = NativeSocket("127.0.0.1", args.port)
                result["hello"] = sock.command("hello")
                break
            except (OSError, RuntimeError) as exc:
                last_error = exc
                if sock:
                    sock.sock.close()
                    sock = None
                if time.monotonic() >= next_echo:
                    print(f"  hello retry: {last_error} "
                          f"(t+{180 - (deadline - time.monotonic()):.0f}s)")
                    next_echo = time.monotonic() + 15
                time.sleep(1)
        if sock is None:
            raise RuntimeError(f"socket hello timeout after 180s: {last_error}")
        sock.step(1)
        result["title"] = {"status": "PASS", "version": result["hello"].get("version")}
        town(sock, result, args.serial, args.screenshots)
        race(sock, result, args.serial, args.screenshots)
        sock.command("quit")
        sock.sock.settimeout(5)
        result["socket_closed"] = sock.sock.recv(1) == b""
        if not result["socket_closed"]:
            raise RuntimeError("socket stayed open after quit")
    except Exception as exc:
        result["error"] = str(exc)
        print(f"SCENARIO ERROR: {exc}", file=sys.stderr)
    finally:
        if sock:
            sock.sock.close()
        args.result.parent.mkdir(parents=True, exist_ok=True)
        args.result.write_text(json.dumps(result, indent=2, sort_keys=True) + "\n")
    return 0 if result["town"]["status"] == "PASS" and result["socket_closed"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
