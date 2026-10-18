# Item economy (purse funnel, shop buy/sell, price tables)

Module: `src/native/gameplay/item_economy.{h,cpp}`. Test: `tests/native_w2/item_economy_test.cpp`
(`w2_item_economy_test`). Batch: W2-06 (`.qoder/wave2/W2-06-report.md`).

No ROM bytes are committed. The test reads the user's `SLUS_203.98` and checks addresses, raw
instruction encodings and ELF-read prices against the module. vaddr → file offset is `vaddr − 0x1FF000`
(PT_LOAD 0x29A880, `p_offset` 0x9B880).

## 1. Money funnel: `SpendTimerBudget` 0x23DC58

| vaddr | raw word | meaning |
|---|---|---|
| 0x0023DC58 | 0x24023448 | `addiu v0,zero,0x3448` — per-player slot stride (two-image) |
| 0x0023DC88 | 0x3C03000F | `lui v1,0xf` |
| 0x0023DC8C | 0x3463423F | `ori v1,v1,0x423f` → cap **0xF423F = 999999** |
| 0x0023DCA0 | 0x10800004 | `beq a0,zero,+4` → cash SFX skipped when amount == 0 |
| 0x0023DCAC | 0x24040305 | `addiu a0,zero,0x305` → cash SFX id |
| 0x00229FFC | 0x240303E8 | `addiu v1,zero,1000` → new-game purse |
| 0x0022A004 | 0xAC430654 | `sw v1,0x654(v0)` → money at progress+0x654 |

Semantics (decomp `0023dc58.c`, pinned above): if `amount > money` → return −1, no change.
Otherwise `money −= amount`; if `money > 999999` → `money = 999999`; SFX `0x305` iff `amount != 0`.
A negative amount is a credit.

**Call-site census.** A whole-segment scan finds exactly 13 `jal 0x23DC58` words, matching
`rules-tables.md` §1.4. Purchase sites are `0x2697E4` and `0x269AFC`. Sell/2P-transfer sites are
`0x269244` and `0x269284`, which pay `−price/2`. Other sites: `0x2366F0` licence, `0x236C84` WGP, `0x23A220`
mileage, `0x23A7E0` / `0x23A850` bank, `0x256584` paint, `0x261934` / `0x262288` slot machine,
`0x2692B4` 2P transfer charge. This module models the purchase and sell flow plus the funnel. The other
sites are call-site pins only.

## 2. Price getters

Selector `0x2DC8A8[row] = row + 1` (14 words, pinned). The shop calls `DispatchByIndex15(class, slot)`,
which indexes the 15-pointer getter table `0x2F1280`. Class 0 is `GetConst500`, a flat 500 (`0x243698`:
`jr ra` then delay slot `li v0,500` = `0x240201F4`).

| class | getter body | record base (getter) | stride | records | price field | terminator (raw, slot = count) |
|---:|---|---|---:|---:|---|---|
| 1 | 0x2436A0 | 0x2ECA78 | 0x1C | 13 | u32 @ base | 0x2ECBE4 = 0x6E696761 ("agin") |
| 2 | 0x2436C0 | 0x2ECED8 | 0x18 | 12 | u32 @ base | 0x2ECFF8 = 0x30303033 |
| 3 | 0x2436E0 | 0x2ED200 | 0x10 | 5 | u32 @ base | 0x2ED250 = 0x70732D70 |
| 4 | 0x2436F8 | 0x2ED330 | 0x1C | 6 | u32 @ base | 0x2ED3D8 = 0x2C656E69 |
| 5 | 0x243718 | 0x2ED4F0 | 0x10 | 4 | u32 @ base | 0x2ED530 = 0x6C6B6369 |
| 6 | 0x243730 | 0x2ED5D8 | 0x2C | 4 | u32 @ base | 0x2ED688 = 0x64206120 |
| 13 | 0x243808 | 0x2F00A8 | 0x10 | 14 | u32 @ base (price at +0) | 0x2F0188 = 0x000003E8 (ambiguous, equals 1000); boundary is the +8 word at 0x2F0190 = 0x69617254 (text), not the price word |

Brake getter (class 6, body 0x243730, 8 raw words): `00051840 00651821 00031880 00651823 00031880 3C02002F
00431021 8C42D5D8` = `sll v1,a1,1; addu v1,v1,a1; sll v1,v1,2; subu v1,v1,a1; sll v1,v1,2; lui v0,0x2f;
addu v0,v0,v1; lw v0,-0x2A28(v0)`. That is `word[0x2ED5D8 + 0x2C·slot]`.

