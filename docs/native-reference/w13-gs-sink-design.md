# W3 gs-sink design (native render chain)

Decision record for the `consumeChannel()` seam in `src/native/guest/guest_libs_dma.cpp`
and what W3 builds behind it. W12 ships the seam only (walk-and-count); this doc pins
the contract so W3 (scanout goldens) has no re-design debt.

## Why a native chain (hard rule)

No PS2Runtime, no VIF1/GIF worker from ps2xRuntime, no recompiler output may link into
the native app. The guest's VIF1 chains are pure data in kRam; the native port
interprets them itself and feeds the app's own Vulkan renderer
(`rt::native::preview`, already linked into rt-app). The oracle (ps2xRuntime + paraLLEl-GS
or hw GS) stays comparison-only: it defines the goldens, never the shipped path.

## Seam contract

`consumeChannel(channelBase, chcr, madr, qwc, tadr)` currently walks VIF1 chain tags
(qwc|id|next at each tag qword) and discards. W3 replaces the discard with:

```cpp
void vif1Consume(uint32_t segAddr, uint32_t qwcWords);   // src/native/guest/guest_vif1.{h,cpp}
```

called per chain segment with the payload start (segAddr+16) and word count. Normal
(non-chain) submissions deliver (madr, qwc) the same way. The walk order IS the DMA
order; completion (`kFireDmacCause`) keeps firing after delivery, unchanged.

## VIF1 subset

Survey first (W3 step 1): instrument the walker to histogram VIF1 code words across a
full boot+attract+race run of the NATIVE app (no oracle needed — chains are in kRam).
Expected set from the oracle's VIF1 worker behavior on this game: NOP, DIRECT,
OFFSET/BASE, MSCALf/MSCALt/MSCAL (entry programs 2,3,4,5,6 — the strip renderer set
documented in docs/vu1-microcode.md and docs/native-reference/action-mscalf6.md),
MIN/MAX (FIFO sizing), plus WL/SI sizing bits. Anything outside the histogram stays
walk-discarded with a fault key so parity sees it.

## VU1 programs: port, don't execute

