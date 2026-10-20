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

## Step 2d done (2026-10-20): model streams live — DMA chain walk fixed, mscals parity

The boot VIF1 stream was not reaching the interpreter (mscals=0 native vs 25,080
oracle). Root causes, both fixed:

1. `consumeChannel` (guest_libs_dma.cpp) now mirrors the oracle chain walk
   (ps2_memory.cpp:1540-1685): tag ids 0 REFE / 1 CNT / 2 REF / 3-4 CALL-class /
   5 CALL(ASP) / 6 RET / 7 END, payload placement per id, CHCR.TTE prefixing the
   tag's upper 64 bits into the VIF stream as two codes, TIE+tag IRQ, and
   MADR/ASR0/ASR1 write-back; per-chain one concatenated `chainBuf` handed to
   the new `vif1Consume(data, sizeBytes)` entry (matches processVIF1Data).
   The old walk followed REFE addr fields as next-tag links and decoded the
   static model blob — all 1.37M census MSCALs were phantoms.
2. Transliteration bug class: 12 stores across 11 builder TUs (all reading the
   ring pointer global 0x3356ec: 002263b8/480/540/6e8/7a8/c28/d58/e88/f70,
   00227048, 00227178×2) were emitted as `*(uint32_t*)G((uint32_t)*(uint32_t*)G(
   (uint32_t)*(uint32_t*)G(0x3356ec) + 0*4))` — a double dereference (host
   codegen loads slot+0 and stores to that value), so the pseudocode's
   `*vifPacketPtr = X` never wrote the CALL/END tag low dword (tq/id). Fixed to
   the single-deref form used by the +4/+8/+12 stores. The game's per-frame
   chain is CALL tags (id=3, tq=qword count of the model block, tag-hi
   FLUSH+DIRECT) at 0x1750bf0/c00/c10 + END, kicked via TADR + CHCR 0x145.

Boot@1801 census (RT_VIF_HIST=1): segs=1801 mscals=25080 (== oracle probe to
the unit) unpackWrites=1,170,345 directQw=92,945, vu1 runs=25080 unported=0;
the `fault 0x10000002` class (garbage pointers from the double-deref stores)
is gone. RAM@1800 vs /tmp/eerm_o1800.bin: 1453 B total (was 1474).

## Step 3a done (2026-10-20): GIF front end decodes XGKICK + GIF channel streams

