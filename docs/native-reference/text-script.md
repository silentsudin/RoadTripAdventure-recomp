# Dialogue & text engine (`text_script`)

Batch W2-07. Native port of the original message-window text path:
`RenderEscapeText 0x23A8E8`, the message-window task `dialogueTask 0x23B168`
(states 0..6), the glyph drawer `uiDrawGlyph 0x2017B0` / `uiTextWidth 0x203F40` /
`uiDrawTextShade 0x204230` / `uiDrawNumberAuto 0x204460`, the window frame
`uiDrawDialogueFrameA/B 0x2028B0/0x202B70` + `uiDrawIcon20 0x203E70`, and the
three string helpers `CopyDialogueTextBounded 0x24E7D8`,
`SubstituteNameTokens 0x264CB0`, `ExtractDelimitedToken 0x24EA00`.

Code: `src/native/gameplay/text_script.{h,cpp}` (namespace
`rt::native::gameplay::text`). Test: `tests/native_w2/text_script_test.cpp`
(`w2_text_script_test`). Fixtures: `tests/native_w2/data/text_script_vectors.txt`
(numbers produced by **executing the ELF code**, see §9) and
`tests/native_w2/data/text_extents.csv` (extents measured on the oracle PNGs).
No ROM bytes are stored; ELF addresses below are `vaddr`, file offset = vaddr − 0x1FF000.

## 1. Headline findings (corrections to earlier notes)

| earlier belief | ELF fact | evidence |
|---|---|---|
| "glyph-advance **word wrap**" | The original has **no automatic wrap**. Lines are hand-broken with `\n` in the data; `RenderEscapeText` never compares x with a width. | decomp + disassembly of 0x23A8E8 (no width compare); census of all 9 366 ELF lines: no single greedy width is consistent with more than 133 of 1 613 multi-line messages (4–5 at 498 px) |
| FONTE is mono 16 px, menu `scaleX 0.77` | Text is **proportional at 1:1**: advance = `uiGlyphWidths[c]` (`0x29A880`, 8..16), sprite drawn at the pen, 24 px tall | 16/16 oracle strings predicted within ±2 UI px (§6) |
| FONTE "uppercase-only; lowercase silently skipped" | **Refuted.** `uiDrawGlyph` draws `a`..`z` with the same slot formula; all 94 atlas cells `0x21..0x7E` are non-empty (26/26 lower-case). | test `font_glyphs=94/94 font_lower=26/26`; oracle shows "Quick Race", "What do you want to do?" |
| next-message arrow tab at `(304, 392)` (`hud-layout.md` §1.4) | The tab is sprite 7 of `uiDrawDialogueFrameA`: **atlas(88,256) 32×24 at `(304, 416)`**; the blinking arrow icon is `uiDrawIcon20(0x136, 0x1A4, 7)` = atlas(260,180) 20×20 at **(310, 420)** | emulated `uiDrawDialogueFrameA(0x158)` event list; `FRAMEA`/`ICON` vectors |
| `boundedText` (`dialogue.h`) "passes bytes through unchanged" | `CopyDialogueTextBounded` **widens ASCII to full-width Shift-JIS** (2 bytes per char), limit counts characters; it has **0 static call sites** (dead code in this build) | §7; 700/700 emulated vectors |
| menu selected label "drawn 6 UI further left" (`menu-calibration.md` §2.2) | Artefact of the bar's bright end-cap at scan x 912..922; after excluding it all five main-menu labels (selected or not) start at scan 924 = UI 462 | §6 measurement (box x ≥ 920, `B ≥ 170`) |

## 2. Glyph metrics (`uiDrawGlyph`)

* Atlas cell 16×24 (`glyph_atlas.h`), ASCII `c ≥ 0x21`: `g = c − 0x21`, `u = (g % 31)·16`, `v = (g / 31)·24`.
* Advance `w = int8 table[c]` (96 entries `0x20..0x7F`, all verified: **96/96**). `c == 0x20` advances 8 without a sprite.
* Sprite (the `uiSpriteWrite 0x2063C8` args, UI space 640×448):
  `x = pen`, `y = y2`, `u' = u + (16 − w)/2`, `v`, `width = w/2 + 8`, `height = 24 − shade`.
  (`(16−w)/2 + w/2 + 8 = 16`, so a quad never leaves its cell.)
