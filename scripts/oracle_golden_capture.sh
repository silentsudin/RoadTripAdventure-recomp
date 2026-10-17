#!/usr/bin/env bash
# Capture ground-truth frames from the PS2Recomp desktop oracle (RoadTrip.app).
#
# Frames land in build/scratch/oracle-golden/ (gitignored via /build/). This script
# never commits images. It drives the game through the regression harness
# (tests/regression/rtharness), which speaks the JSON-lines test socket in
# third_party/PS2Recomp/ps2xRuntime/include/runtime/ps2_test_harness.h.
#
# The oracle does not read RT_DISC. The installed disc tree is symlinked into a
# private RT_DATA_DIR as disc/ (plus game/ for the already-built
# libroadtrip_game.dylib). RT_TEST_SOCKET is set by the harness to a relative
# Unix socket "rt.sock" in the work dir (AF_UNIX paths are limited to 104 bytes).
# "tcp:<port>" also works (the Windows form, and what Android forwards); this
# script does not pin tcp:9876 because the harness owns the socket.
#
# Headless grabs use the socket "frame" command (GS scanout, tightly packed RGBA,
# before presenter post-process). RT_FRAME_DUMP is a window screenshot and does
# nothing useful under RT_HEADLESS=1, so it is intentionally unset.
#
# Usage:
#   scripts/oracle_golden_capture.sh
#   RT_ONLY=title,options scripts/oracle_golden_capture.sh
#   RT_GS_BACKEND=hw scripts/oracle_golden_capture.sh
#
# Exit 0 if every requested scene saved a frame. Exit 1 if the oracle could not
# start. Exit 2 if the oracle ran but one or more scenes failed.

set -uo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"

# ---------------------------------------------------------------------------
# Environment. Every variable the capture consults is listed here.
# ---------------------------------------------------------------------------

# RT_ORACLE_APP
#   Absolute path to the oracle executable (not the .app bundle).
#   Default: the prebuilt macos-release app. Do not point this at src/native.
RT_ORACLE_APP="${RT_ORACLE_APP:-$ROOT/build/macos-release/RoadTrip.app/Contents/MacOS/RoadTrip}"

# RT_BASE_DATA
#   Installed oracle data directory. Must contain disc/ (extracted SLUS-20398
#   tree) and game/ (libroadtrip_game.dylib + build_id matching the app SDK).
#   The harness symlinks those two directories into each run's RT_DATA_DIR.
#   This is NOT RT_DISC; the recomp oracle resolves content through RT_DATA_DIR.
RT_BASE_DATA="${RT_BASE_DATA:-$HOME/Library/Application Support/RoadTripRecomp}"

# RT_GOLDEN_OUT
#   Where PNG frames and manifest.json are written. Must stay under build/ so
#   images are not committed. Each run also keeps per-scene app logs in
#   $RT_GOLDEN_OUT/work/<scene>/app.log.
RT_GOLDEN_OUT="${RT_GOLDEN_OUT:-$ROOT/build/scratch/oracle-golden}"

# RT_CHECKPOINT
#   Memory-card directory (mc0 contents: BASLUS-20398/) used to Continue into
#   Q's Factory and then "Drive around town". Default is the regression
#   checkpoint produced by test_adventure_new_game_save. Empty disables town.
RT_CHECKPOINT="${RT_CHECKPOINT:-$ROOT/build/regression/checkpoints/adventure_first_save}"

# RT_PYTHON
#   Interpreter that can import rtharness's dependencies (Pillow; numpy optional).
#   Default is the regression venv, which already has them.
RT_PYTHON="${RT_PYTHON:-$ROOT/build/regress-venv/bin/python}"

# RT_GS_BACKEND
#   Oracle renderer: pgs (paraLLEl-GS, the regression golden renderer), hw
#   (hardware Vulkan GS), or cpu (software GS). pgs is the default because the
#   committed goldens.json hashes are paraLLEl-GS pictures.
RT_GS_BACKEND="${RT_GS_BACKEND:-pgs}"

