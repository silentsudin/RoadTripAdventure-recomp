# COURSE routes and checkpoint records

Status: **no COURSE route/checkpoint record has been demonstrated.** This
document records a bounded negative result from all 18 loader-referenced
`COURSE/C*.BIN` files (`C00..C14,C18..C20`). It deliberately does not turn
render vertices or prop anchors into gameplay routes.

## Result

`COURSE` uses the FLD-style offset container, but its small final region is a
prop draw chain. It is not a fixed-stride array of checkpoints or centerline
points.

The generic miscellaneous loader is `FUN_0020c188` (`FieldTileMiscLoader`).
It streams the container and sends VIF/DMA chains to VU1. This accounts for
the apparent coordinate-like values in COURSE: they are geometry sent to the
renderer. It does not establish an EE-side route consumer.

The actual AI navigation code is separately grounded in the executable:
`FUN_00250068` reads 24-byte segment records and an 8-byte transition table;
`FUN_0024fcd8` selects those tables from executable/static or runtime data.
Neither function has a demonstrated pointer into a COURSE file. Therefore a
`CourseRoute { centerline, checkpoints }` parser would be speculative.

## Container grammar

All referenced files begin with little-endian u32 boundaries:

```text
[headerSize, offA, offB, offC, declaredSize, ...]
```

`headerSize` is `0x20` or `0x30`. `0x20` headers have the normal three
interior boundaries and declared size; `0x30` headers supply more boundaries.
`C16.BIN` is intentionally excluded: it is an orphan and has no valid tail
boundary in this build.

The loader-referenced corpus tail begins at `offC`:

```text
0x00  i16[8] prologue = [48, 0, 80, 0, 0, 0, N, 0]
0x10  32-byte DMA/VIF setup record
0x30  32-byte DMA/VIF setup record
0x50  DMA CNT+TTE packet (one chain), then a zero-QWC RET
```

The two 32-byte setup records are identical in this corpus, with u32 words
`[0x10000000, 0, 0x01000101, 0, 0x60000000, 0, 0, 0]`.  The earlier
`0x01000201` transcription is not present in this byte order.

The packet at `+0x50` has DMA id `0x10` (CNT), followed by a zero-QWC id
`0x60` RET. Its VIF stream is fully bounded by the CNT QWC. Every corpus
member has exactly `N` repetitions of:

```text
UNPACK V4-32, NUM=1, TOPS+0  (GIF tag)
UNPACK V3-32, TOPS+1        (model vertices; NUM varies)
MSCALF 0x000a
```

It may have NOP padding and starts with one `STCYCL`. `MSCALF 0xA` is the VU1
OffsetStrip entry, rather than the WorldStrip entry (`0x8`) used by terrain.
Thus `N` is **PROVEN as prop draw/model count**, not a checkpoint count.

## Coordinate scale and anchors

The V3 upload contains IEEE-754 render-space vertices, with x/z-like values
inside the same tile-local 0..1600 convention used by terrain. They are not
fixed-point route points. The executable's field streaming uses `1 << 25`
authored units for a 1600-unit tile, giving the only defensible scale
candidate:

```text
1600 tile-local units == 2^25 authored fixed units
1 tile-local unit == 20971.52 authored fixed units
```

This conversion is **INFERRED**, not a route scale: no COURSE route record was
found to which it can be applied. The `N` anchors identify render props only.

## Per-course tail validation

| file | N / MSCALF 0xA draws | tail bytes | CNT QWC |
|---|---:|---:|---:|
| C00 | 12 | 12,352 | 765 |
| C01 | 18 | 15,168 | 941 |
| C02 | 46 | 19,744 | 1,227 |
| C03 | 47 | 11,504 | 712 |
| C04 | 13 | 16,848 | 1,046 |
| C05 | 20 | 22,848 | 1,421 |
| C06 | 18 | 17,472 | 1,085 |
| C07 | 15 | 19,920 | 1,238 |
| C08 | 15 | 17,472 | 1,085 |
| C09 | 22 | 16,720 | 1,038 |
| C10 | 14 | 20,096 | 1,249 |
| C11 | 18 | 17,472 | 1,085 |
| C12 | 28 | 17,504 | 1,087 |
| C13 | 9 | 12,704 | 787 |
| C14 | 44 | 19,728 | 1,226 |
| C18 | 102 | 45,456 | 2,834 |
| C19 | 102 | 45,456 | 2,834 |
| C20 | 9 | 17,552 | 1,090 |

## Ledger

| claim | status | evidence |
|---|---|---|
| COURSE container boundaries are valid FLD-style regions | PROVEN | all 18 referenced files; `FUN_0020c188` |
| trailing `N` is a prop/model draw count | PROVEN | N equals complete `MSCALF 0xA` repetition count in every tail |
| tail is a one-CNT, RET-terminated VIF chain | PROVEN | bounded tag/VIF walk of all 18 tails |
| tail records are checkpoints/goals/centerline | REJECTED | their complete VIF grammar is renderer input, with no gameplay reader |
| checkpoint/goal record stride and field offsets | OPEN | no such COURSE record identified |
| COURSE centerline points and max-gap/closed-loop metrics | OPEN | requires a route source or a demonstrated reconstruction mapping |
| route fixed-point scale | OPEN | only the terrain/tile conversion candidate is known |

## Consequence for native code

No `course_routes.{h,cpp}` or corpus assertions for nonempty routes were
added. Such tests would pin invented values and violate the repository's
provenance rule. A future implementation should start after proving a
COURSE-to-AI-table mapping (or locating the independently loaded route table),
then add a synthetic parser test and disc-gated pins derived from that proven
record grammar.
