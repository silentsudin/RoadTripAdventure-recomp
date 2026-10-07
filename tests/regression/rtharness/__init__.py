"""Drive RoadTrip.app from Python for regression tests.

The app runs in deterministic test mode (virtual time, fake clock, headless) and in lockstep with
the test: the game parks at a vblank until the test asks it to run further, so memory reads, pad
changes and frame grabs happen between two exact vblanks. See
third_party/PS2Recomp/ps2xRuntime/include/runtime/ps2_test_harness.h for the protocol.
"""

from __future__ import annotations

import atexit
import json
import os
import shutil
import signal
import socket
import struct
import subprocess
import sys
import time
import weakref
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


# Games started by this process and not closed yet. The exit hook stops any a crashed script or
# test left behind (the game itself also exits when its controller disconnects or dies).
_LIVE: "weakref.WeakSet[Game]" = weakref.WeakSet()


def _kill_group(proc: subprocess.Popen) -> None:
    try:
        os.killpg(proc.pid, signal.SIGKILL)
    except (ProcessLookupError, PermissionError):
        pass
    try:
        proc.wait(timeout=10)
    except subprocess.TimeoutExpired:
        pass


@atexit.register
def close_all_games() -> int:
    """Stops every game still running; returns how many there were."""
    leftover = [g for g in list(_LIVE) if g.proc is not None]
    for game in leftover:
        try:
            game.close()
        except Exception:
            if game.proc is not None:
                _kill_group(game.proc)
    return len(leftover)


class GameError(RuntimeError):
    pass


