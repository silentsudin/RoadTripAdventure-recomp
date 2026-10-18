# Static RULES tables — race economy, unlocks, scores, physics, clock

Static table reference for `SLUS_203.98`, updated 2026-10-16. Companion to
`save-layout.md` (progress-record field map), `audio-map.md` (race-BGM table),
`car-data-status.md`, and the code in
`src/native/gameplay/rules_tables.{h,cpp}` (authored tables + accessors) pinned by
`tests/native_assets/test_rules_tables.cpp`.

Every claim below is marked **CONFIRMED** (bytes read from the ELF *and*
verified against the instruction stream of a consumer function, or against a
PROVEN anchor in these docs) or **INFERRED** (bytes exact; human-facing semantic
pattern-matched). Unmarked claims do not appear.

## 0. Method

- ELF at `$RT_DISC/SLUS_203.98` (default
  `~/Library/Application Support/RoadTripRecomp/disc`), both `PT_LOAD`s give
  `vaddr - fileoff = 0x1ff000` (re-verified from the program headers: text
  `0x00200000/0x1000`, data `0x0029a880/0x9b880`). Reads use
  `fileOffset = vaddr - 0x1ff000`.
- Instruction-stream evidence via Ghidra (headless `RoadTrip` project,
  program `/SLUS_203.98`): decompiles of `0x23dc58`, `0x23cd58`, `0x227c68`,
  `0x236238`, `0x2364b0`, `0x236858`, `0x237508`, `0x237c48`, `0x2180c0`,
  `0x249458`, `0x21d358`, `0x251b60`, `0x261638`, `0x261de0`, `0x268e40`,
  `0x269568`, `0x269900`, `0x26d1c0`, `0x23c5c0`, plus whole-ELF instruction
  searches (`0x59`, `0xcf69`, `0x3e8`) and xref sets.
- No game bytes are reproduced here beyond decoded constants; the authored
  module carries the same values with the same provenance comments.

## 1. Purse and race economy

### 1.1 The single money mutator — CONFIRMED

`FUN_0023dc58(amount, playerSlot)` is the **only** gameplay money mutation path:

```c
money = (&DAT_0177fdb4)[playerSlot * 0xd12];   // == progress_base + 0x654 + slot*0x3448
if (amount > money) return -1;                 // refuse (insufficient funds)
money -= amount;                               // negative amount => credit
if (money > 999999) money = 999999;            // cap
if (amount) FUN_00258e50(0x305);               // cash SFX
```

- Money address `0x0177FDB4` = `gs_progress_ram_address (0x0177F760) + 0x654`;
  index unit `0xd12` words = `0x3448` bytes = the PROVEN two-image stride.
- **Crediting is negative-spending**: every purse award calls
  `FUN_0023dc58(-value, slot)`.
- New-game seed **1000**: `li v1,0x3e8` at `0x00229ffc` into
  `sw v1,0x654(v0)` at `0x0022a004` (`FUN_00229fd8`) — cross-checks the PROVEN
  save-layout claim (money `0x654`, new game 1000).
- Exhaustive writer census: the only other writers of the money field are the
  RAM-default initializers (`FUN_00229fd8` seed; `FUN_00227c68`, which sets a
  demo purse of 10000, licence 3, bank 100000 for menu/versus RAM and is called
  only by `FUN_0026b848`). All 13 call sites of `0x23dc58` are enumerated in
  §1.4.

| constant | value | evidence | status |
|---|---:|---|---|
| money field | progress+`0x654` | `0x23dc58`, `0x22a004` | CONFIRMED |
| new-game purse | 1000 | `li 0x3e8` → `sw 0x654` | CONFIRMED |
| purse cap | 999999 (`0xF423F`) | clamp in `0x23dc58`, `0x261638`, `0x261de0`, `0x268e40` | CONFIRMED |
| cash SFX | `0x305` | `0x23dc58` | CONFIRMED |
| bank budget | 100000 @`0x1782a6c` | `0x227c68` init; budget check in `0x23a460` | CONFIRMED |
| teller stepper start | 1000 (power-of-ten digits) | `0x23a460` | CONFIRMED |

### 1.2 Race payout ladder `@0x0029FCB8` — CONFIRMED bytes, tier ownership partly INFERRED

