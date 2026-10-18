# Race HUD live-state inputs

ROM: `build/rom/SLUS_203.98` (EE MIPS little-endian).  Instruction addresses
below are ELF virtual addresses.  The executable has two `PT_LOAD` mappings
(`0x00200000 -> file 0x1000`, `0x0029a880 -> file 0x9b880`); do not assume a
single subtraction mapping when reading a pin.

The car records start at `gs_controls_car_object` (`0x0177ac50`) and have a
`0x270` byte stride (`RankCarsByProgress`, `0x0022d88c`).  The fields here are
live car-record fields, not native authored struct layouts.

## (a) Race place: `car + 0x247`

* **Base/field:** car record + `0x247`; one byte.  The normal value is an
  unsigned zero-based running place (`0..activeCars-1`).  `0xff` is an
  inactive/finished sentinel, so consumers that expose a signed host value
  should translate it to `-1`, rather than treating it as place 255.
* **Authoritative writer:** `RankCarsByProgress` (`0x0022d7b8`), instruction
  **`0x0022d8a0`**: `sb t3,0x247(a2)`.  The input rank is seeded by the
  caller, then incremented once for every linked active car.
* **Cadence:** `raceHudTask` calls `RankCarsByProgress` every active race HUD
  update at `0x0022e138` (call instruction; the task's per-car update loop is
  immediately before it).  Thus it is a running position, not
  `GetRaceTimeRank`'s elapsed-time bucket.  It may additionally be set to
  `0xff` by finish/elimination paths (`FinishOnCarApproach` `0x0022dd8c`).
* **HUD path:** the rank badge consumes this zero-based place (the oval/digit
  group is at `0x002ebe40`; the renderer adds one before choosing ordinal
  glyphs).  **GAP:** static decompilation did not recover a direct
  `lbu/lb car+0x247` inside `UpdateRaceHud` handler 0 (`0x002496c8`); its
  handler/index dispatch is partially split/indirect.  The writer and
  zero-based semantics are direct ROM observations; the exact handler-0
  register-to-badge load remains unproven.

## (b) Current-lap elapsed frames: `car + 0x260`

* **Base/field:** car record + `0x260`; 32-bit little-endian unsigned frame
  counter.  It is the current-lap timer, not the shared total timer at another
  base.  It explains the oracle invariant `TOTAL - LAP = 2363` while the car
  is driving: a completed-lap prefix is excluded from this field.
* **Per-frame writer:** in `raceHudTask`'s car loop, **`0x0022e100`** loads
  `lw v0,0x260(s0)`, `0x0022e104` adds one, and **`0x0022e108`** stores
  `sw v0,0x260(s0)`.  The loop runs for eligible cars with nonzero zone
  (`car+0x19a`).  The equivalent second-player path is `0x0022ec18..20`.
* **Lap-boundary writer:** `RecordLapSplitTimes` resets it at
  **`0x0022d4c0`**, `sw zero,0x260(s0)`, before advancing the lap byte.
  This is event-driven and is the reset establishing per-lap timing.
* **HUD read:** the HUD result/lap row path loads it at **`0x0022daa4`**,
  `lw t0,0x260(s2)`, and passes it to the common HUD stage formatter.  This
  is a direct display-side load.
* **GAP:** the static trace proves the timer and a HUD display consumer, but
  does not prove which live-driving overlay element (if any) uses this value
  before the row/result path.  Wiring `FrameSnapshot` should nonetheless use
  `RaceTracker::currentLapFrames()`; it has the documented reset law.

## (c) Gear word: `car + 0x1ff`

* **Base/field:** car record + `0x1ff`; one-byte signed load in the ROM
  (`lb`).  Runtime values used by the gauge are `0` for reverse and `1..6`
  for forward gears; retain an 8-bit storage type and validate/display that
  domain rather than widening arbitrary negative bytes.
* **Per-frame writer:** `gs_controls_car_update` (`0x0021a310`) obtains the
  gearbox callback result and stores it at **`0x0021a38c`**:
  `sb v0,0x1ff(s0)`.  This is a vehicle-controls update, hence per active
  simulation frame.  Reset/respawn writes `1` (for example
  `ResetVehicleState`, `0x0021c9b8`).
* **HUD read:** `GetVehicleGear` (`0x002495b8`) is pinned at
  **`0x002495bc`**: `lb v0,0x1ff(a0)`.  `UpdateCarControlState`
  (`0x0024e1a0`) calls that helper at `0x0024e71c` before issuing the gear
  glyph sprite.  The atlas law is `u=(g%5)*20+140`, `v=(g/5)*16+80`, cells
  `20x14`.

## Native wiring note

`FrameSnapshot::rank` currently represents `getRaceTimeRank`, which is a
time-ranking bucket and must not feed the place badge.  Preserve it for its
existing use; add distinct snapshot/session inputs for running place,
`currentLapFrames`, and gear when the coordinator wires the game loop.
