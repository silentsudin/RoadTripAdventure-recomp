# Developing Road Trip Recomp

How the recomp is built, tested and put together. For playing it, see the [README](../README.md).

- Notes for AI agents and a detailed log of runtime features and switches: [`CLAUDE.md`](../CLAUDE.md).
- Save states: [`save-states.md`](save-states.md).

## Releases

`.github/workflows/release.yml` builds the macOS DMG and the Android APK on every push to `main`
(attached to the run) and publishes a GitHub Release for a pushed tag `vX.Y.Z`, or from a manual run
with *release* ticked. Version numbers come from [GitVersion](https://gitversion.net)
(`GitVersion.yml`): the last `v*` tag, plus one patch per release. Optional secrets sign the builds
(Android keystore, Apple Developer ID and notarization); without them the APK is debug-signed and
the Mac app unsigned. Both jobs fail if any recompiled game function ends up in the app binary.

Android: the APK ships its own compiler (clang + lld, `scripts/android/build_llvm.sh`, LLVM 21 to
match NDK r30) so the phone can build the game from the player's disc. CI caches it; a fresh build
takes about an hour.

## Supported disc

| | |
|---|---|
| Title | Road Trip (USA) |
| Serial | SLUS-20398, `SYSTEM.CNF` VER 1.02 |
| Image | `Road Trip (USA).bin` / `.cue`, SHA-1 `236b902a72580f43a480f18d232723a8676b02e5` |
| Boot ELF | `SLUS_203.98`, 1,280,576 bytes, SHA-1 `2431de1ec3edd0df4be37ba564658d70c4049089`, entry `0x00200008` |

The pipeline accepts `.cue`, raw `.bin` (MODE1 or MODE2/2352) or cooked `.iso`. Only the boot ELF's hash is checked, so any good dump of this version works.

## Building the app (Windows x64)

Prerequisites: Python 3, CMake and Ninja (`winget install Kitware.CMake Ninja-build.Ninja`), Git.
The toolchain is **llvm-mingw** (clang + lld + libc++ + mingw-w64, UCRT): the runtime and the
recompiled game need clang (`ext_vector_type`, `__builtin_elementwise_*`), and the app ships a
trimmed copy so the player's PC can compile the game on first launch, as Android ships its LLVM.
`scripts/fetch_llvm_mingw.py` downloads the pinned release (SHA-256 checked); `LLVM_MINGW_ROOT`
points at one you already have.

```sh
python scripts/pipeline.py bootstrap build      # preset windows-release -> build/windows-release/
python scripts/pipeline.py run --rom "<disc>.cue"
```

Do not `git submodule update --recursive`: Granite's test-only nested submodules exceed Windows
path limits; `bootstrap` initialises only what the build needs.

How it differs from the Mac build:
- `RoadTrip.exe` exports the whole runtime (`--export-all-symbols`, ~17.5k symbols; the PE limit is
  65,535) and CMake writes its import library `RoadTrip.dll.a`, which goes in `Resources/sdk`. The
  game DLL (`roadtrip_game.dll`, built on the player's PC) links against it where the Mac uses
  `-undefined dynamic_lookup`; data is imported through lld's auto-import.
- **State shared with the game DLL must be one exported variable, never a function-local static of
  an inline function or an `inline` variable**: the DLL would get its own copy (the scratchpad host
  pointer and the display field phase were; see `ps2_memory.cpp`).
- libc++, libunwind and libwinpthread are DLLs next to the exe: exceptions and types cross the
  exe/game boundary (guest threads switch by unwinding through game code).
- `Resources/toolchain` is the trimmed llvm-mingw (`bundle_sdk.py --windows`, `KIT_REV` in the
  script forces a refresh when its file list changes). The recompilers are `Resources/recomp/*.exe`.
- The exe's manifest (`platform/windows/RoadTrip.manifest`) makes the ANSI code page UTF-8, so
  `std::filesystem::path::string()` and the narrow Win32 calls agree with SDL's strings.
- `windows.h` and `raylib.h` clash (`Rectangle`, `CloseWindow`, `ShowCursor`): the runtime defines
  `NOGDI NOUSER NOMINMAX WIN32_LEAN_AND_MEAN`, and Granite's DXGI interop is off.
- The test socket is TCP on the loopback here: `RT_TEST_SOCKET=tcp:<port>` (port 0 picks one and
  prints it); Python on Windows has no `AF_UNIX`.
- The icon is the same picture as macOS's and Android's: `scripts/make_android_icon.py` writes `platform/windows/RoadTrip.ico` (16-256 px, from the macOS 1024 px master) and the exe embeds it (`platform/windows/RoadTrip.rc.in`); SDL takes the exe's first icon for the window and taskbar.
- Data and saves: `%LOCALAPPDATA%\RoadTripRecomp` (`RT_DATA_DIR` overrides).

### Upscaler plugins (Windows)

FSR 3, DLSS and XeSS are separate DLLs behind `rt_upscaler_api.h` (a C interface), so the vendors'
SDKs stay out of the GPL app: NVIDIA's and Intel's licences forbid linking them into it, and
they ship as an optional add-on zip instead.

```sh
python scripts/fetch_upscaler_sdks.py      # pinned SDKs into build/sdks; prints the CMake options
# FSR 3: AMD FidelityFX 1.1.4 (MIT; the 2.x SDK has no Vulkan), with the llvm-mingw toolchain
cmake -S plugins/upscalers -B build/plugins -G Ninja -DCMAKE_BUILD_TYPE=Release       -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/llvm-mingw.cmake -DLLVM_MINGW_ROOT=<llvm-mingw> -DFFX_SDK_DIR=build/sdks/ffx
# DLSS + XeSS: MSVC (NGX is a static MSVC library), from a VS developer prompt
cmake -S plugins/upscalers/vendor -B build/plugins-vendor -G Ninja -DCMAKE_BUILD_TYPE=Release       -DDLSS_SDK_DIR=build/sdks/dlss -DXESS_SDK_DIR=build/sdks/xess
```

Copy the DLLs (and the vendor DLLs beside them) next to `RoadTrip.exe`, or point `RT_UPSCALER_PLUGINS`
at them. The Options row for each appears only when its plugin loads and the GPU runs it. In
the presenter, `upscalePlugin()` follows `upscaleArmAsr()`; see CLAUDE.md for the conventions.

## Building the app (macOS arm64)

Prerequisites: Xcode Command Line Tools and `brew install cmake ninja python molten-vk`. **No ROM is needed to build the app.**

```sh
git clone https://github.com/silentsudin/RoadTripAdventure-recomp.git && cd RoadTripAdventure-recomp
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
| `RT_GS_SSAA=1\|2\|4\|8\|16` | GPU supersampling rate (default: the Options setting, 8) |
| `RT_PROGRESSIVE_FIELDS=0\|1` | Overrides Options → Interlacing: 0 keeps the game's original alternating fields |
| `RT_TEXTURE_DUMP=<dir>` | Overrides `[textures] dump` in settings.toml: saves every texture the game decodes once, as `<hash>_<W>x<H>_psm<NN>.png` |
| `RT_TEXTURE_PACK=<dir>` | Overrides Options → Texture pack: a folder of replacement PNGs named by those hashes (any size) |
| `RT_ANISOTROPY=1\|2\|4\|8\|16` | Overrides `[textures] pack_anisotropy` (default 16): anisotropic filtering of texture-pack images (1 = trilinear) |
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
| `RT_TEST_SOCKET=<path>`, `RT_HEADLESS=1` | Lockstep control socket for test drivers; no window (pictures only on request) and no audio device |
| `RT_RENDER=0` | Skip VU1/GS rendering (test runs; the `render` socket command switches it back on to grab frames) |
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
- **Fast:** tests run with rendering off (`RT_RENDER=0`): VU1 and GS work is skipped, which the game's logic never reads back (memory hashes match with and without it). `frame()` switches rendering on for 4 vblanks to grab a picture. With the IOP's cycles batched, a minute of racing takes a few seconds.
- **Driving bot:** `rtharness.driver.Driver` races Adventure events. It waits a lap while an AI car lays down the racing line, then follows that line (steering by look-ahead, braking where the AI did), backs off walls, and for races listed under `[bot]` in `config/game_state.toml` backs out of wrong branches. `step` runs it in one round trip per control update.
- **Cleanup:** a test game exits by itself when its driver disconnects or dies (or none attaches within 5 minutes); `Game` stops its process group on close and at exit, and the run fails if any game outlives it.
- **Checkpoints:** tests that end with an in-game save store the memory card as a checkpoint. Later tests start from it (Continue), so long play-throughs split into independent sections.
- **What's committed:** golden frames are committed as hashes (`tests/regression/goldens.json`). The images, checkpoints and failure diffs are game output and stay in `build/regression`.

Recording play for new sections:
- `RT_TIME=virtual RT_MOVIE_RECORD=<file>` records every pad change by vblank; F5, F6 and F7 add mark, golden-frame and section-end markers.
- `RT_MOVIE_PLAY=<file>` replays a recording exactly.
- `RT_STATE_HASH=<file>` logs memory hashes, for finding where two runs diverge.

More:
- Play-through sections live in `tests/regression/sections`. Record one with `scripts/record_section.py` (normal speed, deterministic, from a checkpoint); `tests/regression/COVERAGE.md` tracks what is covered and what still needs recording.
- `--perf` adds a real-time check: the Quick Race must hold 60 fps.
- `scripts/pcsx2_cards.py` exports a checkpoint to a PCSX2 folder memory card (to confirm saves load in PCSX2) and imports PCSX2 saves as checkpoints.
- `tools/save_parser.py` and `config/game_state.toml` decode Adventure progress from a save or from RAM. Tests jump ahead in the story by editing the progress block while Continue loads it (`progress_edits`, e.g. the town and licence).
- `Game.voice_volumes()` reads the SPU2 voice volume registers (the `iop` space maps them at 0x1F900000).

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
- Audio: `build/macos-release/rt_audio_extract <data folder>/disc <out dir>` writes the disc's three long audio streams (SOUND/1CH-3CH, about an hour each) as WAV.
- Texture packs: `build/macos-release/rt_texture_extract <data folder>/disc <data folder>/textures/dumps` writes every texture on the disc as PNG in a few seconds. With `dump = true` under `[textures]` in settings.toml (or `RT_TEXTURE_DUMP`), the game also saves each texture it uses there, including the ones it builds in code; both are named by a hash of the decoded pixels. Put edited or upscaled copies (same file names, any size) in a folder under `textures/packs` and choose it in Options → Texture pack. Pack images get mipmaps and 16× anisotropic filtering (`pack_anisotropy`). Needs the GPU GS.
- The game draws interlaced 640×224 fields, moving every other one down half a line. With Options → Interlacing set to Off (the default), a game hook stops that shift and each field is shown as a whole progressive picture, like PCSX2's no-interlacing patches. paraLLEl-GS builds the picture from its supersamples with twice as many lines as columns: 2× and 4× give 640×448, 8× (the default) and 16× give 1280×896. A real 448-line frame can't fit in the PS2's 4 MB of VRAM, so this is the clean equivalent. With Interlacing On, the original fields are scanned out as before; `RT_GS_PROGRESSIVE=0` goes back to the plain field deinterlacer.
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