`u32[5][6]`, `[tier][place0]`, place 0 = 1st:

| tier | 1st | 2nd | 3rd | 4th | 5th | 6th | consumer |
|---:|---:|---:|---:|---:|---:|---:|---|
| 0 | 800 | 500 | 400 | 300 | 200 | 100 | licence tests (B-gated races) |
| 1 | 1500 | 1200 | 1000 | 800 | 600 | 500 | licence tests (A-gated) |
| 2 | 2500 | 2000 | 1600 | 1200 | 1000 | 800 | licence tests (Super-A-gated) |
| 3 | 80000 | 60000 | 40000 | 30000 | 20000 | 10000 | WGP-scale purse (**INFERRED** owner) |
| 4 | 10000 | 7500 | 5000 | 3000 | 2000 | 1000 | WGP records 24/25 |

- **Consumer 1 (licence purse) — CONFIRMED:** `FUN_00236238 SumPlayerPoints`
  reads `0x29fcb8[(raceRecord[3] * 6) + position]` for the player (position
  `DAT_0178700a`) and both WGP teammates (`0x0178700e/12`, each < 6);
  `FUN_00237508 EnterLicenseScreen` stores the sum into the screen state
  (`state+0x20`), and `FUN_002364b0 DrawLicenseResultsScreen` credits it with
  `FUN_0023dc58(-(state+0x20), 0)` on confirm, then stamps the new licence into
  progress+`0x651`. Finishing 7th+ (`position ≥ 6`) contributes nothing.
- **Consumer 2 (WGP standings) — CONFIRMED:** `FUN_00236858` ranks 8 drivers by
  summed 3-byte round scores (RAM strip at `0x1782a74..`, round counter
  `DAT_01782a70`, 7 rounds), finds the player's rank `iVar15`, and with
  `rank < 6` sets `state+0x20 = ladder[record[3]][rank]`; at the end of round 7
  it credits `FUN_0023dc58(-(state+0x20), 0)` and resets the counter.
- Tier comes from **race record byte +3** (`*(mode+0x10)+3`), CONFIRMED by both
  consumers.

### 1.3 Race records `@0x002BDDD0` — bytes CONFIRMED, role partially INFERRED

26 records × `0x10`: `{u8 course, u8 0x18, u8 3, u8 tier, u32 textPtr,
u32 0x22DDD0, u32 0x2508E0}`.

- records 0–23: `course ∈ 0..17`, `tier ∈ {0,1,2}` (licence-gated races);
- records 24/25: `course ∈ {17,15}`, `tier = 4` (the two WGP entries);
- the licence award scan in `FUN_00237508` iterates the first `0x18` (24)
  races and requires a **stored place-1 ≤ 5** (top-6 finish) in every race
  whose record tier equals the current licence, then awards licence+1 —
  CONFIRMED; the per-record meaning of the trailing code pointers
  (`0x22DDD0` race-BGM chooser, `0x2508E0` AI steering) is INFERRED.
- Result slots `@progress+0xFF0` and best-time tables `@0x1040/0x1178`:
  `FUN_00237c48` writes place/lap/total into per-player bands of 26 slots
  (`26 × 3 players = 78` u32 rows per table — matches the PROVEN 78-entry
  arity in save-layout.md) and remaps race ids 26/27 → 24/25 — CONFIRMED.
  (`save-layout.md` now records the PROVEN 78-byte blob at `0xFF0` —
  3 bands of 26 slots, one band per player — matching this code.)

### 1.4 Money census — the 13 mutation sites (CONFIRMED exhaustiveness)

spend (positive `amount`) | meaning
---|---
`0x261934` (in `0x261638`) | slot-machine bet debit
`0x2692b4` (in `0x268e40`) | 2P item transfer charge (buyer pays)
`0x2697e4` (in `0x269568`), `0x269afc` (in `0x269900`) | shop buy at category price
`0x256584` (in `0x2564e8`) | paint-shop charge
`0x23a850` (in `0x23a460`) | bank deposit (purse debit, `DAT_01782a6c += amount`)

