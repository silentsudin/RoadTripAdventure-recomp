# Town / shop event-script VM (`InterpretScriptCommands`)

Batch W2-01. Host interpreter for the only opcode VM in the game (town/shop
dialogue "pre-control" prefix), plus an ELF-resident script-image reader.
Code: `src/native/gameplay/script_vm.{h,cpp}`, `src/native/assets/script_image.{h,cpp}`;
test: `tests/native_w2/script_vm_test.cpp` (`w2_script_vm_test`).

Sources: `build/decomp/fn/0023ad00.c` (InterpretScriptCommands),
`build/decomp/fn/0023b168.c` (dialogueTask, the sole caller), `002105b8.c:373`,
`0022aeb0.c:130/188/221` (spawners), helper decomps `0023d*.c`/`0023c7b0.c`,
`docs/native-reference/gsl-vm.md` §5.1 (earlier draft, corrected below), and a
Capstone MIPS disassembly of `0x23AD00..0x23B167` re-read for this batch
(vaddr − 0x1FF000 = file offset). No ROM bytes are stored in the repo.

## 1. Where the scripts live (3-level pointer walk)

| level | address / form | evidence |
|---|---|---|
| 0 | town directory `0x2A7ED0`: 22 words, `[0]=0`, `[1..21]` = per-town level-1 vaddrs | ELF words; `002105b8.c:373` `*(0x2A7ED0 + (s8)shop[0x23]*4)` |
| 1 | per-town table, `word[shop[0x21]]` = level-2 vaddr, zero-terminated | `0026be50.c:49-54` (counts to first zero); spawners index it directly |
| 2 | shop table: `[0]` name string, `[1]` entry script, `[2..]` branch targets; **no stored length** | `0023b168.c:34-39` (`+0x20 = +0x1c`, cursor `= table[1]`), `0023ad00.c` branch `table[idx]` |
| 3 | script bytes (control prefix, ops 1..0x1D) then text, all in ELF `.data` | decode below |

