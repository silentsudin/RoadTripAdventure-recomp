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

Still pending in W12: oracle interleaving probe. (MMIO staging + the
0x00207b50 model landed as increment 8: DMAC CHCR staging regs are typed
`rtguest::MmioReg` write traps — a guest CHCR.STR store from the ported
sendTexturePackets/vifKick bodies walks the chain (consumeChannel), marks
pending polls, fires the channel cause and auto-clears STR/tag-int, mirroring
the oracle's PS2Memory DMAC device; the transliterated kickers stay as-is.)

## Stub mirror layer (increments 2+3)

The oracle recompiler replaces 192 guest library functions (`[general].stubs` in
`config/roadtrip.toml`: libgraph/dma/vpu/pad/cdvd/sd/mc/newlib/sif/rpc/fs/load/
deci2) with host implementations — the guest's libsd/libpad/libfs copies never
run there. The native build mirrors that split: `config/native_host_stubs.txt`
(192 addr+name) drives CMake, which excludes those gen TUs from the `rt_guest`
compile (they stay on disk; coverage denominator stays 1542), while host bodies
in `src/native/guest/guest_libs*.cpp` define the same symbols and re-register
`rtguest::registerGuest(addr, ...)` so indirect guest calls reach them.

Canonical name+signature per stub: the gen TU's `RT_GUEST_FN` line names the
symbol; the exact `guest_decls.inc` line (what every caller compiles against —
it flattens some Ghidra u64 folds to positional u32) is the signature, falling
back to the gen def for disambiguated names (e.g. `sceSifWriteBackDCache__
002875b8`) which the .inc never declares. `scripts/gen_guest_libs.py` regenerates
`guest_libs.cpp` (pending bodies = log-once + return 0: a PORT gap, not an
oracle TODO_NAMED) and `scripts/sync_hand_sigs.py` retypes hand defs to the .inc.
Increment-1's iWakeupThread ring pin (test §4) is re-pinned to the new truth:
with iWakeupThread host-served, the game's ring copy stays dead (poison word
untouched), mirroring the oracle where it also never runs.

Wave 1 `guest_libs_sif.cpp` (46): SIF cmd handlers (SendCmd copies EE→IOP
payload and returns 1 — deviation: no IOP delivery yet), RPC semantics per
RPC.cpp (BindRpc always 0 + dummy server via kHeapAlloc once a module table
exists; CallRpc synchronous: EE-registered server funcs dispatch via rtCall,
else copy-send/zero-recv fallback, end_function called synchronously; no IOP
CPU ever runs — real answers come from the (sid,fno) host-service checklist),
IOP heap bump-alloc over a fake 2 MB IOP-RAM buffer (shortcut: unpinned base,
pin in W13), libload (`sceSifLoadModule` itself is NOT stubbed — the guest TU
runs and calls the host `_sceSifLoadModule`), libkernel helpers (InitThread→1,
InitAlarm→0, iWakeupThread/iRotateThreadReadyQueue via the syscall dispatcher),
newlib mem/str + __divdi3.

Wave 2 `guest_libs_pad.cpp` (28): libpad state machine, InfoMode table and
status-block layout verbatim from the oracle's Pad.cpp; input source is neutral
(shortcut: W13 wires rt::input). `scePadInit` returns 1 — gen 002001b8 spins
`while (PadInit != 1)` forever otherwise.

`rtguest::rtPrintf` (guest_mem.cpp + transliterate.py, 121 sites in 56 gen TUs
sed-renamed): corpus `::printf` passthrough fed guest-RAM `%s` args to host
libc → boot SIGSEGV; rtPrintf walks the format and maps each `%s` through G().
sprintf/snprintf stay `::`-routed (index-wrapped pointer args; `%s`-arg hazard
tracked for W13 if it surfaces).