credit (negative `amount`) | meaning
---|---
`0x269244 / 0x269284` (in `0x268e40`) | item sell refund `-price/2`; 2P transfer pays the seller
`0x2366f0` (in `0x2364b0`) | licence-test purse
`0x236c84` (in `0x236858`) | WGP purse after round 7
`0x23a220` (in `0x23a140`) | mileage-exchange payout (credit-only)
`0x23a7e0` (in `0x23a460`) | bank withdrawal (`DAT_01782a6c -= amount`)
`0x262288` (in `0x261de0`) | slot payout

**Negative result (bounded):** there are **no** other purse mutations in the
shipped ELF — in particular, no per-race entry-fee debit and no per-customer
"taxi fare" credit exists anywhere in the static census. Race entry fees / job
fares as purse deductions are absent; any such claim must come from resident
dialogue *records* flowing through one of the handlers above, and none of the
13 sites is a fixed-fee race entry. Race rewards are the §1.2 ladder; the only
stipend-like income is the §2 mileage exchange.

### 1.5 Shop pricing — CONFIRMED architecture

- Category byte → price-getter dispatch: selector table `@0x002DC8A8`
  (category ≤ 13 → index 1..14) into the 15-pointer table `@0x002F1280`
  (`GetConst500 = flat 500` at index 0, then `GetIntField_*` for the per-family
  price fields); the label getters are `@0x002F1240` (16 entries). Both
  dispatched from `FUN_00243670` / `FUN_00246250`-family code — CONFIRMED.
- Purchase: mark owned (`FUN_0023d2c0`), pay `price` (`0x2697e4`/`0x269afc`) — CONFIRMED.
- Sell: clear owned, credit `price/2` (`0x269284`) — CONFIRMED.
- 2P transfer: buyer pays `FUN_00243a78` scaled value, seller is credited the
  same value (`0x268e40` state 3) — CONFIRMED.

## 2. Job fares / customer payouts (distance income)

**Bounded negative result + one CONFIRMED faucet.** No taxi-style per-fare
constants exist. The single distance-derived purse income is the
**mileage-exchange dialog** (message opcode `0x16` → `FUN_0023a140`):

- u32 counters at `progress+0x65C[band]` (array starts where the PROVEN odometer
  `0x658` ends) — CONFIRMED (`lw 0x650(0x177F76C + band*4)` at
  `0x0023a18c/0x0023a190`, i.e. effective address `progress+0x65C`).
- whole **miles = counter / 1609** (`li a2,0x649` at `0x0023a170`) — CONFIRMED;
- **rate = band*10 + 10 G per mile** (record byte band, multiply in
  `FUN_0023a140`) — CONFIRMED; rate ladder 10..100 G/mile;
- remainder stays in the counter; credit `FUN_0023dc58(-miles*rate, 0)` — CONFIRMED.

`save-layout.md` marks `0x65C` unpinned; this identifies it as the
mileage-exchange counters consumed by `FUN_0023a140`.

## 3. Progression unlocks

### 3.1 Story-gate pair table `@0x002A2B70` — CONFIRMED bytes/consumer, INFERRED meaning

25 `{u8 type, u8 id}` pairs; `FUN_0023cd58 CheckUnlockCondition(type, id, time)`
scans linearly and on match returns progress-flag bit `<index>` of
`DAT_0177FDD0` (set by `FUN_0023cb68` on story completion; INFERRED:
chapter-gating meanings).

```text
 0 (1,29)   1 (6,34)   2 (1,30)   3 (7,33)   4 (0,0)    5 (0,0)
 6 (1,31)   7 (0,0)    8 (7,35)   9 (0,0)   10 (6,35) 11 (3,39)
12 (3,38)  13 (3,37)  14 (2,35)  15 (1,32)  16 (4,25) 17 (3,35)
18 (2,36)  19 (4,26)  20 (7,36)  21 (1,33)  22 (2,37) 23 (3,36)
24 (6,36)
```

### 3.2 Time-of-day gates in `FUN_0023cd58` — CONFIRMED literals, INFERRED events

With `t` = param_3 (read here as the time-of-day tick):
type/id during clock hours... gate passes when