`guest_gif.{h,cpp}` decodes GIF tag streams with the oracle front end's
semantics (gs_frontend.cpp:919-993 walk + `writeRegisterPacked` :1144-1260):
NLOOP lo 14:0, FLG lo 59:58, NREG lo 63:60 (0=16), PRE lo 46 with A+D prim
57:47, odd-N REGLIST pads 8; packed data is 16 B per register, REGLIST 8 B
with the kick bit in the 64-bit value; XYZ(F)2 kick = ADK hi bit 47 clear
(case 0x04 unpacks x@lo 15:0, y@lo 47:32, z@hi 27:4, fog@hi 43:36 — the
game's own quad layout, not the textbook one). FLG 3 drops the stream.
0x0E = GIF DIRECT: reg addr in hi 7:0, value in lo — how the batch headers
carry TEX0/CLAMP writes. Wiring: `vu1SetKick(&gifSink)` (XGKICK), GIF
channel (0x1000A000) chain payload concatenated like VIF1 (payload carries
its GIF tags; oracle prepends tag-hi only for VIF channels,
ps2_memory.cpp:1557-1560) and normal-mode payload.

Boot@1801 census (RT_VIF_HIST=1): packed tags 29122 (pre 27747), reglist
1368, image 96 (imageB=837,664 texture-upload bytes), verts 285,450,
kicks 49,830 all TRI_STRIP(4); regs 0/1/2/4 = 27747/285450/285450/285450,
6 and 8 (TEX0/CLAMP via reglists) 1368 each, E (DIRECT) 20911. RAM@1800
with the sink wired is byte-identical to the pre-sink dump — decode-only,
no boot perturbation. `gif_test` (tests/native_guest) pins tag/loop/kick/
image/reserved-FLG semantics.

Next (3b/3c): GS register state (TEX0/CLAMP/FRAME/ZBUF/TEST + IMAGE writes
into a GS VRAM model), draw batches with decoded vertices, feed the W2
preview renderer, then title/race/town scanout goldens per the contract
above.

### Step 3a addendum: DIRECT-target census = the boot register surface (gs_types.h ids)

Histogramming DIRECT (0x0E) targets at boot@1801: UV 5472, XYZ2 5472 (the
2D pass writes sprite verts through DIRECT pairs), TEX0_1 1368, TEX1_1 1368,
CLAMP_1 4104, ALPHA_1 1368, TEST_1 1368 (per-2D-batch setup: 1368 = the
reglist tag count), TEXFLUSH 7, and BITBLTBUF/TRXPOS/TRXREG/TRXDIR 96 each —
the game sets the BITBLT regs via DIRECT right before the 96 IMAGE-format
data tags: that is the texture-upload path (837,664 B at boot). FRAME/ZBUF
never appear via DIRECT: framebuffer switching must arrive another way —
scope for 3b (PATH3 A+D or reglist tag path in the oracle walk). This is the
exact GS state surface the native renderer sink must model.

## Step 3b: GS device model (guest_gs) + the frame-switch route

`guest_gs.{h,cpp}` is the native GS device: a raw register file, the draw-context
state the game writes, a 4 MiB VRAM with the oracle's page-addressing (layout
table DATA transliterated from ps2_gs_memory.cpp:45-205; PageId/BlockId/
ColumnId arithmetic inlined), and the vertex queue/batch pipeline. Citations:
gs_frontend.cpp writeRegisterUnlocked (register switch incl. BITBLT/TRX/
BeginTransfer), writeRegisterPacked (packed quad decode incl. XYZF3/XYZ3
no-kick cases), vertexKick (needed/submit/trim), processImageData, and
GSCpuBackend::UploadImage (host->local pixel copy per PSM). REGLIST entries
route to gsWriteRegister exactly as the oracle walk does.

gifSink now routes PRE-prim, packed, reglist and IMAGE payloads into it.
The game's fog word (dword3 = ftoi4(fog)|flag<<15) carries a per-vertex flag
in the ADK bit position: boot shows only two words, 0x8FF0 (set, 235,620) and
0x0FF0 (clear, 49,830). The oracle front end treats bit47 clear as the draw
kick — and with that, its front end itself submits only 2,736 batches at boot
(55,302 draw kicks, 52,566 below-needed): the game's XGKICK 3D geometry does
NOT flow through the front end's batch windows. In the oracle those strips
render as native draws (hwGS) or via the packet mirror (parallel-gs, which
decodes PATH1 itself — the pgs goldens' path). So the native renderer feed
must consume the full vertex stream (gsStream: every XYZ(F) write with prim +
kick flag, 290,922 at boot = 285,450 packed + 5,472 DIRECT 2D verts), the
register file and VRAM — not only the batched triangles.

Frame/ZBUF switching (the "never via DIRECT" puzzle) resolved: the game calls
sceGsSwapDBuffDc once per vblank (census 1799) plus sceGsSetDefDBuffDc twice
and sceGsResetGraph once. In the ELF these are the game's own libgs functions:
SwapDBuffDc(0x26f738) = sceGsPutDispEnv(env+which*0x28) +
sceGsPutDrawEnv(env+{0x50,0x1c0}) — 8 {data,command} pairs each, applied to
the GS. SetDefDBuffDc(0x26f450) seeds the whole 0x330-byte gsDBuffDc struct
(disp[2]@0, giftag0@0x50, draw01@0x60, draw02@0xE0, clear0@0x160, giftag1@
0x1C0, draw11@0x1D0, draw12@0x250, clear1@0x2D0) via sceGsSetDefDispEnv/
DrawEnv/DrawEnv2/Clear + a giftag patch + zbuf-addr patch through
sceGszbufaddr and the sceGsGetEnv static. The native env struct at 0x335fb0
is zero where the oracle's holds the seeded values (~124 B — part of the
0x330000 residual in the W12 RAM parity ledger). Native SwapDBuffDc now
applies the struct (gsSetDisplayEnv + both draw-env pair sets, frame clear
seam via gsClearFrameData); the seeding chain transliteration is the next
3b-2 increment and should close that residual AND land FRAME/ZBUF
(census wreg 0x4C/0x4E) for the renderer.

