# Navigation index bindings

This is the binding layer for the two selectors described in
[`nav-tables.md`](nav-tables.md).  It distinguishes the **pack type** used by
the static directory from the **scene/event tag field** used by the outer
directory.  In particular, an outer `T` is not a COURSE number.

## Producer of the static selector

**PROVEN.** `FUN_0020f950` selects a 16-byte per-race record from one of the
adventure, quick-race, or two-player race-record tables, then performs:

```
param_2[0x20] = selectedRace;
param_2[0x22] = *(u8 *)selectedRaceRecord;
```

It immediately uses that same byte to select the category-`0x40000` asset
task.  `FUN_0024fcd8` subsequently reads the signed byte at `param_2+0x22`.
For values other than `-1` and `0x22`, it indexes the static directory at
`0x002bd578` (with the two aliases below).  Independently,
`FUN_0020c188`'s category-`0x50000` loader reads `tcb+0x22`; the descriptor
mapping in `asset-map.md` proves `0..17` are the contiguous COURSE slots and
`18..38` the contiguous ACTION slots.  Therefore the static nav selector and
the misc-loader pack-type byte are the same value.

The labels below are disc pack identities, not inferred human course titles.
The two types with absent numbered course files are absent from this physical
slot sequence: indices 15--17 are `C18`, `C19`, `C20`.

## Static directory: type/index -> pack

| type | pack selected by the byte | nav result |
|---:|---|---|
| 0 | `COURSE/C00.BIN` | static 0 |
| 1 | `COURSE/C01.BIN` | static 1 |
| 2 | `COURSE/C02.BIN` | static 2 |
| 3 | `COURSE/C03.BIN` | static 3 |
| 4 | `COURSE/C04.BIN` | static 4 |
| 5 | `COURSE/C05.BIN` | static 5 |
| 6 | `COURSE/C06.BIN` | static 6 |
| 7 | `COURSE/C07.BIN` | static 7 |
| 8 | `COURSE/C08.BIN` | static 8 |
| 9 | `COURSE/C09.BIN` | static 9 |
| 10 | `COURSE/C10.BIN` | static 10 |
| 11 | `COURSE/C11.BIN` | static 6 (alias) |
| 12 | `COURSE/C12.BIN` | static 12 |
| 13 | `COURSE/C13.BIN` | static 13 |
| 14 | `COURSE/C14.BIN` | static 14 |
| 15 | `COURSE/C18.BIN` | static 15 |
| 16 | `COURSE/C19.BIN` | static 15 (alias) |
| 17 | `COURSE/C20.BIN` | static 17 |
| 18 | `ACTION/A00.BIN` | null |
| 19 | `ACTION/A01.BIN` | static 19 |
| 20 | `ACTION/A02.BIN` | static 20 |
| 21 | `ACTION/A03.BIN` | null |
| 22 | `ACTION/A04.BIN` | static 22 |
| 23 | `ACTION/A05.BIN` | null |

The town names in the scripted scene table are **not** an additional direct
static-index naming scheme.  (Corrected 2026-10-16 per scene-tile-map.md: the
earlier draft said the table "proves scene 0 is Peach Town" — scene 0 is
**My Garage** (tile 11, `FLD/023.BIN`); **Peach Town is scene 1** (tile 43,
`FLD/223.BIN`).)  Nothing in the table names a static index.  Static index 0
is proven to be the `C00` pack type; its graph's bounds are compatible with,
but do not identify, that course geography.

## Outer directory: `T = (taskTag >> 10) & 0x1f`

`FUN_00210158` writes `-1` to `+0x22` for ordinary scripted scene entries and
`0x22` for scene IDs 8 and 22.  `FUN_00210430` and `FUN_0020e708` then spawn
`FUN_00218458` tasks with the scene ID shifted left by 10.  Thus its `T` field
is the scripted-scene/event ID, whose authoritative names are the records at
`0x002be438`, not a COURSE or ACTION type.  `dir` is `outer[T]`; `car0` is the
first `{segments, transitions}` pair at that directory (shown to make the
binding reproducible).

(Corrected 2026-10-16 per scene-tile-map.md: the name column below was
previously the scene-table names shifted by one — it skipped My Garage and My
City and put `LightHouse` on T=7 — and left the SJIS records untranslated.
Names are now the `0x002be438` record strings at each `T`; directory and pair
pointers are unchanged.)