Road Trip's VIF1 code drives a fixed set of VU0/VU1 entry programs (2,3,4,5,6; entry4
also the course carousel). Per the decomp rule these are transliterated to C++
routines — one per program, semantics from docs/vu1-microcode.md (the shared reference
the recomp's `vu1hle::run` also implements) — not interpreted at runtime. Each routine
takes the recorded VIF1 REG macros + memory refs and emits its GIF packet exactly as
the program does (this mirrors the recomp's own NativeDraw shape: MSCAL at a known
strip entry => registers+vertex block => one host draw, `docs/native-render.md`).

## GIF -> renderer

GIF tags carry PATH1/2/3 packets (ADPCM, PATH2/3 = vertex prim: RGBAQ/ST/UV/XYZ(F)2,
FOG, scissor, TEST/FRAME setup). W3 decodes them into the same vertex-primitive record
the W2 preview renderer consumes (ScreenQuad/vertex batches), replacing the state-driven
preview scene with the real packet-driven scene, HUD included.

## Scanout goldens

Oracle reference: `RT_FRAME_DUMP=<dir>` PNGs from the recompile product at pinned
virtual-time vblanks (pgs backend, `RT_PROGRESSIVE_FIELDS=0`, suite-style env — the
same harness that pins `RT_GS_BACKEND=pgs` goldens). Native scanout is captured from
the presenter backbuffer (RT_APP_SNAPSHOT already does this offscreen). Golden pair =
same vblank, 640x448 progressive (W12 already forces half-offset off). Compare with the
regression goldens.py neighborhood metric (both directions, >48/channel off, 3% or
30%-block thresholds) until exactness is argued per scene.

## Order inside W3

1. Histogram + `guest_vif1.cpp` for the surveyed subset (unit-testable: feed captured
   chain bytes, assert emitted draws).
2. Port entry programs 2-6 to C++ (evidence: docs/vu1-microcode.md + captures in
   build/vu1hle-caps; `vu1_replay --hle` stays the oracle for program semantics).
3. Renderer feed replacing preview stand-ins; title, race, town scanout goldens land.

## Survey result (step 1 landed 2026-10-19)

`guest_vif1.{h,cpp}` decodes every VIF1 segment delivered by the CHCR trap over a
full boot+attract (1801 vblanks) with no boot-state change (`vif1_test` pins the
semantics). Census at segs=4000 (`RT_VIF_HIST=1`): MSCALF@VU1-pc 0x20 dominates
(~1.38M calls), MSCALF@0x50 secondary (~29K), MSCAL mostly one-time inits (counts
23-46), MSCNT/MPG/DIRECT present (237 MPG uploads observed). Step 2 targets:
port the entry at 0x20 (per-frame strip path) and 0x50 first; the uploaded micro
programs live in the interpreter's `vu1Code()` after the MPG/chain loaders run —
capture before transliterating (dump hook pending).

## Step 2 grounding (2026-10-19 evening)

`RT_VIF1_CODE_DUMP` landed (committed): the captured 16 KiB image hashes
FNV-1a 64 `0xe2868a5859386847` over the whole image — byte-identical to the
oracle's `Vu1Native::imageHash` (docs/vu1-microcode.md), so the MPG reconstruction
is exact. Jump-table slots decode as documented (0x1268, 0x1438, 0x1620, 0x03e8,
0x0080, 0x1898, 0x1aa0, 0x1318). Naming caution (docs/native-reference/
action-mscalf6.md): MSCALF imm = byte-addr/8, so census `pc 0x20` = slot 2 =
**LitStrip** (code 0x1620, lit two-light+specular strip, unclipped) and `pc 0x50`
= slot 5 = **OffsetStrip** (0x1898, unlit per-vertex offset, unclipped).
Disassembler: build/scratch/w3-step2/vu1dis.py (decoder fields/op tables lifted
from the capture-verified ps2_vu1_exec.inl: upper op = word bits 5-0, dest mask
bits 24-21 bit0=w; lower opHi=(w>>25)&0x7F with Dobie selectors). Entry dumps:
entry2_litstrip.dis (0 unresolved), entry5_offsetstrip.dis. LitStrip opens:
XTOP vi1 (TOP), vi13=TOP+402 (batch header qword — docs confirm), vertex loop
reading LQ @TOP+1.., writes GIF vertex qwords via SQ into the double buffer
(141/571 halves), kicks via XGKICK vi13/vi2, clears the batch-sent flag via
ISWR. Port those two first, entries 3/4/6 when race/town scenes come up.

## Step 2 done (2026-10-19): slot programs ported and capture-verified

`src/native/guest/guest_vu1.{h,cpp}` transliterates the four slot programs
(Init, SetupLit, LitStrip, OffsetStrip) over the interpreter's VIF1 data
memory; blocks cite micro-op ranges of the entry dumps above. Float semantics
are implemented compiler-independently (exact double + single round-toward-
zero + the `.inl` clamps), so no host FE_TOWARDZERO/FMA-contraction reliance.
Verified by `vu1strips_test` (tests/native_guest/test_vu1strips.cpp): replays
the oracle's captures — field_lit 6000/6000 LitStrip records and qr0_rare
2400 OffsetStrip records — with every XGKICK packet byte-exact, the whole
data memory (outside the interpreter's 1006..1023 save-stack scratch) byte-
exact, and the live VF registers + q + vi14 exact. The oracle's own HLE
(ps2xRuntime vu1_hle) is comparison ground truth only, nothing links here.
The seam is live at boot (`vif1SetHooks(&vu1OnMscal, nullptr)` from the DMA
consumer): 1.37M program runs through boot+attract, 828 invocations still
unported (the ~24 direct-PC one-shot programs 0x90..0x1908 and slots
3/4/6/7 — ObjectStrip/WorldStrip/Sky/SetupClipped, race/interior scenes),
and RAM at vblank 1800 is byte-identical to the pre-port build (programs
touch VIF1 memory only, never EE RAM). XGKICK output is counted, not yet
fed to the render chain — step 3 takes it. `RT_VU1_CAPTURE`/`RT_VU1_RECORDS`
select the capture replayed in the test (skip when absent).

## Step 2b scoping (2026-10-19, measured boot+attract census)

Direct-PC one-shot programs actually invoked in boot+attract (per-boot counts
x23/x46 = once per load / once per object): 0x90, 0xa8, 0xb8, 0xd8, 0xe8,
0xf0, 0xf8, 0x110, 0x128, 0x800, 0x868, 0x8a8, 0x8e0, 0xbd0, 0x1060, 0x1070,
0x10a0, 0x10a8, 0x10c8, 0x1120, plus 0x1890/0x1898/0x18b0/0x1908 — the last
four call *into* OffsetStrip (its body starts at 0x1898) bypassing the slot
table, so a dispatcher keyed on program regions (not just slot PCs) covers
them. The 0x90 program is a LitStrip-loop variant that loads the matrices
from data memory inside the loop (LQ f17/18/19/27 + f13..16 at 0x140-0x158);
the 0x800/0x868/0x8a8 programs loop with LQI f21..23 from qword 101/61 and
BAL to shared subroutines in the 0x2f8..0x420 region (never MSCALed — internal
calls). Micro image for this reading: the code blob embedded in
build/vu1hle-caps/*.vu1cap (extracted to build/scratch/w3-step2/code.bin,
same hash), dumps in oneshots1.dis. Transliterate these for the full attract
scanout in step 2b; disassembler branch-offset display has a cosmetic imm11
bug (shows +16344 for a loop-back) — semantics from .inl unchanged.
Correction: the display is simply the raw imm*8 without the interpreter's
pc+8 base (branches are pc-relative: `.inl` 2155
`target=(pc+8+imm*8)&pcMask`) — e.g. the WorldStrip slot at 0x40 reads
`B 0x38` but really targets 0x80.

## Step 2c done (2026-10-19 late): clipped family — slot table complete

ObjectStrip (pc 0x30, body 0x03e8), WorldStrip (pc 0x40, body 0x0080),
SkyStrip (pc 0x60, body 0x1aa0) and SetupClipped (pc 0x70, body 0x1318) are
now ported too: `runColourSetStrip(object)` (one loop for 3/4, colour sets
A/B/C blended by VF9, fog set VF10/VF11 by tag.y bit 1), the six-plane
Sutherland-Hodgman clipper (triangle state in VU61/101 ping-pong, polygon
packets built at VU30 and kicked per triangle, empty EOP tag at VU29 between
them, batch header with the first), and SkyStrip (per-vertex weight cycle
along VU6, tint VU24, winding cull by the cross product's exact sign —
dis6 1bf8/1c18-1c20). Verified byte-exact on all captures including the
clipper paths: qr0_rare 6000/6000 (ObjectStrip 2840, SkyStrip 720,
SetupClipped 40, OffsetStrip 2400) and qr0_world 8000/8000 (WorldStrip);
field_lit unchanged 6000/6000. Boot census unchanged (slots 3/4/6/7 never
fire before attract; races are the first users), RAM@1800 still byte-identical.

Init fidelity fix found reading the body (0x1268-0x1308): VF9..11 load from
VU19..21 (kFogLit), not VU20..22 — the capture set starts mid-scene so Init
was never compared there; vi14 = IADDIU -1024 (only low 10 bits reach
save-stack addressing; 1024 kept as the canonical base).

Remaining unported at boot: only the 828 direct-PC one-shot invocations
(step 2b scope above).
