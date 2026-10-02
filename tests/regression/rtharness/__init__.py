"""Drive RoadTrip.app from Python for regression tests.

The app runs in deterministic test mode (virtual time, fake clock, headless) and in lockstep with
the test: the game parks at a vblank until the test asks it to run further, so memory reads, pad
changes and frame grabs happen between two exact vblanks. See
third_party/PS2Recomp/ps2xRuntime/include/runtime/ps2_test_harness.h for the protocol.
"""

from __future__ import annotations

import json
import os
import shutil
import socket
import struct
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]
DEFAULT_APP = REPO / "build/macos-release/RoadTrip.app/Contents/MacOS/RoadTrip"
VBLANKS_PER_SECOND = 60000 / 1001

# DualShock 2 buttons (active-low bits in the pad data).
BUTTONS = {
    "select": 0x0001, "l3": 0x0002, "r3": 0x0004, "start": 0x0008,
    "up": 0x0010, "right": 0x0020, "down": 0x0040, "left": 0x0080,
    "l2": 0x0100, "r2": 0x0200, "l1": 0x0400, "r1": 0x0800,
    "triangle": 0x1000, "circle": 0x2000, "cross": 0x4000, "square": 0x8000,
}


def seconds(s: float) -> int:
    """Guest seconds -> vblanks."""
    return round(s * VBLANKS_PER_SECOND)


class GameError(RuntimeError):
    pass


@dataclass
class Frame:
    width: int
    height: int
    rgba: bytes

    def image(self):
        from PIL import Image

        return Image.frombytes("RGBA", (self.width, self.height), self.rgba).convert("RGB")

    def array(self):
        import numpy as np

        return np.frombuffer(self.rgba, dtype=np.uint8).reshape(self.height, self.width, 4)[:, :, :3]


