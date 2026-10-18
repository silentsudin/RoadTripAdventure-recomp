# W2-06 report — item economy (purse funnel, shop buy/sell, ELF price tables)

Status: **module + test green on the direct build; the CMake target does NOT link** (see §5, gap G1).

## 1. Files (new only)
- `src/native/gameplay/item_economy.h`, `src/native/gameplay/item_economy.cpp`
- `tests/native_w2/item_economy_test.cpp`
- `docs/native-reference/item-economy.md` (grounding table, catalog, gaps)
- `.qoder/wave2/W2-06-report.md` (this file)

No edits to main.cpp, CMakeLists.txt, third_party, build/native, the board/journal, or other batches' files. No git operations.

## 2. SCORE line (exact)
```
W2-06 SCORE pins=125/125 traces=6/6 invariant_runs=1000 invariant_violations=0 catalog=58 unbounded_categories=7 deterministic=1
```
exit 0. Without an ELF: `NO_ELF (...)` line, same SCORE line with `pins=0/125`, exit 0 (as the coordinator asked; see §5 G4).

## 3. Grounding table (vaddr → raw word → meaning), ELF re-verified this pass

| vaddr | raw word | meaning |
|---|---|---|
| 0x2DC8A8+4i, i=0..13 | i+1 | category selector → getter class 1..14 |
| 0x2F1280+4k, k=0..14 | 0x243698,0x2436A0,…,0x243820 | 15-entry price-getter table |
| 0x243698 | 0x03E00008 | GetConst500: `jr ra` |
| 0x24369C | 0x240201F4 | delay slot `li 500` (class 0 flat price) |
| 0x243730..0x243750 | 00051840 00651821 00031880 00651823 00031880 3C02002F 00431021 8C42D5D8 | class 6 brake getter: word[0x2ED5D8 + 0x2C·slot] |
| 0x2ECA78+0x1C·i (i<13) | 200,1000,2000,5000,10000,2000,3000,500,3000,1000,3000,5000,200000 | class 1 tire prices; terminator 0x2ECBE4 = 0x6E696761 (text) |
| 0x2ECED8+0x18·i (i<12) | 200…160000 (12 values) | class 2 gearbox prices; terminator 0x2ECFF8 = 0x30303033 |
| 0x2ED200+0x10·i (i<5) | 200,500,1000,2000,4000 | class 3 chassis; terminator 0x2ED250 = 0x70732D70 |
| 0x2ED330+0x1C·i (i<6) | 200,1000,2000,4000,7000,10000 | class 4 engine; terminator 0x2ED3D8 = 0x2C656E69 |
| 0x2ED4F0+0x10·i (i<4) | 200,500,1000,2000 | class 5 steering; terminator 0x2ED530 = 0x6C6B6369 |
| 0x2ED5D8+0x2C·i (i<4) | 500,1000,1500,2000 | class 6 brakes; terminator 0x2ED688 = 0x64206120 |
| 0x2F00A8+0x10·i (i<14) | 0, then 1000 ×13 | class 13 horns (price at +0); 0x2F0188 = 0x3E8 ambiguous; +8 label at 0x2F0190 = 0x69617254 (text) marks the end |
| 0x23DC58 | 0x24023448 | SpendTimerBudget: addiu v0,zero,0x3448 (slot stride) |
| 0x23DC88 / 0x23DC8C | 0x3C03000F / 0x3463423F | cap 999999 = 0xF423F |
| 0x23DCA0 | 0x10800004 | `beq a0,zero,+4`: no SFX when amount == 0 |
| 0x23DCAC | 0x24040305 | cash SFX 0x305 |
| 0x229FFC / 0x22A004 | 0x240303E8 / 0xAC430654 | seed 1000 → progress+0x654 |
| 13 callers (jal 0x23DC58) | 0x0C08F716 at 0x2366F0, 0x236C84, 0x23A220, 0x23A7E0, 0x23A850, 0x256584, 0x261934, 0x262288, 0x269244, 0x269284, 0x2692B4, 0x2697E4, 0x269AFC | exactly the 13 documented money sites (whole code-segment scan) |

