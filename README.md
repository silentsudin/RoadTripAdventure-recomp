# RoadTripAdventure-decomp

A static recompilation of **Road Trip** (*Choro Q HG 2* / *Road Trip Adventure*, PS2, USA **SLUS-20398**) into native code, built on [PS2Recomp](https://github.com/ran-j/PS2Recomp). The first target is macOS on Apple Silicon.

> **No game code or data is included, not even in the built app.** `RoadTrip.app` contains only our code, the PS2Recomp runtime and recompiler, and runtime headers. On first launch it asks for *your* disc image, copies the game files off it, recompiles the game's program into C++ and compiles that with your Mac's own clang. The result is a game library in `~/Library/Application Support/RoadTripRecomp/`. That means the app can be shared without sharing anything of Takara's. Don't distribute anything from that Application Support folder.

## Status

This is early, experimental work. The game boots and renders the **publisher logo and the title screen** correctly through the software GS and VU1. Known problems:
- It runs well below full speed.
- The picture is shown at field height (640×224, squashed) instead of the interlaced 640×448.
- After the title it cycles to a black screen, probably the attract demo.
- Input and audio haven't been looked at.

| Milestone | State |
|---|---|
| M0 Scaffold: pipeline, ISO reader, first-run extraction, app bundle | ✅ |
| M1 PS2Recomp tools and runtime build on macOS arm64 with Apple clang | ✅ |
| M2 Full recompilation of SLUS_203.98 compiles and links (1,376 functions incl. 229 SDK stubs, 0 errors) | ✅ |
| M2b No game code in the app: build on first launch from the user's disc | ✅ |
| M3 Boot to `main` (crt0, kernel syscalls, stubs triaged) | ✅ |
| M4 IOP modules (IOPRP234, LIBSD, SNDMOD Tamsoft driver, MCMAN, PADMAN) | 🔄 all load via ps2xIOP; behaviour unverified |
| M5 First pixels: logos and title (VU1 microcode → GS) | ✅ |
| Next: interlaced display height, attract/demo black screen, input, audio, speed (GPU GS), symbol names | ⬜ |

## Supported disc

| | |
|---|---|
| Title | Road Trip (USA) |
| Serial | SLUS-20398, `SYSTEM.CNF` VER 1.02 |
| Image | `Road Trip (USA).bin` / `.cue`, SHA-1 `236b902a72580f43a480f18d232723a8676b02e5` |
| Boot ELF | `SLUS_203.98`, 1,280,576 bytes, SHA-1 `2431de1ec3edd0df4be37ba564658d70c4049089`, entry `0x00200008` |

The pipeline accepts `.cue`, raw `.bin` (MODE1 or MODE2/2352) or cooked `.iso`. Only the boot ELF's hash is checked, so any good dump of this version works.

## Building the app (macOS arm64)

Prerequisites: Xcode Command Line Tools and `brew install cmake ninja python`. **No ROM is needed to build the app.**

```sh
git clone --recurse-submodules <this repo> && cd RoadTripAdventure-decomp
python3 scripts/pipeline.py all run
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
| `RT_DEBUG_UI=1` | Shows the PS2Recomp debug panel at startup (F1 toggles it) |
| `RT_THREAD_DUMP=<s>` | Prints guest threads, wait reasons and semaphores every *s* seconds |
| `RT_FRAME_DUMP=<dir>` | Saves a PNG of the game picture every `RT_FRAME_DUMP_SECONDS` (default 2) |

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
patches/             our changes to upstream PS2Recomp (applied by `bootstrap`)
third_party/PS2Recomp  upstream submodule, pinned
generated/           (ignored) ps2_recomp output
build/               (ignored)
```

## Working on the recomp

- **Config.** Edit `config/roadtrip.base.toml`. It holds stubs, skips, extra entry points, jump tables and MMIO. Then run `pipeline.py analyze build` and relaunch; the changed `build_id` rebuilds the game. `config/roadtrip.toml` is regenerated, so don't edit it by hand.
- **Game hooks.** Runtime hooks for this game live in `src/game/overrides.cpp` (`PS2_REGISTER_GAME_OVERRIDE`). Use `bindAddressHandler` to route an address to a runtime stub.
- **Upstream fixes.** Commit them inside `third_party/PS2Recomp`, then export them with `git -C third_party/PS2Recomp format-patch -o ../../patches <base>`.
- **Debugging.** Use PCSX2's debugger as ground truth for PCs and register state. Upstream's [stripped-game walkthrough](https://github.com/ran-j/PS2Recomp/wiki) covers `Function not found`, `[Syscall TODO]` and the other common failures.

## Upstream patches

| Patch | Why |
|---|---|
| `0001-cd-register-files-at-original-lbn` | The game reads sectors by LBN from a TOC baked into the ELF; it does not use `sceCdSearchFile`. The installer records each file's original LBN in `disc/.lbn_map.tsv`, and the app registers them at boot. |
| `0002-runtime-keep-custom-memory-card-root` | `loadELF` resets the IOP, which runs MCSERV init. Without this patch the memory card root is reset to `<elf dir>/mc0` and saves land inside the disc tree. |
| `0003-recomp-cmake-allow-add-subdirectory` | Lets `ps2_recomp` build as part of our CMake project, so it can be bundled. |
| `0004-recomp-synthesize-undiscovered-stub-functions` | In a stripped ELF many SDK routines are only reached by a tail-call `J`, or never called, so function discovery misses them and their configured `name@addr` stubs were silently dropped (72 of 229). The main thread died on `J scePadRead`. This patch creates the missing stub wrappers. |

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