* Accent prefixes `/e`, `` `a|e|u ``, `^a|e|i|o|u`, `~A|C|O|U|a|c|e|i|o|s|u`, `~R`: composite cells at v = 0x60/0x78/0x90;
  the pen advances by the **follower's** width. The string drawers (`uiDrawText`, `uiDrawTextShade`, `uiTextWidth`) consume **2 bytes**
  for `/ ` ^ ~`; `RenderEscapeText` consumes **1**, so an accent prefix in dialogue text draws the composite *and then the plain follower* (original quirk, kept).
  Follower bytes ≥ 0x80 read unrelated ELF bytes as the width (GAP: not modelled, natural data never does it).
* Shift-JIS (`byte ≥ 0x80`, 2 bytes): `0x8140` advance 10 (no sprite); `0x818B` advance 16 (u 0xC0, v 0x60, w 0x10); anything else advance 20, sprite 20×20 at `y2+2`
  with (u,v) from `uiKanaGlyphUV` / `uiKanjiGlyphUV` — **GAP (tables not decoded)**. Only 1 of the ELF script characters is non-ASCII.
* `uiDrawNumberAuto` (`$S`): digits + thousands separators, 10 px each: `<1000` → 1/2/3 cells, `1 000` → 5, `10 000` → 6, `100 000` → 7, `1 000 000` → 9, `10 000 000` → 10, `10^8` → 11, `10^9` → 13; the number is right-aligned at `x + cells·10`.

## 3. `RenderEscapeText` (one page)

Signature `(task, text, ctx)`; task `+0x02` = typewriter limit (s16, `0x7FFF` = unlimited), `+0x26` = scroll (u8).

1. **Skip** `scroll / 12` lines: scan to the first control byte (`< 0x20`, normally `\n`), swallow it and any control bytes that follow until a `\n` or printable byte. Every glyph or pair counts 1 *element*; in this pass the bytes of a `$` code count individually (`$G` = 2, `$A1` = 3), when drawn they count 1 / 2 — original inconsistency, kept. **No end check**: with fewer lines than `scroll/12`, or when the text ends right after its last newline, the cursor runs on through zero bytes forever. The native port stops at the buffer end and reports `runaway` (360 emulated vectors prove the original would not terminate within 60 000 steps).
2. **Draw 3 lines.** Line `i` is at `y2 = (173 − k + 12·i + s1)·2` with `k = scroll % 12`; the first line has `s1 = max(0, k − 4)` (it creeps up 4 units then is shaded: sprite height `24 − 2·s1`), the others `s1 = 0`. `x` restarts at **70** each line. Line pitch 24 px.
3. Per element, **before** drawing: `if (elements == limit) return cursor` (typewriter pause).
4. Element = `$` + code, else one glyph (pair if `byte ≥ 0x80`). `elements++` after each element.
5. End of a line: `\n` consumed, `i++`. After the third `\n` the page is complete: return NULL, and if `(scroll == 0 || (scroll/12) % 3 == 0) && limit == 0x7FFF` then `limit = elements`.
6. A control byte that is not `\n` ends the text: the function returns a pointer to it — **that byte is the message's command** (`dialogueTask` switch, §5).

### `$` escape codes (jump table `0x2EC6C0`, index `code − 'A'`, 21 entries; verified 22/22)

