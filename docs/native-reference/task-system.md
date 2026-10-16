# Async task system — tag and payload taxonomy

Static reference for `SLUS_203.98`, updated 2026-10-16.  This is deliberately
about the game's *content-request spine*, not a proposed host implementation.
Addresses are ELF virtual addresses.  The two `PT_LOAD` mappings independently
give the file-to-ELF delta `vaddr - fileoff = 0x1ff000`:
`0x200000-0x1000` (text) and `0x29a880-0x9b880` (data). **PROVEN.**

## Executive map

```
producer -> gs_tasks_spawn(parent, FUN_0020dc50, tag)
         -> 0x20dc50 serializes one request in globals
         -> task becomes FUN_0020d120 (dispatcher)
         -> loader steps CD/DMA/relativizes payload
         -> dispatcher installs FUN_0020df98
         -> 0x20df98 restores the queued tag or finishes the service task
```

`FUN_0020dc50` is **not** the completion callback that receives a loaded
buffer.  It is the admission/serialization callback for a request task.  It
copies `task+0x1c` to global `DAT_00333374`, marks the caller frame context
busy (`+0xf=1`), conditionally sets its `+0x28 |= 0x400000`, kills the request
task, and leaves the resident service task to dispatch the tag. **PROVEN.**

## Task records and queue lifecycle

The fixed `gs_tasks_pool` has 48 records of `0x30` bytes.  `gs_tasks_spawn`
(`0x204c48`) linearly picks the first byte-zero record, initializes it, puts
the callback at `+0x0c`, tag/payload at `+0x1c`, links it at the tail of the
parent's child list (`parent+0x10`, siblings `+0x14`), and stores parent at
`+0x18`.  Exhaustion calls `fatal`. **PROVEN.**

| offset | role | evidence |
|---:|---|---|
| `+0x00` | live byte; bit 1 is the deferred-kill flag | `0x204c48`, `0x204ed0`, `0x204a00` |
| `+0x02` | 16-bit cursor/scratch | initialized by spawn; used by consumers |
| `+0x04` | result/parent-visible state | initialized by spawn |
| `+0x08` | per-task tick count | incremented by walker code | 
| `+0x0c` | current step callback | spawn and `0x20df98` |
| `+0x10/+0x14/+0x18` | first child / sibling / parent | spawn and destroy paths |
| `+0x1c` | opaque request tag | dispatcher masks this word |
| `+0x20..+0x27` | loader/local state bytes | loaders use `+0x22`, `+0x26`, `+0x27` |
| `+0x28` | second opaque word / frame flags | completion and kill clear it |
| `+0x2c` | cleanup callback | kill and destroy invoke it |

The task walker clears the deferred-kill bit on a pass, calls the task callback,
then follows child/sibling lists.  `gs_tasks_kill` (`0x204de8`) runs cleanup,
destroys descendants, unlinks the task, and frees it.  `FUN_00204ed0` sets bit
1 on one task; `0x204ee0`/`0x204f18` set/clear it across a child chain;
`0x204a00` performs the actual deferred destruction. **PROVEN.**  The exact
order in which newly appended siblings run in the same frame is **OPEN** from
the currently recovered walker body; do not treat host "next frame" behavior
as an authentic claim.

### Serialized loader state machine

`DAT_003356c0` is a global, single in-flight loader phase, not per request:

| phase | action |
|---:|---|
| `0` | issue initial read / first stage |
| `1` | issue second/header-body read or DMA rebase stage |
| `2` | walk DMA/REF chain, synchronize channel 1, send textures, issue bulk read |
| `3` | fix up loaded directories and package-local structures |
| `4` | loader-specific final store, then `-1` completion sentinel |
| `0x8000` bit | CD read is outstanding; `FUN_0020d120` polls it via `FUN_0020e440` |
| `-1` | dispatcher completion: clears serialization flag and installs `FUN_0020df98` |