| gate | condition | equivalent | cross-check |
|---|---|---|---|
| (6,10) | `45000 < t - 0x1a5e0` | `t ≥ 153001` | 153000 = **17:00**, sunset band start |
| (13,0)/(13,5) | `0x1c906 < t - 0xafc9` | `t ≥ 161998` | ≈ **18:00**, dusk band end |
| (2,29) | `0x32898 < t` (and flag `0x400000`) | `t ≥ 206233` | ≈ **22:55**, deep night |
| (6,0x11) | flag `DAT_0177FCA0 & 8 == 0` | flag-gated | — |
| (6,0x20) | `(long)DAT_0177FC98 < 0` | bit 63 | — |
| (7,10) | `DAT_0177FC98 & (1<<56) == 0` | flag-gated | — |
| (9,0x17..0x1c) | progress bits `0x8000/0x10000/0x2000/0x4000/0x1000/0x20000` inverted | see `save-layout.md` story flags | — |

The 17:00/18:00 alignments independently reproduce the tod phase bands in
`sound-and-daynight.md` (CONFIRMED cross-check).

### 3.3 Collectibles — PROVEN + CONFIRMED extensions

- Quick-Pic photos: 100 slots (photo *n* at bit *n-1*, save `0x508`) —
  `gs_photos_take_function 0x23C5C0` sets the bit and keeps a 12-byte per-photo
  record at `progress+0x554`; when all **100** bits are set (completion test
  reads `DAT_0177FC68 == -1 && DAT_0177FC70 == 0xfffffffff` = 64+36 bits) it
  awards **stamp 0x60** — CONFIRMED. Photo save-bitmap arity matches
  save-layout.md (16-byte bitmap).
