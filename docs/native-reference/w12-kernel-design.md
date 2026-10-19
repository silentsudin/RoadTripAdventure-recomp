# W12 kernel dispatcher — design notes

Ground truth for the 48 called kernel stubs (ledger: `build/scratch/port/w12_link_ledger.txt`,
full unresolved list `w12_unresolved_nonguest.txt`). Oracle semantics reference (read-only,
never shipped): `ps2xRuntime/src/lib/Kernel/Syscalls/{Thread,Sync,System,Interrupt}.cpp` and
`Stubs/{SIF,RPC,CD,DMA}.cpp`.

## Ported-but-blocked inputs

- 0x00207b50 (P15b skip): writes DMAC GIF-port MMIO registers; needs the W12 MMIO model
  in the shim (same class as the kRam-hole MMIO staging) — port once handlers exist.
- VIF1 register block (0x1000_0000 range): menu tasks whose FIRST frame reaches
  uiFade→vifDirect are unpinnable before W12 — the VIF1 MMIO fatal-loop kills the pin
  process (exit 133 class). Include the VIF1 block in the W12 MMIO scope (REG_* staging
  already in guest_decls.inc fallbacks).
- Syscall v0 semantics: kernel stubs' `in_v0 = syscall(N)` captures must return the REAL
  v0 per the guest ABI — iSleepThread/iWakeupThread return the caller's thread id (W11-39
  fixed the two stubs to `in_v0 = syscall(...)` capture; the value model lands with W12).
- retnarrow ledger (`build/scratch/port/retnarrow.py`, 773 pseudocode-64-bit vs uint32_t
  return/param sites): the 10 trap/softfloat-path sites are fixed; the rest are W12-gated
  per-site ABI evidence — do NOT blanket-widen. Also bare-call $4-forwarding sites
  (incl. bare `dpsub()` in cos 00274810) stay on that ledger.
- Deferred stubs on disk: exactly the 27 in `build/scratch/port/kernel_stubs.json`
  (`syscall(0xN)` shape, corpus 1515+27=1542). The 48-stub link ledger counts called
  entries; some already have real TUs.
- trapscan gate (`build/scratch/port/trapscan.py`, 0 entry-brk corpus-wide): W12 dispatcher
  code must keep Ghidra `in_v*` pseudo-local initializations and never `>> 0x20` on
  uint32_t, or clang -O2 replaces the function with a `brk #0x1` stub (UB entry-brk class).

## Threading model found in the corpus (call-site evidence)

Only 3 `CreateThread` sites; the game runs essentially one main task plus three library
service threads, all blocking cooperatively:

| thread | entry | stack | evidence |
|---|---|---|---|
| main game task | `InitThread` 0x285e00 → ring loop 0x285d28 | 0x400 @0x1776170 | threadRequestLoop note |
| libcdvd EE callback | 0x277dd0 (`_Cdvd_cbLoop`) | 188 B @0x17735c8 region | sceCdInitEeCB 0x277e90 |
| libcdvd Sd callback | 0x279660 | 0x1000 @0x1773a00 | sceSdRemoteCallbackInit 0x279588 |
| libkernel request loop | 0x285d28 | 0x400 @0x1776170 | InitThread 0x285e00 |
| libmc delays | mcDelayThread 0x27a160 = SetAlarm + SleepThread | (caller thread) | mcHearAlarm note |

`SleepThread` blocking loops only in 0x27a160 (libmc) and 0x288588 (parking loop).
No preemption-dependent code found: every thread parks on WaitSema/SleepThread and is
woken by iWakeupThread/SetVSyncFlag/alarm callbacks.

## Design

- **Cooperative green threads, host-single-threaded.** Each kernel thread = a ucontext
  running on the thread's own guest-RAM stack (the `th` struct's sp/size from CreateThread).
  Determinism (state hashes) requires guest-like interleaving: switch only at kernel
  syscall boundaries (SleepThread, WaitSema, block/return), never on a wall-clock timer.
  A `SwitchContext` helper in the dispatcher picks the highest-priority ready thread —
  mirrors the guest ready-queue (RotateThreadReadyQueue is used by libkernel's loop).
- **Entries resolve through the registry**: thread entry address → `rtCall`/registry lookup,
  so the ported native function runs; `ExitDeleteThread` retires the context.
- **Semaphores**: int-count sema table (CreateSema/SignalSema/WaitSema/PollSema/
  DeleteSema + `_i` in-handler variants) with wait queues; blocking = switch, not host lock.
- **Alarms/vblank**: SetAlarm registers (sec, delta, callback, arg) fired from the app's
  guest-vblank boundary; SetVSyncFlag(wake-queue) likewise — one vblank pump in the frame
  loop drives every SleepThread park in the game.
- **Intc/Dmac handlers** (Add/Remove*Handler, _Enable*/_i*): handler lists keyed by line;
  fire from the app's synthetic interrupt sources (vblank line 0, GIF 3, etc.). FlushCache = no-op.
- **SIF/SIF RPC** (sceSifSetDma/SetDChain/DmaStat/StopDma/GetReg/SetReg + RPC from the
  0x285xxx lib): these are the IOP-request path. W12 keeps a guest-memory SIF command
  bridge: calls queue into the request ring at 0x177657c exactly as the game writes it, and
  our host-side IOP service implementations (SND/CDVDMAN/MC — authored, guided by the
  oracle Stubs) consume and reply. sceSifSetRpcQueue/RegisterRpc/ExecRequest implemented per
  oracle RPC.cpp semantics.
- **SetGsCrt / GsGet/PutIMR / Ps2InitGsDoubleBuffer**: write the shadow GS register state
  the W12 display path reads; IMR is just a masked word.
- **_Exit / syscallMemInstall / getSyscall / setSyscall**: no-op table ops + atexit-style
  exit flag.

## Non-goals

- Host std::thread per kernel thread (breaks determinism) — rejected.
- Porting unused stubs (InitAlarm, InitMode*, PowerOffCB…): implement only the 48 called
  entries; add on demand when a later link shows new references.

## RPC surface to implement (surveyed from 132 `sceSifCallRpc` sites)

IOP module stack the game loads (002001b8, strings verified in ELF at 0x2e9a58..):
`IOPRP234.IMG`, SIO2MAN, MCMAN, MCSERV, PADMAN, LIBSD, SDRDRV, SNDMOD (Takara's own
sequencer — the same module the recomp's IOP-native `set_reverb` stand-in targeted).
Server names in the table below are inferred (fno ranges), not yet pinned to modules.

| client end | fnos | presumed server |
|---|---|---|
| 0x175e840 | 11–16, 31–38, 48–55, 0xFFF0 | SCDVD (cd read/status family, game 0x257xxx + libcdvd) |
| 0x1774b40 | 12–18, 20, 21, 0xFE | SPU2 core 0 (sceSd*, SNDMAN) |
| 0x1779b80 | 11–15, 17, 19, 20, 22–26, 0xFF | SPU2 core 1 / MCSERV |
| 0x177a000 | 0xFF | core mgmt |
| 0x2e80d0 | 14 | libcdvd-internal |
| 0x2e8948 | 22, 34 | libcdvd-internal |

51 distinct (end, fno) pairs — the host-side IOP service checklist for W12.

## Open questions for W12 implementation day

- Exact interleaving the oracle produces at vblank (probe with oracle thread dump
  `RT_THREAD_DUMP=5` on title/town/race before pinning scheduler order).
- The bare `_print` undefined reference (origin TU unknown; likely a newlib debug call).