Boot census after 3b (RT_VIF_HIST): gs stream 290,922 verts, 2,736 batches,
96/96 IMAGE uploads completed (trxdir=0 only; 829,088 px, 965,280 vram
writes), display env still zero (chain not ported yet). RAM@1800 stays
byte-identical to the pre-sink dump — the model is a pure consumer.
`gs_test` pins CT32 + P4 upload addressing, the strip queue/trim/ADK batch
pipeline and the raw reg file.

Next (3b-2/3c): transliterate the game's env-seed chain (0x26f450 +
0x26f798/0x26fb90/0x26ff68/0x26fd78/0x26fac8 + sceGsGetEnv) so SwapDBuffDc
carries real FRAME/ZBUF/DISPFB per vblank, then feed gsStream+regs+VRAM to
the W2 preview renderer and start the scanout goldens.

## 3b-2 landed: env seed chain + FB clear (2026-10-20)

sceGsSetDefDBuffDc's native stub now seeds the whole 0x330 struct, and
sceGsSwapDBuffDc applies it fully. Decision realized: the SHIPPED stub
bodies mirror the oracle HLE seeders (Kernel/Stubs/GS.cpp:916-989 Dc seed,
1182-1240 swap chain, Helpers/Support.h:1597-1684 encoders + seedGsDrawEnv
1/2 at 1911/1930, seedGsClearPacket GS.cpp:36, hasSeededGsClearPacket GS.
cpp:63, applyGsClearPacket GS.cpp:155), not the game's own pseudocode chain
(build/decomp/fn/0026f450.c et al. write pmode=0x66 — the oracle HLE
intercepted those calls, so the goldens' ground truth is the oracle seed:
pmode 0x8007, smode2 from GParam, display=makeLibgraphDisplay(636,32,w,h)).
The game's own chain pseudocode stays the reference for the W14 re-align
question (host stub vs compiled game libgs).

Ground truth decoded from the oracle dump @0x335fb0: game called ResetGraph
(0, interlace=1, omode=2, ffmode=1) and SetDefDBuffDc(0x335fb0, CT32, 640,
224, ztest=2, zpsm=1, clear=on). ZBUF zbp=140 = sceGszbufaddr's v0 product
(10x7 blocks) x2 under the scratch GParam test (GS.cpp:1398 reads scratch
+0x100 as u64: GParam bytes 01 02 01 12 != 1 under the 0xFFFF0000FFFF mask);
native ResetGraph now writes those 4 GParam bytes to scratchpad 0x100
(writeGsGParamToScratch mirror). FRAME fbp goes through framePageBaseToBlock
(<< 5, ps2_gs_common.h:42) exactly as the oracle's ClearFramebuffer
(gs_cpu_backend.cpp:1757): scissor-box fill per PSM (CT32/CT24 u32, CT16/
CT16S 5-5-5|A>=0x40), FBMSK read-merge via gsReadVramPsm, FBA-up forces
alpha |0x80. applyGsClearPacket's markFrameStart is the model's frame
boundary (gsClearFrameData moved there).

Boot census after 3b-2: swapDc=1799 all apply draw pairs (wreg 0x4c/0x4e/
0x18/0x40 x3598, ctx2 0x48 x3597), clears=1799, one sprite batch per swap
(batchesTotal 4535, stream 294520 = +3598 XYZ2), vramWrites 258.9M.
RAM@1800 vs oracle: 1453 -> 1187 diff bytes; the 0x335fb0 struct is now
byte-identical (0x330000 residual 286 -> 20, the rest are game-state vars
0x335690/0x335900/0x33592c/0x33593c). gs_test pins the seed bytes
(pmode/dispfb/display/zbuf/giftag/clear-packet) and the FB clear fills
incl. FBMSK merge + page-base frame 2.

