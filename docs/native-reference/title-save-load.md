# Title Menu and Save/Load Boundary

## ROM-grounded flow

```text
0x00208A88 titlePressStart (title picture / PRESS START BUTTON)
    | confirm
    v
0x00209520 titleNewGameContinue
    |-- row 0: New Game --> initialise progress defaults (0x00229FD8)
    `-- row 1: Continue --> save/load spinner (0x00228D10)
                                  |
                                  v
                            load save slot task (0x00228B10)
                              |-- card-info (0x002282C0)
                              |-- read slot (0x00228510; 0x3448 bytes)
                              `-- confirmation/preview (0x002280D8)
```

`0x00209520` builds a two-entry menu (two `FUN_002058A8` item insertions at
`0x00209638` and `0x00209654`) and its maintained symbol-map description is
“title menu scene (New Game / Continue)”. This is the evidence for the row
set and order used by the native state module.

| Row | ROM grounding | Native behaviour |
| --- | --- | --- |
| New Game | `0x00209520` `titleNewGameContinue`; New-Game branch reaches `0x00229FD8` at `0x00209B84` | Always enabled; confirm returns `StartNewGame`, then fades. |
| Continue | `0x00209520` `titleNewGameContinue`; save/load entry point `0x00228D10` is a documented caller target | Enabled when the selected fake-card slot exists. A valid slot returns `LoadContinue`; corrupt/invalid existing data returns `SaveCorrupt` without a fade. |

The raw ELF byte scan found **no ASCII literals** `CONTINUE`, `NEW GAME`,
`OPTIONS`, `CREDITS`, `SAVE`, or `LOAD`. The game renders text through its
FONTE/UI resources, so row names above come from the maintained function map,
not invented raw-string hits.

## Save-card observations

`0x002277B0 InitSaveSlotDefaults` clears and seeds a `0x3448`-byte progress
record. `0x00228510 MemcardReadSlotTask` reads that same size; `0x00228708
MemcardCreateSlotTask` creates a directory, `icon.sys`, `choroQ.ico`, and the
save file. `0x00227F90 DrawSaveSlotEntry` displays a player name, track, time,
and free-block count in the ROM UI.

`FakeCardBackend` deliberately does not read disk. Its two memory slots model
only `exists`, `valid`, `corrupt`, and preview metadata (`playerName`,
`course`, `elapsedFrames`) so headless tests are deterministic.

## Preview layout and navigation

The authored preview uses 640x448 UI design positions through the project’s
centred 640x480 FONTE frame convention (+16 Y): panel `(144,136,352,152)`;
New Game row `(168,160,304,40)`; Continue row `(168,216,304,40)`; labels start
at `(192,168)` and `(192,224)`. The panel and highlight are `ScreenQuad`s;
labels are FONTE `HudQuad`s.

Navigation clamps rather than wraps. This is an authored safety policy: it
matches the title’s linear two-item construction, avoids silently jumping past
an unavailable Continue entry, and is explicitly covered by the W6-02 test.

The fade is an authored deterministic 32-frame state machine. The ROM calls
the screen-fade helper `0x002080F8` from the title handler, but this module does
not yet establish that helper’s exact duration.

## GAP list

- Exact glyph/resource indices and raw localized strings for the two title
  labels are not recovered; direct ASCII ELF search was negative.
- No ROM evidence found for title-level `Options` or `Credits`; they are not
  exposed as title rows.
- Exact title-menu rectangle coordinates, palette, and cursor art are authored
  preview defaults, not ROM measurements.
- Exact fade duration/easing is not established; 32 frames is authored.
- The save-header field offsets/semantics for name, course, and elapsed time
  are not established here; these fake-backend metadata fields are authored.
- ROM validation/corrupt-save presentation is not fully recovered; the native
  `SaveCorrupt` action is an authored integration seam.