| code | handler | output | elements |
|---|---|---|---|
| `$A<id>` | 0x23AB08 | `sprintf("%d", word[0x177FCFC + 4·id] / 1000)` via `uiDrawTextShade` (C division) | 2 |
| `$G` | 0x23AA80 | string at `0x177FDA6` (progress+0x646) | 1 |
| `$N` | 0x23AAA0 | player name, string at `0x177FD8C` (progress+0x62C, `save-layout.md`) | 1 |
| `$R` | 0x23AA50 | `gs_game_course_names[*(byte*)ctx[+0x10]]` (table `0x2BE398`) | 1 |
| `$S` | 0x23AAE0 | `uiDrawNumberAuto(x, y2, word[0x1782A6C], 10)` — the bank balance (`rules-tables.md` §1.4: deposit `DAT_01782a6c += amount`) | 1 |
| `$T7` / `$T<id>` | 0x23AB68 | `sprintf("%d:%02d", v/9000, (v%9000)/150)` (9000 vblanks/h, 150/min — the town clock units); `'7'` → `word[0x1780A10]`, else `word[0x1787008 + 4·id]` | 1 (id consumed with the code) |
| `$U` | 0x23AAC0 | string at `0x177FD9F` (progress+0x63F, "currency name" in `save-layout.md`) | 1 |
| `B–F H–M O–Q` | 0x23AC48 | nothing (consumed with the code byte) | 1 |
| any other code byte (`< 'A'`, `> 'U'`, digits, space, `0x80+`) | 0x23AC4C | nothing; a code byte `≥ 0x80` also swallows its trail byte | 1 |

An id byte `≥ 0x80` (`$A`/`$T`) also swallows one extra byte. Formats at `0x333918` (`%d`) and `0x333920` (`%d:%02d`). In the 2 478 ELF scripts the codes used are
`$G` ×51, `$U` ×16, `$T` ×12, `$S` ×2, `$R` ×1 — `$A` and `$N` never occur in script text.
**GAP:** meaning of the `$G` / `$U` strings and of the `$A`/`$T` id words (only addresses and formats are pinned).

## 4. Window geometry (all from `uiDrawDialogueFrameA/B(0x158)`, `uiDrawIcon20`)

* Frame A (7 sprites, atlas row v = 256): corners `(32,328) 40×56 u0`, `(32,384) 40×48 u120`, `(568,328) 40×56 u40`, `(568,384) 40×48 u160`; edges `(72,328) 496×56 u80`, `(72,384) 496×48 u200`; **tab `(304,416) 32×24 u88`**. Window = **(32, 328, 576, 104)** (bottom 432; tab to 440).
* Frame B (name plate): `(0x232 − w, 316)` 8×28 u88 v280, `(0x23A − w, 316)` `(w+4)`×28 u96 v280, `(0x23E, 316)` 24×28 u208 v256, with `w = uiTextWidth(table[0])`; plate text `uiDrawText(0x23E − w, 0xA0)` (y2 = 320), i.e. right-aligned at x = 574.
* Text origin x = 70, first baseline row y2 = 346; interior right edge 568 ⇒ **498 px** of advance per line (`kTextMaxAdvance`). 9 260 of 9 286 `$`-free ELF lines (99.7 %) fit; the non-final lines of the 1 613 multi-line, `$`-free messages are 275 px (median) / 375 px (p90) wide, max 520.
* Open (state 0) and Close (state 3) draw `uiSetTexa(tick·8)` / `uiSetTexa(128 − tick·8)` before the frame; during Open the plate width is `w·tick >> 4`.
* Palette banks (recorded, not modelled): frame 7, text body `uiSetPaletteBank(10)`, plate and arrow icon bank 0.

## 5. Message window (`dialogueTask`, `MessageWindow`)

Task fields: `+2` limit, `+4` child result, `+8` tick (incremented by the walker **after** the callback), `+0x18` parent, `+0x1C`→`+0x20` shop table, `+0x24` child-done pulse (**cleared by the walker after every callback** — visible to one tick), `+0x25` state, `+0x26` scroll, `+0x27` pending result, `+0x28` cursor. Table: `[0]` name string, `[1]` entry script, `[2..]` branch targets. Confirm = `(ctx[+0x30] & 0x50) != 0` (Cross 0x40 | Circle 0x10).

