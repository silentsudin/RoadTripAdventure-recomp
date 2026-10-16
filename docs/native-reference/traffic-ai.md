# Traffic AI — spawn, graph selection, and speed evidence

Updated 2026-10-16, `SLUS_203.98`, Ghidra `r5900:LE:32:default` / PS2.
Addresses below are ELF vaddrs; file offset = vaddr − `0x1ff000`.
**CONFIRMED** means instruction-verified (literal table values additionally
memory-verified); **INFERRED** means interpretation/pattern match, not a runtime
observation. Open questions are explicitly identified rather than filled in.

## 1. Producer correction and town-car setup

| confidence | vaddr | evidence / claim |
|---|---|---|
| **CONFIRMED** | `0x20e744..0x20e74c` | Town block task installs controller `0x2508e0` via `0x21ce28`. |
| **CONFIRMED** | `0x20e898..0x20e914`, `0x20eb04..0x20eb6c` | Ordinary resident loop calls task spawn `0x204c48` with callback `0x218458`, **not** `0x20dc50`. Tag = `0x800000 | (scene<<10) | ((i+4)<<5) | (i+1)`. Iteration bound is scene-table byte `+7`; predicate `0x23cd58(scene, sceneTable[scene].byte6+i, context+0x14)` suppresses a spawn when nonzero. |
| **CONFIRMED** | `0x20ea88` | The same producer also spawns `0x20dc50` with `0x370400 | residentCount` to prepare resident assets; see `task-system.md`. That request is not a vehicle instance/position/heading. |
| **CONFIRMED** | `0x218458` | Vehicle record is zeroed for `0x270` bytes. Tag bits `4:0` select vehicle slot; `9:5` select entry/role; `14:10` reach `CarAiInit` as scene/route selector; high 16 bits initialize status. Initial speed is zero from the clear, not an encoded task-tag speed. |
| **CONFIRMED** | `0x218cd0..0x218d6c` | Ordinary town resident uses spawn directory `0x2c2e78[scene]`, record `(entry-4)*16`. Copies three f32s at `+0/+4/+8` to car `+0x90/+0x94/+0x98`, u16 at `+0xc` to heading `+0x1d4`; resident word `+8 & 0x100` selects speed-cap byte 40, otherwise 20. Sets steering gate `+0x19a=1`. |
| **CONFIRMED** | `0x218ad8..0x218cc8` | Special scene cases use other record-index adjustments: scene `0x16`, entry>=3 → `entry-1`, cap 5; scene>=`0x17`, entry>=3 → `entry-3`, cap 15. Lower entries index directly. These caps are index bytes, not world velocity. |
| **INFERRED** | `0x218458`, `0x21a990` | This is a shared resident-car spawn path, not proof every spawned car moves. A zero-input/parked resident and a moving resident need their status and route presence traced individually. No complete parked-vs-traffic spawn-site taxonomy is claimed. |

**INFERRED / open:** No random route seed was identified in this tag. The observed
selection is deterministic scene/slot indexing; absence of RNG in these small
paths does not exclude randomized state elsewhere. The 92 loader callback
references are not 92 traffic-spawn sites.

## 2. Route families — do not conflate course graph 0 and town routes

| confidence | vaddr | evidence / claim |
|---|---|---|
| **CONFIRMED** | `0x24fd24..0x24fdc4` | Context signed byte `+0x22 == -1` or `0x22` selects outer directory lookup `0x25fbc0(scene,slot-1)` for nonzero slot; context flags `&0x80000` instead use `slot` including zero. Successful lookup supplies car `+0x250/+0x254` and route-speed table `0x2abf80`. |
| **CONFIRMED** | `0x25fbc0..0x25fbf0` | Outer lookup reads u32 directory `0x2d98a8[scene]`, then pair `base+slot*8`; returns null if directory pointer or pair's first pointer is zero. |
| **CONFIRMED** | `0x24fdc8..0x24fe04` | Other contexts select pair at `0x2bd578 + graph*8`, with context `0x0b→6` and `0x10→15`, otherwise graph=context byte. Thus this directory is not the town-scene outer directory. |
| **CONFIRMED** | `0x2500bc..0x250110`, `0x2501b4..0x250200` | Transition stride is 8; byte 0 indexes the current 24-byte segment, byte 1 the target segment. A segment's first two Vec2s form a crossing line. No waypoint-arrival-radius comparison occurs in this localization portion. |
| **CONFIRMED** | `0x250138..0x2501a4`, `0x250230..0x2502c8` | Forward transition candidates are bytes 4/5; backward candidates 2/3. Identical candidates bypass geometry. Otherwise the line joining segment third points (`+0x10/+0x14`) chooses first for a negative side, second for nonnegative side. |
| **CONFIRMED** | `0x250284..0x2502c0` | Side expression is `(b.z-a.z)*(p.x-b.x) - (b.x-a.x)*(p.z-b.z)`; `c.le.S 0,side` includes equality in second candidate. |
| **INFERRED** | `0x250068`, `0x24fed8` | This is a geometric corridor/branch representation rather than a random turn-at-waypoint rule. Segment endpoints/third points may encode lanes, but a universal lateral lane-offset constant is not established. |

**INFERRED / open:** The full graph-localization loop, aim-mode angle packing,
lateral error, off-course recovery, and per-node adaptive scratch state are not
reimplemented here. `chooseTrafficBranch` implements only the verified candidate
choice, not a complete graph walker.

### Coordinate and identity cross-check

