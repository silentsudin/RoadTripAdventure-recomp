# Mini-game logic — SLUS-203.98 (evidence ledger)

Updated 2026-10-16. Per-minigame gameplay logic, scoring tiers, prizes and
score-persistence fields, decompiled from the shipped ELF. Every claim is
marked **CONFIRMED** (instruction-verified at the cited vaddr) or **INFERRED**
(pattern-matched but not proven). Address→file offset conversion is
`vaddr − 0x1ff000`.

**Salvage audit (2026-10-16):** this is a corrected inherited ledger, not a
claim that every original CONFIRMED item was independently repeated. §4 lists
the fresh checks and corrections. Unlisted CONFIRMED items retain the previous
pass's evidence attribution. Entry/unlock permission remains OPEN: the progress
pair checks below select result tiers after a clear, not permission to enter.

## 0. Shared machinery

### Result channel — `GetOrSetPresidentRaceResult` `0x0023D8C8`

28-byte get/set in global `gs_president_race_result_address`:
`FUN_0023d8c8(x)` stores `x` when `x != -1`, always returns the stored value.
The decoded minigame tasks write their outcome code here; parent task `+4`
is a separate completion/child-notification channel (often −1 on teardown),
not a copy of the outcome. **CONFIRMED** (e.g. `0x25EB44` versus `0x25E898`;
callers listed in `functions.tsv` row
`GetOrSetPresidentRaceResult`). Outcome codes as used by the games below:

| code | meaning in the games below |
|---:|---|
| 0 | fail / forfeit / no prize |
| 1 | cleared (or first-time clear) |
| 2 | cleared, better tier (time limit met / opponent beaten) |
| 3 | game-specific: best tier in some games; unchanged balance in roulette |

The mapping result → money/stamps lives in the completion dialog layer
(`UpdateRaceGameTask 0x0020F3B8` → `AwardRaceResult 0x00238950`, region
`0x0023BC00–0x0023E100` owned by the rules-tables pass) — **OPEN here**.

### Money — `FUN_0023DC58` (spend/credit, "SpendTimerBudget" in functions.tsv)

`FUN_0023dc58(amount, player)` subtracts `amount` from
`(&DAT_0177fdb4)[player * 0xd12]` = money at `0x0177F760 + player*0x3448 + 0x654`:

* returns `-1` (`0xffffffff`) and does nothing if `amount > balance` (insufficient funds),
* clamps the resulting balance to 999999 (`0xF423F`),
* charges or credits by sign — the roulette payout path calls it with
  `-payout`, so a negative "spend" is a credit. **CONFIRMED** (`0x23dc58`).

### Player position globals (all minigame sensors read these)

`0x0177ACE0` = player car position vec3 {x, y(height), z}. Ghidra prints
`gs_race_cars_address` for the x component (label), `DAT_0177ace4` for y,
`DAT_0177ace8` for z. **CONFIRMED** by the goal-plane Y-band test and
`UpdateNpcTailCarTransform 0x00259A08` ("car position 0x177ace0").

The frame-context flags word at `param_2+0x28` supplies the widely tested
mask `0x4` (bit index 2): the decoded timed zone games increment a run-frame counter while
`(flags & 4) && !(flags & 0x8000)`, and scores its *time tier* against that
counter (see per-game limits below). Read as "game live / not in a fresh-press
frame". **CONFIRMED counter mechanics; semantic name of bit 4 INFERRED.**

### Convex-quad / half-plane sensor

All zone games test "player (x,z) vs 4-edge quad" via the same four
cross-products. Storage is AoS vec2 corners {f32 x, f32 z} × 4; test point is
(player x, player z); all four `cross(T−Qk, Qk−Q1−Qk) > 0` must hold (some games
inflate one cross-product by a sign byte from a table). **CONFIRMED**
(`0x25e630`, `0x25f6a0`, `0x25eb70`, `0x25a120`).

## 1. Event table and spawn path

