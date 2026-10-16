# Frame timing — PS2 SLUS-203.98

Static investigation of the retail ELF, updated 2026-10-16.  Addresses are
EE virtual addresses; ELF file offset = `vaddr - 0x1ff000`.  **CONFIRMED**
means the stated instruction/data flow is present in the ELF.  **INFERRED**
marks the timing consequence drawn from that flow; it is not a runtime trace.

## Verdict

**(a) Logic is locked to the VBlank-paced fixed step, nominally 60 Hz on the
configured NTSC output.**  This is not a real-time-delta game, and there is no
static evidence of an independent logic accumulator that could run more than
one game step per display refresh.

The root loop performs one task-walker pass, then waits for the GS VBlank
interrupt before it presents and begins the next pass.  Task ticks, the town
clock, vehicle gravity/integration, and minigame timers advance by fixed
integer amounts per such pass.  The result is **not** merely a renderer
limited to 60 Hz: advancing this same loop at 30/120 Hz would respectively
halve/double the rates of those systems.

The configured CRT call is `SetGsCrt(1, 2, 1)`: the standard PS2 SDK argument
shape makes this **INFERRED** as NTSC, interlaced, frame mode (nominal
59.94-Hz field cadence).  The stronger observation is that the game waits on
every INTC VBlank event and runs its task pass once per wait.  Thus, even if
the CRT's interlaced presentation is observed as 29.97 completed frames,
logic is VBlank/field paced, not a deliberate 30-Hz software update.

## Evidence ledger

| vaddr | evidence | status |
|---|---|---|
| `0x00200528..0x002005a8` | Root loop `FUN_002003b8`: `jal gs_tasks_walker` (`0x204f98`), DMA/VIF kick, `jal sceGsSyncPath`, then `jal sceGsSyncV`; it next clears `REG_RCNT0_COUNT`, selects the buffer env, and continues to `sceGsSwapDBuffDc`. | **CONFIRMED** |
| `0x0020067c..0x002006d8` | The same loop calls `sceGsSwapDBuffDc`, `sceGsSyncPath`, clears a flag, increments global `DAT_00335900` by one, XORs the buffer index by one, then branches back to `0x00200528`.  No catch-up/update-count loop appears between the VBlank call and the back edge. | **CONFIRMED** |
| `0x0026e458..0x0026e480` | `sceGsSyncV` calls `FUN_00285420` in its normal mode. | **CONFIRMED** |
| `0x00285420..0x0028544c` | The wait is concrete: `lui v0,0x1000`/`ori v0,v0,0xf000`/`sw v1,0(v0)` writes `4` to `INTC_STAT` (`0x1000_F000`) to ack-and-clear VBlank; `lui v0,0x1001` + `lw v0,-0x1000(v0)` re-reads the same `INTC_STAT` register at `0x1000_F000`, `andi v0,v0,4`, and `beq v0,zero,0x00285430` spins until it is set.  This is the VBlank INTC wait used by the main loop. | **CONFIRMED** |
| `0x0026e494..0x0026e4a4` | After the wait, `sceGsSyncV` reads `0x1200_1000` (GS CSR) and extracts bit 13 (field parity).  It is used for buffer/display-env selection, not as a measured elapsed-time value. | **CONFIRMED** |
| `0x00204fb0..0x00204fec` | `gs_tasks_walker` calls each active task callback once, then `lw v0,8(s0); addiu v0,v0,1; sw v0,8(s0)`.  The per-task counter has exactly one increment per root-loop traversal. | **CONFIRMED** |
| `0x002105b8` (state-4/state-8/default branches) | The town/state task owns the world-clock word at context `+0x14` (reads at `0x002106c4`, `0x00210714`; store at `0x0021072c`).  Per-update advancement is fixed integer addition — no delta operand anywhere in the task.  The `+1`-per-update reading follows the walker cadence (line above); a literal wrap immediate for `0x34bc0` (216000 = 24 h x 9000) was not found inside the task range `0x002105b8..0x00211200`, so the wrap itself is INFERRED from the 9000/hour unit below, not byte-confirmed here. | tick cadence **CONFIRMED**; wrap **INFERRED** |
| `0x002105b8` (state-2 transition) | State transition sets the clock to `old + ((taskTick & 3) * 9000) + 9000`; this grounds the documented 9000 ticks/hour unit in shipped code. | **CONFIRMED** |
| `0x0026d1e0..0x0026d224` | Day-cycle step task compares its task's `+8` counter directly to the `u16` duration table at `0x002dcd50`, with a second comparison at duration minus `0x40`; no real-time conversion is involved. | **CONFIRMED** |
| `0x0021ba70`, `0x0021bd1c` | Vehicle master tick applies gravity with a literal `addiu ..., -0x59` to vertical velocity. | **CONFIRMED** |
| `0x0021c308..0x0021c35c` (within vehicle master tick `0x0021ba70`) | Vehicle master tick computes position increments from the current fixed-point velocity (`velocity << 4 / 0x19`) and adds them directly into `car+0xa0/+0xa4/+0xa8`; no elapsed-time operand is supplied to this task callback. | **CONFIRMED** |
| `0x0021ba70` | The master vehicle tick's callers/callees/reference set contains physics, input, collision, surface and position helpers but no timer-read or delta helper.  Its only parameters are task/frame context and car state. | **CONFIRMED** |
| Whole ELF immediate-address scan | Capstone scan of all decodable words looked for `lui 0x1000` / `lui 0x1f00` plus nearby address construction and loads/stores.  The only counter-flavoured use in the root loop is the store-zero at `0x002005ac` — a register-indirect `sw $zero, ($s5)` whose target `REG_RCNT0_COUNT` is a liveness inference, not an immediate address.  Observed timer/INTC uses are library synchronization/setup.  No R3000/RCNT count read was found flowing into town/vehicle/minigame updates.  This is a bounded negative static result: indirect or dynamically generated addresses are not ruled out. | **CONFIRMED observation; INFERRED no real-time motion source in the examined code** |
| `0x002000d4..0x0020018c` | Display setup calls `sceGsResetGraph(0,1,2,1)` and `sceGsSetDefDBuffDc` twice.  The two resolution alternatives are `0x280×0xe0` and `0x200×0x100`, each with double buffers. | **CONFIRMED** |
| `0x0026e328..0x0026e348` | `sceGsResetGraph` masks those arguments and invokes `SetGsCrt(a1&1,a2&0xff,a3&1)`; `SetGsCrt` is PS2 syscall 2 at `0x00284ba0..0x00284ba4`. | **CONFIRMED** |
| `SetGsCrt(1,2,1)` | PS2 SDK convention identifies arguments as interlace=1, display mode=2 (NTSC), ffmode=1 (frame).  The ELF itself proves the numeric tuple and syscall but does not embed symbolic mode names. | **INFERRED** |
| `0x002005b0..0x002005bc`, `0x0026f340/0x0026f3c8` calls | The field-parity return from `sceGsSyncV` becomes a flag passed to the display environments.  Root-loop task execution is unconditional before this point; there is no every-other-VBlank software update gate here. | **CONFIRMED** |

