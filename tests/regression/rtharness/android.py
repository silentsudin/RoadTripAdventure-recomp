"""The game on an Android device over USB debugging, with the same API as Game.

The app reads RT_* switches from files/env.txt (no environment on Android). With
RT_TEST_SOCKET=@roadtrip-test the runtime's control socket listens in the abstract namespace and
`adb forward tcp:<port> localabstract:roadtrip-test` reaches it from here; frames come back inline.
The device keeps its own data (disc, game library, saves) in the app's files/ directory.

    from rtharness.android import AndroidGame
    with AndroidGame() as g:
        g.run(seconds(12)); g.press("start"); g.frame().image().save("title.png")
"""

from __future__ import annotations

import base64
import json
import socket
import subprocess
import time
from pathlib import Path

from . import Frame, Game, GameError

PACKAGE = "io.github.roadtrip.recomp"
ACTIVITY = f"{PACKAGE}/io.github.roadtrip.RoadTripActivity"
SOCKET_NAME = "roadtrip-test"


def adb(*args: str, check: bool = True, **kw) -> subprocess.CompletedProcess:
    return subprocess.run(["adb", *args], check=check, capture_output=True, text=True, **kw)


def run_as(cmd: str) -> str:
    return adb("shell", f"run-as {PACKAGE} sh -c '{cmd}'").stdout


class AndroidGame(Game):
    def __init__(self, env: dict | None = None, port: int = 47811, lockstep: bool = True,
                 render: bool = True, work_dir: Path | None = None):
        # Game's fields; nothing local is prepared (the device has its own data).
        self.extra_env = env or {}
        self.port = port
        self.lockstep = lockstep
        self.rendering = render
        self.work_dir = Path(work_dir or Path.cwd() / "build/scratch/android-runs")
        self.proc = None
        self.sock = None
        self.vblank = 0

    def start(self) -> "AndroidGame":
        env = {"RT_TEST_SOCKET": "@" + SOCKET_NAME, "RT_RENDER": "1" if self.rendering else "0"}
        if self.lockstep:
            env.update({"RT_TIME": "virtual", "RT_SPEED": "max"})
        else:
            env["RT_TEST_LIVE"] = "1"  # real time; the app's menus work (they pause the game)
        env.update(self.extra_env)
        adb("shell", "am", "force-stop", PACKAGE)
        lines = "\\n".join(f"{k}={v}" for k, v in env.items())
        run_as(f'printf "{lines}\\n" > files/env.txt')
        adb("logcat", "-c")
        adb("shell", "am", "start", "-n", ACTIVITY)
        adb("forward", f"tcp:{self.port}", f"localabstract:{SOCKET_NAME}")
        deadline = time.time() + 300  # the first launch may build the game
        while True:
            try:
                self.sock = socket.create_connection(("127.0.0.1", self.port), timeout=5)
                self.sock.settimeout(None)
                self._file = self.sock.makefile("rw")
                self.run(1)  # fails until the app's socket really answers
                break
            except (OSError, GameError, json.JSONDecodeError):
                if self.sock:
                    self.sock.close()
                    self.sock = None
                if time.time() > deadline:
                    raise GameError("the app did not open its test socket (adb logcat -s RoadTrip)")
                time.sleep(1)
        return self

    def frame(self, warmup: int = 4) -> Frame:
        if not self.rendering:
            self.render(True)
            self.run(warmup)
            self.render(False)
        reply = self._call("frame", path="-")
        return Frame(reply["width"], reply["height"], base64.b64decode(reply["data"]))

    def sdl(self, *buttons: str, type: str | None = None, hold: float = 0.15):
        """Holds buttons on a virtual SDL gamepad (type xbox, ps or switch) for `hold` seconds,
        then releases them: input through SDL, as from a real controller (the app's menus,
        rebinding, button icons). Names: a b x y back guide start lb rb lt rt l3 r3 up down left right."""
        args = {"buttons": ",".join(buttons)}
        if type:
            args["type"] = type
        self._call("sdlpad", **args)
        time.sleep(hold)
        self._call("sdlpad", buttons="")
        time.sleep(0.1)

    def screencap(self, path: Path | str) -> Path:
        """The device's screen as it is (the game and the app's UI), as PNG."""
        png = subprocess.run(["adb", "exec-out", "screencap", "-p"], check=True, capture_output=True).stdout
        Path(path).write_bytes(png)
        return Path(path)

    def close(self):
        if self.sock:
            try:
                self._call("quit")
            except Exception:
                pass
            self._file.close()
            self.sock.close()
            self.sock = None
        adb("forward", "--remove", f"tcp:{self.port}", check=False)
        adb("shell", "am", "force-stop", PACKAGE, check=False)
        run_as("rm -f files/env.txt")

    def log(self) -> str:
        return adb("logcat", "-d", "-s", "RoadTrip").stdout
