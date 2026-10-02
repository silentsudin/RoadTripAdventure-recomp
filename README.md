# RoadTripAdventure-decomp

A static recompilation of **Road Trip** (*Choro Q HG 2* / *Road Trip Adventure*, PS2, USA **SLUS-20398**) into native code, built on [PS2Recomp](https://github.com/ran-j/PS2Recomp). The first target is macOS on Apple Silicon.

> **No game code or data is included, not even in the built app.** `RoadTrip.app` contains only our code, the PS2Recomp runtime and recompiler, and runtime headers. On first launch it asks for *your* disc image, copies the game files off it, recompiles the game's program into C++ and compiles that with your Mac's own clang. The result is a game library in `~/Library/Application Support/RoadTripRecomp/`. That means the app can be shared without sharing anything of Takara's. Don't distribute anything from that Application Support folder.

## For players: what you need

- **A Mac with Apple Silicon** running macOS 13 or later.
- **Your own copy of Road Trip (USA, SLUS-20398)**, dumped to a `.cue`/`.bin` or `.iso`.
- **Apple's Command Line Tools.** This is a free developer download from Apple, often over 1 GB, and most gamers won't already have it.
  - Road Trip builds the game from your disc the first time it opens, so it never has to ship any of the game's code, and that build needs Apple's compiler.
  - If the tools are missing, the app shows macOS's install prompt. Once the install finishes, open Road Trip again.
  - You can also install them ahead of time with `xcode-select --install` in Terminal.
- **First launch takes a little while.** It copies about 512 MB of game files from your disc and then builds the game, which takes under a minute on an M-series Mac. Later launches start straight away.

## Status

This is early, experimental work. The game boots and runs these screens without flicker at the correct 4:3 aspect:
- the publisher logo;
- the title screen (60 fps);
- "NOW LOADING…";
- the **3D attract demo**: a full 20-car race, matching PCSX2;
- the menus, a **playable Quick Race**, and **Adventure mode** with working memory-card saves (Save data at Q's Factory, Continue on the title menu), with the keyboard or a gamepad (see [Controls](#controls)). The game's own VU1 microcode is statically recompiled on your Mac, and graphics are rendered on the GPU through Vulkan (paraLLEl-GS on MoltenVK).

Races run at a steady 60 fps on an M3 Max, with the busiest thread about half loaded. The picture is progressive and full resolution (1280×896): the game's interlaced 224-line fields are rendered at double resolution, with no combing or field shake.

Sound: the SPU2 is emulated (ADPCM voices, envelopes, DMA, streaming input, MMIX routing, reverb). The title screen is silent; that matches the original (checked against PCSX2).

Known problems:
- Adventure mode has only been played as far as the first save at Q's Factory.

| Milestone | State |
|---|---|
| M0 Scaffold: pipeline, ISO reader, first-run extraction, app bundle | ✅ |
| M1 PS2Recomp tools and runtime build on macOS arm64 with Apple clang | ✅ |
| M2 Full recompilation of SLUS_203.98 compiles and links (1,376 functions incl. 229 SDK stubs, 0 errors) | ✅ |
| M2b No game code in the app: build on first launch from the user's disc | ✅ |
| M3 Boot to `main` (crt0, kernel syscalls, stubs triaged) | ✅ |
| M4 IOP modules (IOPRP234, LIBSD, SNDMOD Tamsoft driver, MCMAN, PADMAN) | 🔄 all load via ps2xIOP; behaviour unverified |
| M5 First pixels: logos and title | ✅ |
| M6 Stable display: 4:3 aspect, no flicker (double-buffer layout), 3D via VU1 | ✅ |
| M7 GPU GS (Vulkan/MoltenVK) and recompiled VU1 microcode, bit-exact with the interpreter on captured runs | ✅ |
| M8 Attract demo races (fixed EE FPU and VU0 macro-mode semantics, see below) | ✅ |
| M9 Input: keyboard and gamepads merged, configurable, race playable | ✅ |
| M10 Sound: SPU2 emulation, the game's own sound driver loads its banks and plays | ✅ |
| M11 Full speed: VIF1/VU1 and GS on their own threads, fast VU1 code, progressive output | ✅ |
| M12 Reverb, memory-card saves, full-resolution progressive picture | ✅ |
| Next: play further into Adventure mode, symbol names, CI | ⬜ |

## Controls

The keyboard and every connected gamepad work at the same time. Escape doesn't quit; use Cmd+Q or the close button.

| PS2 | Keyboard | Gamepad |
|---|---|---|
| D-pad | Arrow keys | D-pad |
| Left stick | W A S D | Left stick |
| Right stick | I J K L | Right stick |
| Cross | X, Space | A / Cross |
| Circle | C | B / Circle |
| Square | Z | X / Square |
| Triangle | V | Y / Triangle |
| L1 / R1 | Q / E | Bumpers |
| L2 / R2 | 1 / 3 | Triggers |
| Start / Select | Enter / Tab | Menu / View |
| L3 / R3 | F / G | Stick clicks |

The mapping is stored in `input.toml` in the data directory (`~/Library/Application Support/RoadTripRecomp`). It's written with the defaults on first run; edit it to rebind, or delete it to reset.

## Supported disc

| | |
|---|---|
| Title | Road Trip (USA) |
| Serial | SLUS-20398, `SYSTEM.CNF` VER 1.02 |
| Image | `Road Trip (USA).bin` / `.cue`, SHA-1 `236b902a72580f43a480f18d232723a8676b02e5` |
| Boot ELF | `SLUS_203.98`, 1,280,576 bytes, SHA-1 `2431de1ec3edd0df4be37ba564658d70c4049089`, entry `0x00200008` |

The pipeline accepts `.cue`, raw `.bin` (MODE1 or MODE2/2352) or cooked `.iso`. Only the boot ELF's hash is checked, so any good dump of this version works.

## Building the app (macOS arm64)

Prerequisites: Xcode Command Line Tools and `brew install cmake ninja python molten-vk`. **No ROM is needed to build the app.**

```sh
git clone <this repo> && cd RoadTripAdventure-decomp
python3 scripts/pipeline.py all run   # bootstrap inits only the submodules that are needed
```

This produces `build/macos-release/RoadTrip.app`. Its `Contents/Resources` holds:
- `recomp/ps2_recomp`, the recompiler (GPL-3.0);
- `recomp/roadtrip.toml`, the recompiler config, which holds function addresses only;
- `sdk/`, the runtime headers, `game_shim.cpp`, the compiler flags matching the app's runtime ABI, and a `build_id`.

None of it is derived from the game.

### First launch

1. The app asks for your disc image, or takes `--rom <path>` / `RT_ROM`. It checks the boot ELF and extracts all 376 files (about 512 MB).
2. It checks for the Xcode Command Line Tools and offers to install them if they're missing. It then runs the bundled `ps2_recomp` on `SLUS_203.98` from your disc, compiles the C++ in parallel, and links `libroadtrip_game.dylib`. This takes about 20 seconds on an M-series Mac.
3. It `dlopen`s the library, registers the game's functions with the runtime, and boots.

```
~/Library/Application Support/RoadTripRecomp/
  disc/    extracted disc tree, served to the game as cdrom0: (plus .lbn_map.tsv)
  game/    libroadtrip_game.dylib built from your disc, build_id, build.log
  saves/   memory card (mc0:, mc1:)
```

The game library is rebuilt automatically when the app's `build_id` changes, for example after an update to the recompiler, runtime headers or config.

Launch options:
- `--reinstall` extracts and rebuilds everything.
- `--rebuild-game` rebuilds only the game library.

Environment variables:

| Variable | Effect |
|---|---|
| `RT_DATA_DIR` | Moves the data directory |
| `RT_GS_BACKEND=cpu` | Uses the software GS instead of the Vulkan GPU GS (paraLLEl-GS on the bundled MoltenVK) |
| `RT_DEBUG_UI=1` | Shows the PS2Recomp debug panel at startup (F1 toggles it) |
| `RT_THREAD_DUMP=<s>` | Prints guest threads, wait reasons and semaphores every *s* seconds |
| `RT_FRAME_DUMP=<dir>` | Saves a PNG of the game picture every `RT_FRAME_DUMP_SECONDS` (default 2) |
| `RT_VU1_MODE=interp` | Runs VU1 microcode in the interpreter instead of the recompiled code, for A/B checks |
| `RT_SHOW_FPS=1` | Shows the game's frame rate (frames presented per second, counted at each vblank) on screen. The log also shows the host rate, the vblank rate and how busy the VIF1/VU1 and GS threads are |
| `RT_GS_SSAA=1\|2\|4\|8\|16` | GPU supersampling rate (default 4) |
| `RT_GS_PROGRESSIVE=0` | Uses paraLLEl-GS's field deinterlacer instead of the progressive high-resolution scanout |
| `RT_GIF_THREAD=0` / `RT_GS_THREAD=0` | Runs GIF/VIF1/VU1 work, or the GS, on the game thread instead of their own threads (A/B checks) |
| `RT_HOST_FPS=<n>` | Paces the window with raylib's frame limiter instead of the game's vblanks |
| `RT_VIF_VERIFY=1` | Checks the fast VIF1 UNPACK path against the generic one and logs mismatches |
| `RT_VU1_STATS=1` | Logs VU1 runs, VU cycles and host time per second |
| `RT_RPC_TRACE=1` | Prints every SIF RPC call between the game and the IOP |
| `RT_RAM_DUMP=<dir>` | Writes EE RAM (`ram_NNNN.bin`) and IOP RAM (`iop_NNNN.bin`) to `<dir>` every `RT_RAM_DUMP_SECONDS` (default 10) |
| `RT_GAME_EXTRA_CFLAGS` | Extra compiler flags for the on-device game build, e.g. `-DPS2X_WRITE_WATCH` to enable `RT_WRITE_WATCH=<hexaddr>:<hexlen>`, which logs the PC of every write into that range. Use a separate `RT_DATA_DIR`, because a normal build won't be replaced |
| `RT_AUDIO=0` | Turns sound off |
| `RT_AUDIO_DUMP=<file>` | Also writes the sound output as raw 48 kHz stereo 16-bit PCM |
| `RT_SPU2_TRACE=1` / `2` | Logs sound-chip activity per second (including routing and reverb registers) / every transfer and IOP disc read |
| `RT_MC_TRACE=1` | Logs memory card opens and directory creation with guest and host paths |
| `RT_IOP_IMPORT_TRACE=1` / `2` | Logs IOP kernel calls made by the game's IOP modules |
| `RT_INPUT_SCRIPT=<s>:<button>[:<hold>],...` | Presses buttons at fixed times after start, e.g. `10:start,15:cross:3` (testing without a player) |
| `RT_TIME=virtual`, `RT_SPEED=<x>\|max`, `RT_FAKE_CLOCK=<unix s>` | Deterministic guest time (from EE cycles only), its pace relative to real time, and a fixed guest clock (see Regression suite) |
| `RT_MOVIE_RECORD` / `RT_MOVIE_PLAY=<file>`, `RT_STATE_HASH=<file>`, `RT_EXIT_AT_VBLANK=<n>` | Record/replay pad input per vblank, log memory hashes, stop after n vblanks |
| `RT_TEST_SOCKET=<path>`, `RT_HEADLESS=1` | Lockstep control socket for test drivers; hidden window and no audio device |
| `RT_KEEP_GAME_WORK=1` | Keeps the generated C++ (with the MIPS disassembly in comments) under `<data>/game/work` |

Upstream's verbose runtime logging (a hook on every function entry, plus logging of every GS register write) is off by default because it costs a lot of speed. Configure with `-DRT_VERBOSE_RUNTIME_LOGS=ON` to turn it back on.

### Developer steps (need your ROM)

These maintain `config/roadtrip.toml`. The app never uses `generated/`; it builds its own copy.

| Step | What it does |
|---|---|
| `tools` | Builds `ps2_analyzer` and `ps2_recomp` into `build/tools/` |
| `extract-elf` | Reads `SLUS_203.98` from your image into `build/rom/` and checks its hash |
| `analyze` | Runs `ps2_analyzer`, then merges `config/roadtrip.base.toml` into `config/roadtrip.toml` |
| `recomp` | Runs `ps2_recomp` into `generated/`, so you can read the C++ |

### Tests

```sh
RT_ROM="/path/to/Road Trip (USA).cue" ctest --preset macos-release
```

The disc checks are skipped when `RT_ROM` is not set.

### Regression suite

`tests/regression` plays the game from Python and checks what it shows and stores:

```sh
python3 scripts/regress.py            # whole suite (installs its venv on first use)
python3 scripts/regress.py -n 4       # four games in parallel
python3 scripts/regress.py -k adventure --update-goldens
```

- It uses your installed game (the app's data directory, or `--base-data`; `--rom` installs one into `build/regression/base`).
- **Deterministic:** the app runs headless with `RT_TIME=virtual`, `RT_SPEED=max` and a fake clock, so the same inputs reach the same vblanks and produce bit-identical frames every run.
- **Lockstep control:** tests drive the game over a Unix socket. The game parks between vblanks while a test reads memory, sets the pad or grabs the exact picture (`rtharness.Game`: `run`, `press`, `pad`, `read`/`u32`, `frame`, `stats`).
- **Checkpoints:** tests that end with an in-game save store the memory card as a checkpoint. Later tests start from it (Continue), so long play-throughs split into independent sections.
- **What's committed:** golden frames are committed as hashes (`tests/regression/goldens.json`). The images, checkpoints and failure diffs are game output and stay in `build/regression`.

Recording play for new sections:
- `RT_TIME=virtual RT_MOVIE_RECORD=<file>` records every pad change by vblank; F5, F6 and F7 add mark, golden-frame and section-end markers.
- `RT_MOVIE_PLAY=<file>` replays a recording exactly.
- `RT_STATE_HASH=<file>` logs memory hashes, for finding where two runs diverge.

## Repository layout

```
config/              recompiler config (addresses only; committed) + known ROM hashes
scripts/             pipeline.py, iso9660.py, merge_config.py
src/                 app: first-run installer + game builder, ISO reader, macOS dialogs, debug tools
src/game/            GameBuilder (on-device recompile/compile/load), game_shim.cpp, overrides
third_party/PS2Recomp  submodule: silentsudin/PS2Recomp, branch `roadtrip` (upstream ran-j/PS2Recomp + our runtime changes)
generated/           (ignored) ps2_recomp output
build/               (ignored)
```

## Working on the recomp

- **Config.** Edit `config/roadtrip.base.toml`. It holds stubs, skips, extra entry points, jump tables and MMIO. Then run `pipeline.py analyze build` and relaunch; the changed `build_id` rebuilds the game. `config/roadtrip.toml` is regenerated, so don't edit it by hand.
- **Game hooks.** Runtime hooks for this game live in `src/game/overrides.cpp` (`PS2_REGISTER_GAME_OVERRIDE`). Use `bindAddressHandler` to route an address to a runtime stub.
- **Runtime changes.** Commit them in `third_party/PS2Recomp` on branch `roadtrip` (remote `origin` = the fork, `upstream` = ran-j), push, then commit the new submodule pointer here. Send generic fixes upstream as PRs.
- **Debugging.** Use PCSX2's debugger as ground truth for PCs and register state. Upstream's [stripped-game walkthrough](https://github.com/ran-j/PS2Recomp/wiki) covers `Function not found`, `[Syscall TODO]` and the other common failures.

## Changes on the PS2Recomp fork (`roadtrip` branch)

| Commit | Why |
|---|---|
| `cd register files at original lbn` | The game reads sectors by LBN from a TOC baked into the ELF; it does not use `sceCdSearchFile`. The installer records each file's original LBN in `disc/.lbn_map.tsv`, and the app registers them at boot. |
| `runtime keep custom memory card root` | `loadELF` resets the IOP, which runs MCSERV init. Without this patch the memory card root is reset to `<elf dir>/mc0` and saves land inside the disc tree. |
| `recomp cmake allow add subdirectory` | Lets `ps2_recomp` build as part of our CMake project, so it can be bundled. |
| `recomp synthesize undiscovered stub functions` | In a stripped ELF many SDK routines are only reached by a tail-call `J`, or never called, so function discovery misses them and their configured `name@addr` stubs were silently dropped (72 of 229). The main thread died on `J scePadRead`. This patch creates the missing stub wrappers. |
| `runtime present at 4x3 display aspect` | The PS2 drives a 4:3 TV. A 640×224 field buffer was being shown 1:1, which squashed the picture to half height. |
| `gs dbuffdc second buffer after first` | The runtime's `sceGsSetDefDBuffDc` put buffer 1 at the Z buffer address (FBP 140) instead of right after buffer 0 (FBP 70). The game's post-process reads FBP 70, so every other frame was garbage, which caused the constant flicker. |
| `recomp/runtime: EE FPU and VU0 macro-mode semantics fixes` | `SQRT.S` read the wrong register, and `RSQRT.S` and `VRSQRT` ignored the numerator. Division by zero gave Inf, and `CVT.W.S` rounded instead of truncating. VF0 started as zero in guest threads and could be overwritten, and VU0 macro ops never set MAC/status flags. Together these made every car matrix zero, so the cars were invisible and frozen and the attract demo never raced. Found by diffing RAM against a PCSX2 savestate. |
| `vu1 skip idle pipeline commits` | Speed: the VU1 interpreter scanned every pipeline slot on every emulated cycle. It now skips empty pipelines and cycles where nothing is due. Output is bit-identical. |

### GPU rendering (Vulkan)

- The GS (the PS2's rasterizer) runs on **paraLLEl-GS**, a Vulkan compute implementation. It lives on the fork as the submodule `ps2xRuntime/third_party/parallel-gs`.
- On macOS it runs through **MoltenVK**. The build copies `libMoltenVK.dylib` (Apache-2.0) into `RoadTrip.app/Contents/Frameworks`, so players need neither the Vulkan SDK nor Homebrew.
- The runtime's GS front end still parses the command stream, so CSR, FINISH/SIGNAL and transfers keep working, but it mirrors the raw GIF packets and register writes to the GPU instead of rasterizing them.
- VRAM is only copied back to the CPU when the game reads it.
- The game draws interlaced 640×224 fields. paraLLEl-GS renders them with 4× supersampling and scans out a high-resolution progressive picture (shown at its native 1280×896), which removes the interlacing artefacts. The field phase is held per display buffer, matching the half-line offset the game draws each field with, so the picture doesn't shake. `RT_GS_SSAA` sets the rate; `RT_GS_PROGRESSIVE=0` goes back to the plain field deinterlacer.
- If Vulkan can't start, the app logs why and falls back to the software GS.

### Recompiled VU1 microcode

- On first launch, `ps2_vu1_recomp` reads the VU1 overlays from the game's ELF.
- From the MSCAL entry points it explores every reachable instruction and pipeline-timing state, then emits C++ with each stall worked out at compile time.
- That C++ is compiled into `libroadtrip_game.dylib` with the rest of the game.
- When the code can't predict the timing (XGKICK overlap, D/T bits, unexpected jumps), it hands over to the interpreter with the exact state.
- The runtime only uses the recompiled code while VU1 code memory matches the image it was compiled from.
- Checking: run the game with `RT_VU1_CAPTURE=<dir>` to record VU1 runs, then compare them bit for bit with the fork's `vu1_replay` / `vu1_replay_native` tools.

### Threads

- **Game thread:** the recompiled EE code and the IOP (sound driver, disc, pads).
- **VIF1/VU1 worker:** the game's GIF and VIF1 DMA lists are copied when kicked and processed in order on their own thread, together with the VU1 microprograms they start, as on real hardware where the EE runs ahead of the VU1. Whenever the EE looks at that side (GIF/VIF1 registers, VU1 memory, GS registers or VRAM), it first waits for the worker to catch up.
- **GS thread:** takes the ordered GIF packets from the worker and feeds paraLLEl-GS.
- **Main thread:** presents one picture per guest vblank.

### Recompiler config notes

- **libvu0 is recompiled, not stubbed.** `drop_stubs` in `config/roadtrip.base.toml` makes the game run its own `sceVu0*` matrix code. One of the runtime's replacement stubs produced bad matrices, so every 3D vertex was culled and the screen stayed white.
- **Code pointers.** `scripts/code_pointers.py` adds every `lui`/`addiu` code address in `.text` as an entry point, so callbacks the game installs through function pointers are always registered.

### Distribution notes

- The app links the runtime statically and exports its symbols (`-export_dynamic`). The game library is linked with `-undefined dynamic_lookup`.
- `game_shim.cpp` copies the generated function table into the app's full-range table when the library loads.
- For a notarized build with the hardened runtime, add the `com.apple.security.cs.disable-library-validation` entitlement so the app can load the locally built game library.

## ELF notes

The ELF is stripped and has no symbols. Its main sections are:

- `.text` 0x200000–0x28E72C
- `.vutext` 0x28E730 (VU1 microcode, about 48 KB)
- `.data` and `.rodata` up to 0x335680
- a `.bss` of about 20 MB

There are four `.DVP.overlay` sections. The SDK pieces it links include libgraph, libdma, libvu0, libpad, libmc and libcdvd, plus newlib. `ps2_analyzer` recognises 229 SDK functions with runtime handlers and 38 jump tables.

## Related work

- [mholeys/choroq-hg2-decomp](https://github.com/mholeys/choroq-hg2-decomp): a splat decomp of the PAL release (SLES-51356). Its symbol names can be ported to this NTSC build by matching function signatures.
- [mholeys/roadtrip-choroq-tools](https://github.com/mholeys/roadtrip-choroq-tools): documentation and tools for the asset formats (cars, courses, fields, textures).
- [loveemu/tsq2psf](https://github.com/loveemu/tsq2psf): the TSQ/TVB music format.

## License

The code in this repository is GPL-3.0, to match PS2Recomp. *Road Trip* / *Choro Q* is © Takara / E-game / Conspiracy Entertainment. This project contains none of their code or data.