Ungrounded classes: 7, 8, 9, 10, 11, 12, 14 have getters (pointers pinned) but no proven record
boundary. Their raw values at slot 0..7 include text and pointers, so they are not catalogued (GAP).

The price values in the catalog are the words at these addresses. The test checks all 58 of them.

Catalog (ELF-read; `category` = class):

- **1 tire** (13): 200, 1000, 2000, 5000, 10000, 2000, 3000, 500, 3000, 1000, 3000, 5000, 200000
- **2 gearbox** (12): 200, 500, 1000, 1500, 2000, 4000, 8000, 12000, 16000, 20000, 80000, 160000
- **3 chassis** (5): 200, 500, 1000, 2000, 4000
- **4 engine** (6): 200, 1000, 2000, 4000, 7000, 10000
- **5 steering** (4): 200, 500, 1000, 2000
- **6 brakes** (4): 500, 1000, 1500, 2000
- **13 horn** (14): 0, then 1000 × 13

Sell refund = `price / 2` (truncating; `0x268E40` passes `-value/2`).

## 3. Purchase / sell flow (`dialogue/shop state 0x269568`, `0x269900`, `0x268E40`)

Purchase, in decomp order:
1. `money < price` → "insufficient" message (flags 0x200). No change.
2. Owned bit set (`TestFlaggedSlotBit`) → "already owned" message (0x300). No change.
3. Otherwise confirm (0x268890). On confirm: `SetFlaggedSlotBit` + `SpendTimerBudget(+price)`.

Sell: requires the owned bit. Clear the bit and credit `price/2` through the funnel.

## 4. Invariant

Let `start` be the starting money and `credited`, `spent`, `clamped` be the accepted totals. After every
operation:

- **I1 (ledger):** `money == start + credited − spent − clamped` and `0 ≤ money ≤ 999999`.
- **I2 (inventory):** for each catalog item, `owned == buys − sells` with `owned ∈ {0,1}`.
- **I3 (refusal):** insufficient, already-owned and not-owned operations change neither money nor ownership.

Checked over 1000 seeded random sequences of 200 operations (xorshift64*, seed 0x9E3779B97F4A7C15).

## 5. Golden purchase traces (hand-verified from the ELF prices)

| # | trace | result |
|---|---|---|
| G1 | start 1000, buy tire 1 (1000) | Bought, money 0, SFX |
| G2 | start 999, buy tire 1 | Insufficient, money 999, no SFX |
| G3 | start 5000, buy gearbox 3 (1500), sell it | 3500, then 4250 (+750), SFX on both |
| G4 | start 5000, buy chassis 1 (500), buy again | 4500, then AlreadyOwned, no change |
| G5 | start 950000, own tire 12 (200000), sell it | +100000 → cap 999999, SFX, 50001 clamped |
| G6 | start 0, buy horn 0 (0), sell it | 0 both times, no SFX (amount 0) |

## 6. Gaps

- `docs/native-reference/item-db.md` and `shop-menu.md` do not exist in this repo. Grounding uses
  `rules-tables.md` §1 and §5.2, `functions/menu-shop-ui.md`, and the ELF directly.
- **Gearbox:** the ELF has 12 priced records, not the 8 named in `rules-tables.md`. The extra four
  (16000, 20000, 80000, 160000) have no names. GAP.
- **Brakes:** the getter reads `0x2ED5D8 + 0x2C·slot`, so slot 0 = 500. `rules-tables.md` says the price
  is at `0x2ED5DC + 0x28` (1000 / 1500 / 2000), which is one slot shifted. The module uses the getter.
- **Horn:** 14 records. The price is at +0 and slot 14 has text at +8. The rules table says ≥ 10.
- **Classes 7–12, 14:** getters present, record bounds not proven. Not in the catalog (7 unbounded).
- **Town shop inventory tables** (which items each shop stocks): not pinned here. Depends on the script
  image (W2-01) and spawner arguments. GAP.
- **Owned-bit layout:** `SetFlaggedSlotBit` / `TestFlaggedSlotBit` storage is modelled as one bool per
  catalog item. The storage layout is not pinned.
- **Other 11 money sites** (bank, licence, WGP, slot, mileage, paint, 2P transfer) are call-site pins
  only; their logic is out of scope here.
- **Build:** `item_economy.cpp` is not in the coordinator's Step-0 staged list (`gameplay/economy.cpp` is).
  The test target therefore does not link under `src/native`; the module was compiled directly
  (see the report). Coordinator must add `gameplay/item_economy.cpp` to the staged list.