## Questions resolved

### 1. Per-frame wait

The game does **not** poll the proposed `0x1000_2000` GS interrupt-status
address in the root loop.  It waits through `sceGsSyncV`'s `INTC_STAT` loop at
`0x00285420..0x0028544c`, polling `INTC_STAT` (`0x1000_F000`) bit `0x4`; the
library then
uses GS CSR `0x1200_1000` bit 13 at `0x0026e494..0x0026e4a4` for parity.  The
wait is between task update/render submission and the double-buffer swap.

### 2. Real-time source

No confirmed gameplay data flow from an RCNT/TM counter, VBlank counter, or a
`sceGetTimerCount`-style helper exists in the inspected root, task, town, and
vehicle paths.  `REG_RCNT0_COUNT = 0` after `sceGsSyncV` is a write/reset, not
a delta read.  The hardware scan should not be read as proof that *no* obscure
indirect timer access exists anywhere in the binary; it does establish that
the main timing-critical consumers above do not use one.

### 3. Fixed step

The task walker provides the common fixed cadence.  Town time advances one
tick per active update and is explicitly compared with task-frame durations;
9000 ticks are an in-game hour (at this cadence, 150 seconds / 2.5 minutes per
hour).  Vehicle gravity and position integration are literal fixed-point
operations each vehicle update.  Existing minigame evidence is consistent:
the game stores records and evaluates limits in frame counts (for example,
the documented 300-frame penalty and 360000-frame cap in
[`minigame-logic.md`](minigame-logic.md)).

### 4. Deliberate frame skip / half rate

No deliberate root-loop frame skipping or `update every other VBlank` branch
was found.  It alternates GS buffers each VBlank and supplies parity to display
setup.  **INFERRED:** interlaced CRT output can make the visual presentation
field-based/approximately 30 completed frames per second, but it does not
make the software task/logic loop half-rate.  Scene-specific pauses and flags
can of course suppress individual task callbacks; that is not a global
half-rate scheduler.

## Native implications

### Fidelity baseline

Run one original logic step for every 60-Hz-equivalent VBlank tick and retain
integer ordering: task callbacks, task-tick increment, town clock, vehicle
fixed-point integration, and minigame frame counters.  Use a fixed accumulator
for host timing; do not substitute host `dt` directly into the recovered
integer physics.  A 30-Hz logic tick would make the town clock, task timers,
minigame times, gravity and vehicle motion run at roughly half authentic rate;
120 Hz would make them roughly double-rate (and halve apparent per-step motion
only if the constants are separately altered).

### Flexible-FPS work (a behavioural change, not the default rebuild)

To make logic FPS-flexible, inventory and convert every per-step quantity:

* time-of-day `+1`, task counters, transition-duration tables and all
  frame-count minigame/race limits;
* vehicle acceleration/gravity (`-0x59`), drag, steering/angle increments,
  integration and collision iteration counts;
* animation/effect counters and input repeat tables.

That conversion needs a carefully defined 60-Hz simulation quantum or
fractional accumulator, plus deterministic handling of multiple simulation
steps per render.  It cannot be achieved safely by removing `sceGsSyncV` or
by scaling only car velocity.

### Higher refresh visuals

For fidelity, keep the fixed 60-Hz simulation and render interpolated
snapshots at host refresh; frame generation/interpolation is therefore the
appropriate route for >60-Hz *visuals*.  Preserve fixed-step state for UI,
clock, physics and gameplay decisions, and keep the interlaced/field parity
model only where it is needed to reproduce the original output path rather
than as a host-render timing constraint.