Total pins: 14 selector + 15 getter pointers + 2 GetConst500 + 8 brake body + 58 prices + 7 terminators + 7 money + 13 callers + 1 count = **125**.

## 4. Reproduce
```
# direct build (used; see §5 G1)
mkdir -p build/w-W2-06/direct
c++ -std=c++20 -Wall -Wextra -Wpedantic -O2 -Isrc/native -c src/native/gameplay/item_economy.cpp -o build/w-W2-06/direct/item_economy.o
c++ -std=c++20 -Wall -Wextra -Wpedantic -O2 -Isrc/native -c tests/native_w2/item_economy_test.cpp -o build/w-W2-06/direct/test.o
c++ build/w-W2-06/direct/item_economy.o build/w-W2-06/direct/test.o -o build/w-W2-06/direct/w2_item_economy_test
RT_ELF="$HOME/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98" build/w-W2-06/direct/w2_item_economy_test
echo $?          # 0

# CMake protocol (configures; target does not link yet, see G1)
cmake -S src/native -B build/w-W2-06/native -DRT_NATIVE_ASSETS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/w-W2-06/native --target w2_item_economy_test
```
Also: root configure `cmake -B build/w-W2-06 -S . -DRT_STAGED_SOURCES=OFF` succeeds (~2 min); the root does not register native tests, so the target only exists under `-S src/native`.

Checks run: bogus `RT_ELF` → NO_ELF exit 0 (env overrides `--elf`, as in W2-01); `--elf` with the real ELF → 125/125 exit 0; no `PIN FAIL` with `--verbose`. Warnings: none with `-Wall -Wextra -Wpedantic`. Test wall time 0.07 s.

## 5. Gaps and open questions
- **G1 (blocker for CMake, coordinator):** `gameplay/item_economy.cpp` is not in the Step-0 staged list (`src/native/CMakeLists.txt` lists `gameplay/economy.cpp`, the planned name). The `w2_item_economy_test` target therefore fails to link (undefined `item_economy::*`). I did not edit CMake. Fix: add `gameplay/item_economy.cpp` to the staged list (or to the W2 test link line). The direct build above is the verified path.
- **G2:** `docs/native-reference/item-db.md` and `shop-menu.md` (named as Wave-1 ground truth) do not exist in the repo. Grounding used `rules-tables.md` §1/§5.2, `functions/menu-shop-ui.md`, and the ELF.
- **G3 (data corrections vs rules-tables):**
  - gearbox has **12** priced ELF records (rules-tables names 8; 16000/20000/80000/160000 unnamed);
  - brakes: getter base `0x2ED5D8` (slot 0 = 500) vs rules-tables `0x2ED5DC+0x28` (1000/1500/2000), one slot shift; the module uses the getter values;
  - horns: 14 records (rules-tables ≥ 10).
- **G4 (spec conflict):** plan §0 row 4 says missing ELF ⇒ exit 2. The coordinator prompt says NO_ELF exits 0 with fixture tables. I followed the prompt; the NO_ELF SCORE shows `pins=0/125`, so it cannot be mistaken for a pass.
- **G5:** classes 7–12 and 14 have getters (pointers pinned) but no proven record bounds. 7 categories unbounded; not in the catalog.
- **G6:** town shop inventory tables (which items each shop stocks) not pinned. Depends on the script image (W2-01) and spawner args.
- **G7:** owned-bit storage (`SetFlaggedSlotBit`/`TestFlaggedSlotBit`) modelled as one bool per item; bit layout not pinned.
- **G8:** only purchase, sell and the funnel are modelled. Bank, licence, WGP, slot, mileage, paint and 2P transfer are call-site pins only.
- **G9:** the plan's category range is 1..13; the selector actually yields 14 classes (1..14).

## 6. Confidence
- High: funnel semantics and cap/SFX/seed (raw words), 13/13 call-site census, price words for classes 1–5 and 13 (exact getter read), GetConst500.
- Medium: brake slot indexing (G3), gearbox count (12) — both checked by raw words, but names/meaning unverified.
- Not claimed: town shop stock, the other 11 money sites, and the owned-bit layout.