Remaining W12 residual @1800: scratchpad 491 B (scattered game-lib scratch
writes, oracle zeroes at @1800), tail 0x1ff0000 291 B, 0x1770000 265 B
(0x177e6d0-7xx block oracle-nonzero), 0x1780000 49, game vars above,
0x1f10000 27, 0x2e99a8 8 (-1 sentinel vs oracle 01..02), 0x29a844 3
(oracle holds pointer 0x33aaf0), 0x1750000 15, exception page 18.

## Step 3c-1 (2026-10-20): scanout readback + dump seam — first pixel parity

`gsScanoutRgba` (guest_gs.cpp): applied display env -> RGBA8. DISPFB
{fbp0-8 (<< 5 blocks), fbw9-14, psm15-19, dbx32-42, dby43-53} + DISPLAY
{magh23-26, magv27-28 (2-bit mask: a 0x3F mask leaked dw bits -> magv 33,
found by the seam, pinned in gs_test)}; source rect (dw+1)/(magh+1) x
(dh+1)/(magv+1); CT32/CT24/CT16(S) decode, GS alpha 0..128 -> 0..255.
Seam: `RT_NATIVE_GS_DUMP="vb:path,..."` in rt-app writes 'RGBS' + u32 w,h +
RGBA8. Oracle side: test socket `{"cmd":"frame","path":F}` writes RAW
1280x896 RGBA (not PNG despite the .png name; inline "-" is base64).
Multi-entry `vb:path` parsing shared in parseVbPaths() — the old inline
lambdas left p on the comma so strtoull returned 0 and EVERY 2nd+ entry
fired at vblank 0 (the HANDOFF5 "RT_RAM_DUMP landmine" was this bug; all
multi-slice conclusions were void; fixed + re-verified: single run dumping
1400/1550/1600/1665/1750/1800 all clean, 1187-1196 vs oracle).

Boot scanout vs oracle frames (native vb=N <-> oracle park N-1, lockstep
socket): vb5 = PIXEL-IDENTICAL (mean 0.0 max 0 — boot black). vb60: native
shows the single buffer the last swap applied (FB1); the oracle's 448-line
scanout composites both buffer fields (top=FB0 black, bot=FB1+game draws)
— graders must compare per-field. vb1700/1798 attract: frac>16 0.994,
native = clear fills only (expected pre-rasterizer gap). Next 3c steps:
sprite rasterization (boot logos), then the PATH1 strip feed.

## Step 3c-2 (2026-10-20): sprite rasterizer + pixel writes + TRX — content parity

GSCpuBackend transliterated into guest_gs.cpp with citations: writePixel
(TEST/Z/alpha-blend/fog/fbwrite as gs_cpu_backend.cpp:913), sampleTexture/
samplePoint + wrap/CLD dedup/lookupClut (:1062-1230), TRX model
beginTransfer/uploadImage/completeTransfer (:1533-1660 — DIR=0 at BITBLTBUF
activates Host->Local, TRXREG rrw/rrh, TRXPOS dsax/dsay). Loader burst
carrier: VIF1 DIRECT -> gifPath2Direct -> GIF Path2 (guest_gif carry:
partial packets are not consumed; guest_libs_dma hook wiring).

Root cause of loader content divergence (found by otap/ntap tap probes at
(170-175,166-171) of tbp=7566, olut/nlut at lookup, oclut-dump/nclut-dump
at every cbp=8142 load): the oracle rasterizes a completed primitive
SYNCHRONOUSLY at its last vertex kick; native queued batches and drew them
at the vblank drain, so a LATER TEX0 write in the same burst reloaded
g_clut under an earlier draw (CLD=1 every bg swap) — every loader draw
after the first in a burst sampled the last-loaded CLUT. Fix: draw sprite
batches inline at the kick (drawSpriteNative at vertexKick; gsRenderBatches
skips them). Verified: taps byte-identical (u,v,index,entry), CLUT loads
identical at every vblank, TRX arms/completions identical (CLUT 256 px and
P8 image 147456 px both done at vb16-17 on both sides — the earlier
"upload lag ~10-25 vb" reading was a stale dump-file-name artifact, void).