# RT_GS_SSAA
#   GPU supersample factor passed to the oracle. 4 matches the regression
#   harness and yields the native GS scanout of 1280x447 (not a 1280x720 window).
#   The harness hardcodes 4 unless this script overrides it via Game(env=).
RT_GS_SSAA="${RT_GS_SSAA:-4}"

# RT_PROGRESSIVE_FIELDS
#   0 keeps the game's original alternating fields (regression default).
#   1 forces the app's progressive scanout. Goldens are captured at 0.
RT_PROGRESSIVE_FIELDS="${RT_PROGRESSIVE_FIELDS:-0}"

# RT_GAME_OPTIONS
#   1 shows the game's own Title > Options screens (what this capture wants).
#   0 replaces Options with the recomp app menu (test_options_replaced).
RT_GAME_OPTIONS="${RT_GAME_OPTIONS:-1}"

# RT_FAKE_CLOCK
#   Unix seconds reported to the guest when RT_TIME=virtual. The harness pins
#   RT_TIME=virtual and RT_SPEED=max so the same pad sequence hits the same
#   vblanks. Override only to A/B a different guest date.
RT_FAKE_CLOCK="${RT_FAKE_CLOCK:-1000000000}"

# RT_SPEED
#   Guest pace. "max" runs as fast as the host can (harness default). A number
#   such as 1 runs in real time. Capture uses max so menu navigation is cheap
#   while rendering is off.
RT_SPEED="${RT_SPEED:-max}"

# RT_FRAME_WARMUP
#   Vblanks rendered immediately before each socket frame grab when the run
#   had RT_RENDER=0. The harness default is 4; 12 gives the GS a few extra
#   presents to fill a 3D scene that was not drawing. A near-black grab is
#   retried once with 4x this warmup.
RT_FRAME_WARMUP="${RT_FRAME_WARMUP:-12}"

# RT_ONLY
#   Comma-separated scene ids to capture. Empty means all.
#   Ids: title, main_menu, options, attract, race_hud, race_hud_driving,
#        factory_menu, town_driving, town_driving_street.
RT_ONLY="${RT_ONLY:-}"

# RT_HEADLESS
#   Documented for callers. The harness forces RT_HEADLESS=1 (no window, no
#   audio device). Pictures are taken with the socket frame command. Setting
#   this in the environment does not open a window; do not combine a windowed
#   RT_FRAME_DUMP run with this script.
RT_HEADLESS="${RT_HEADLESS:-1}"

# RT_TIME
#   Documented for callers. The harness forces RT_TIME=virtual (guest time from
#   EE cycles only) so captures are reproducible. Do not expect wall-clock
#   RT_FRAME_DUMP intervals to mean anything here.
RT_TIME="${RT_TIME:-virtual}"

# RT_RENDER
#   Documented for callers. The harness starts with RT_RENDER=0 (skip VU1/GS
#   while navigating; game logic does not read it back) and switches rendering
#   on only for the frame grab. Set RT_TEST_RENDER_ALWAYS=1 to draw every
#   vblank (much slower; not needed for goldens).
RT_RENDER="${RT_RENDER:-0}"

# RT_TEST_RENDER_ALWAYS
#   If 1, rtharness draws every vblank. Leave unset/0 for capture.
RT_TEST_RENDER_ALWAYS="${RT_TEST_RENDER_ALWAYS:-0}"

# RT_DATA_DIR
#   Not taken from the caller. Each scene gets a fresh directory under
#   $RT_GOLDEN_OUT/work/<scene>/data with disc/ and game/ symlinks. Overriding
#   the ambient RT_DATA_DIR would point the oracle at the user's real saves;
#   the harness sets it itself and this script unsets any inherited value.
unset RT_DATA_DIR

# RT_TEST_SOCKET
#   Not taken from the caller. On macOS the harness sets RT_TEST_SOCKET=rt.sock
#   (relative AF_UNIX in the work dir) and connects after chdir. A caller-supplied
#   tcp:9876 would race the harness. Unset so the harness chooses.
unset RT_TEST_SOCKET

