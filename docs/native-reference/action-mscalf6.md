# ACTION `MSCALF 6` — ObjectStrip batches (A00..A20)

Updated 2026-10-16. Resolves the open item in
[`action-data-status.md`](action-data-status.md) ("Decode the `MSCALF 6` VIF
program and its 21-/51-vector input batches before treating every ACTION V3
payload as a 60-byte WorldStrip vertex stream"). Method: read-only Python scans
over the user disc extract `ACTION/A00.BIN..A20.BIN` (throwaway scripts in the
session temp dir, no game bytes retained), plus a static read of the VU1
microprogram, disassembled from the ELF's `.vutext` overlays using the opcode
tables of the capture-verified VU1 interpreter
(`third_party/PS2Recomp/ps2xRuntime/include/runtime/vu/ps2_vu1_exec.inl`). The
reconstructed 16 KiB VU1 image reproduces the documented FNV-1a 64
`0xe2868a5859386847` exactly, and every semantic claim below was cross-checked
against the verified HLE reference `vu1_hle_clipped.cpp` (`runObjectStrip`).

## Naming warning: `MSCALF 6` is not "VU1 entry 6"

The MSCAL/MSCALF immediate is a VU1 **byte address divided by 8**, so it
selects one 16-byte slot of the image's jump table (two instruction pairs per
slot). `MSCALF 0x6` therefore lands on slot 3 — `B 0x03E8`, the program this
repo calls **ObjectStrip** ("clipped strip of a rigid object", see
[`vu1-microcode.md`](../vu1-microcode.md) and the slot table in
[`vu1-programs.md`](vu1-programs.md)). SkyStrip (slot 6, code `0x1AA0`) would be
`MSCALF 0xc`; it has **zero** occurrences in ACTION. Both prior docs' tables
already mapped imm `0x6` → `0x03e8` → ObjectStrip; only the casual phrase
"entry 6" is misleading.

Jump table bytes read from the ELF (all eight slots decode as documented:
0x1268, 0x1438, 0x1620, 0x03e8, 0x0080, 0x1898, 0x1aa0, 0x1318).

## Corpus result (PROVEN)

Across all 21 files there are exactly two MSCALF immediates:

| MSCALF imm | slot | program | calls |
|---:|---:|---|---:|
| `0x6` | 3 | ObjectStrip | 3,065 |
| `0x8` | 4 | WorldStrip | 77,474 |

Every `MSCALF 6` batch lives in **directory entry 64** (the 65th chain at
`offA`, the last one before `offB`) of the ten alternate-class files. Each such
chain 64 is a single CNT frame (TTE) plus a zero-QWC RET, identical to the
corpus grammar; STCYCL is always `0x0101` (CL=1, WL=1 — no write masking, every
UNPACK vector writes its three lanes into one qword, NUM is never 0-encoded).

## Batch grammar (PROVEN)

```text
[optional] UNPACK V3-32, NUM=4 (once NUM=5), @TOPS+0x192   material header
UNPACK V4-32, NUM=1, @TOPS+0                GIF strip tag
UNPACK V3-32, NUM = 1 + 5*n, @TOPS+1        origin + n vertex records
MSCALF 6
```

* `NUM = 21` ⇒ `1 + 5·4`: origin + **4 records = one quad** (2 triangles) —
  3,049 batches.
* `NUM = 51` ⇒ `1 + 5·10`: origin + **10 records = one 10-vertex strip**
  (8 triangles) — 16 batches, all in A06.
* No other NUM values occur. The V4 tag's NLOOP equals `n` exactly in all
  3,065 batches (EOP set, PRE set, FLG=0 PACKED, NREG=3, REGS `0x412`).
* Tag PRIM: `0x27c` (2,971) or `0x26c` (94; FGE clear), same two classes as the
  WorldStrip tags. Tag word w is `0.0` (float), which becomes the clipped fan's
  tag word.
* 29 batches (28 NUM-21, 1 NUM-51) carry the material header immediately in
  front of the tag; the remaining 65 headers in chain 64 (93 NUM-4 + 1 NUM-5
  total) re-arm material state elsewhere in the chain — the same mid-run
  header-switching behaviour `cell-vif-chains.md` documents for FLD.