Adventure-mode event records 16 bytes each, tagged: tag `< 0x23` = races and
`tag >= 0x23` selects record `0x2BDDE8 + tag*0x10` inside the same blob
(equivalently records at `0x2BE018 + (tag−0x23)*0x10`).
**CONFIRMED** (StartRaceModeTask `0x20F950` prologue `0x20f9b8–0x20fa3c`;
tables labelled `gs_game_race_tables_adventure @0x2BDDD0`,
`gs_game_race_tables_quick_race @0x2BE158`, `gs_game_race_tables_two_player
@0x2BE1D8`).

Record layout: `{u8 nameIdx/id, u8 kind, u8 a, u8 b, u32 paramPtr, u32 taskFn,
u32 auxFn}`. `nameIdx` selects the venue name + spawn position struct at
`0x2A88E8 + nameIdx*8` and the venue misc-pack in the switch at
`0x20FAB4+`. **CONFIRMED** (prologue reads, `0x20fa4c–0x20fa80`).

Minigame records found (tag, id, kind, taskFn):

| tag | venue id | byte +1 | taskFn | game (identity below) |
|---:|---:|---:|---|---|
| 0x23 | 0x13 | 2 | `0x2595F0` | street scene |
| 0x24 | 0x14 | 2 | `0x25FF00` | dual-view |
| 0x25 | 0x16 | 2 | `0x260638` | goal-area |
| 0x26 | 0x1C | 2 | `0x22DDD0` | other race |
| 0x27 | 0x25 | 2 | `0x2638C0` | not decoded |
| 0x28 | 0x26 | 2 | `0x22DDD0` | other race |
| 0x29 | 0x12 | 1 | `0x25F6A0` | finish-zone |
| 0x2A | 0x15 | 1 | `0x25FBF8` | half-plane |
| 0x2B | 0x17 | 1 | `0x251B60` | golf |
| 0x2C | 0x18 | 1 | `0x260DC0` | roulette |
| 0x2D | 0x19 | 1 | `0x25A120` | five gate courses |
| 0x2E | 0x1A | 6 | `0x254430` | arena |
| 0x2F | 0x1B | 1 | `0x254D20` | car-shop visit |
| 0x30 | 0x1D | 1 | `0x25EB70` | duel |
| 0x31 | 0x1E | 1 | `0x262678` | staged half-plane course |
| 0x32 | 0x1F | 1 | `0x25E630` | goal-plane |
| 0x33 | 0x20 | 1 | `0x2631A0` | three-round target course |
| 0x34 | 0x21 | 1 | `0x25E278` | balloon course |
| 0x35 | 0x23 | 1 | `0x263E68` | not decoded |
| 0x36 | 0x24 | 1 | `0x25DBB8` | collection |

(the id↔task pairing and the dual-phone/ski-jump columns of the
quick-race/2-player tables reuse the same record shape. Adventure rows above
are **CONFIRMED**, independently reread from `0x2BE018–0x2BE157` and paired
with `0x20FA28–0x20FA38`. The salvage's original tag/id/task pairs were wrong.
Byte +1 is called `kind` above, but the spawn path uses it as a participant
count; semantic game identities beyond the decoded tasks remain INFERRED.)

The adventure race-select menu `0x26B1B8` (spawned by location menu
`0x26B848` cases 1/2) lists 14×2 rows and spawns `0x20F950` with the chosen
tag. **CONFIRMED** (`0x26b9e8` region). Town desks (e.g. Sandpolis roulette,
see `tests/regression/rtharness/minigames.py`) reach the same tasks by door
scripts — **INFERRED** (script blob at the record `paramPtr`s, not yet traced).

## 2. Games

### 2.1 Golf (9-hole putt) — task `0x251B60`, helpers `0x252738`, `0x252608`

Course table `0x2AC0C0`, 9 records × 0x40 bytes: `{vec3 A, vec3 B, s16 flags
@+0x20, s16 par @+0x22}`. Pars = **[3,3,5,3,5,3,5,4,5]**, Σpar 36 — the Σ is
computed at run time (`0x252608`). Yardage display = `(dist(A,B) − 8.5856) /
0.9144` metres→yards conversion; constants at `0x33302C/0x333030`.
**CONFIRMED** (`0x251b60` case 0 loop).

