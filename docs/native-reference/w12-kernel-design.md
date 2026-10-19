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

## Increment 1 landed (kernel core + stub wiring)

`src/native/guest/guest_kernel.{h,cpp}` implements the host libkernel: ucontext green
threads on host stacks, per-priority ready queues, semas/event-free alarms/intc+dmac
chains, SetVSyncFlag one-shot, heap model, Copy with blob tracking, SIF-DMA applied
immediately, setSyscall override dispatch resolved through installed blobs. All 48
stub TUs (21 rewired + 27 new from kernel_stubs.json) forward their $4..$ args to
`guest::syscall(num, a0..a7)`; the fallback `syscall` is gone from transliterate.py.
ELF entry 0x200008 now forwards the disasm-recovered SetupThread($4=0x33AAF0 gp,
$5=0x1F80000, $6=0x80000) and SetupHeap($4=0x17874E0, $5=-1) args (its bss-loop
`in_zero_*` locals were uninitialized — fixed to 0). Pins: `test_w12_kernel`
(return-id sema results incl. KE codes, caller-tid GetThreadId proven through the
game ring path at 00285ed8, priority order, alarm invocation args $4=id $5=ticks
$6=argument, intc cause gate, ReferThreadStatus layout, boot thread id 1). The two
bare-call $4-forwarding sites (00285ed8/00285ff0 → `_iWakeupThread`/`_iSuspendThread`)
are FIXED; the retnarrow ledger's other sites stay W12-gated.

Documented deviations from the oracle (all shortcut-commented in code):
- Host green-thread stacks (max(stackSize,0x10000)) instead of the declared guest
  RAM stack region; the EE stack itself is not modelled yet (frame arena RT_FRAME_END
  stays 0x2000000 — move to 0x1F80000 at w12-boot so SetupThread's [0x1F80000,
  0x2000000) stack region is reserved).
- Alarms/vblank fire at vblank-tick granularity (`kVblank` advances virtual EE time
  16667 us; alarm deadline = ticks x 64 us); sub-field alarm-vs-vblank tie ordering
  deferred to the W13 hash-parity chase.
- Handler invocations run on the pump stack with the would-be-next ready thread as
  "current" (owner tid makes the game's iWakeupThread ring path work); blocking
  syscalls in handler context return KE_ERROR (guest UB).
- `entry`'s SetupThread $t0 root_func (0x2000C0 = Ps2InitGsDoubleBuffer) is ignored,
  as the oracle never invokes it.
- InitAlarm's syscall-override blob (Copy 0x2e9148→0x76000 + setSyscall) is stored
  and resolved through the Copy install table; the blob's own code (no Ghidra
  function at 0x2e9148) is NOT ported yet — if the boot path dispatches through that
  override, w12-boot must manually port the blob. (Note: disasm 0x285e70 shows
  InitThread's CreateThread passes initial_priority 0; the oracle's createThread
  rejects priority <1, so the oracle likely returns KE_ILLEGAL_PRIORITY -403 there,
  DAT_2e98f0 stays -1 (non-zero → the ring gate in the i* wrappers stays open), and
  the ring consumer thread may never exist on either side — our kernel mirrors the
  rejection exactly; probe the oracle to confirm which threads it really creates.)

Still pending in W12: MMIO staging (VIF1 block + DMAC-GIF), port 0x00207b50, rt-app
frame-loop wiring (kBootMainThread/kVblank/kPumpToIdle), boot to title, oracle
interleaving probe, and the `_print` undefined reference.