* Batches cluster in same-imm runs (quad runs of 4–9+ alternate with
  WorldStrip runs inside the same chain 64).

So ACTION's V3 payloads are **not** uniform: a 4-vertex WorldStrip batch is
`NUM=20 @TOPS+1` followed by `MSCALF 8`, while a 4-vertex ObjectStrip batch is
`NUM=21 @TOPS+1` (extra origin vector) followed by `MSCALF 6`. Only the
following MSCALF immediate (or the tag) disambiguates them.

## Record layout (PROVEN data; lane ranges over the whole corpus)

`UNPACK V3-32` writes three lanes per qword. Vector 0 (at `TOP+1`) is the
object origin; vectors `1 + 5k .. 5 + 5k` are vertex k's record. This is
exactly the entry-3 contract in [`vu1-microcode.md`](../vu1-microcode.md)
("Record (5 qwords): position, colour set A (rgb), B (x = alpha), C (rgb),
(s, t, alpha add, –); records start at TOP+2 with TOP+1 = the object's
origin").

NUM=21 quad batches (3,049 batches / 12,196 vertex records):

| qword | contents | lane ranges observed |
|---|---|---|
| 0 (origin) | world `(x, height, y)`, same frame as the chain's WorldStrip positions | x 5.37..1599, h 9..410, y 180..1576 |
| 1 | local position `(x, height, y)` | x ±110.6 (never 0), h 0..234 (≈50 % exactly 0), y −10..0 (99.9 % 0) |
| 2 | colour set A, rgb, 0–255-scale floats | 30.4..200 |
| 3 | colour set B; **x = alpha 0..1**, yz = colour | x 0..1, yz 30.4..200 |
| 4 | colour set C, rgb | 20..255 |
| 5 | `(s, t, alphaAdd, –)` | s,t ∈ [0,1] with wrapping outliers to −8/7; the four corner UVs (0,0),(1,0),(0,1),(1,1) cover 11,874 of 12,196 records; alphaAdd 0..1 (~97.8 %), rare values up to ~266 |

All payload words are IEEE floats (no integer/index mixes); the fourth lane of
every record qword is never written and never read by the program (the VU uses
`LQ.xyz`).

NUM=51 ribbon batches (16, all A06): origin `(360..441, −60.57..0, 360..440)`
— eight origins at height 0, eight at −60.57 — with local positions
`x ∈ {−1..1}` (never 0), height 0..60.57, depth always 0: a thin vertical
10-vertex zigzag ribbon, width 2, height ≈ 60.6, alternating above/below the
ground reference. Colours 0..183 with some zero lanes, alpha (B.x) = 1.0 and
alphaAdd = 1.0 on every vertex, UVs within [0,1].

### Instancing shape (PROVEN counts)

Only **100 distinct whole-record-set payloads** exist across the 3,049 quads
(2–29 per file; 8 record-sets shared verbatim by A09/A16/A20), while origins
are almost all distinct (A02: 765/765 unique; A17: 308 distinct of 310). One
batch = one placed instance drawn from a small per-file palette of quad
geometries. Local positions say the quads stand upright in the object's
local x–height plane (≈ half the vertices grounded), textured full-surface
(corner UVs) — consistent with fixed-orientation vertical planes such as
bushes/trees/signs/event props [INFERRED: exact asset identity OPEN]. The A06
ribbons are a distinct prop class; what they depict is OPEN (thin 60-unit
column, eight instances hanging below the reference plane, eight standing).

## The ObjectStrip microprogram (slot 3, code `0x03E8`)

Static read of `0x03E8..0x720` (the shared clipper begins at `0x0720`); the
instruction sequence matches the capture-verified HLE line for line:

* **Prologue** — `XTOP` (VI1 = TOP); `LQ VF31, 26(VI0)` (batch offset VU26);
  origin `VF21 ← [TOP+1]` then `VF21 += VU26.xyzw`; fog set `VF12 = VF10 + 0`;
  record pointer `VI3 = TOP+2`; header pointer `VI13 = TOP+0x192`; strip tag
  `VF30 ← [TOP+0]` with `VF30.y ← VF30.w` (the clipped fan's tag word);
  header-NLOOP test via `ILWR` from `[VI13]` with `IBNE` — a zero NLOOP clears
  VI13 so the header is never kicked (the "header once per re-arm" protocol);
  origin clip-transformed: `VF8 = VF13–16 · (origin + VU26.xyz)`.
* **Per-vertex loop** (hand-scheduled, software-pipelined) — reads the record
  with `LQ.xyz` at `0(VI3)` (position), `1` (colour A), `2` (colour B), `3`
  (colour C), `4` (st), then `IADDIU VI3, VI3, 5` — **stride 5 qwords**; clip
  position = `VF5·x + VF6·y + VF7·z + VF8` (object rotation × local + origin);
  screen = `VF1–4 · clip`; `DIV Q, 1, w`; colour blend by the VF9 weights with
  `A.w := B.x` and `C.w := VU26.w + st.z`; alpha ×= the VF12 fog fade (entry 3
  clamps nothing, so alphaAdd > 1 wraps in the GS's 8 bits); outputs
  `FTOI0` RGBA and `FTOI4` ST/XYZF2 (`MULq`, `FTOI4`, `FTOI0` present) stored
  as the PACKED `ST, RGBAQ, XYZF2` triple into the output area (tag copy at
  `out = TOP+2+250`, per-vertex triples following).
* **Clipping** — after the loop, a `FCAND`/`IBNE` cascade dispatches via `BAL`
  into the shared clipper helpers around `0x720+` (guard-band Sutherland–
  Hodgman, fan packets, ADC), as documented for entries 3/4/6.
* **Epilogue** — `XGKICK VI13` (batch header, only if its NLOOP was non-zero),
  `XGKICK VI2` (the strip packet), `ISWR` clears the header's NLOOP word.

Register summary: inputs VF1–4 (clip→screen), VF5–7 (object rotation;
SetupClipped loads VU16–18 × world), VF8 (origin in clip space, computed here),
VF9.xyz (colour-set weights), VF10/VF11 (fog sets, selected into VF12),
VF13–16 (world→clip), VU26.xyzw (batch offset + alpha base); data reads
`TOP+0/1/2..`; output `TOP+2+250..`; header `TOP+0x192` of the current
half-buffer.

## Per-file usage (PROVEN)

Chain 64 = directory entry 64 (relative to `offA`); "hdr" = `V3 @TOPS+0x192`
material-header uploads inside chain 64; "types" = distinct whole-record-set
payloads in that file.

| file | `offA` | dir64 | chain 64 len | MSCALF 6 | NUM=21 | NUM=51 | MSCALF 8 (same chain) | hdr | types |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| A02 | 0x6a800 | 0x2855e0 | 0x347f0 | 765 | 765 | 0 | 2 | 5 | 29 |
| A04 | 0x93800 | 0x371790 | 0x1ece0 | 233 | 233 | 0 | 174 | 16 | 20 |
| A05 | 0x27800 | 0x129460 | 0x1f4e0 | 9 | 9 | 0 | 103 | 4 | 3 |
| A06 | 0x16000 | 0x21b80 | 0x2850 | 16 | 0 | 16 | 0 | 1 | 2 |
| A09 | 0x7d000 | 0xe2bc0 | 0x28f60 | 181 | 181 | 0 | 232 | 10 | 8 |
| A11 | 0x52000 | 0x9e6c0 | 0x5d400 | 1,175 | 1,175 | 0 | 118 | 8 | 16 |
| A13 | 0x2e000 | 0x6fdd0 | 0xd8d0 | 15 | 15 | 0 | 168 | 9 | 4 |
| A16 | 0x71800 | 0xde2a0 | 0x2a050 | 180 | 180 | 0 | 243 | 10 | 8 |
| A17 | 0x76800 | 0x27c6f0 | 0x2dc00 | 310 | 310 | 0 | 256 | 19 | 18 |
| A20 | 0x6b800 | 0xdc150 | 0x2a0a0 | 181 | 181 | 0 | 249 | 12 | 8 |
| **total** | | | | **3,065** | **3,049** | **16** | 1,545 | 94 | 100 global |

A00, A01, A03, A07, A08, A10, A12, A14, A15, A18, A19 contain none. Totals:
12,356 ObjectStrip vertices = 3,049 quads (6,098 triangles) + 16 ribbons
(128 triangles). The other 11 files and all 64 terrain chains use WorldStrip
only. The counts reproduce `action-data-status.md`'s 3,049 + 16 exactly; the
"21 vs 51" split is not a count pattern but **n = 4 vs n = 10 vertex strips**
with the extra origin vector making `NUM = 1 + 5n`.

Material headers in chain 64 decode as the same TEX0 family as FLD
(`cell-vif-chains.md` bit layout): PSMT4/PSMT8, TBW=2, dimensions
256²..1024×4096, TBP0 `0x309e..0x36b6`, paired CLUT (`cbp` within `tbp+0x40`).
These page numbers live in a different GS-bank region than FLD's
`0x285e + digit·0x420` family, and ACTION has no FLD-grammar uploads to resolve
them against [OPEN: ACTION texture staging].

## Native parser guidance

Branch on the **MSCALF immediate**, never on NUM alone:

1. Parse each chain as a VIF stream (STCYCL 0x0101; 32-bit UNPACKs only; the
   five-command corpus grammar, plus `@TOPS+0x192` headers).
2. Keep the pending batch = the UNPACKs since the last MSCALF/FLUSH. On
   `MSCALF imm`:
   * `imm == 8` (WorldStrip): require `V4-32(1)@TOPS+0` then
     `V3-32(5n)@TOPS+1`; n = NUM/5 60-byte vertex records from `TOP+1`; world
     positions (`+ VU26.xyz`).
   * `imm == 6` (ObjectStrip): require `V4-32(1)@TOPS+0` then
     `V3-32(1+5n)@TOPS+1`; qword 0 = object origin (world position; the batch
     offset is added by the program), records of 5 qwords from the next qword;
     n must equal the tag NLOOP (holds 3,065/3,065). Emit an n-vertex strip.
   * anything else: none exist in ACTION; treat as a parse error rather than
     guessing.
3. Record layout: WorldStrip record = position/colA/colB/colC/st with world
   position; ObjectStrip record = identical 5-qword shape but local position,
   and one extra leading origin qword in the same V3 payload.
4. Output/PRIM side: same tag handling as WorldStrip (NLOOP=n, `0x412` regs);
   PRIM `0x27c`/`0x26c`; clipped fans take the strip tag's w word (0.0 here).
   If reproducing VU output bytes, remember entry 3 applies no RGB/alpha
   clamps (alphaAdd > 1 wraps) and its STQ.w is carried state (VF27.w).
5. For offline geometry conversion: world quad corners = R·local + origin,
   with R = the runtime object rotation fed to SetupClipped — **not stored in
   the file** [OPEN: rotation source]. Origin + local positions still give the
   correct placement frame and sizes; textures come from the most recent
   `@TOPS+0x192` header (TEX0/TEX1/CLAMP decode as in `cell-vif-chains.md`).

## Verdict

| claim | status |
|---|---|
| `MSCALF 6` dispatches to ObjectStrip (slot 3, `0x03E8`), not SkyStrip | PROVEN (jump-table bytes; both VU docs) |
| every `MSCALF 6` batch = `[V4(1)@TOPS+0][V3(1+5n)@TOPS+1]`, n ∈ {4,10} = tag NLOOP | PROVEN (3,065/3,065, no exceptions) |
| payload = origin + 5-qword records (pos, colA, colB(x=alpha), colC, st); all-float, 4th lane unused; stride 5 in the VU | PROVEN (data + program reads `LQ.xyz`, `VI3 += 5`) |
| program semantics: R·local + origin, world→clip, fog-faded 3-set colour blend, guard-band clipping, header protocol | PROVEN (disassembly ingredients + capture-verified HLE) |
| quads = upright full-texture local-space planes (event props); ribbons = thin vertical strips | shape PROVEN, asset identity INFERRED/OPEN |
| object rotation source for offline conversion | OPEN (not in file; runtime SetupClipped state) |
| ACTION texture pages for these headers resolvable to file data | OPEN (no ACTION upload grammar yet) |