State: main `0x10E30`-byte block at task+0x20; score-helper base at +0x8d50.
Offsets **from that helper base** include a +0x8000 prefix:
`par[9] @+0x800c`, `Σpar @+0x8030`, `yards[9] @+0x8034`, `Σyards @+0x8058`,
`bestHole[9](s32) @+0x805c`, `bestTotal @+0x8080`, `scorecard[9] @+0x8084`,
`runTotal @+0x80a8`, `newHoleRecord[9] @+0x80ac`, `newTotalRecord @+0x80d0`.
Stroke counter per hole at ball-state `+0x8cb4+(hole+4)*4`, capped 98 while
playing (`case 0x23`). **CONFIRMED** (`0x252608`, `0x252738`, `0x253218`).

Scoring (`RecordGolfHoleScore 0x252738`), all **CONFIRMED** by instructions:

* recorded hole score = `min(strokes+1, 99) − par[hole]` (relative to par);
* running total = `min(prevTotal + holeScore, 99)`, upper clamp only;
* per-hole best: if `holeScore < bestHole[hole]` → store (signed byte) to
  `DAT_01780a30 + hole` (= save image 0 offset **`0x12D0 + hole`**), set
  new-record flag; the byte array at `0x1780a30` is copied into state on
  course init (`0x252608`);
* after hole 8 (9th): if `runTotal < bestTotal` → store (signed byte) to
  `DAT_01780A39` = save offset **`0x12D9`** and set the 9-round new-record
  flag;
* round outcome: `runTotal > 0` → result 1; `runTotal ≤ 0` (under/equal par)
  → result 2 (`FUN_0023d8c8(1|2)` tail `0x252850–0x252870`).

Persistence (save record `0x3448`, base `0x0177F760`): **per-hole best
rel-to-par 9 × s8 @ `0x12D0–0x12D8`; best 9-round total rel-to-par s8 @
`0x12D9`** — new fields, no collision with ski-jump `0x12C8` or location
`0x12DC`. **CONFIRMED** (`sb a3,0xa30(at)` at `0x2527e4`, `sb a0,0xa39` at
`0x252830`; loaded `lb`-loops in `0x252608` via `&DAT_01780a30`).

### 2.2 Roulette (Sandpolis casino; "slot-machine task") — `0x260DC0` + helpers

