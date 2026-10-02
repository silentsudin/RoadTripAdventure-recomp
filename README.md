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
- the **3D attract demo** at a full 60 fps (measured on an M3 Max). The game's own VU1 microcode is statically recompiled on your Mac, and graphics are rendered on the GPU through Vulkan (paraLLEl-GS on MoltenVK). With `RT_VU1_MODE=interp` the same demo runs at about 13 fps.

Known problems:
- The attract demo stays at the start line instead of starting the race.
- Input and audio haven't been looked at.

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
| Next: demo stuck at start line, input, audio, symbol names | ⬜ |

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
| `RT_SHOW_FPS=1` | Shows the game's frame rate (buffer flips per second) on screen and in the log |
| `RT_VU1_STATS=1` | Logs VU1 runs, VU cycles and host time per second |

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
| `vu1 skip idle pipeline commits` | Speed: the VU1 interpreter scanned every pipeline slot on every emulated cycle. It now skips empty pipelines and cycles where nothing is due. Output is bit-identical. |

### GPU rendering (Vulkan)

- The GS (the PS2's rasterizer) runs on **paraLLEl-GS**, a Vulkan compute implementation. It lives on the fork as the submodule `ps2xRuntime/third_party/parallel-gs`.
- On macOS it runs through **MoltenVK**. The build copies `libMoltenVK.dylib` (Apache-2.0) into `RoadTrip.app/Contents/Frameworks`, so players need neither the Vulkan SDK nor Homebrew.
- The runtime's GS front end still parses the command stream, so CSR, FINISH/SIGNAL and transfers keep working, but it mirrors the raw GIF packets and register writes to the GPU instead of rasterizing them.
- VRAM is only copied back to the CPU when the game reads it.
- The interlaced 640×224 fields are deinterlaced to a full 448-line picture.
- If Vulkan can't start, the app logs why and falls back to the software GS.

### Recompiled VU1 microcode

- On first launch, `ps2_vu1_recomp` reads the VU1 overlays from the game's ELF.
- From the MSCAL entry points it explores every reachable instruction and pipeline-timing state, then emits C++ with each stall worked out at compile time.
- That C++ is compiled into `libroadtrip_game.dylib` with the rest of the game.
- When the code can't predict the timing (XGKICK overlap, D/T bits, unexpected jumps), it hands over to the interpreter with the exact state.
- The runtime only uses the recompiled code while VU1 code memory matches the image it was compiled from.
- Checking: run the game with `RT_VU1_CAPTURE=<dir>` to record VU1 runs, then compare them bit for bit with the fork's `vu1_replay` / `vu1_replay_native` tools.

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