`0x20df98` either adopts `DAT_00333374` as the next tag, zeros its local
scratch, changes its own callback back to `0x20d120`, and immediately starts
that request; or clears its tick/state and idles. **PROVEN.**  Thus requests
are FIFO only to the extent that callback execution and the parent child list
preserve producer order; there is no per-category parallel queue. **INFERRED**
(the one-global-state and one-pending-tag facts are proven).

## Tag layout

The dispatcher uses `category = tag & 0x000f0000`, not the top nibble of the
u32.  The low byte is normally a descriptor/type/index.  Bits `15:8` are
consumer-defined slot/variant data; for CAR loads `(tag >> 8) & 7` selects the
RAM slot band.  Some producers also put a semantic count in bits `15:8` or use
the low nibble as a sub-operation. **PROVEN where stated; do not assume one
uniform bitfield beyond the category and low byte.**

| tag family | dispatch / observed interpretation | descriptor payload |
|---|---|---|
| `0x00000` | sub-op low nibble: staged garage/notebook/scene chains | table B/D fixed records |
| `0x10000`, `0x20000`, `0x60000`, `0x80000`, `0xd0000` | fixed title/logo/PUTI/parts/icon packs | table B fixed records or PUTI chunks |
| `0x30000` | `FUN_0020cdc0` shop-pack state machine | table D chain |
| `0x40000 | type` | direct misc-pack descriptor | B[`type`] |
| `0x50000` | `FUN_0020c188`; **type is signed byte at service task `+0x22`, not tag low byte** | F[64+type]: COURSE 0..17; ACTION 18..38 |
| `0x70000` | remembered car-model reload/variant | table E/model slot |
| `0x90000 | id` (usually written `0x390000|id`) | field loader; `id`, slot=`id&3` | F[0..63] FLD; unchecked id 64 demonstrates C00 spill |
| `0xa0000 | idx` | meter/gauge load | D+22, METER0..10 |
| `0xb0000 | slot/index` | car model load | table E Q00..Q150 |
| `0xc0000` | OPTION model path | lone table-G OPTION.BIN |

The written forms `0x370xxx`, `0x380xxx`, and `0x390xxx` are therefore
categories 7, 8, and 9 respectively: the visible leading `0x3` is outside the
dispatcher mask. **PROVEN.**  `0x390000|id` is FLD streaming; `0x380xxx` uses
the parts/icon category; `0x370xxx` reaches the car reload category.  The
resident-table helpers use `0x370300/0x370400 | count`: their count/payload
meaning is proven at the producer, while their ultimate category-7 model
effect needs a focused trace. **OPEN.**

## Direct request producers (`FUN_0020dc50`)

Ghidra's cross-reference set contains **92 direct parameter references**, in
**34 producer functions**, to `0x20dc50`.  This is the complete static set of
directly encoded callback uses in this program database (dynamic callback
pointers are not included).  Multiple sites in a function count separately.

| family / count | direct sites and tags | purpose |
|---|---|---|
| `0x39` / 7 | `0x210158` (scene-table `tileId`), `0x2201c0` ×6 (`id` from position) | scripted scene/POI preload and five-neighbour terrain streaming |
| `0x37` / 8+ | `0x210158`, `0x20e708`, `0x20f950`, `0x211530`, `0x211648` | resident/car-table payload preparation; count placed in tag low byte |
| `0x38` / 0 statically explicit in recovered calls | no direct constant recovered in this pass | category exists in dispatcher (parts/icon pack) |
| `0x40` / 10+ | `0x20f950` race selection; `0x26b848` types 5, 0x17, 0x18; other menu/location callers | per-screen/UI/model misc packs |
| `0x50` / producer-controlled | town/race context sets service `+0x22`; dispatcher calls misc loader | COURSE/ACTION container pack, type byte governs selection |
| `0x70` / 7+ | `0x211170`, `0x211320`, `0x26b848`, race paths | car model remembered-slot reload |
| `0x90` / 7 | `0x210158`; `0x2201c0` ×6 | FLD (`id|0x390000`) |
| `0xa0` / 3 | `0x211170` (`0xa0000`), `0x211320` (`0xa000a`), `0x26b848` (runtime idx) | gauges/meters |
| `0x30` / 10+ | town exploration/dialogue, `0x26b848` (`0x300001`) | shop/scene pack state machine |
| fixed / 20+ | boot/menu/race/location functions below | logo, title, option and staged support packs |

