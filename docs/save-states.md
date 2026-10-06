# Save states: feasibility and plan (task #28)

## Verdict

Doable with restrictions, no redesign. The runtime keeps no guest state on host stacks:
`EeScheduler` runs every guest thread on one host thread from plain `GuestThread` contexts;
recompiled functions are resumable (every `jal` site and back edge is a resume label), and every
pause returns or throws to `EeScheduler::run()`. At the top of `run()` (after
`processPendingEvents()`) everything is in RDRAM, scheduler members and host-side emulation state.

## What stands in the way

1. Continuations are `std::function` closures (`EeWaitState::completion`,
   `GuestThread::resumeCompletion`, `GuestInvocation::onComplete`), about 10 creation sites (vsync
   waits, CD streaming, RPC completion, SIF commands, exit handlers, MPEG/IPU, a syscall override).
   Each needs a serializable tag `{kind, args}`; untagged ones mean "not now".
2. About 200 globals of host-side emulation state (HLE stubs: RPC, SIF, pad, memory card, CD, VFS,
   guest heap; the IOP: kernel, CPU, modules, timers, ioman, cdvd; SPU2).
3. GPU-resident GS state (paraLLEl-GS VRAM, the hardware GS's render targets).

## Restrictions

- Snapshots only at the scheduler loop top, after a vblank's events; postponed a few frames while
  something can't be saved (untagged continuation, movies/MPEG, a memory card operation, a PATH3
  FIFO or half-done GS transfer).
- States are tied to the build (ELF CRC 0x5A49851D, SDK build id, function-table hash). Memory card
  saves stay the durable format.

## Contents and difficulty

| Part | Notes | Difficulty |
|---|---|---|
| EE RAM 32 MiB, scratchpad | memcpy | trivial |
| Guest threads (`EeScheduler`) | contexts field by field, queues, semaphores, event flags, alarms, INTC/DMAC handlers, deadlines (rebased on load), invocations, cycle/vsync counters; fixed iteration order | medium |
| EE hardware (`PS2Memory`) | DMA, VIF, IO registers, timers, TLB, GS privileged registers, PATH3 state; `syncGifVif1()` first | easy |
| VU0 / VU1 | memories, VU1 interpreter state; bump the native-code generation on load | easy |
| HLE stubs | group each file's globals in a struct; VFS descriptors reopened by path | medium, tedious |
| IOP + SPU2 | `serialize()` per class (IOP RAM, kernel, modules, rpc, timers, ioman with paths, cdvd; SPU2 RAM, registers, voices, AutoDMA, reverb); flush the host audio queue on load | medium-high |
| GS frontend | registers; require an idle transfer | easy |
| GS VRAM | CPU: bulk `RestoreVram()`; paraLLEl-GS: upload VRAM, reset, replay registers, invalidate caches; hardware GS: restore local memory, drop targets (first frame after a load may be wrong, hidden behind the menu) | medium |
| Game hooks / app | `overrides.cpp` camera build state; app caches get `onStateLoaded()` | easy |

## Design

- Quiesce: `serviceStateRequest()` in `EeScheduler::run()` after `processPendingEvents()`;
  requests from the menu or test socket; while paused, run up to ~120 vblanks behind the menu
  until `canSnapshot()` says yes.
- Serialization: one `template<class Ar> void serialize(Ar&)` per subsystem, explicit
  little-endian fields.
- File: `<data>/states/slot<N>.rtstate` — header (magic, version, identity, vblank, backend, place)
  then chunks `{fourcc, version, sizes, xxhash}` (EERM, SPAD, EESC, EEHW, VU0M, VU1M, VU1S, GSPR,
  GSFE, GSVR, IOPR, IOPK, IOPM, SPU2, HLE*, VFS, GAME, THMB). ~40 MiB raw, ~8-15 MB with zstd -3
  (full zstd via FetchContent; libchdr's is decode-only). Compress and write on a worker,
  temp file + rename.
- Memory card: separate from states; no snapshot during card I/O; on load mark the ports
  "card changed".
- UI: pause menu "Save / Load state", 4 slots with thumbnail, place and time, confirmations;
  hidden outside gameplay and during movies. Android: an automatic suspend slot later.

## Risks and spikes

1. Forgotten host state: cross-process hash test from the start, load into a different moment,
   an extended `RT_STATE_HASH_FULL` (scheduler digest, SPU2 RAM, VRAM, VU1).
2. Continuations: Spike A (0.5-1 day) logs `canSnapshot()` per vblank over race and town movies:
   the share of savable vblanks and the worst wait.
3. Spike B (2-3 days): in one process, copy everything at vblank N, run 600 vblanks, restore, run
   again, compare hashes (first without the IOP, to see what it costs).
4. GPU GS restore, IOP/SPU2 (the SNDMOD radio stream), `unordered_map` order.

## Effort

About 20-25 agent-days in all; a Mac MVP (CPU GS and paraLLEl-GS, strict versioning) 12-15.

## Tests

`tests/regression/test_save_states.py` with socket commands `save_state` / `load_state`: same
process and fresh process hash equality over 600 vblanks, scenes (race, 2P, town, shops, title,
radio audio hash, loading), backends, memory card interplay, robustness (corrupt files, identity
mismatch, 50 loads, Android background during a save), byte-identical save-load-save, Android via
`AndroidGame`.
