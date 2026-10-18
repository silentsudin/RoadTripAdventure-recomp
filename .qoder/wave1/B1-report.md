# Batch B1 report: likely/guess symbol notes

## Result

All 28 entries whose note contained `likely` or `guess` were re-read from the
guest ELF. Their notes in `config/game_symbols.toml` now state observed
prologue/body evidence. No name was changed: the binary proves the operation
shape and call graph, but not a more specific gameplay label for the handful
of contextual renderer names. No entry remained unverifiable, so no residual
`likely`/`guess` tag was necessary.

## Reproducible ELF check

The image is MIPS little-endian and uses the requested virtual-to-file mapping.
This is the core snippet used for every address window (the printed words are
the raw instruction words, not copied ROM data):

```python
import struct
ELF = "/Users/holzr/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98"
BASE = 0x1FF000
def words(va, count):
    with open(ELF, "rb") as f:
        f.seek(va - BASE)
        return struct.unpack("<%dI" % count, f.read(4 * count))
for va in (0x21FA38, 0x220030, 0x2213E8, 0x221570, 0x221F70,
           0x222F08, 0x225120, 0x223588, 0x2238A0, 0x223B30,
           0x244D60, 0x244F60, 0x2455C0, 0x20B348, 0x200DC0,
           0x203CD0, 0x203D90, 0x2058A8, 0x205C20, 0x205C08,
           0x21D470, 0x22C648, 0x26C588, 0x2584B0, 0x2574A0,
           0x2588B8, 0x258EB0, 0x2728F8):
    print(hex(va), [hex(x) for x in words(va, 4)])
```

Representative raw words that anchor the rewritten claims:

* `0x21FA38`: `27bdffd0`, `ffb10008`, `0080882d`, `26240100`; body calls
  `0x21D0C0` four times and tests each result.
* `0x220030`: `83838ac8`, `27bdffe0`; body has four offset-`0x10` slot
  branches, `slti ... 0x1a`, and calls `0x226610`/`0x227048`.
* `0x222F08`: `27bdff30`, `ffb00080`; body has `slti ... 0x20` and advances
  the record pointer by `0x40`, emitting `+0x270`.
* `0x225120`: `27bdffc0`, `ffb00000`; the loop terminates at `slti ... 0x18`
  and dispatches the four effect callees recorded in the note.
* `0x205C20`: `27bdffe0`, `3c030035`; the allocator loop tests `0x30`, scans
  seven pool steps, and initializes `0x140` by `0x70`.
* `0x2584B0`: `27bdffa0`, `ffb20030`; three calls load literal packet codes
  `0x8050`, `0x8130`, and `0x8070`.
* `0x2574A0` begins with `addiu a1,zero,0x23`; the second RPC record uses
  `0x22`. `0x2588B8` uses `0x26`; `0x258EB0` passes literal `1`.
* `0x21D470` starts with the COP2/VU macro words `240a0000`, `d8a80000`,
  `d8b80010`, `d8ba0020`; `0x2728F8` likewise starts with COP2 qword/scalar
  operations and ends its short body with a qword store.

## Entries deepened

### Rendering and VIF/VU (17)

`0x21FA38`, `0x220030`, `0x2213E8`, `0x221570`, `0x221F70`, `0x222F08`,
`0x225120`, `0x223588`, `0x2238A0`, `0x223B30`, `0x244D60`, `0x244F60`,
`0x2455C0`, `0x21D470`, `0x22C648`, and `0x26C588` now cite fixed loops,
flag masks, packet helpers, table strides, or VU/COP2 bodies. `0x20B348`
was included here because its loader evidence is the source of the pose
packet/TBP claim. Confidence is **high** for operation shape and **medium**
for contextual names such as showroom, carousel, wheel LOD, and credits.

Notable checks: `0x2213E8` loops three model records, `0x221570` loops four;
`0x222F08` loops 32 records; `0x225120` loops 24 records; `0x22C648`
loops 13 stamp records; and `0x26C588` repeatedly calls the two sprite
emitters from a fixed arena. These are static observations, not runtime
observations.

### UI (6)

`0x200DC0`, `0x203CD0`, `0x203D90`, `0x2058A8`, `0x205C20`, and `0x205C08`
were all verified. Confidence is **high**: the bodies directly select sprite
dimensions/palette, call the named UI helpers, update the menu record, scan
the pool, or tail-jump to `0x205350`. The previous icon-bank wording was
made less specific where the body only established selector dispatch rather
than a complete semantic mapping.

### Sound and GS wrappers (4)

`0x2584B0`, `0x2574A0`, `0x2588B8`, and `0x258EB0` were verified with
literal packet/RPC command construction. Confidence is **high** for command
identity and argument flow; the old fade/stop semantic guesses were removed.

## Rejected or contradicted hypotheses

No hypothesis was conclusively contradicted enough to justify a name change.
The static body supports every maintained name at the subsystem level. The
following narrower claims were deliberately not promoted: exact showroom
versus garage usage, exact wheel LOD distance, exact carousel screen, and
credits presentation semantics. Their notes now describe the observed call
and branch structures instead.

## Open questions

* Runtime call sites would be needed to distinguish the contextual renderer
  labels (showroom/garage, wheel distance, and carousel mode).
* The VU macro inputs at `0x21D470` and `0x2728F8` are structurally verified,
  but assigning every COP2 operation to a high-level projection/divide name
  requires a VU disassembler or execution trace.
* The static pass cannot prove which sound command `0x23`/`0x22` or `0x26`
  means inside the sound server; only the EE-side RPC records are proven.