### Full call-site ledger by producer

This is intentionally address-complete (the first column is the producer;
parenthesized count is the number of direct `0x20dc50` parameter xrefs):

| producer | n | tag(s) or role observed |
|---|---:|---|
| `0x2052d0` | 1 | `0x400000`, menu misc |
| `0x208848` | 2 | `0x310000`, menu staged pack |
| `0x2086b0` | 1 | option/menu support |
| `0x20a2b0` | 1 | `0x410000` |
| `0x20e708` | 1 | `0x370400|residentCount` |
| `0x20f950` | 28 | race start: `0x40000|type`, `0x370300/0x370300|count`, `0x450000`, `0x750000` |
| `0x210158` | 3 | `0x370400|count`, `0x340008`, then `0x350000` or `0x390000|tileId` |
| `0x2105b8` | 6 | town dialogue/house `0x30000|resident`, then scene handler |
| `0x211170` | 3 | `0xa0000`, `0x70001`, `0x000004` |
| `0x211320` | 5 | `0x000005`, `0xa000a`, `0x70006` |
| `0x215220` | 2 | pause/menu support |
| `0x211530` | 1 | `0x70400|callerBits|count` |
| `0x211648` | 1 | `0x70300|callerBits|(count-1)` |
| `0x211708` | 2 | `0x30000|resident` / `0x30000` |
| `0x211808` | 2 | same town-start pair |
| `0x214ad0` | 3 | pause/menu support |
| `0x2201c0` | 6 | six guarded `0x390000|tileId` requests |
| `0x22aeb0` | 2 | course-select support |
| `0x22bb88` | 1 | scene intro support |
| `0x22bfd0` | 3 | scene intro support |
| `0x22cd38` | 1 | course-select hub support |
| `0x246250` | 1 | carousel/result support |
| `0x248580` | 1 | carousel/player-select support |
| `0x228708` | 1 | save/load UI support |
| `0x265b30` | 1 | visited-house/menu support |
| `0x265568` | 1 | visited-house menu support |
| `0x2665b8` | 1 | location mode support |
| `0x26b848` | 3 | `0xa0000|idx`, `0x300001`, `0x70003`; plus direct misc types 5/17/18 |
| `0x26be50` | 1 | location-arrival support |
| `0x26d108` | 1 | house entry |
| `0x26d370` | 3 | boot/town transition support |
| `0x26d438` | 1 | boot/menu transition support |
| `0x26d550` | 1 | boot/menu transition support |
| `0x26d678` | 1 | boot/town transition support |

The total above deliberately separates 92 raw sites from tag-family counts:
large race-start sites reuse a small number of expressions, and some remaining
menu/bootstrap calls have only dynamic tag expressions.  Those exact dynamic
values are **OPEN**, not silently guessed.

## Completion and loaded payloads

Completion is signaling, not a callback argument containing a buffer handle.
The service task owns the tag and global in-flight phase.  On completion it
sets `FUN_0020df98`; producers poll their frame/task state (`+0xf`, `+0x24`,
and context-specific flags) before moving to the next phase. **PROVEN.**

`loadFieldTile` (`0x20bac8`) and `FUN_0020c188` both:

1. load through scratch `0x100000`, walk REF DMA chains, and upload textures;
2. bulk-copy into `fieldTileData + slot*0x3a8000` (four slots; tile slot 3 has
   a `0x528000` cap);
3. add the load base in place to offset directories (65 field-cell entries;
   then 256 PMP entries), so on completion consumers see host/EE absolute
   pointers rather than file-relative offsets.