Boot diagnostic (RT_APP_NATIVE_BOOT=1) now passes InitRpc/CdInit/RebootIop/
McInit/PadInit/PortOpen and sits in the CD-read wait: `sceCdRead` + `sceCdSyncS`
polling + a WaitSema nothing signals. Next wave: CD/FileIO host bodies
(oracle Stubs/CD.cpp, FileIO.cpp) so SYSTEM.CNF/BOOT2 load. Scheduling note: a
pure guest spin (no syscalls) never yields to the pump — the game's CD wait
parks on a sema, so CD services close it; timer preemption stays a W13 risk.

## Increment 4 (boot to TITLE): CD/FS/DMA/GS waves + fixes

Boot diagnostic now reaches `state=TITLE` (`Initialize Start...` →
`Initialized Done.`, 120 vblanks, faults=0). What landed:

- Wave 3 `guest_libs_dma.cpp` (9 HAND of 14; Recv/Watch/Pause = oracle
  TODO_NAMED, stay pending): kRam IS the DMAC register file (guest MMIO
  reads stay coherent with the stubs); `submitSend`/`submitSync` mirror
  Support.h:1338/1440; chains/normal transfers are WALKED and walk-discarded
  — `consumeChannel()` is the seam for the future native GS sink. Completion
  fires `kFireDmacCause` (kernel dmac chain + EnableDmacHandler mask).
  Direct-MMIO kicks (sendTexturePackets@00207b50 CHCR 0x105, vifKick@002271f0
  CHCR 0x145) complete via the MmioReg CHCR write trap (inc-8); the earlier
  `submitSync`-side completion covers the HLE-libsdma path.
- Wave 4 `guest_libs_cd.cpp` (13) + `guest_libs_fs.cpp` (9): sceCdRead =
  synchronous sector copy → return 1 (oracle CD.cpp:226); disc substrate =
  extracted tree + `.lbn_map.tsv` sidecar (path→ISO LBA; the game's CD LSN
  IS the ISO LBA, proven by the game's static SYS table entry0 0x3fbe6 =
  SYS/FONTE.GSL @261094). Unmanifested LBNs need the raw image (RT_IMAGE
  fallback wired; FirstRunExtraction retains no image path — deviation).
- Wave 5 `guest_libs_gs.cpp` (17): state faithful (GParam from ResetGraph →
  sceGsSyncV returns interlace?-1:1; GsImage 12-byte struct from SetDefLoad/
  StoreImage; SwapDBuffDc returns which^1). Kernel `kWaitVSync(fixedResult)`
  parks the green thread until kVblank (WaitVSyncTick mirror,
  Interrupt.cpp:78). GS DEVICE calls are state-only (gs-sink TODO): packets
  reach the DMA model walk-discarded; scanout parity comes with the native
  renderer.
- BindRpc serves a dummy server for ANY sid (ungated): boot binds SdRpc
  0x80000701 before any LoadModule; client->server != 0 is the SDK's bound
  signal (sceSdRemoteInit spins on client+0x24).

Fix-class found (W11 corpus, boot path): **Ghidra halves the GSL chunk
walk.** All 11 `fatal(0x2ea490)` ("GSL DMA Tag Error!") walker TUs read the
tag qword at cursor+0x10 and stride size*8+8 in pseudocode; real MIPS is
`ld/sd 0x20(cursor)` and `addiu cursor, size<<4, 0x10` (lbu type at +3;
exit char 'p'; patch = FB block += 0x1a40 — the same texture-base remap the
texture docs carry). Fixed with capstone citations against the user's ELF
(vaddr−0x1FF000). Recipe: when a boot scene self-fats, disassemble before
touching host stubs.

Bug fixed en route: host stub files masked RAM with 0x1FFFFFFF (512 MB!) —
PS2 RAM mask is 0x01FFFFFF; DMAC reg writes at 0x1000Exxx overran kRam and
SIGSEGV'd at sceDmaReset (kRam+0x1000E020). The gen corpus masks correctly
via G(); only hand waves carried the typo.

