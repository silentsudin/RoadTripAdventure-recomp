# Native button-glyph repaint contract

`src/native/gameplay/glyphs.{h,cpp}` is a pure C++ prompt-selection module for
the native rebuild. It does not decode, modify, or ship a game atlas. The GPU
wave consumes its resolved `GlyphSpec` at each prompt draw.

## Grounding ledger

### Verdict: **UNVALIDATED** for non-ASCII FONTE PlayStation codes

The FONTE text atlas is proven to be 512 slots of 16x24 pixels, 32 columns,
with ASCII `c` (`0x21..0x7e`) in
`slot=((c-0x21)/31)*32+((c-0x21)%31)`; see `hud-layout.md` §2 and
`assets/glyph_atlas.h`. That proves normal ASCII labels only. It does **not**
assign a byte/code point to circle, cross, square, triangle, L1/R1/L2/R2,
SELECT, or START.

Static ELF inspection was performed on
`$HOME/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98`, using the
documented mapping `fileOffset = vaddr - 0x1ff000`:

| observation | vaddr | file offset | exact bytes | verdict |
|---|---:|---:|---|---|
| wing help text begins | `0x326EAD` | `0x127EAD` | `46 75 72 74 68 65 72 6D 6F 72 65 2C 20 70 72 65` (`Furthermore, pre`) | CONFIRMED |
| first wing label | `0x326EBD` | `0x127EBD` | `73 73 20 74 68 65 20 4C 31 20 62 75 74 74 6F 6E` (`ss the L1 button`) | CONFIRMED ASCII only |
| second wing label | `0x326EED` | `0x127EED` | `64 20 70 72 65 73 73 20 74 68 65 20 4C 32 20 62` (`d press the L2 b`) | CONFIRMED ASCII only |

A Capstone 5 MIPS32 little-endian scan for the direct `lui 0x32` +
`addiu/ori 0x6ead` materialization found no direct pair, so this dialogue
string is likely reached through an indirect text table. That does not weaken
the byte observation and does not identify any symbol code. No scanned prompt
string in this pass demonstrated a non-ASCII FONTE button byte. Therefore
`kPlaystationFontCodesGrounded == false`, and PlayStation button requests are
provisional `Shape`s (triggers remain required textual labels), never invented
`FontCode`s.

### Deterministic closure for the GPU wave

Add a tiny, opt-in `RT_APP_FONT_DUMP=<dir>` hook beside the existing P6/PNG
snapshot writer. After the existing `decodeGlyphAtlas(SYS/FONTE.GSL)` succeeds,
write one labeled contact-sheet PNG or TGA: 32 columns x 16 rows, each source
slot enlarged 4x, with an ASCII `slot NN / code 0xNN` label beside/below every
tile. Preserve alpha and label all 512 slots, including empty ones. The dump's
file name must include the source hash. A captured sheet plus a string whose
non-ASCII byte is read from the ELF provides the missing byte -> slot -> visual
symbol correspondence. Only then replace a provisional PS Shape by its exact
`FontCode`, flip the grounding flag, and update its byte/vaddr row here.

## SDL family detection

`mapSdlPadType(int)` intentionally accepts numeric SDL3 enum values without a
SDL dependency. SDL3 `SDL_GamepadType` values are: unknown=0, standard=1,
Xbox360=2, XboxOne=3, PS3=4, PS4=5, PS5=6, Switch Pro=7, Joy-Con left=8,
right=9, pair=10, GameCube=11, Steam=12.

| SDL type | family |
|---|---|
| 4, 5, 6 | PlayStation |
| 7, 8, 9, 10 | Nintendo |
| 1, 2, 3, 11, 12, and unknown future non-PS values | XboxStyle |
| 0 (`UNKNOWN`) | Unknown / PS-default |

This preserves the sibling policy that a known non-PlayStation pad is
Xbox-style unless it is a Switch variant. `Unknown` is deliberately retained:
without a trustworthy device identity the renderer leaves the PS default alone.
`isRepaintNeeded()` is false for PlayStation and Unknown, true for XboxStyle
and Nintendo.

## Full prompt matrix

Entries are the `labelFor()` result; bracketed entries are vector Shapes.
`LS`, `RS`, and trigger values are FONTE-ASCII Text, not icon-atlas glyphs.
All PS shapes carry the provisional marker until the closure above completes.

| action | PlayStation | XboxStyle | Nintendo | Unknown (PS default) |
|---|---|---|---|---|
| Steer | `LS` | `LS` | `LS` | `LS` |
| Pitch | `RS` | `RS` | `RS` | `RS` |
| Reverse | [◯] | [B] | [B] | [◯] |
| Jet | [X] | [A] | [A] | [X] |
| Wing up | [L1] | [LB] | [L] | [L1] |
| Wing down | [L2] | [RB] | [R] | [L2] |
| Horn | [Triangle] | [Y] | [X] | [Triangle] |
| Lights | [R1] | [Y] | [X] | [R1] |
| View | [◯] | [B] | [B] | [◯] |
| Navigator | [Select] | [Back] | [-] | [Select] |
| Confirm | [◯] | [A] | [A] | [◯] |
| Back | [X] | [B] | [B] | [X] |
| Pause | [Select] | [Menu] | [Home] | [Select] |
| Start | [Start] | [Start] | [+] | [Start] |
| Gas | `R2` | `RT` | `ZR` | `R2` |
| Brake | `L2` | `LT` | `ZL` | `L2` |

The table captures the native controller config's driving actions in
`gameplay/menu.h` (accelerate, brake, reverse, jet, wing up/down, horn, view,
lights, navigator) through the named prompt actions above; a prompt bound to a
custom physical button should resolve that button through the same family
shapes rather than reuse a semantic default.

## Rendering and app glue contract

* `FontCode`: draw the cited 16x24 FONTE cell only after the grounding ledger
  says CONFIRMED. There are none in this initial module.
* `Text`: use FONTE ASCII via the proven mapping. Typical labels are `LS`,
  `RT`, and `ZR`; use the normal 12px layout advance and scale with surrounding
  help text.
* `Shape`: draw at approximately 16x16 logical UI pixels (24x16 for shoulder
  labels). Circle is a stroked/filled circle; Cross/Square/Triangle are their
  conventional outlines; shoulder/system shapes are a rounded rectangle with
  the centered fallback text. Render an ASCII centered label at roughly 9-11px
  using existing native text primitives; symbol labels such as `◯` describe
  the already-drawn shape and need no FONTE text quad. Do not sample an
  ungrounded FONTE symbol slot.
* Environment selection: `RT_APP_GLYPH_FAMILY=ps|xbox|nintendo` forces the
  named family. Absent/invalid means auto-detect from the opened SDL pad type;
  no pad or SDL `UNKNOWN` uses the PS-default `Unknown` family.

Glue pseudocode:

```cpp
onPadOpened(pad) {
  family = forcedFamilyOr(mapSdlPadType(SDL_GetGamepadType(pad)));
}
drawPrompt(action, rect) {
  auto spec = resolve(action, family);
  switch (spec.kind) {
  case FontCode: drawFonteCode(spec.fontCode, rect); break;
  case Shape:    drawPromptShape(spec.shape, spec.textFallback, rect); break;
  case Text:     drawFonteAscii(spec.textFallback, rect); break;
  }
}
```

Every prompt path must call `resolve` at draw time (or invalidate its cached
spec on pad-open/family change), including controller help, menus, driving
hints, and custom-bind labels.