| T | binding from scripted scene table | dir | car0 graph pair |
|---:|---|---:|---|
| 0 | My Garage | `0x002d9558` | `0x002d5750/0x002d5a20` |
| 1 | Peach Town | `0x002d9310` | `0x002c5ff0/0x002c67d0` |
| 2 | Fuji City | `0x002d9368` | null car 0 |
| 3 | Sandpolis | `0x002d93c0` | `0x002ca910/0x002cac58` |
| 4 | Chestnut Canyon | `0x002d9408` | `0x002cf810/0x002cfb28` |
| 5 | Mushroom Road | null | — |
| 6 | White Mountain | `0x002d9460` | `0x002d08f0/0x002d0a40` |
| 7 | Papaya Island | `0x002d94b0` | `0x002d1c10/0x002d1ee0` |
| 8 | Cloud Hill | `0x002d9500` | null car 0 |
| 9 | My City | `0x002d9558` | `0x002d5750/0x002d5a20` |
| 10 | `???` (literal scene string) | null | — |
| 11 | Bridge | null | — |
| 12 | UFO | null | — |
| 13 | Ruins | null | — |
| 14 | LightHouse | null | — |
| 15 | `022` (literal FLD name) | null | — |
| 16 | `110` (literal scene string) | null | — |
| 17 | `120` (literal scene string) | null | — |
| 18 | `202` (literal scene string) | `0x002d9598` | `0x002d6390/0x002d6600` |
| 19 | `212` (literal scene string) | null | — |
| 20 | `221` (literal scene string) | null | — |
| 21 | `232` (literal scene string) | null | — |
| 22 | クラウド (SJIS "Cloud") | `0x002d95a0` | `0x002d66d0/0x002d67a8` |
| 23 | パパイヤ (SJIS "Papaya") | `0x002d9848` | `0x002d8850/0x002d9060` |
| 24 | ホワイト (SJIS "White") | `0x002d97e8` | `0x002d81f0/0x002d86b8` |
| 25 | いせき (SJIS "ruins") | `0x002d9778` | `0x002d7b90/0x002d7cb0` |
| 26 | マロン (SJIS "Marron/chestnut") | `0x002d9788` | `0x002d7d10/0x002d80b8` |
| 27 | ピーチ (SJIS "Peach") | `0x002d9648` | `0x002d67f0/0x002d6b20` |
| 28 | はし (SJIS "bridge") | `0x002d96a8` | `0x002d6c30/0x002d6db0` |
| 29 | フジ (SJIS "Fuji") | `0x002d96b0` | `0x002d6e30/0x002d7580` |
| 30 | こうそく (SJIS "highway") | `0x002d9710` | `0x002d77f0/0x002d7958` |
| 31 | サンド (SJIS "Sand") | `0x002d9718` | `0x002d79d0/0x002d7b20` |

`T=0` and `T=9` intentionally share the same outer directory pointer.
`car0` being null at T=2/8 does not make the outer directory null: later car
slots contain graph pairs and are selected by `FUN_0024fcd8`'s car ordinal.

## Pair-overlap matrix

The comparison is exact `{segmentVaddr, transitionVaddr}` equality, not a
nearby-address or geometry comparison.  Every non-null outer graph pair was
compared with all populated static pairs (`0..17, 19, 20, 22`): **no exact
pair overlaps**.

| outer T set | exact static-index overlap |
|---|---|
| 0, 1, 2, 3, 4, 6, 7, 8, 9, 13, 18, 22--31 | none |
| 5, 10--12, 14--17, 19--21 | outer pointer null |

The static aliases remain the only proven cross-index sharing in this pass:
`11 -> 6` and `16 -> 15`.  Do not infer shared geography between a town/event
and a course merely from a nearby ROM address; this exact-pair test found none.

## Ledger

| status | claim and evidence chain |
|---|---|
| PROVEN | `FUN_0020f950` copies byte 0 of a selected race record to `param_2+0x22`; `FUN_0024fcd8` consumes that byte as the static selector except `-1/0x22`. |
| PROVEN | The copied byte is the misc-loader type: `FUN_0020c188` reads the same offset, and `asset-map.md` proves its descriptor mapping. |
| PROVEN | Static type-to-pack identities, aliases, and null pairs above follow from that mapping plus the raw directory at `0x002bd578`. |
| PROVEN | `T` is the task-tag field `(tag>>10)&0x1f`; scene/event spawners construct it from the scripted scene ID.  The raw outer directory is at `0x002d98a8`. |
| PROVEN | No populated outer pair equals a populated static pair in this executable. |
| INFERRED | A static course graph and a location with compatible tile-local bounds describe the same geography.  The chain is pack type -> course asset plus geometry compatibility; no code associates a static pair with a town name. |
| OPEN | Human translations for the SJIS scene strings, semantic names for numeric literal scene labels, and human course titles beyond the `Cnn` pack identifiers. |
