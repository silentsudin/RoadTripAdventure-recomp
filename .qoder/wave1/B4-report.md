# B4 report — GSL container / native VM spec

Worker: B4 (senior RE). Date: 2026-10-17.  
Deliverable: `docs/native-reference/gsl-vm.md`  
Write scope: only that doc + this report. No git ops. No ROM bytes in repo.

---

## Executive result

**`.GSL` is a DMA/GIF texture-upload stream, not a script container.**  
There is no unit TOC, no magic/version header, and **no script bytecode inside any of the 49 disc `.GSL` files**. A native “GSL VM” that can redraw a SCORE HUD screen is:

1. a chain walker + texel/CLUT decoder (already half-done in `hud_art.cpp` / `sys-gsl-packs.md`), plus  
2. an arena/blit layer driven by **ELF-side** race-HUD records and ROM UV templates — not opcodes read from the `.GSL`.

The only real jump-table opcode interpreter under script/interp symbols is **`InterpretScriptCommands` (`0x0023AD00`)** — town/shop *event* scripts (flags, branches, results). It does not draw and does not read GSL.

Template **`0x3380B`** is a **GS `TEST_2` constant** in `mapStatePacket` (`0x002A43B0`), kicked by minimap drawers via `vifDirect`. It is not a draw-op opcode and is not walked as a script. A code search hit at `0x260CEC` is a **false positive** (`movn` encoding).

---

## Steps completed

### 1. Disc inventory

- 49× `.GSL` under `SYS/` + `ITEM/` (no `SCOREA16`).
- Path, size, first-16 hex, unit counts tabulated in `gsl-vm.md` §1.
- First 16 bytes = first DMA CNT tag (`qwc` + byte3 `0x10`), not a container magic.

### 2. Container parse (SCORE, METER0, METER10, + STAMP/SELECT/S00/FONTE/POSE/NOTE)

- Grammar matches `sys-gsl-packs.md` and `hud_art.cpp` on every file: CNT `0x10`, END `0x70`, stride `(qwc+1)×16`, unit image at `T+0x70`, invariant `qwc == 6+NLOOP` on **1142/1142** units.
- Full offset/size tables for SCORE / METER0 / METER10 in doc §2.3.
- 32 B post-END trailer on all files; walker never consumes it.

### 3. Unit classes

| class | exists in GSL? |
|---|---|
| Texture (T4/T8) | yes |
| CLUT (CT16/CT32) | yes, paired after texture |
| Font | yes, as large T4 (FONTE 512×384) — not a separate type |
| Script | **no** |

`hud_art.cpp` unit-0-at-`0x70` / PSMT4 256×256 + CT16 assumptions: **validated** on SCORE.GSL.

### 4. “Script interpreter” search

| candidate | verdict |
|---|---|
| Grep symbols/toml for GSL/interp/opcode/script/draw-template | town script + map packet + loaders only |
| `InterpretScriptCommands` `0x23AD00` | **real VM**: JT @ `0x2EC750`, ops `1..0x1D`, `jr` dispatch — **event script**, opcode table in `gsl-vm.md` §5.1 |
| Code refs to `0x3380B` | data @ `0x2A43B0` only (map); `0x260CEC` = `movn` false+ |
| Map drawers `0x242438` / `0x2427D0` / `0x242A88` | `vifDirect(mapStatePacket,3)` then PATH1 models — no GSL op walk |
| `raceHudDraw` / `raceHudElementA` | C builders; type 1/2/3 draw modes on 44 B records — closest HUD “opcodes”, not file bytecode |
| `loadStep` `0x20D120` + `SendTexturePacketsDMA` `0x20E030` + `sendTexturePackets` `0x207B50` | **GSL consumer path** (validate tags, rebase DBP, GIF kick) |

No fabricated opcodes: every town-script op has a verified case vaddr; HUD types cite decomp.

### 5. Docs written

- `docs/native-reference/gsl-vm.md` — container PROVEN, unit layout, opcode draft (town script + HUD element types), upload flow, MVP native scope, open questions with probes.
- This report.

---

## Key citations (prior docs)

| doc | what it already proved (reused, not re-litigated) |
|---|---|
| `sys-gsl-packs.md` | DMA-stream grammar, 49/49 walk, loader categories/DBP rebase, FONTE atlas |
| `hud-layout.md` | 640×448 space, ElementA 44 B grammar types 1–3, SCORE TEX0 pages, METER anchors, ROM templates `0x2EBD00+` |
| `race-hud-atlas.md` | SCORE unit0 @ `0x70`, CSA0 vs CSA1 colour banks, banner/timer UV rects vs oracle |
| `hud_art.cpp` | parseUnit bitfields, PSMT4/CT16 decode, CSA base 56 for bank 1 |

---

## MVP native scope (SCORE HUD strip)

1. Walk SCORE.GSL → decode unit0 T4 256×256 + unit1 CT16.  
2. Blit atlas rects from `race-hud-atlas.md` with CSA 0/1.  
3. Stub timer digits; skip minimap / ElementA animation / shop scripts.

---

## Open / residual

1. Post-END footer purpose.  
2. Exact GIF consumption of tool junk qwords (harmless for decode).  
3. Spawner that issues the SCORE `loadStep` request tag.  
4. Shop script binary location on disc (not GSL).  
5. No separate “draw-template bytecode VM” found; if one exists it is not selected by the symbol’s 0x3380B / GSL / interp search seeds.

---

## Confidence summary

| claim | conf |
|---|---|
| GSL = DMA texture chain only | **PROVEN** (49 files + loader decomp) |
| No script units in GSL | **PROVEN** (full corpus) |
| Town script opcodes 1–0x1d | **PROVEN** (JT + decomp) |
| 0x3380B = map TEST_2 packet | **PROVEN** |
| ElementA types 1–3 = HUD draw modes | **PROVEN** |
| Native MVP path above sufficient for SCORE strip | **STRONG** (needs implementation + pixel check) |