| state | meaning | per-tick behaviour |
|---|---|---|
| 0 Open | fade-in | tick 0: cursor = table[1], sound 0x25. TEXA, frame A, plate (w·tick>>4); at tick 16 → 1 |
| 1 Text | page + command | `cursor = Interpret(cursor)`; frame A; page; frame B + plate; then (a) page complete: if `scroll == 0 || (scroll/12)%3 == 0` blink arrow (`tick & 0x10`) and wait for confirm (sound 0x1A) else fall through → state 2; (b) command dispatch (below) |
| 2 Scroll | scroll one line | `scroll += 2`; at `scroll % 12 == 0` → 1. Draws like state 1 |
| 3 Close | fade-out | TEXA 128 − tick·8; at tick 16: `ctx.flags` bit 0x400 cleared if `pending == 4` else set; if `pending != 0xFF` parent done, result `pending==0 ? 5 : pending`; kill |
| 4 ChildMenu | child running | on done pulse: cursor = table[result], "next msg", scroll = limit = 0, → 1. Draws like state 1 |
| 5 ChildResult | child returns an index | on done: `result != −1` → parent done(result), pending = 0xFF, sound 0x26, → 3; else cursor = table[pending], pending = 0, → 1 |
| 6 ChildNext | wait, **draws nothing** | on done → 1 |

**Typewriter** (`limit` stop, byte at the cursor ≥ 0x20, or command 0x0A / ≥ 0x19): every tick with `(tick & 1) == 0` → `limit++` (30 elements/s at 60 Hz); every 4th tick sound 0x23 and `ctx[+0x18] = 2`; confirm sets `limit = 0x7FFF`.

**Commands** (the non-newline control byte that ends the text; `kCommands`, all 24 live values occur in the ELF, 2 476/2 476 messages end on one):

| cmd | action | count in ELF |
|---|---|---:|
| 00 | wait confirm → close (result 5) | 698 |
| 01 | spawn 0x239260 (menu message) → state 4 | 312 |
| 02 | spawn 0x23BC90 (yes/no) → 4 | 3 |
| 03 | wait confirm → close, pending 4, `ctx[+0x20] = arg0` if ≠ 0 | 30 |
| 04 / 06 | spawn 0x239900 (arg 9 / 8), pending = arg0 → state 5 | 2 / 2 |
| 05 | spawn 0x23A320 → 4 | 1 |
| 07 | arrow + confirm, spawn 0x239F68 → 4 | 90 |
| 08 | spawn 0x239A80 → 4 | 1 |
| 09 | spawn 0x23BCD0 (tab list menu) → 4 | 156 |
| 0A | **not dispatched** (typewriter default) | 0 |
| 0B | spawn 0x23BE88 → 4 | 2 |
| 0C | arrow + confirm, cursor = table[arg0] | 726 |
| 0D | on confirm: award stamps `arg…` (0-terminated), close | 90 |
| 0E | `DAT_01780A3C = ctx[+0x23]`, spawn 0x229538, cursor = table[arg0] → 6 | 1 |
| 0F | pending = 6, then like 00 | 1 |
| 10 | spawn 0x239748 (sequenced messages) → 4 | 226 |
| 11 | arrow + confirm, photo(arg0), spawn 0x22BFD0, cursor = table[arg1], sound 0x306 → 6 | 100 |
| 12 | spawn 0x23BE18 (text box) → 4 | 2 |
| 13 | arrow + confirm, spawn 0x269E88, cursor = table[arg0] → 6 | 7 |
| 14 | on confirm: ReleaseStampFlag, pending 0x0B, `ctx[+0x23] = arg0`, `ctx[+0x21] = arg1`, close | 9 |
| 15 | arrow + confirm, SetObjectAttribute(arg0,arg1), sound 0x303, cursor = table[arg2] | 8 |
| 16 | arrow + confirm, spawn 0x23A140 (prize screen) → 4 | 5 |
| 17 | spawn 0x23A460 (amount adjuster) → 4 | 2 |
| 18 | on confirm: pending 0x0F, close | 2 |

The control **prefix** namespace (ops 1..0x1D, `InterpretScriptCommands`, W2-01 `script_vm`) is a different dispatcher: it runs at the cursor
before the text and its ops 9, 0x0A, 0x0C share the unknown stub `0x23B0E0`. The window takes the prefix runner as a callback (`runPrefix`).