Grading (native scanout RGBS vs oracle test-socket `frame` raw RGBA —
field-accurate, same DISPLAY FB; the 1280x896 pgs composite interleaves
both fields so its halves are not single-field refs): bit-identical
(0 px >16) at N in {580,590,640,650} for native vb=N vs oracle park N-1;
vb300-era VRAM fb diffs decayed 28508 -> 21819 -> 3933 -> 0 windows as the
pre-fix history flushed through the fade-blend contraction.

Residual (deferred, W13 remainder): a game-state phase drift starts around
vb~550 — oracle processes 9 CLEARs fewer than native by vb650 (both count
per-CLEAR-packet: native g_clears 649/650 vs oracle g_wpSeq 641/650) —
scanout shows a 1436-2221 px band (x210-454 y136-148, attract title-text
fade) at N in {570,600,610,620,630} and 0 at {580,590,640,650}. Not a GS
defect: the guest itself emits different per-vblank content (candidate:
the W12 RAM residual blocks 0x177e6d0-7xx / memory-card HLE answers). Also
open: the 21 boot DIR=0 chains (DBP=4 C32 640x32) native misses.
Next: 3c-3 PATH1 triangle/strip/line feed (oracle DrawTriangle :1325,
DrawLine :1443; ~49830 kickprim-4 targets; XGKICK qword Z/fog hi-words
diverge natively at the sink).

## Step 3c-3 (2026-10-20): triangle/strip/fan/line/point rasterizers

GSCpuBackend::DrawTriangle (gs_cpu_backend.cpp:1325, barycentric coverage,
winding-normalized weights, kEdgeEpsilon 1e-4), DrawLine (:1443, Bresenham
with linear colour/z/fog, never textured) and the POINT case (:882)
transliterated into guest_gs.cpp; expressions copied one for one so C++
promotion rules keep the float intermediates bit-identical (float DDA into
double z; z+0.5 rounding on triangles only). Drawn inline at the kick
per the 3c-2 contract; dispatch mirrors RenderBatch (:869); the batch
queue trim (strip/fan windows) already existed from 3b.

gs_test pins (hand-verified coverage): right triangle (10,10)-(40,10)-(10,40)
covers exactly 465 px — the 30 px hypotenuse passes through pixel centres
with x+y=49, drawn at w=0 (>= -eps); flat colour = v2 (TRI) / v1 (LINE);
collinear denom<0.001 draws nothing; LINESTRIP 21+11 px; POINT 1 px; IIP
centroid r ~ 85. All green; boot scanout stays bit-identical at
{580,590,640,650} while native now draws the XGKICK stream — the hidden
loader-window 3D never leaks into scanout even though the two models
draw different hidden contents.

XGKICK qword divergence (the open item from 3c-2) is root-caused and is
NOT in VU1 or the GS feed: our transliterated SetupLit+LitStrip replay
6000 boot-scene oracle runs byte-exactly including full input data memory
(vu1strips_test with RT_VU1_CAPTURE=/tmp/boot_lit.vu1cap; oracle
RT_VU1_CAPTURE writes VU1CAP01: code image + per-run state in/out, 16KB
data in/out, expected XGKICK packets; RT_VU1_CAPTURE_PCS filters entries).
The per-run VIF1 UNPACK write streams to VU1 qwords 0..19 align vector
for vector, in order, with identical opcodes/imm/NUM — but the payload
values differ from the first camera write onward (native camera w-columns
zero + normalized rows; oracle viewport-scaled projection). The game's
EE-side attract-scene matrix math diverges upstream -> moved to the W12
game-state-drift line (candidates: 0x177e6d0-7xx RAM residuals,
memory-card HLE answers, EE round-to-zero float in decompiled routines).

Dev-loop gotcha: the oracle game dylib compiles its VU1 lean code against
the BUNDLED kit (RoadTrip.app/Contents/Resources/sdk), not the source
tree; `cmake --build --target RoadTrip` does not refresh it (rt_bundle is
an ALL target only in full builds). Kit/header changes need
`--target rt_bundle` (or a full build) plus RT_REBUILD_GAME=1.

Next: W12 boot-phase drift fix, then scanout parity on attract frames
with visible 3D (~vb 900+), pgs golden re-grade + divergence-policy
appendix.