- Stamps: 100 bits (save `0x518`, PROVEN); `FUN_0023cb68` awards **stamp 0x33**
  when the progress bitmap `DAT_0177FDD0` fills — CONFIRMED (doc'd previously).
- Mailbox capacity 128 (save `0x628`, PROVEN).

## 4. Mini-game scoring

### 4.1 Golf course table `@0x002AC0C0` — CONFIRMED

9 records × `0x40`: `{f32[4] point A, f32[4] point B, ..., s16 par @+0x22}`.
`FUN_00251b60` computes par list and per-hole yardage and hands both to
`FUN_00252608 InitGolfCourseTable`.

- **pars** `{3, 3, 5, 3, 5, 3, 5, 4, 5}` (course par 36) — CONFIRMED
  (`*(s16*)(0x2AC0E0 + i*0x40 + 2)`).
- **yardage = (|A-B| − 8.5856) / 0.9144**, truncated, 0 when below the offset
  — CONFIRMED. Constants: `0x0033302C = 8.5856`, `0x00333030 = 0.9144`
  (the exact m→yard factor).

### 4.2 Slot machine — CONFIRMED arithmetic, row mapping INFERRED

- Bet stepper `FUN_00261638`: bet starts **1000**, digits step ×10, clamped to
  `min(purse, 10000)`; bet debited on spin confirm (site `0x261934`).
- Result rows `@0x002D9980`, `0x30` stride, **24 rows**; first u32 per row is
  the multiplier; the rest of each record is reel-strip/stop data for the
  state machine (`0x262318`... `0x262458`).

| rows | multiplier |
|---|---:|
| 0–17 | 18 |
| 18–20 | 3 |
| 21–22 | 2 |
| 23 | 12 |

- Payout `FUN_00261de0`: only with the win flag set (task+`0x8028` from
  `FUN_00262458`), pay `= bet × rowMult` credited via `FUN_0023dc58(-pay, 0)`;
  balance display clamps at 999999. Which reel combination selects which row
  is INFERRED (reel strips decoded only far enough to pin the multiplier
  records).
- Highest ladder entry observed elsewhere in that record family:
  `li/andi 0xc1 = 193` comparisons inside `FUN_00261de0` (reel-faces byte
  range); not further decoded — OPEN.

## 5. Driving physics constants

### 5.1 Speed algebra — CONFIRMED

| constant | value | evidence |
|---|---:|---|
| f15→raw display speed | `length × 216.0` | `FUN_0021d358` (`SQRT(..) * 216.0`; m/frame → km/h at 60 Hz) |
| raw → speedometer mph | `÷ 1.6089999676` | `FUN_00249458`, divisor f32 `@0x00332F44` (`lwc1 f1,-0x7bac(gp)`, `div.S`) |
| speedometer clamp | **186** (`0xBA`) | `FUN_00249458` `slti 0xba`; tach path rescales by `max/186` |
| gravity (airborne fall) | `-0x59` per frame | `addiu v0,v0,-0x59` @ `0x0021bd20` (whole-ELF search) |
| wading/water force | `+0x59` | `addiu a1,v0,0x59` @ `0x0021becc` |
| nitro force cap | `0xCF69` | `ori ?,zero,0xcf69` @ `0x0021be68/0x0021be80` (only 2 sites in ELF) |

186 mph × 1.609 ≈ 299.27 raw-units ceiling — the cap and divisor are
self-consistent (CONFIRMED arithmetic). The arithmetic divides by the km→mile
factor, so the displayed unit is **mph** (unit *label* INFERRED; divisor,
clamp and scale CONFIRMED).

### 5.2 Equipped-part spec tables (loader `FUN_002180c0`) — CONFIRMED fields

Car-mode part byte → table:
`tire(+1) @0x2ECA70`, `gearbox(+2) @0x2ECED0`, `chassis(+3) @0x2ED1F8`,
`engine(+4) @0x2ED328`, `steering(+5) @0x2ED4E8`, `brake(+6) @0x2ED5DC`,
`horn(+0xd) @0x2F00A8`.

**Tires `@0x2ECA70`, 0x1C stride, 13 records** `{label, price@+8, s16 grip[6]@+0xC, bonus u16@+0x1A}` —
grips copied to `car+0x21C[contact&7]` (6 populated classes; class order per
`car-physics.md` INFERRED); bonus adds +5 to the chassis stat ("Big"):

| tire | price | grip (6 classes) |
|---|---:|---|
| Normal | 200 | 196 168 128 168 96 64 |
| Sports | 1000 | 240 196 154 160 96 64 |
| Semi-Racing | 2000 | 333 168 154 140 80 64 |
| Racing | 5000 | 512 102 102 100 51 64 |
| HG Racing | 10000 | 512 160 102 128 51 64 |
| Wet | 2000 | 240 196 196 160 128 64 |
| HG Wet | 3000 | 240 196 240 160 128 64 |
| Off-Road | 500 | 196 196 160 160 128 64 |
| HG Off-Road | 3000 | 240 240 160 180 168 128 |
| Studless | 1000 | 196 196 160 168 168 196 |
| HG Studless | 3000 | 240 196 160 168 196 196 |
| Big | 5000 | 240 240 230 210 196 96 |
| Devil | 200000 | 65280 ×6 |

**Gearboxes `@0x2ECED0`, 0x18 stride, 8 records** `{label, price@+8, u32 speedClass@+0xC→car+0x214, u16 gears@+0x10→car+0x240}`:
Normal 200/1500/3, Panther 500/1800/4, Blue MAX 1000/2200/5, Blue MAX V2
1500/2600/6, MAD 2000/2900/7, MAD V2 4000/3300/8, Long MAD 8000/3600/6,
Black MAX 12000/3900/8.

**Chassis `@0x2ED1F8`, 0x10 stride, 5 records** `{label, price@+8, s32 weightStat@+0xC→car+0x218}` (engine-force/drag
divisor per car-physics.md): Normal 200/25, Light 500/22, Feather 1000/20,
Phantom 2000/18, Hyper 4000/15.

**Engines `@0x2ED328`, 0x1C stride, 6 records** `{label, price@+8, u16[7]@+0xC→car+0x22C}` (param meanings INFERRED,
first word constant 0xFFA1):
Normal 200, Sports 1000, Power 2000, Speed 4000, Wide 7000, Hyper 10000.
Profiles in `rules_tables.h`.

**Steering `@0x2ED4E8`, 0x10 stride, 4 records** `{label, price@+8, s16 multiplier@+0xC→car+0x242}` (steering-angle
multiplier): Normal 200/64, Quick 500/96, X2 Quick 1000/128, X3 Quick 2000/160.

**Brakes `@0x2ED5DC`, 0x2C stride, 4 records** `{u8 curve[32]@+0, price u32@+0x28}` — curve = brake force ramp over the
CONFIRMED 32 held frames, applied as `curve[ramp-1] * 10000 >> 5`
(car-physics.md). Prices {1000, 1500, 2000, (4th unpinned)}. Curves in
`rules_tables.h`.

**Horns `@0x2F00A8`, 0x10 stride, ≥10 records** `{u32, byte SFX id region @+4, labelPtr, soundPtr}`; horn SFX id byte →
`car+0x245` (+ player port) — CONFIRMED loader read `(&DAT_002f00ac)[idx*0x10]`.

Prices double as shop costs via the §1.5 dispatch (CONFIRMED same tables).

## 6. Day / night cycle

The free-running clock (wrap `0x34BC0` = 216000 ticks, PROVEN in
`sound-and-daynight.md` ⇒ 9000 ticks per hour at 60 Hz) is extended here by
the **scripted step tables** of the area day-cycle cinematics:

- **Step start clocks `@0x002DCD28`, u32[10]** — CONFIRMED: bytes + consumer
  `FUN_0026ca48` (`ApplyTimeOfDayLighting` sets the world clock from
  `DAT_002DCD28[step]`; xref at `0x0026caf8`).

| step | clock | ticks | time of day |
|---:|---:|---:|---|
| 0 | 72000 | 72000 | 08:00 |
| 1 | 81000 | 81000 | 09:00 |
| 2 | 108000 | 108000 | 12:00 |
| 3 | 117000 | 117000 | 13:00 |
| 4 | 126000 | 126000 | 14:00 |
| 5 | 135000 | 135000 | 15:00 |
| 6 | 162000 | 162000 | 18:00 |
| 7 | 163000 | 163000 | 18:06:40 |
| 8 | 171000 | 171000 | 19:00 |
| 9 | 189000 | 189000 | 21:00 |

- **Step durations `@0x002DCD50`, u16[10]** — CONFIRMED: bytes + consumer
  `FUN_0026d1c0` (`AdvanceDayCycleStep` waits for task tick ==
  `*(u16*)(0x2DCD50 + step*2)`, spawns the fade-in 64 ticks early, kills and
  respawns the lighting task per step, ends after step 9):
  `{0, 1260, 2520, 3780, 5040, 6300, 7560, 8820, 10080, 12033}` ticks
  (1260 = 21 s each).

`src/native/gameplay/daynight.cpp` / `daycycle.cpp` already carry the PROVEN
phase bands; the tables above are the authentic step-clock values that the
step machine feeds into them.

## 7. Ledger

### CONFIRMED
- Money mutator `0x23dc58` (funnel, cap, SFX, negative-credit convention),
  seed pair at `0x229ffc/0x22a004`, bank budget 100000 @`0x1782a6c`,
  teller stepper 1000.
- Payout ladder `@0x29FCB8 [5][6]` bytes + both consumers (licence purse incl.
  teammate positions; WGP 7-round standings purse).
- Licence award rule (24 scanned races, place-1 ≤ 5 in all tier-equal races),
  licence field `0x651` write.
- Race records `@0x2BDDD0` (24 licence + 2 WGP, byte3 tier, course 0..17) and
  the 26-slot × 3-player result geometry (78 u32 best-time rows).
- 13-site money census; shop buy/sell (price, price/2) and 2P transfer prices.
- Mileage exchange: counters `progress+0x65C`, miles /1609, rate band*10+10,
  credit path.
- Unlock pair table `@0x2A2B70` (25 entries) + FUN_0023cd58 special clock/flag
  thresholds (153001 / 161998 / 206233).
- Quick-Pic photo bit-arithmetic: 100 slots, all-complete test (64+36 bits),
  stamp 0x60 award.
- Golf pars/yardage (`0x2AC0C0`, pars 3/3/5/3/5/3/5/4/5, offset 8.5856, factor
  0.9144) and the §4.2 slot arithmetic (bet ≤ 10000, rows @0x2D9980 with
  multipliers 18/3/3/3/2/2/12, win flag +0x8028, pay = bet × mult).
- Speed algebra (216.0, 1.6089999676 @0x332F44, clamp 186), gravity −0x59,
  water +0x59, nitro cap 0xCF69 — all by instruction search.
- Part spec tables of §5.2 with the loader field mapping; brake 32-entry ramp
  curves; part prices.
- Day-cycle step clocks `@0x2DCD28` (10) and step durations `@0x2DCD50` (u16
  ×10) with consumers `0x26ca48`/`0x26d1c0`.

### INFERRED
- Payout tier 3's owning mode (WGP-scale); per-record trailing code pointers in
  the 0x2BDDD0 race records; the "grip class" order of tire columns;
  gearbox "speedClass" field semantics; engine param meanings; the 0x2DC8A8
  category names; slot row ↔ reel-combination mapping; chapter meanings of the
  unlock pair table; event names for the clock gates; horn record fields
  beyond the SFX byte.
- `progress+0x65C` counter accumulation site (who adds metres to it is not
  traced; only its consumption is).

### OPEN
- Per-race **entry fee**: no static implementation exists (bounded negative
  result above); if the shipped game charges entry fees, they must be produced
  by resident dialogue records whose handler is one of the 13 census sites —
  no candidate record was found in this pass.
- Which dialogue record ids select the mileage-exchange bands (record bytes are
  per-resident script data, out of scope here).
- The demo-RAM initializer `FUN_00227c68`'s caller semantics (menu/versus
  defaults incl. purse 10000, licence 3).