For FLD, the load base is also published in `fieldCellTables[slot]`; the
misc/course/action path publishes PMP/model pointers into its per-slot service
context (`+0x60`, `+0x70`, `+0xe8`) and global `fieldMiscModels`/`fieldMapModels`.
**PROVEN.**  This is why a faithful loader must expose an owned loaded-object
handle only after fix-up and texture upload, rather than exposing raw file
bytes early.

## Descriptor and resident-list cross-links

The dispatcher bindings are: FLD F[0..63] via category 9; COURSE F[64..81]
and ACTION F[82..102] via category 5's `service+0x22` type byte; misc table B
via category 4; meters via A; cars table E via B/7; and OPTION table G via C.
See `asset-map.md` for exact file identities. **PROVEN**, except ACTION type
18..38 is physically proven by contiguous stride but has no observed constant
producer (**INFERRED**).

Scene table `0x2be438..0x2be537` is 32 × 8 bytes:

```
{ u32 sceneNamePtr, u16 tileId, u8 residentStart, u8 residentCount }
```

`SpawnTownEntities` reads `residentStart`/`residentCount`, resolves each index
through `FUN_0023db80`; an unresolved record becomes
`gs_houses_residents[scene] + index*0x10`, otherwise it becomes
`gs_progress_ram_address + result*0x1c`.  It writes those pointers to
`DAT_0177ea60[]`, then requests `0x370400|residentCount`. **PROVEN.**
This decodes the resident-list *entry target structure* as a 16-byte resident
record or a 28-byte progress record, but the fields inside either record and
the semantics of the category-7 consumer are still **OPEN**.  The two
`tileId=64` rows deliberately/unguardedly select F[64] (`COURSE/C00.BIN`) under
category 9; they are not FLD filenames. **PROVEN.**

## Native mapping guidance (no implementation change)

For later work in `src/native/gameplay/task.*` and asset loaders:

- Preserve the 48-slot, first-free allocation and parent/child/sibling shape;
  model a task payload/tag separately from an asset-result handle.
- Provide one serialized asset-service lane, with a pending tag and explicit
  phase transitions; do not make tile, car, and UI requests silently parallel
  if authenticity is the goal.
- Decode category as `tag & 0x000f0000`; retain all other bits intact until
  the specific category owns them.  In particular, category 5 must use the
  service type byte, not `tag&0xff`.
- Publish a loaded asset only after its relative directories/pointers have
  been rebased and its required texture packet upload has completed.
- Treat FLD slot selection (`id&3`) and category-9's unchecked descriptor
  stride as compatibility behavior; validate host bounds without changing the
  known `id=64 -> C00` behavior unless an explicit safety policy says otherwise.
- Keep resident/progress resolution as an adapter boundary: the observed
  `16-byte resident` versus `28-byte progress` selection is authentic, but
  their individual schemas are not yet documented.

## Ledger / open work

| status | claim |
|---|---|
| **PROVEN** | 48×`0x30` pool, spawn fields/tree links, deferred-kill mechanics, one global loader phase and pending tag. |
| **PROVEN** | dispatcher category mask; table bindings listed above; category-5 type source is `service+0x22`. |
| **PROVEN** | `0x20dc50` is admission/serialization, not loaded-buffer completion. |
| **PROVEN** | FLD and misc pointer relativization/publication; scene record layout and resident/progress pointer strides. |
| **INFERRED** | FIFO behavior beyond one pending tag, because scheduler execution ordering is not fully recovered. |
| **INFERRED** | ACTION type 18..38 is the ACTION subtable, from F-table continuity and loader stride. |
| **OPEN** | exact semantic fields of the 16-byte resident and 28-byte progress records. |
| **OPEN** | category-7/8 consumer meaning for every `0x37xxxx`/`0x38xxxx` count/variant and every dynamic menu/bootstrap tag. |
| **OPEN** | exact same-frame ordering of tasks newly appended during the walker pass. |