Behavior names verified by real playthrough in `tests/regression` ("Roulette
(29): bet, drive the car-ball into a pocket, leave the table"); static decode:

* Bet selection `0x261638` **CONFIRMED**: bet starts **1000** (coordinator's
  verified `rules-tables.md` §4.2; salvage's 0 was incorrect); max bet =
  `min(money, 10000)` (`0x2617xx`); up/down steers the bet by `10^digit`
  (digit from money's digit count), clamped `[0, maxBet]`; bet 0 + confirm →
  "make a bet" prompt; Circle → yes/no confirm task `0x261508`; confirm-yes →
  `FUN_0023dc58(bet, 0)` deducts the bet; Triangle cancels (parent result
  −1). The bet window preview shows `money + bet*(rate−1)` (post-win
  balance), clamped 999999 (`FUN_0022e7f0` call at the tail of `0x261638`).
* Wheel/pocket mapping `0x262358` **CONFIRMED**: reel strip value table @
  `0x2D9DB8` (u32 per position, 0 = blank): `[0,6,8,18,10,4,12,16,14,2,0,5,7,
  17,9,3,...]`; a bet "target" ∈ 0..22: targets 0..17 win iff
  `target == value−1` (single pocket), 18..20 win iff
  `target == (value−1)/6 + 0x12` (group of six = colour class), 21/22 win iff
  `target == parity(reelPos)+0x15` where `parity = (reelPos<10 ? reelPos :
  reelPos+1) % 2` (even/odd).
* Payout `0x261de0` case 3 **CONFIRMED**: `payout = win ? bet × rate[target]
  : 0`; `rate[target] = *(0x2D9980 + target*0x30)` — **exact pocket 18, colour
  class 3, parity 2** (u32 words read at `0x2D9980–0x2D9DD0`);
  credit via `FUN_0023dc58(-payout, 0)`, balance clamp 999999; parent result 2.
* Outcome audit `0x262458` **CONFIRMED**: compares starting vs ending money:
  less → 1, more → 2, equal → 3 (`FUN_0023d8c8`).

No high-score persistence — money only. Roulette notebook stamp (29 in the
regression suite) is awarded by the surrounding dialog, not by the task.
**INFERRED** (award site not yet traced).

### 2.3 Stamp-collection — task `0x25DBB8`, child `0x25D7C8`

Spawns 30 NPC updaters (`0x25d7c8`); NPCs randomize in a box clamped
**[720, 880] on both axes** (child code clamps `pfVar2[0]`/`pfVar2[2]`).
**CONFIRMED.**

Collect condition (child tail) **CONFIRMED**: `dist(npc, player@0x177ace0) <
2.5 && parent+0x24 == 0 && (frame flags & 4)` → beep `0x302`, NPC dead flag,
parent notification `task+4 = 1`, `task+0x24 = 1`; the parent consumes that
notification, clears it, and increments `state+0x9250`. Win at exactly **20**
(`if (iVar7 == 0x14)` → `FUN_0023d8c8(1)`). No per-run timer tiers.
Prize is a dialog-layer stamp (regression harness: Curling stamp 69 etc.).
**INFERRED** which stamp ids map to which booth; collection predicate is
**CONFIRMED**.

### 2.4 Gate-course games ("stunts") — task `0x25A120`, results `0x25AA58`,
screen `0x25B0C0`

Five courses. Tables **CONFIRMED** (bytes @ `0x2C3A00` block): world spawn
pairs u16 @ `0x2C3A18` = (180,117), (795,117), (1395,107), (500,617),
(1100,587); rival-car counts u8 @ `0x2C3A30` = 2,5,10,13,17; gate counts u8 @
`0x2C3A38` = 11,18,30,31,44; **stamp ids u8 @ `0x2C3A40` = 32,33,34,35,36**
(one notebook stamp per course). Candidate identities (Rock Climbing, Volcano,
Figure 8, Obstacle Course, Sliding-Door race) **INFERRED** from the
regression coverage notes.

Gate logic (task state 1) **CONFIRMED**: global `DAT_0177AE9A` = current gate
index; per-gate 6-byte spec from the per-course table `PTR_PTR_002C3238`
(two/three signed half-plane tests vs corner records at `0x2C3250`, stride
0x10); forward gate → advance, backward gate → retreat; leaving the course
band → result 0 + fade. Crossing the final gate → `FUN_0023d8c8(1)` and
`gs_stamps_award_function(stampIds[course])` on the session's first completion
(state `+0x90fc == 0`). Run timer `state+0x90f8` increments while the gate
index is non-zero.

Scoring & persistence (`FUN_0025AA58(state, time, penalties)`) all
**CONFIRMED**:

* `time` clamped ≤ 360000 frames and stored; **time 0 then skips all penalty,
  score, best and new-record writes** (`beq a1,zero` at `0x25AAA8`);
  otherwise `penalties` clamped ≤ 99;
* **score = min(time + penalties × 300, 360000)** (each penalty = 300 frames
  = 5 s);
* last time / penalty count / score stored per course in the results
  sub-struct (`+0x80ac/+0x80c0/+0x80d4 + course*4`);
* if `score < best[course]` → new record: best stored to
  `(&DAT_01780A14)[course]` = save image 0 offset **`0x12B4 + course*4`**
  (u32 frames), plus new-record flag `+0x80e8`.

So save-offset range **`0x12B4–0x12C7` = 5 × u32 course best scores**, ending
immediately before ski-jump `0x12C8`; no collision. **CONFIRMED** (`sw`/`lw` at `0x25aa58`, global
`DAT_01780A14`).

Stamp award pipeline (`gs_stamps_award_function 0x23C8A8`) **CONFIRMED**:

* stamp id 0 → no-op; queue at save **`0x3300`** = count (byte), entries
  bytes at **`0x3304..0x330B`**, overflow at 8;
* duplicate suppression tests the earned bitmap `0x0177FC78` = save offset
  **`0x518`** (bit `id−1`, matching save-layout's proven notebook bitmap);
* special ids also queue mail into the mailbox array at `0x177FD08` = save
  **`0x5A8`** with count `0x177FD88` = save **`0x628`** (full mailbox of 128
  shifts oldest out): id 5 → mail 0x7B, id 17 → 0x7C, id 30 → 0x7D,
  id 50 → 0x1B, id 100 → 0x7F. Return 1 = queued.

### 2.5 Goal-plane game — task `0x25E630`

Course: quad corners @ `0x2C4C40` = (311,286.5), (311,282.2), (306.5,282.2),
(306.5,286.5) (**CONFIRMED** bytes); prop spin starts 170.0/180°.

Rules, all **CONFIRMED**:

* GOAL = `169.0 ≤ playerY < 171.0` **and** player (x,z) in the quad;
* outcome at GOAL: first clear (progress pair `FUN_0023D628(0x37)` returns 0)
  → result 1; repeat clear with run-frames `< 0x1069` (4201 ≈ 70 s) → 2;
  repeat but ≥ 4201 frames → 0;
* FAIL (checked every frame while state 1): `playerY ≤ 100.0` → result 0, or
  player x ≤ 110.0 → result 0 (`c.le.S` at `0x25E8E0`, `0x25E92C`;
  the salvage's unaligned `0x25e7c1` citation was invalid).

### 2.6 Finish-zone course — task `0x25F6A0`

Quad @ `0x2C4F50` = (54.8,346.4), (48.5,346.4), (48.5,353.6), (54.8,353.6).
**CONFIRMED.** Rules **CONFIRMED**:

* entering the quad → clear: result 2 iff run-frames `< 0x2A31` (10801 ≈
  180 s) **and** progress pair `FUN_0023D628(0x0E)` is set; else result 1;
* FAIL: `44.0 ≤ x ≤ 56.0 && z ≤ 12.0` (the start pad) → result 0.

### 2.7 Half-plane variant — task `0x25FBF8`

Sensors from the 0x2D99xx menu-neighbor tables: half-planes @ `0x2D9938`
(clear) and `0x2D9928` (fail; salvage's +4 base was wrong). **CONFIRMED**:
nonnegative clear cross-product → result 2 iff
run-frames `< 0x1C21` (7201 = 120 s) and progress pair 0x39 set, else 1; fail
half-plane → 0.

### 2.8 Goal-area game — task `0x260638`

**CONFIRMED, corrected**: FAIL if `x < 410 || x > 430 || z < 370` → result 0
(`0x260764–0x2607B4`). Independently reaching `z > DAT_00333170` → goal
and overwrites an earlier failure on the same update; tiers: opponent-ahead (`DAT_0177AF58 >
DAT_00333170`) → result 1; else run-frames `< 0x1E1` (481 ≈ 8 s) → 3, else →
2. `DAT_00333170` is **790.0** (fresh ELF read, consumer `0x2607F4`);
the separate opponent-stop threshold `DAT_00333174` is not modeled here.

### 2.9 Duel course (14 gates) — task `0x25EB70`

Two mirrored state machines (player substate `+0x8cb4`, opponent `+0x8cb6`;
2P split when frame byte `+0xa & 2`). **CONFIRMED**:

* gate table @ `0x2C4C60`, 14 × {f32,f32,f32,f32} full-width crossing lines
  (e.g. first two at z=149.5 and z=400), forward cross → advance,
  backward cross of the previous gate → retreat, global gate index
  `DAT_0177AE9A` (player) / `DAT_0177B10A` (opponent);
* height rule: while gate < 14, `playerY ≤ (f32)heightTable[gate]` → result 0;
  byte table @ `0x2C4F00` = [0,100,60,60,0,10,0,0,0,50,50,50,50,50,0,0];
* forfeit half-planes @ `0x2C4F10`-based line (x=250, z∈[95,110] on
  retreat-to-start) → result 0;
* WIN at gate 14: 1P → result 1; upgraded to **2 iff run-frames ≤ 0x1C20**
  (7200 = 120 s); 2P variant: opponent finishing first → result 2, per-view
  fades;
* opponent positions globals `0x177AF50/54/58` (2P mirror course inputs).

### 2.10 Dual-view zone game — task `0x25FF00`

Both views check script-zone ids in `DAT_0177AE9A` (view 1) /
`DAT_0177B10A` (view 2) against z-thresholds `DAT_00333144/48/4C/50` and
`DAT_00333158/5C/60/64` (806, 994, 994, 806 pattern present in both).
**CONFIRMED**: view-2 finish at zone 0x55 → result 1; view-1 late-finish path:
run-frames `< 0xBB9` (3001 ≈ 50 s) → result 3, else → 2; zone 0x40 reached
early / 0x41 not-yet → result per-mode 0/1; 2P: opponent pairing results 1/2.

### 2.11 Balloon/goal zone game — tasks `0x25E278` (+children `0x25DF80`)

Spawns 32 balloon children; model selected from 5-entry table `0x2C4BF0`.
**Corrected positions**: initial **z** = `880 + i*19`, i=0..31, hence
880..1469 (not the salvage's x=880..1773); initial y = 88.474998
(`0x333114`), vertical step = 0.1 (`0x333118`). Reset y/z/step are
88.474998/1448.5/0.1 (`0x333100/04/08`). The GOAL line is
player z ≥ **1350.0** (`DAT_0033311C`), FAIL = any balloon within **3.3**
units of the player position vec (`DAT_0033310C`, child `0x25df80` tail).
Result 1 (GOAL) / 0 (MISS), with GOAL taking priority over a pending child
collision notification. **CONFIRMED** (`0x25E468` for goal comparison;
the salvage's unaligned `0x25e3c1` was invalid; child `0x25DF80` for MISS).

The shared constants (state speeds `0.15/1.575/0.01/0.1`, spawn `88.47499`,
`1448.5`, goal `1350.0`, radius `3.3`) are floats @ `0x3330E0–0x33311C`.
**CONFIRMED** values read from the ELF.

## 3. Decode blind spots (do not re-decode)

* Ski-jump record save offset `0x12C8` and radio/location fields: proven in
  `save-layout.md`. The salvage's identification of `0x262678` as ski-jump
  was unsupported: its decompile is a staged half-plane game, not proof of
  a distance-record writer. Exact ski-jump task attribution remains OPEN.
* Tasks not fully decoded from the event table: `0x262678` (id 0x1E),
  `0x2631A0` (id 0x20), `0x2638C0` (0x25), `0x263E68` (0x23),
  `0x26CCD0` — **OPEN**.
* Result-code → money/stamp prize amounts live in the completion dialog /
  rules-tables region (`0x23BC00–0x23E100`, `AwardRaceResult 0x238950`,
  `FUN_0023DCF8` seen from `UpdateRaceRecords 0x237C48`) — tracked by the
  parallel economy pass; this ledger only cites their addresses.
* Entry cost handling for booth games was not found inside the tasks
  themselves except the roulette bet; booth fees likely ride the door
  dialogs — **OPEN**.

## 4. Independent salvage audit and authored implementation

Fresh input: host ELF SHA-256
`0843892381e6014340e802d6b936a8821d433b51b1d842c7ad9f4e90dec9bb30`.
Instruction listings/decompiles were read from the existing Ghidra
`RoadTrip` / `SLUS_203.98` program (`r5900:LE:32:default`, PS2 variant),
with selected instruction words independently read from the host ELF using
`vaddr−0x1ff000`. No target execution or retail game bytes added to the repo.

Freshly rechecked **CONFIRMED** evidence:

| subject | independently rechecked evidence / correction |
|---|---|
| adventure dispatch | `0x20FA00–0x20FA38`: tag comparison, shift by 4, base `0x2BDDE8`; all 20 rows reread at `0x2BE018–0x2BE157`. Corrected most original tag/venue/task pairings in §1. |
| golf scoring | `0x252748/+0x14`: add one, clamp strokes to 99; `0x252790`: subtract par; `0x2527AC`: total upper clamp; `0x2527C8/0x252818`: strict improvement; `0x2527E4/0x252830`: signed-byte best fields; `0x252850`: total ≤0 selects 2. No lower total clamp. Flags set on improvement and remain sticky for the round. |
| golf helper layout | `0x252608` initializer plus `0x252738` listing confirm +0x8000-prefixed offsets; `0x251B60` decompile supplies helper base main+0x8D50. Corrected missing prefix and hole-record flag offset. Shared pars/yardage were **not** re-decoded; see `rules-tables.md` §4.1. |
| gate records | `0x25AAA8` zero-time branch skips penalty/score/best writes; `0x25AAB8–0x25AAE0` computes capped penalty×300; `0x25AAF4/0x25AB10` strict best and store at `0x1780A14 + course*4`. Course counts/stamps independently reread at `0x2C3A30/38/40`, but award consumer not rechecked. |
| goal-plane | `0x25E738/54` inclusive 169 / exclusive 171; four strict cross-products; `0x25E880–0x25E8BC` pair 0x37 and 4201 threshold; `0x25E8E0/0x25E92C` Y≤100 or **world X**≤110 fail. Quad floats independently reread at `0x2C4C40`. |
| finish-zone | listing/decompile `0x25F6A0`, especially `0x25F8C4–0x25F8F4`: pair 0x0E, 10801 exclusive threshold. Quad floats reread at `0x2C4F50`; inclusive start-pad failure confirmed. |
| half-plane | `0x25FD08/0x25FDC8` nonnegative cross-products; `0x25FD1C–0x25FD4C`: pair 0x39, 7201 exclusive threshold. Fail-line base corrected to `0x2D9928`. |
| goal-area | `0x260764–0x2607B4` outside-X **OR** low-Z failure; `0x2607FC–0x260884` independent goal override, opponent ahead, 481 threshold. Float `0x333170` reread as 790.0. |
| duel / dual-view | only tier branches rechecked: `0x25F000` 7201 exclusive; `0x2601CC` 3001 exclusive. Full mirrored state-machine claims remain inherited; no 2P implementation authored. |
| collection | `0x25DDF8–0x25DE08` equality to 20 selects 1, not ≥20; parent increments counter at `0x25DDE4`; child `0x25D7C8` decompile confirms notification handoff and strict radius 2.5. |
| balloon course | `0x25E278` decompile / listing `0x25E468` confirms Z≥1350 before collision; child `0x25DF80` decompile confirms strict radius 3.3. Floats `0x3330E0–0x33311C` reread. Corrected initial Z range/axis and stale unaligned citation. |
| roulette audit | `0x262458` decompile confirms less/more/equal balance → 1/2/3. Shared betting/multiplier/money tables not re-decoded; coordinator-verified `rules-tables.md` §1/§4.2 controls (including start 1000 and row 23 multiplier 12). Pocket-to-target mapping remains inherited, not independently promoted by this audit. |
| timer / completion | `0x25E95C–0x25E978` confirms `(flags & 0x8004)==4`; `0x25EB44` confirms parent completion −1 is separate from the outcome channel. |

### Authored scope and verification

`src/native/gameplay/minigames.{h,cpp}` now provides golf score/record updates,
gate score/record updates (including zero-time suppression), raw tier helpers,
roulette balance audit, exact collection completion, timer gating, and separate
actual goal-plane / goal-area / balloon-course predicates. Save field constants
describe the confirmed offsets; **no save serializer, entry gating, prize
dialog, complete task loop or physical simulation was added**. `GolfRecords`
accepts caller-supplied signed-byte best values; it does not invent new-game
sentinels. Golf/gate best updates are strict, not tie-inclusive.

The pre-existing `balloonPopResult`, configurable `goalZoneResult`, and
goal-counting `MinigameScorer` remain compatibility helpers, not evidence of
actual game rules. In particular their `playerY`/`speed` parameter names must
not be used to infer the real balloon Z sensor or goal-plane X failure test.
Unsigned helper counters cover reachable nonnegative gameplay counts; invalid
golf indices are rejected by an authored guard. Zero-time `gateCourseScore`
returns 0 as an authored unscored sentinel; the ELF instead leaves its score
field untouched, which `recordGateCourse` models by not updating best.

Verification: requested CMake configure/full build in `build/native-mini`
completed with `-Wall -Wextra`, no warnings. `native_assets_minigames_test`:
**80 checks passed**, including original compatibility expectations, strict
boundary tiers, tie records, signed golf totals, zero-time gate handling,
actual sensor axes, and goal-over-failure precedence. All **52** built
`native_assets_*_test` executables also returned 0. CTest itself has no tests
registered, so binaries were invoked directly. No git write operations used.