Census (reproduced by the test and by an independent Python reference):
**21 towns / 330 level-1 slots / 307 distinct level-2 tables** (matches the
plan's un-pinned 21/330/307). Level-2 table words total 3174; there are 2867
`[1..len)` script references to 2478 distinct script starts, plus 279 distinct
name pointers (`[0]`): 2478 + 279 = **2757 distinct level-3 pointers**, which
reproduces plan §0 row 1c's un-pinned "2757" exactly.

Level-2 length (no stored length): the run of leading words that are script
pointers (`>= 0x2F0000`, `< 0x33567B` = end of the file-backed PT_LOAD that
holds the directory), clipped at the next level-2 table start. The bound is
insensitive: any lo in `[0x2A8000, 0x2F1300]` gives identical lengths. Table
ends: 96 run into the next table, 205 end on a zero alignment word, 5 end on a
word that is a level-1 table pointer (tables are interleaved with level-1
tables in `0x2A4450..0x2A7EC8`), 1 is last. Slot 0 of towns 1–9 shares one
table (`0x2A4450`, length 73).

## 2. Jump table and dispatcher (re-verified against the ELF)

* Dispatcher `0x23AD28..0x23AD48`: `addiu v1,v1,-1 ; sltiu v0,v1,0x1D ; beqz → 0x23B0E0 ; addiu s0,s0,1 ; sll ; lui 0x2F ; addu ; lw −0x38B0 ; jr`. The test checks the instruction words.
* Jump table `0x2EC750`: 29 words, targets in `0x23AD50..0x23B0E0`; **27 distinct targets**; ops 9, 10, 12 (words 8, 9, 11) all equal `0x23B0E0`; **26 live handlers**. The word after the table (1-based word 30) is `0`, and word 31 is the first bytes of the next string (`"next msg #%d"`, the printf in dialogueTask) — an adjacent string, not a terminator. (Coordinator ground truth: confirmed, 1-based indexing.)
* Because the bound check is `op−1 < 0x1D` (unsigned): op `0` and ops `0x1E`, `0x1F` also fall to `0x23B0E0`. Bytes `>= 0x20` end the prefix (`0x23B140` `sltiu 0x20`).

## 3. Op-handler table

Case vaddrs are the jump-table words (verified by the test). Branch = cursor
becomes `*(u32*)(table_base + idx*4)` (`table_base` = task `+0x20`); skip =
cursor advances `width` bytes. `town` = shop `+0x23` (signed byte), `shop` =
shop `+0x21`, `label` = shop `+0x22`. Decomp line = `0023ad00.c`.

| op | width | operands (after opcode) | behaviour | callee | case vaddr | decomp |
|---|---|---|---|---|---|---|
| 01 | 3 | flag, idx | flag set → branch, else skip | TestFlagBitFc98 `0x23D778` | 23AD50 | L28, L240+ |
| 02 | 3 | flag, idx | flag **clear** → branch, else skip | TestFlagBitFc98 | 23AD70 | L30-37 |
| 03 | **4** | slot, bit, idx | count≠0 → branch, else skip 4 | GetFlaggedSlotCount(slot,bit,0) `0x23D488` | 23AD90 | L38-40,202 |
| 04 | 2 | idx | TestStampFlag(town,shop)≠0 → branch, else skip | `0x23D5F8` | 23ADA8 | L41-48 |
| 05 | 3 | id, idx | TestFlagFc68≠0 → branch | `0x23D8E8` | 23ADC8 | L49-56 |
| 06 | 3 | bit, idx | TestStateBitFc78≠0 → branch | `0x23D628` | 23ADE8 | L57-64 |
| 07 | 2 | idx | CheckSlotPresence≠0 → branch | `0x23DBC8` | 23AE08 | L65-72 |
| 08 | 3 | idxA, idxB | MatchSlotPairState(town,shop)≠0 → `table[idxA]`; elif TestFlagFcb8(town,shop)≠0 → skip 3; else `table[idxB]` | `0x23DB80`,`0x23DCC0` | 23AE20 | L73-85 |
| 09, 0A, 0C | – | – | alias of unknown stub (report + skip, §4) | – | 23B0E0 | L86 |
| 0B | 3 | result, idx | `GetOrSetPresidentRaceResult(-1) == result` (64-bit compare) → branch | `0x23D8C8` | 23AEC0 | L95-99 |
| 0D | 3 | id, idx | TestFlagFca8Clear≠0 (bit **clear**) → branch | `0x23DBF0` | 23AEE0 | L102-109 |
| 0E | **7** | six idx bytes | TestFlagBitFc98(0x50)==0 → skip 7; else `cursor = table[byte[1 + FindCurrentTaskIndex()]]` (index 0..5) | `0x23DDB8` | 23AF00 | L110-119 |
| 0F | 2 | id | SetFlagBitFc98 | `0x23D7A8` | 23AFF8 | L120 |
| 10 | 2 | id | ClearFlagBitFc98 | `0x23D7E0` | 23B010 | L125 |
| 11 | 3 | slot, bit | ClearFlaggedSlotBit(slot,bit,0) | `0x23D390` | 23B028 | L130 |
| 12 | 1 | – | parent.done=1, parent.result=10 | – | 23B048 | L136-140 |
| 13 | 2 | code | RecordTaskResult(code) | `0x23DD30` | 23B060 | L141 |
| 14 | 3 | town', shop' | parent.done=1, result=12; ReleaseStampFlag(shop record) **then** `town=a, shop=b` (warp) | `0x23C7B0` | 23B078 | L146-154 |
| 15 | 2 | id | SetStateBitFc88 | `0x23D730` | 23B0B0 | L155 |
| 16 | 2 | idx | TestProgressFlagForPair(town,shop)≠0 → branch | `0x23DE50` | 23AE58 | L160-167 |
| 17 | 2 | id | SetProgressFlagAndQueue | `0x23CB68` | 23B0C8 | L168 |
| 18 | 2 | idx | `(u32)(shop.word14 − 54000) < 0x23281` (i.e. 54000..198000) → skip 2; else branch | – | 23AE78 | L170-176 |
| 19 | 3 | bit, idx | TestProgressFlag≠0 → branch | `0x23DE30` | 23AEA8 | L177-184 |
| 1A | 3 | n, idx | `n <= 100 − CountBitsFca8Fcb0(0)` → branch | `0x23D968` | 23AF38 | L185-189 |
| 1B | 2 | idx | licence byte (`0x177FDB1` = progress+0x651) ≠3 → skip; else TestStateBitFc78(100) → branch; elif TestFlagBitFc98(0x57) → skip; else branch | `0x23D628`,`0x23D778` | 23AF60 | L192,209-228 |
| 1C | 3 | id, idx | `(s8)town == id` (sign-extended vs zero-extended) → branch | – | 23AFA0 | L194-199 |
| 1D | 4 | a, b, idx | TestObjectAttribute(0x177F760,a,b)≠0 → branch | `0x23D5E0` | 23AFC0 | L200-208 |
| 00, 1E, 1F | – | – | unknown path (§4) | – | 23B0E0 | L86 |
| ≥ 20 | – | – | end of prefix, return cursor | – | – | L21-22 |

Corrections to `gsl-vm.md` §5.1 (all from disassembly):

1. Op 03 has three operands (`slot, bit, idx`; width 4) — the draft listed two.
2. Op 08's two operands are **two branch indices** with a three-way outcome; the "optional skip" is the Fcb8 arm (skip 3).
3. Op 0E consumes 6 operand bytes (width 7): per-task entry indices, selected by `FindCurrentTaskIndex()` (0..5).
4. Op 18's gate: continue when `54000 <= word14 <= 198000` (`0x23281` span), otherwise branch. `word14` meaning is OPEN (the 0x22AEB0 spawner writes 54000 there).
5. Op 1B tests the licence byte (progress+0x651, `save-layout.md`), `3` = top licence.
6. Shop `+0x23` is the **town** index (spawners index `0x2A7ED0` with it, op 14 writes it), not a "shop id"; `+0x21` is the shop slot; `+0x22` is only the label in the unknown-op report.
7. The unknown path also covers op `0`, `0x1E`, `0x1F`, not just 9/10/12.
8. Ops 12/14 write to the **parent** task (`task+0x18`): `+0x24 = 1`, `+4 = 10 / 12`.

## 4. Unknown-op rule (reproduced and tested)

At `0x23B0E0` (`s0` is already opcode+1): `printf("Unknown pre-control code %d in town T%02d shop %d\n", opcode, shop+0x22, shop+0x21)`; then if `byte[opcode+1] < 0x20`, the cursor is `opcode+2` and advances while `byte < 0x20` (all following control bytes are swallowed); otherwise the cursor stays at `opcode+1`. A second printf (format pointer at `0x333928`) prints the cursor; the loop continues. VM hooks: `onUnknownPreControl`, `onUnknownSkipped`.

## 5. Tick semantics

One `runScriptPrefix` call runs one message's prefix to completion and
returns the cursor of the first byte ≥ 0x20 (text). It never yields. In-game
the caller is `dialogueTask` (`0023b168.c:137`, `+0x28 = Interpret(...)`) at the
start of each message; escape handlers set `+0x28 = table[idx]` (`next msg #%d`)
and the next tick runs the prefix of that message. There is no timer/frame
operand in this VM (the race-timer "frames − 1" rule belongs to W2-10, not here).
The original has no step bound; the host version stops at `maxOps` (default
65536) and returns `StepLimit` (replay of all real scripts: 0 hits).

## 6. Measured on the real scripts (all counts re-derived independently)

| item | value |
|---|---|
| level-2 `[1]` entry scripts decoding clean (end on byte ≥ 0x20) | **307 / 307** |
| ops {0, 9, 10, 12, 0x1E, 0x1F} in an entry prefix | **0** |
| `[1..len)` script references / distinct starts | 2867 / 2478 |
| of those decoding clean / non-clean | 2864 / 3 |
| ops decoded / branch operands | 1431 / 1096 |
| branch operands `>= table length` (strict OOB) | **0** |
| branch operands `== length` where the next table follows directly ("bleed") | 1 |
| reachable from entries via prefix branches / unclean among them | 1026 / 0 |
| replay: 330 slots × 3 host modes, ops / branches / StepLimit / unknown hits | 1604 / 524 / 0 / 0 |

Op usage over all references (hex): 01:152 02:6 03:66 04:93 05:100 06:185
07:98 08:96 0B:70 0D:100 0E:2 0F:147 10:91 11:32 12:2 13:7 14:3 15:137 17:22
18:2 1A:10 1B:2 1C:7 1D:1. Ops 09/0A/0C/16/19 never occur. The previously
ambiguous ops (0E/12/14/18/1B) occur 2/2/3/2/2 times and all decode on a clean
boundary with the widths above.

* **The 3 non-clean references** (non-entry, one in each of: table `0x2A4450` idx 39, `0x2A5650` idx 16, `0x2A5FF0` idx 3): idx 39 starts with op 0E followed by a `0x00` byte; the other two point at zero bytes. They are not reachable from any entry by prefix branches (reach set is fully clean); they can only be reached via text escape codes (W2-07 scope) or are dead. Recorded as an open question.
* **The 1 "bleed"**: table `0x2A7138` (town 7, slot 28) has an op 08 whose second branch idx (10) equals the table length (10); the next level-2 table starts directly after, so `table[10]` is that table's **name** word. Native behaviour reproduces the original's raw indexing (`readU32(table+idx*4)`); it is a source-data quirk, counted separately as `branch_bleed` and **not** as OOB.

## 6a. Bytes 0x85..0x8B are rumble-hold bytes (coordinator ruling 2026-10-18)

Opcodes `0x85..0x8B` are **rumble-hold bytes** of the pad-pattern interpreter (default arm `0x20A7E0..0x20A7E4`
writes the byte to the hold counter; ROM-proven, see `event-bridge.md` and `audio-runtime.md` rumble section). They are
**NOT** radio or cutscene triggers: `0x8A` is not "radio" and `0x8B` is not "cutscene". They are not part of the
town prefix VM above (which stops at any byte `>= 0x20`, `0x23B140`).

* Records of unknown width terminate stream walks (a walker must stop, not guess a width) and must be drained
  immediately before the app session update seam.
* No countdown opcode exists in the ROM. **GAP:** countdown semantics are unresolved.

## 7. Binding later waves

`IScriptHost` has one method per named callee (§3), so a SaveData/economy host
needs no change to the VM. `ScriptImageMemory` adapts the ELF image; any
`IScriptMemory` works (the synthetic vectors use a flat buffer).

## 8. Repro

```
cmake -S src/native -B build/w-W2-01 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/w-W2-01 --target w2_script_vm_test -j
build/w-W2-01/w2_script_vm_test --elf "$RT_ELF" [--verbose]    # exit 0 only on PASS; 2 = NO_ELF
```

Independent reference (census, decode hashes, op histogram, and the mode-0/1
VM replay totals 549/311/1 and 537/39/0 ops/branches/done): the Python in the
batch report appendix (`.qoder/wave2/W2-01-report.md`).