# RT_FRAME_DUMP / RT_FRAME_DUMP_SECONDS / RT_FRAME_DUMP_AFTER / RT_FRAME_DUMP_COUNT
#   Window PNG dumps (src/debug/FrameDump.cpp). Useless while headless, and a
#   leftover value would surprise a non-headless rerun. Unset.
unset RT_FRAME_DUMP RT_FRAME_DUMP_SECONDS RT_FRAME_DUMP_AFTER RT_FRAME_DUMP_COUNT

# RT_MENU_SHOT / RT_MENU_PAGE / RT_MENU_AT
#   One-shot pause-menu screenshots that quit the app. Must not be set during
#   a harness run or the process exits after the first menu picture.
unset RT_MENU_SHOT RT_MENU_PAGE RT_MENU_AT RT_MENU_SHOT_HEADLESS

export RT_ORACLE_APP RT_BASE_DATA RT_GOLDEN_OUT RT_CHECKPOINT RT_PYTHON
export RT_GS_BACKEND RT_GS_SSAA RT_PROGRESSIVE_FIELDS RT_GAME_OPTIONS
export RT_FAKE_CLOCK RT_SPEED RT_FRAME_WARMUP RT_ONLY
export RT_HEADLESS RT_TIME RT_RENDER RT_TEST_RENDER_ALWAYS
export RT_REPO="$ROOT"

if [[ ! -x "$RT_PYTHON" ]]; then
  if command -v python3 >/dev/null 2>&1; then
    RT_PYTHON="$(command -v python3)"
    export RT_PYTHON
  else
    echo "oracle-golden: no python at $RT_PYTHON and python3 is not on PATH" >&2
    exit 1
  fi
fi

mkdir -p "$RT_GOLDEN_OUT"

echo "oracle-golden: app=$RT_ORACLE_APP"
echo "oracle-golden: base=$RT_BASE_DATA"
echo "oracle-golden: out=$RT_GOLDEN_OUT"
echo "oracle-golden: gs=$RT_GS_BACKEND ssaa=$RT_GS_SSAA progressive=$RT_PROGRESSIVE_FIELDS"

"$RT_PYTHON" - <<'PY'
import json, os, sys, time, traceback
from pathlib import Path

ROOT = Path(os.environ["RT_REPO"])
sys.path.insert(0, str(ROOT / "tests" / "regression"))

try:
    from PIL import Image
except ImportError as exc:
    print(f"oracle-golden: Pillow is required ({exc})", file=sys.stderr)
    sys.exit(1)

try:
    import numpy as np
except ImportError:
    np = None

from rtharness import Game, GameError, seconds
from rtharness.adventure import boot_to_main_menu, continue_to_factory, scene as adventure_scene

OUT = Path(os.environ["RT_GOLDEN_OUT"])
APP = Path(os.environ["RT_ORACLE_APP"])
BASE = Path(os.environ["RT_BASE_DATA"])
CHECKPOINT = Path(os.environ["RT_CHECKPOINT"]) if os.environ.get("RT_CHECKPOINT") else None
WARMUP = int(os.environ.get("RT_FRAME_WARMUP") or "12")
ONLY = {s.strip() for s in os.environ.get("RT_ONLY", "").split(",") if s.strip()}

# Guest words read at each grab. Addresses are from config/game_state.toml.
PROGRESS = 0x0177F760
SCENE_OFF = 0x538
CARS = 0x0177ACE0
PLACE = 0x015B0088
COUNTDOWN = 0x15AFB8C
TOTAL_TIME = 0x15AFC1C
CLOCK = 0x335914
GAME_MODE = 0x33590A
DRIVE_FLAGS = 0x335928
SCENE3D = 0x3356C4
SCENE3D_HANDLER = 0x3356D8
LOCATION = 0x335923
UI_CONTEXT = 0x3356CC
# Confirmed handler values (game_state.toml [game]).
HANDLER_RACE = 0x21CE48
HANDLER_TOWN = 0x21CF80
UI_TITLE = 0x29AFA0