- Payout ladder rows beyond index 4 do not exist in the data; tier indices
  > 4 would read unrelated bytes (the consumers bound race tiers to 0..4 via
  the record byte).

## 8. Reproducing this ledger

```sh
# Commands 1-4 run from anywhere; the disc root is $RT_DISC (default
# "$HOME/Library/Application Support/RoadTripRecomp/disc"), as in
# car-data-status.md. Adjust if your bash cannot read ~/Library directly.

# 1) phdr mapping (both PT_LOAD deltas must print 0x1ff000)
python3 - <<'EOF'
import struct, os
DISC = os.environ.get("RT_DISC", os.path.expanduser("~/Library/Application Support/RoadTripRecomp/disc"))
d = open(os.path.join(DISC, "SLUS_203.98"), "rb").read()
phoff, = struct.unpack_from("<I", d, 0x1c); phes, phn = struct.unpack_from("<HH", d, 0x2a)
for i in range(phn):
    t, o, v, _, fsz, _, _, _ = struct.unpack_from("<8I", d, phoff + i * phes)
    if t == 1: print(hex(v), hex(o), "delta=%#x" % (v - o))
EOF

# 2) decode the payout ladder @0x29FCB8 (vaddr -> -0x1ff000)
python3 - <<'EOF'
import struct, os
DISC = os.environ.get("RT_DISC", os.path.expanduser("~/Library/Application Support/RoadTripRecomp/disc"))
d = open(os.path.join(DISC, "SLUS_203.98"), "rb").read()
u = lambda a: struct.unpack_from("<I", d, a - 0x1ff000)[0]
for t in range(5): print([u(0x29FCB8 + 4 * (t * 6 + p)) for p in range(6)])
EOF

# 3) day/night step tables (@0x2DCD28 clocks, +0x28 the u16 durations)
python3 - <<'EOF'
import struct, os
DISC = os.environ.get("RT_DISC", os.path.expanduser("~/Library/Application Support/RoadTripRecomp/disc"))
d = open(os.path.join(DISC, "SLUS_203.98"), "rb").read()
o = 0x2DCD28 - 0x1ff000
print("clocks:", struct.unpack_from("<10I", d, o))
print("durations:", struct.unpack_from("<10H", d, o + 0x28))
EOF

# 4) golf pars @0x2AC0C0 +0x22, unlock gates @0x2A2B70
python3 - <<'EOF'
import struct, os
DISC = os.environ.get("RT_DISC", os.path.expanduser("~/Library/Application Support/RoadTripRecomp/disc"))
d = open(os.path.join(DISC, "SLUS_203.98"), "rb").read()
print("pars:", [struct.unpack_from("<h", d, 0x2AC0C0 - 0x1ff000 + i * 0x40 + 0x22)[0] for i in range(9)])
print("gates:", [struct.unpack_from("<BB", d, 0x2A2B70 - 0x1ff000 + i * 2) for i in range(25)])
EOF

# 5) the authored module + contract test (pins every table above)
cmake -S src/native -B build/native-rules -DRT_NATIVE_ASSETS_BUILD_TESTS=ON
cmake --build build/native-rules --target native_assets_rules_tables_test -j 4
./build/native-rules/native_assets_rules_tables_test   # "253 checks passed"
```

Ghidra session: programs `/SLUS_203.98` in project `RoadTrip`; decompiles/xrefs
listed in §0. No game bytes retained.