## 6. Oracle comparison (pixel widths)

Strings visible in `main_menu.png`, `options.png`, `factory_menu.png` (`text_extents.csv`: scan boxes, colour thresholds, measured extents; scan x = 2·UI x):

* 16/16 strings: |predicted ink width − measured| ≤ 2 UI px (mean +1.25, max 2; the prediction counts the anti-aliased fringe the threshold drops). Ink is taken from the FONTE atlas (R ≥ 150, α > 0) through the visible sprite columns.
* Positions: dialogue line 1 `What do you want to do?` starts at pen x = **70** (ELF) → predicted ink 71 vs measured 72; the plate `Q's Factory` right-aligned at 574 → predicted ink 442 vs measured 443.
* Without `SYS/FONTE.GSL` the test falls back to `advance − 4` and prints `font=NO_FONTE`.

## 7. String helpers

* **CopyDialogueTextBounded**: per source char: `0x80+` → copy lead+trail; digits `0x824F+`, caps `0x8260+`, lower `0x8281+`, punctuation rows `0x2A9A82/0x2A9AB6/0x2A9AEA/0x2A9AFE`, every other byte → `0x8140` (`0x334498`). `limit == 0` → until NUL, else exactly `limit` characters, **running past the NUL** (a short source is padded with `0x8140`). Native: `dstCapacity` is a hard bound, a pair is never split, input is never read out of bounds. 96/96 table bytes match; 700/700 emulated vectors; 10 000/10 000 fuzz (prefix of the unbounded reference, stops only because of the cap).
* **SubstituteNameTokens**: first `$N` → name; the original copies the tail through a 48-byte stack buffer (overflow); native checks `capacity`. 300/300 vectors.
* **ExtractDelimitedToken(buf, str, delims, k)**: skips *k+1* delimiter-separated groups (k < 0 skips none), then copies to the next delimiter; leading-delimiter strings (`"\n"+text`, `'#'`-captions) are indexed from 0; returns NULL when the string ends. 500/500 vectors.
* `wrapGreedy` is **new-text only** (the original has no wrap): greedy, breaks at spaces, hard-splits over-long words, `$` codes measured as zero width.

## 8. Open items / GAPs

* `uiKanaGlyphUV` / `uiKanjiGlyphUV` tables (Japanese sprites) — advance 20 pinned, (u,v) not.
* Palette banks and the sound ids are recorded as constants, not given semantics.
* Meaning of `$G`/`$U` strings and of the `$A`/`$T` id words.
* `CopyDialogueTextBounded` / `SubstituteNameTokens` have no static callers (no `jal`, no pointer word); possibly reached through code the decompile missed.
* The 3 non-clean script references of `script-vm.md` §6 could be reached through a text escape — **no**: no `$`-code changes the cursor to another message; escapes only draw (closes that question for W2-01).

## 9. How the numbers are produced

`tests/native_w2/data/text_script_vectors.txt` was generated by a ~200-line MIPS interpreter that executes the **real instructions** of
`RenderEscapeText`, `uiDrawGlyph`, `uiTextWidth`, `uiDrawNumberAuto`, `uiDrawDialogueFrameA/B`, `uiDrawIcon20`, `CopyDialogueTextBounded`,
`SubstituteNameTokens`, `ExtractDelimitedToken` and the whole `dialogueTask` (libc and GS writers stubbed; inputs synthetic, seed fixed).
Generator and interpreter are in `.qoder/wave2/W2-07-report.md` (appendix). 13 306 vectors, all reproduced bit-exactly by the C++ port (`oracle_vectors=13306/13306`),
including 48 full message-window scenarios (9 376 ticks, all 7 states; effect log: spawns of all 15 child functions, 8 stamp awards, 2 warps, 3 photos, 7 object attributes, 29 closes).

```
cmake -S src/native -B build/w-W2-07 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/w-W2-07 --target w2_text_script_test -j
build/w-W2-07/w2_text_script_test --elf "$RT_ELF" [--verbose]    # exit 0 only on PASS; no ELF -> NO_ELF + ROM-independent checks
```