class Game:
    """One running instance of the game.

    `base_data` is an installed data directory (extracted disc + built game library); the instance
    gets its own directory with links to those and its own memory cards, optionally seeded from a
    checkpoint (a saved mc0 directory).
    """

    def __init__(self, base_data: Path, work_dir: Path, *, checkpoint: Path | None = None,
                 speed: str = "max", fake_clock: int = 1_000_000_000, app: Path = DEFAULT_APP,
                 state_hash: bool = False, env: dict | None = None):
        self.base_data = Path(base_data)
        self.work_dir = Path(work_dir)
        self.checkpoint = checkpoint
        self.speed = speed
        self.fake_clock = fake_clock
        self.app = Path(app)
        self.state_hash = state_hash
        self.extra_env = env or {}
        self.proc: subprocess.Popen | None = None
        self.sock: socket.socket | None = None
        self.vblank = 0

    # ------------------------------------------------------------------ lifecycle
    @property
    def data_dir(self) -> Path:
        return self.work_dir / "data"

    @property
    def saves_dir(self) -> Path:
        return self.data_dir / "saves"

    def _prepare(self):
        if self.work_dir.exists():
            shutil.rmtree(self.work_dir)
        self.data_dir.mkdir(parents=True)
        for name in ("disc", "game"):
            os.symlink(self.base_data / name, self.data_dir / name)
        (self.saves_dir / "mc1").mkdir(parents=True)
        if self.checkpoint:
            shutil.copytree(self.checkpoint, self.saves_dir / "mc0")
        else:
            (self.saves_dir / "mc0").mkdir()

    def start(self) -> "Game":
        self._prepare()
        env = dict(os.environ)
        env.update({
            "RT_DATA_DIR": str(self.data_dir),
            "RT_TIME": "virtual",
            "RT_SPEED": self.speed,
            "RT_FAKE_CLOCK": str(self.fake_clock),
            "RT_HEADLESS": "1",
            "RT_TEST_SOCKET": "rt.sock",  # relative: AF_UNIX paths are limited to 104 bytes
        })
        if self.state_hash:
            env["RT_STATE_HASH"] = str(self.work_dir / "state_hash.txt")
        env.update(self.extra_env)
        self.log = open(self.work_dir / "app.log", "wb")
        self.proc = subprocess.Popen([str(self.app)], cwd=self.work_dir, env=env,
                                     stdout=self.log, stderr=subprocess.STDOUT)
        sock_path = self.work_dir / "rt.sock"
        deadline = time.time() + 120  # the game library may be (re)built on first launch
        while not sock_path.exists():
            if self.proc.poll() is not None:
                raise GameError(f"app exited ({self.proc.returncode}); see {self.work_dir / 'app.log'}")
            if time.time() > deadline:
                raise GameError("app did not open its test socket")
            time.sleep(0.05)
        cwd = os.getcwd()
        try:
            os.chdir(self.work_dir)
            self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
            self.sock.connect("rt.sock")
        finally:
            os.chdir(cwd)
        self._file = self.sock.makefile("rw")
        self.run(1)
        return self

    def close(self):
        if self.sock:
            try:
                self._call("quit")
            except Exception:
                pass
            self.sock.close()
            self.sock = None
        if self.proc:
            try:
                self.proc.wait(timeout=10)
            except subprocess.TimeoutExpired:
                self.proc.kill()
                self.proc.wait()
            self.proc = None
        if getattr(self, "log", None):
            self.log.close()

    def __enter__(self):
        return self.start()

    def __exit__(self, *exc):
        self.close()

    # ------------------------------------------------------------------ protocol
    def _call(self, cmd: str, **args) -> dict:
        self._file.write(json.dumps({"cmd": cmd, **args}) + "\n")
        self._file.flush()
        line = self._file.readline()
        if not line:
            raise GameError(f"app closed the connection; see {self.work_dir / 'app.log'}")
        reply = json.loads(line)
        if not reply.get("ok"):
            raise GameError(f"{cmd}: {reply.get('error')}")
        return reply

    # ------------------------------------------------------------------ time and input
    def run(self, vblanks: int = 1) -> int:
        """Lets the game run `vblanks` vblanks; returns the vblank it is parked at."""
        self.vblank = self._call("run", vblanks=int(vblanks))["vblank"]
        return self.vblank

    def run_seconds(self, s: float) -> int:
        return self.run(seconds(s))

    def pad(self, *buttons: str, lx: int = 128, ly: int = 128, rx: int = 128, ry: int = 128):
        """Holds `buttons` (and stick positions) from the next vblank on."""
        mask = 0xFFFF
        for b in buttons:
            mask &= ~BUTTONS[b]
        self._call("pad", buttons=mask, lx=lx, ly=ly, rx=rx, ry=ry)

    def release(self):
        self.pad()

    def press(self, *buttons: str, frames: int = 8, then: int = 8):
        """Presses and releases `buttons`, then lets `then` more vblanks pass."""
        self.pad(*buttons)
        self.run(frames)
        self.release()
        self.run(then)

    def wait_until(self, predicate, timeout_vblanks: int, step: int = 10) -> int:
        """Runs in steps until predicate(self) is true; returns the vblank."""
        end = self.vblank + timeout_vblanks
        while self.vblank < end:
            if predicate(self):
                return self.vblank
            self.run(step)
        raise GameError(f"condition not reached within {timeout_vblanks} vblanks")

    # ------------------------------------------------------------------ memory
    def read(self, addr: int, length: int, space: str = "ee") -> bytes:
        return bytes.fromhex(self._call("read", space=space, addr=addr, len=length)["data"])

    def write(self, addr: int, data: bytes, space: str = "ee"):
        self._call("write", space=space, addr=addr, data=data.hex())

    def u8(self, addr, space="ee"):
        return self.read(addr, 1, space)[0]

    def u16(self, addr, space="ee"):
        return struct.unpack("<H", self.read(addr, 2, space))[0]

    def u32(self, addr, space="ee"):
        return struct.unpack("<I", self.read(addr, 4, space))[0]

    def f32(self, addr, space="ee"):
        return struct.unpack("<f", self.read(addr, 4, space))[0]

    # ------------------------------------------------------------------ observation
    def frame(self) -> Frame:
        """The picture currently presented (exact pixels, GPU GS resolution)."""
        path = self.work_dir / "frame.rgba"
        reply = self._call("frame", path=str(path))
        return Frame(reply["width"], reply["height"], path.read_bytes())

    def stats(self) -> dict:
        return self._call("stats")

    def progress(self) -> dict:
        """Adventure progress decoded from live RAM (config/game_state.toml)."""
        sys.path.insert(0, str(REPO / "tools"))
        import save_parser

        game_map = save_parser.load_map()["progress"]
        return save_parser.decode(self.read(game_map["ram_address"], game_map["size"]))

    def saved_progress(self) -> dict:
        """Adventure progress decoded from the save on memory card 1."""
        sys.path.insert(0, str(REPO / "tools"))
        import save_parser

        return save_parser.decode((self.saves_dir / "mc0/BASLUS-20398/BASLUS-20398").read_bytes())

    def marker(self, kind: str, text: str = ""):
        """Adds a marker to the movie being recorded (RT_MOVIE_RECORD)."""
        self._call("marker", kind=kind, text=text)

    def audio(self) -> dict:
        """Sound produced since the last call: frames (48 kHz), rms, peak and an exact hash."""
        return self._call("audio")

    def snapshot_card(self, dest: Path):
        """Copies memory card 1 (a checkpoint for later tests)."""
        dest = Path(dest)
        if dest.exists():
            shutil.rmtree(dest)
        shutil.copytree(self.saves_dir / "mc0", dest)