RENDER_ALWAYS = os.environ.get("RT_TEST_RENDER_ALWAYS") == "1"


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
    checkpoint (a saved mc0 directory); card2 likewise puts one on memory card 2.
    """

    def __init__(self, base_data: Path, work_dir: Path, *, checkpoint: Path | None = None,
                 card2: Path | None = None,
                 speed: str = "max", fake_clock: int = 1_000_000_000, app: Path = DEFAULT_APP,
                 state_hash: bool = False, env: dict | None = None,
                 progress_edits: dict[int, bytes] | None = None, render: bool = False,
                 settings: str | None = None):
        self.base_data = Path(base_data)
        self.work_dir = Path(work_dir)
        self.checkpoint = checkpoint
        self.card2 = card2
        self.speed = speed
        self.fake_clock = fake_clock
        self.app = Path(app)
        self.state_hash = state_hash
        self.extra_env = env or {}
        # offset -> bytes patched into the Adventure save on memory card 1 before boot
        # (config/game_state.toml), e.g. to start in another town or with a licence.
        self.progress_edits = progress_edits or {}
        # settings.toml for the app's own options (e.g. '[display]\naspect = "16:9"'); default: none.
        self.settings = settings
        # Rendering (VU1 + drawing) only when a picture is taken; see frame(). RT_TEST_RENDER_ALWAYS=1
        # draws every frame (e.g. with RT_TEXTURE_DUMP, to dump every texture the suite shows).
        self.rendering = render or RENDER_ALWAYS
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
        self.saves_dir.mkdir(parents=True)
        if self.card2:
            shutil.copytree(self.card2, self.saves_dir / "mc1")
        else:
            (self.saves_dir / "mc1").mkdir()
        if self.checkpoint:
            shutil.copytree(self.checkpoint, self.saves_dir / "mc0")
        else:
            (self.saves_dir / "mc0").mkdir()
        if self.settings is not None:
            (self.data_dir / "settings.toml").write_text(self.settings + "\n")
        if self.progress_edits:
            save = self.saves_dir / "mc0/BASLUS-20398/BASLUS-20398"
            data = bytearray(save.read_bytes())
            for offset, value in self.progress_edits.items():
                data[offset:offset + len(value)] = value
            save.write_bytes(bytes(data))

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
            "RT_RENDER": "1" if self.rendering else "0",
            # The original interlaced fields at 4x: goldens and state hashes record the game as is.
            "RT_PROGRESSIVE_FIELDS": "0",
            "RT_GS_SSAA": "4",
            # The game's own Title > Options screens (test_menus checks them); the app's
            # replacement is tested with this off (test_options_replaced).
            "RT_GAME_OPTIONS": "1",
        })
        if self.state_hash:
            env["RT_STATE_HASH"] = str(self.work_dir / "state_hash.txt")
        env.update(self.extra_env)
        self.log = open(self.work_dir / "app.log", "wb")
        # Its own process group, so close() (or the exit hook) can stop everything it started.
        self.proc = subprocess.Popen([str(self.app)], cwd=self.work_dir, env=env,
                                     stdout=self.log, stderr=subprocess.STDOUT, start_new_session=True)
        _LIVE.add(self)
        try:
            self._connect()
        except BaseException:
            self.close()  # never leave a half-started game running
            raise
        return self

    def _connect(self):
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

    def close(self):
        if self.sock:
            try:
                self._call("quit")
            except Exception:
                pass
            if getattr(self, "_file", None):
                self._file.close()
                self._file = None
            self.sock.close()
            self.sock = None
        if self.proc:
            try:
                self.proc.wait(timeout=10)
            except subprocess.TimeoutExpired:
                _kill_group(self.proc)
            self.proc = None
        _LIVE.discard(self)
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

    def step(self, vblanks: int, buttons: list[str] | None = None, lx: int = 128,
             reads: list[tuple[int, int]] | None = None, port: int | None = None) -> list[bytes]:
        """Sets the pad (if `buttons` is given; both pads, or only `port`), runs `vblanks` and
        reads EE memory ranges (addr, len), in one round trip."""
        args = {"vblanks": int(vblanks), "reads": ",".join(f"ee:{a}:{n}" for a, n in (reads or []))}
        if buttons is not None:
            mask = 0xFFFF
            for b in buttons:
                mask &= ~BUTTONS[b]
            args.update(buttons=mask, lx=int(lx))
            if port is not None:
                args["port"] = int(port)
        reply = self._call("step", **args)
        self.vblank = reply["vblank"]
        return [bytes.fromhex(h) for h in reply["data"]]

    def pad(self, *buttons: str, lx: int = 128, ly: int = 128, rx: int = 128, ry: int = 128,
            port: int | None = None, connected: bool | None = None):
        """Holds `buttons` (and stick positions) from the next vblank on: on both pads, or only
        on `port` (0 or 1). `connected=False` unplugs that pad (True plugs it back in)."""
        mask = 0xFFFF
        for b in buttons:
            mask &= ~BUTTONS[b]
        args = dict(buttons=mask, lx=lx, ly=ly, rx=rx, ry=ry)
        if port is not None:
            args["port"] = int(port)
        if connected is not None:
            args["connected"] = 1 if connected else 0
        self._call("pad", **args)

    def release(self, port: int | None = None):
        self.pad(port=port)

    def actuators(self) -> list[dict]:
        """Vibration per pad port: {"small": 0/1, "large": 0..255, "changes": n}."""
        return self._call("actuators")["ports"]

    def pad_info(self) -> list[dict]:
        """Per pad port: open, analog (DualShock mode), connected, reads, align (actuator table)."""
        return self._call("pad_info")["ports"]

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

    def voice_volumes(self) -> list[tuple[int, int, int, int]]:
        """(core, voice, VOLL, VOLR) of every SPU2 voice with a volume set, from the registers."""
        regs = self.read(0x1F900000, 0x800, space="iop")
        out = []
        for core in range(2):
            for voice in range(24):
                left, right = struct.unpack_from("<hh", regs, core * 0x400 + voice * 0x10)
                if left or right:
                    out.append((core, voice, left, right))
        return out

    # ------------------------------------------------------------------ observation
    def render(self, on: bool):
        if RENDER_ALWAYS:
            on = True
        self._call("render", on=1 if on else 0)
        self.rendering = on

    def frame(self, warmup: int = 4) -> Frame:
        """The picture currently presented (exact pixels, GPU GS resolution). With rendering off
        (the default in tests: it is most of the cost in 3D scenes and the game's logic does not
        depend on it), it is switched on for `warmup` vblanks first so the screen is redrawn."""
        if not self.rendering:
            self.render(True)
            self.run(warmup)
            self.render(False)
        path = self.work_dir / "frame.rgba"
        reply = self._call("frame", path=str(path))
        return Frame(reply["width"], reply["height"], path.read_bytes())

    def log_text(self) -> str:
        """What the app has logged so far (stdout and stderr)."""
        self.log.flush()
        return (self.work_dir / "app.log").read_text(errors="replace")

    def wide(self) -> dict:
        """Widescreen verdicts: frame_2d (the frame on display is shown 4:3), driving, k."""
        return self._call("wide")

    def stats(self) -> dict:
        return self._call("stats")

    def progress(self) -> dict:
        """Adventure progress decoded from live RAM (config/game_state.toml)."""
        sys.path.insert(0, str(REPO / "tools"))
        import save_parser

        game_map = save_parser.load_map()["progress"]
        return save_parser.decode(self.read(game_map["ram_address"], game_map["size"]))

    def saved_progress(self, card: int = 1) -> dict:
        """Adventure progress decoded from the save on memory card 1 (or 2)."""
        sys.path.insert(0, str(REPO / "tools"))
        import save_parser

        return save_parser.decode((self.saves_dir / f"mc{card - 1}/BASLUS-20398/BASLUS-20398").read_bytes())

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