**CONFIRMED (table bytes):** Course graph 0 pair is
`{0x2ac620,0x2ac968}` at `0x2bd578`; its 35 segment records and bounds
`x=[358,822], z=[328,572]` are independently documented in `world-geography.md`.
They fit tile-local `[0,1600]` coordinates. This check does not bind a town name.

**CONFIRMED (table bytes + spawn consumer):** Scene row 1 at `0x2be440` is
`{name=0x328760, tile=0x2b, start=0x1c, count=11}`; string `0x328760` is
“Peach Town”. Row 0 at `0x2be438` has name `0x328770` (“My Garage”), tile
`0x0b`, count 0. This conflicts with the earlier *Peach = tile 11* example;
do not silently apply that name association to traffic. No geography doc was
modified by this pass.

**CONFIRMED (table bytes + instructions):** Peach spawn-directory entry 1 is
`0x2c2348`; its first record decodes to `(459,30,786)` with heading 0.
Its Z=786 is outside course graph 0's Z=[328,572]. In the town-context branch,
scene=1 / car slot=1 instead uses outer pair at `0x2d9310`, whose pointers are
`{0x2c5ff0,0x2c67d0}`, not graph 0. **INFERRED:** Treating graph 0 as the
Peach resident route would mix two different selection domains.

## 3. Speed / braking law (implemented subset)

| confidence | vaddr | evidence / claim |
|---|---|---|
| **CONFIRMED** | `0x250978..0x2509c0` | Reads signed car speed `+0x1b8`; shift/subtract sequence computes low32(speed*216), signed integer division by `0x28000` yields route speed index. This is integer quantization, not an f32 speed-band cluster. |
| **CONFIRMED** | `0x250c24..0x250c44` | Target=`routeByte & 0x7f`. Target 0 → input 1. Otherwise delta=index−target: delta<=0 →1; delta=1 →0; delta>=2 →2. |
| **CONFIRMED** | `0x250c48..0x250c9c` | Nonzero car cap byte `+0x246` computes a **different** index `low32(speed*216)/0x14000`. Delta<=0 preserves prior input; delta=1 or 2 forces 0; delta>=3 forces 2. Cap byte zero disables override. |
| **CONFIRMED** | `0x250ca0..0x250d80` | Signed 16-bit wrapped angle errors gate steering. Positive low-angle error up to `0x4000` can OR `0x2000`; larger positive error can nudge heading +`0x100` when scene flags `&0xc==4`. Negative branch requires high-angle error<0; low error>=−`0x4000` can OR `0x8000`, otherwise can nudge −`0x100`. Steering gate is car byte `+0x19a`. Old prose saying steer applies *beyond* ±`0x4000` is reversed. |
| **CONFIRMED** | `0x250d70..0x250da8` | On the straddling/on-target branch input 1 becomes 9; scratch phase is 0x40 versus 0x80 in turning branches, with index 2..62 ORed into scratch low bits. Exact gameplay meaning of extra bit 8 is not asserted. |
| **INFERRED** | `0x2508e0` | Quantized speed command 2 is braking and 1 is gas, consistent with the existing physics/input vocabulary. The observed control-bit values themselves are confirmed; reverse/jet semantics outside the authored adapter are not inferred into new physics. |

**CONFIRMED (table bytes):** First 32 bytes at fallback `0x2abf80` read as zero
in this database. **INFERRED / open:** Whether they remain zero during gameplay
requires runtime evidence. No whole-table immutability claim is made.

## 4. Density, distance, and collision coverage

| confidence | vaddr | evidence / claim |
|---|---|---|
| **CONFIRMED** | `0x20e76c..0x20e788`, `0x20e908..0x20e914` | Town resident iteration count comes from scene byte `+7`, not a recovered vehicles-per-area density scalar. Peach row 1 supplies 11 before suppression. |
| **CONFIRMED** | `0x221258..0x221284` | Block-loaded predicate compares `context+0x60[(tile&3)]` with tile and tests the sign of streaming state byte at `0x3335b8+(tile&3)`. It does not compute a car-to-player distance. |
| **INFERRED** | `0x20e708`, `0x221258` | Observed activation/deactivation is streamed-block/resident-list driven. Exact highway population, spawn-radius/despawn-radius, maximum simultaneously moving population, and parked classification remain open. Do not substitute the 48-slot task limit for traffic density. |
| **CONFIRMED** | `0x2508e0` entire 310-instruction listing | This controller has no other-car scan or proximity-distance calculation: input uses own route, own speed/cap, heading, and scratch state. Its sole call is angle helper `0x24fed8`. |
| **INFERRED** | `0x2508e0` | No collision-avoidance following distance or time-to-collision braking constant is established. Avoidance could exist in another update or a producer of cap/route state; absence in this function is not proof of absence game-wide. |

## Authored mapping / tests

**CONFIRMED implementation scope:** `ai.h/.cpp` now expose verified integer
constants, `trafficSpeedInput`, and `chooseTrafficBranch`, with instruction
addresses in comments. `AiParams::routeTargetSpeed=-1` preserves the existing
public behavior; setting 0..255 opts into the speed law and bypasses authored
corner interpolation / lookahead braking. Arrival handling and steering remain
authored. The adapter intentionally does not claim faithful angle/scratch/jet
behavior or implement unsupported avoidance tuning.

**CONFIRMED verification scope:** `tests/native_assets/test_ai.cpp` pins both
divisors, target-zero and high-bit masking, signed division, equality/coast/brake
boundaries, cap preserving prior input, branch equality, and the opt-in adapter.
All inputs are synthetic; no shipped code or asset arrays are committed.