## Increment 5 (MC wave + post-TITLE load runs clean)

Wave 6 `guest_libs_mc.cpp` (23 HAND): host-emulated formatted PS2 card per
port at `<RT_DATA_DIR>/saves/mc0|mc1` (oracle src/main.cpp:1189 mcRoot),
libmc async protocol mirrored exactly: every sceMc* completes immediately,
records (cmd,result)+pending (setMcCommandResultLocked MemoryCard.cpp:377);
sceMcSync returns 1+values once then -1 idle (games rely on it, :1220).
GetInfo fills type=2/free=0x2000/format=1 — with the pending bodies returning
bare 0 the game looped `sceMcGetInfo : Timeout!` retries; that tail is gone.
Definitions must match guest_decls.inc arity EXACTLY (extra named params =
overload, registerGuest reinterpret_cast fails); when .inc names fewer
params than the oracle reads ($a1..), reach them via va_arg like
sceMcChdir/sceMcMkdir do.

Boot diagnostics extended: the per-30-vblank line now prints
scene3d(0x3356C4)/loading(0x333370)/task0+taskHandler(0x177A0D0 table) —
facts recorded in config/game_state.toml [game]. Post-TITLE the load machine
(loadStep 0x20D120 -> loadCarPartsSet -> cdReadEntry) runs continuously:
600-frame boot end-to-end is seconds, faults=0, no pendings. The earlier
"post-TITLE spin" was NOT a hang: lldb-attached rt-app instances were left
stopped (SIGTERM can't land; pkill -CONT then -9), 15 of them starved the
machine so every measurement crawled. Recipe: `pgrep -fl rt-app` before any
timing run. Diagnostic envs: RT_CD_TRACE / RT_CD_TRACE_BT (per-sceCdRead log,
symbolized backtrace), RT_BOOT_TRACE_WIN=lo,hi (per-vblank load-machine trace;
window end also dumps the live task-node table), RT_RAM_CANARY (.rodata memcmp
per vblank), RT_BOOT_START (pad Start injection), RT_FAULT_BT (backtrace on
first hit of each fault key), RT_STATE_HASH=<file> + RT_STATE_HASH_INTERVAL
(ee= FNV-1a RAM hashes per vblank, mirroring the oracle ps2_test_harness) and
RT_RAM_DUMP="vb:path,..." (raw 32MB kRam slices for byte diffing against
oracle socket state_dump EERM chunks; pair native vb+1 with oracle vb), and
RT_VIF_HIST (W3 step 1: histogram VIF1 opcodes/MSCAL entries + GIF chain
segments during the walk; stderr [vifhist]/[gifhist]).

## Sema boot fix + trustworthy multi-slice dumps (2026-10-20)

supplement_crt0 (0x28cfc0) hand-fix (982d4ca): the transliterator mis-slotted
the ee_sema_t writes from Ghidra pseudocode; real disasm sets +4/+8=1 and
+0x28=1 (struct2.init), struct2.max = sema1 id (jal1 delay slot). Native now
creates both boot semaphores (semas=2); 0x2e99a8 pair byte-identical (oracle
probe: DATs fill [1,2] at park 8 — each DAT pairs with its own call).
Multi-slice RT_RAM_DUMP (parseVbPaths fix, a4aff83) onset map @60/300/600:
most residuals born early (by vb60); 0x1770000 block jumps 11->325 by vb300
then settles 265; scratch 84->468 by vb300.
Corrected residual ledger @1800 (1183 B): 0x1e00000 495 (frame-arena scratch
leftovers), 0x1fff000 291 (game stack frames), 0x177e000 228 + 0x177f000 26,
0x1787000 49, 0x1f10000 27, env page 0x335000 20 (0x335690/0x335900/
0x33592c/0x33593c), 0x175e000 15, exception pages 0x0 7 + 0x9000/0xa000 5+5,
0x1773000 7.