SCENE_NAME = {
    0: "unset/title",
    2: "town",
    3: "race_over",
    6: "factory_menu",
    10: "racing",
}


def want(name: str) -> bool:
    return not ONLY or name in ONLY


def preflight() -> list[str]:
    problems = []
    if not APP.is_file():
        problems.append(f"oracle binary missing: {APP}")
    elf = BASE / "disc" / "SLUS_203.98"
    if not elf.is_file():
        problems.append(f"disc ELF missing: {elf} (RT_BASE_DATA must contain disc/)")
    elif elf.stat().st_size != 1_280_576:
        problems.append(f"disc ELF size {elf.stat().st_size}, expected 1280576")
    game = BASE / "game" / "libroadtrip_game.dylib"
    if not game.is_file():
        problems.append(f"game library missing: {game} (oracle rebuilds it on first launch if the SDK matches)")
    sdk_id = APP.parent.parent / "Resources" / "sdk" / "build_id"
    game_id = BASE / "game" / "build_id"
    if sdk_id.is_file() and game_id.is_file():
        a, b = sdk_id.read_text().strip(), game_id.read_text().strip()
        if a != b:
            problems.append(f"build_id mismatch app={a} game={b}; launch may rebuild the game library")
    if CHECKPOINT and want("town_driving") and not (CHECKPOINT / "BASLUS-20398" / "BASLUS-20398").is_file():
        problems.append(f"town checkpoint missing: {CHECKPOINT}")
    return problems


def analyze(path: Path) -> dict:
    im = Image.open(path).convert("RGB")
    w, h = im.size
    if np is not None:
        a = np.asarray(im).reshape(-1, 3).astype(np.float64)
        avg = a.mean(axis=0)
        nonblack = float((a.sum(axis=1) > 12).mean())
        std = float(a.std())
    else:
        px = list(im.getdata())
        n = len(px)
        sr = sg = sb = 0
        nb = 0
        for r, g, b in px:
            sr += r
            sg += g
            sb += b
            if r + g + b > 12:
                nb += 1
        avg = (sr / n, sg / n, sb / n)
        nonblack = nb / n
        var = 0.0
        for r, g, b in px:
            var += (r - avg[0]) ** 2 + (g - avg[1]) ** 2 + (b - avg[2]) ** 2
        std = (var / (n * 3)) ** 0.5
    return {
        "width": w,
        "height": h,
        "avg_rgb": [round(float(avg[0]), 2), round(float(avg[1]), 2), round(float(avg[2]), 2)],
        "nonblack_fraction": round(nonblack, 4),
        "channel_std": round(std, 2),
    }


def read_state(game: Game) -> dict:
    def safe(fn, default=None):
        try:
            return fn()
        except Exception as exc:
            return {"error": str(exc)} if default is None else default

    scene = safe(lambda: game.u32(PROGRESS + SCENE_OFF))
    mode = safe(lambda: game.u8(GAME_MODE))
    flags = safe(lambda: game.u32(DRIVE_FLAGS))
    handler = safe(lambda: game.u32(SCENE3D_HANDLER))
    ui = safe(lambda: game.u32(UI_CONTEXT))
    clock = safe(lambda: game.u32(CLOCK))
    state = {
        "vblank": game.vblank,
        "stats": safe(lambda: {k: v for k, v in game.stats().items() if k != "ok"}),
        "progress_scene": scene,
        "progress_scene_name": SCENE_NAME.get(scene, "other") if isinstance(scene, int) else None,
        "game_mode": mode,
        "drive_flags": flags,
        "scene3d_active": safe(lambda: game.u32(SCENE3D)),
        "scene3d_handler": handler,
        "scene3d_handler_name": (
            "race" if handler == HANDLER_RACE else "town" if handler == HANDLER_TOWN else "other"
        ) if isinstance(handler, int) else None,
        "location": safe(lambda: game.u8(LOCATION)),
        "ui_context": ui,
        "ui_context_is_title": ui == UI_TITLE if isinstance(ui, int) else None,
        "clock_vblanks": clock,
        "clock_minute": (clock // 150) if isinstance(clock, int) else None,
        "car_xyz": safe(lambda: [round(game.f32(CARS), 3), round(game.f32(CARS + 4), 3), round(game.f32(CARS + 8), 3)]),
        "race_place_minus_1": safe(lambda: game.u32(PLACE)),
        "race_countdown": safe(lambda: game.u32(COUNTDOWN)),
        "race_total_time_frames": safe(lambda: game.u32(TOTAL_TIME)),
    }
    return state


def save_frame(game: Game, name: str, how: str, screen: str) -> dict:
    path = OUT / f"{name}.png"
    fr = game.frame(warmup=WARMUP)
    img = fr.image()
    img.save(path)
    color = analyze(path)
    if color["nonblack_fraction"] < 0.05:
        # Rendering was off during navigation. One short warmup can still be a
        # cleared buffer; give the GS a longer present and grab again.
        game.render(True)
        game.run(max(WARMUP * 4, seconds(1)))
        fr = game.frame(warmup=0)
        img = fr.image()
        img.save(path)
        color = analyze(path)
        game.render(False)
        how += f"; retried after {max(WARMUP * 4, seconds(1))} rendered vblanks (first grab nonblack<0.05)"
    rec = {
        "file": f"build/scratch/oracle-golden/{name}.png",
        "abs": str(path),
        "screen": screen,
        "how": how,
        "resolution": [fr.width, fr.height],
        "color": color,
        "state": read_state(game),
        "ok": True,
    }
    print(
        f"  saved {name} {fr.width}x{fr.height} "
        f"avg={color['avg_rgb']} nonblack={color['nonblack_fraction']} vblank={game.vblank}",
        flush=True,
    )
    return rec


def open_game(name: str, checkpoint: Path | None = None) -> Game:
    work = OUT / "work" / name
    extra = {
        "RT_GS_BACKEND": os.environ.get("RT_GS_BACKEND") or "pgs",
        "RT_GS_SSAA": os.environ.get("RT_GS_SSAA") or "4",
        "RT_PROGRESSIVE_FIELDS": os.environ.get("RT_PROGRESSIVE_FIELDS") or "0",
        "RT_GAME_OPTIONS": os.environ.get("RT_GAME_OPTIONS") or "1",
    }
    game = Game(
        BASE,
        work,
        checkpoint=checkpoint,
        speed=os.environ.get("RT_SPEED") or "max",
        fake_clock=int(os.environ.get("RT_FAKE_CLOCK") or "1000000000"),
        app=APP,
        env=extra,
        render=os.environ.get("RT_TEST_RENDER_ALWAYS") == "1",
    )
    print(f"== boot {name} ==", flush=True)
    game.start()
    return game


def run_group(name: str, fn, checkpoint: Path | None = None) -> dict:
    started = time.time()
    try:
        game = open_game(name, checkpoint)
    except Exception as exc:
        traceback.print_exc()
        return {"boot_error": str(exc), "seconds": round(time.time() - started, 1)}
    try:
        return fn(game) | {"seconds": round(time.time() - started, 1)}
    except Exception as exc:
        traceback.print_exc()
        log = game.work_dir / "app.log"
        tail = ""
        if log.is_file():
            tail = log.read_text(errors="replace")[-2000:]
        return {"error": str(exc), "log_tail": tail, "seconds": round(time.time() - started, 1)}
    finally:
        game.close()


def capture_menus(game: Game) -> dict:
    out = {}
    # test_boot_to_title grabs the title at 12 guest seconds (logo jingle by 9s).
    if any(want(s) for s in ("title", "main_menu", "options")):
        game.run(seconds(12))
    if want("title"):
        out["title"] = save_frame(
            game, "title",
            "boot, RT_TIME=virtual RT_SPEED=max, run 12 guest seconds, socket frame (test_boot_to_title timing)",
            "Title screen (ROAD TRIP license plate, publisher line, three cars)",
        )
    if want("main_menu") or want("options"):
        # Start from the title. boot_to_main_menu presses Start at 10s; we are at 12s, still on the title.
        game.press("start")
        game.run(seconds(3))
    if want("main_menu"):
        out["main_menu"] = save_frame(
            game, "main_menu",
            "title + Start, then 3 guest seconds (rtharness.adventure.boot_to_main_menu, Start pressed at 12s not 10s)",
            "Main menu (Adventure / Quick Race / 2 Player / Options / Results)",
        )
    if want("options"):
        for _ in range(3):  # Adventure is the default; Options is index 3
            game.press("down")
        game.press("cross")
        game.run(seconds(2))
        out["options"] = save_frame(
            game, "options",
            "main menu, Down x3, Cross, 2 guest seconds (test_menus.open_main_item OPTIONS=3); RT_GAME_OPTIONS=1",
            "Game Options page (Vibration / Speaker / Sound Volume / Exit)",
        )
    return out


def capture_attract(game: Game) -> dict:
    game.run(seconds(45))
    return {
        "attract": save_frame(
            game, "attract",
            "boot, no input, run 45 guest seconds (test_attract_demo)",
            "Attract demo (computer race after the title is left alone)",
        )
    }


def capture_race(game: Game) -> dict:
    out = {}
    # Exact pad sequence from tests/regression/test_smoke.py test_quick_race,
    # which goldens quick_race_start on the default course (Peach Raceway).
    boot_to_main_menu(game)
    game.press("down")  # Quick Race
    game.run(seconds(2))
    for _ in range(8):
        game.press("cross")
        game.run(seconds(4))
    end = game.vblank + seconds(20)
    while game.vblank < end and adventure_scene(game) != 10:
        game.run(seconds(1))
    if want("race_hud"):
        out["race_hud"] = save_frame(
            game, "race_hud",
            "main menu, Down, Cross x8 with 4s between (test_quick_race), wait until progress.scene==10 or 20s",
            "Quick Race HUD at the start of the default course (Peach Raceway)",
        )
    if want("race_hud_driving"):
        game.pad("cross")  # accelerate, held
        game.run(seconds(12))
        out["race_hud_driving"] = save_frame(
            game, "race_hud_driving",
            "race_hud pose, Cross held 12 guest seconds (car moving, HUD still up)",
            "Quick Race HUD while driving (Peach Raceway, gas held)",
        )
        game.release()
    return out


def capture_town(game: Game) -> dict:
    out = {}
    continue_to_factory(game)
    if want("factory_menu"):
        out["factory_menu"] = save_frame(
            game, "factory_menu",
            "Continue slot 1 from adventure_first_save (rtharness.adventure.continue_to_factory)",
            "Q's Factory menu after Continue (Peach Town save)",
        )
    if want("town_driving") or want("town_driving_street"):
        # Change parts / Race / Save data / Quit game / Drive around town.
        # Licence is not Super A on this checkpoint, so Drive is the 5th row (4 Downs).
        for _ in range(4):
            game.press("down")
        game.press("cross")
        game.run(seconds(2))
        game.press("cross")  # "Come again!"
        game.run(seconds(12))
        try:
            from rtharness.town import finish_dialogue
            finish_dialogue(game)
        except Exception as exc:
            print(f"  finish_dialogue skipped: {exc}", flush=True)
        if want("town_driving"):
            out["town_driving"] = save_frame(
                game, "town_driving",
                "factory menu, Down x4, Cross, Cross (Come again), 12s, finish_dialogue (test_map_overlay.drive_into_town without the town bot)",
                "Peach Town driving view just after leaving Q's Factory",
            )
        if want("town_driving_street"):
            before = game.f32(CARS)
            game.pad("cross")
            game.run(seconds(5))
            game.release()
            game.run(seconds(1))
            out["town_driving_street"] = save_frame(
                game, "town_driving_street",
                "town_driving pose, Cross held 5 guest seconds (drive away from the factory door)",
                "Peach Town street, a few seconds of throttle after leaving the factory",
            )
            out["town_driving_street"]["moved_x"] = round(game.f32(CARS) - before, 3)
    return out


def main() -> int:
    problems = preflight()
    manifest = {
        "route": "desktop",
        "app": str(APP),
        "base_data": str(BASE),
        "checkpoint": str(CHECKPOINT) if CHECKPOINT else None,
        "gs_backend": os.environ.get("RT_GS_BACKEND"),
        "gs_ssaa": os.environ.get("RT_GS_SSAA"),
        "progressive_fields": os.environ.get("RT_PROGRESSIVE_FIELDS"),
        "game_options": os.environ.get("RT_GAME_OPTIONS"),
        "fake_clock": os.environ.get("RT_FAKE_CLOCK"),
        "speed": os.environ.get("RT_SPEED"),
        "frame_warmup": WARMUP,
        "only": sorted(ONLY),
        "preflight": problems,
        "notes": [
            "GS scanout via socket frame command, not a 1280x720 window. With RT_GS_SSAA=4 and RT_PROGRESSIVE_FIELDS=0 the regression goldens are 1280x447.",
            "RT_DISC is not consulted. disc/ is a symlink of RT_BASE_DATA/disc inside the per-scene RT_DATA_DIR.",
            "RT_TEST_SOCKET is the harness relative Unix socket rt.sock, not tcp:9876.",
            "RT_FRAME_DUMP is unset; headless pictures come from the frame command.",
            "progress.scene is stale at the title (ui_context 0x29AFA0). game_mode 8 means the attract task has run.",
        ],
        "frames": {},
        "errors": {},
    }
    if any(p.startswith("oracle binary") or p.startswith("disc ELF missing") for p in problems):
        manifest["route"] = "desktop-preflight-failed"
        (OUT / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
        print("oracle-golden: preflight failed", file=sys.stderr)
        for p in problems:
            print(" ", p, file=sys.stderr)
        return 1
    for p in problems:
        print("preflight:", p, flush=True)

    groups = []
    if any(want(s) for s in ("title", "main_menu", "options")):
        groups.append(("menus", capture_menus, None))
    if want("attract"):
        groups.append(("attract", capture_attract, None))
    if any(want(s) for s in ("race_hud", "race_hud_driving")):
        groups.append(("race", capture_race, None))
    if any(want(s) for s in ("factory_menu", "town_driving", "town_driving_street")):
        if CHECKPOINT and (CHECKPOINT / "BASLUS-20398" / "BASLUS-20398").is_file():
            groups.append(("town", capture_town, CHECKPOINT))
        else:
            manifest["errors"]["town"] = "checkpoint missing; town/factory frames skipped"

    for name, fn, cp in groups:
        result = run_group(name, fn, cp)
        err = result.pop("error", None) or result.pop("boot_error", None)
        if err:
            manifest["errors"][name] = {"error": err, "log_tail": result.get("log_tail", ""), "seconds": result.get("seconds")}
            print(f"!! {name} failed: {err}", flush=True)
        for key, rec in result.items():
            if isinstance(rec, dict) and rec.get("file"):
                manifest["frames"][key] = rec
        manifest.setdefault("group_seconds", {})[name] = result.get("seconds")

    (OUT / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print("--- summary ---", flush=True)
    for key, rec in manifest["frames"].items():
        c = rec["color"]
        print(f"{key:22} {rec['resolution'][0]}x{rec['resolution'][1]}  nonblack={c['nonblack_fraction']:.3f}  avg={c['avg_rgb']}")
    if manifest["errors"]:
        print("errors:", ", ".join(manifest["errors"]), flush=True)
        return 2
    required = [s for s in ("title", "main_menu", "options", "race_hud", "town_driving") if want(s)]
    missing = [s for s in required if s not in manifest["frames"]]
    if missing:
        print("missing required:", ", ".join(missing), flush=True)
        return 2
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except GameError as exc:
        print(f"oracle-golden: {exc}", file=sys.stderr)
        sys.exit(1)
PY
status=$?
echo "oracle-golden: exit $status"
exit "$status"
