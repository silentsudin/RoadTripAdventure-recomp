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

## W12 camera-chain fixes (2026-10-20): projection restored, residue found

Boot attract camera block (0x177e6d0, 0x190 B, matrices at +0/+0x40/+0x80/
+C0/+0x100 — caller RunMenuPanelTask 0x208848 passes camera struct 0x29abd0)
diverged from the oracle in three stacked layers, all fixed on-machine-code
evidence (llvm-objdump vaddr direct on build/rom/SLUS_203.98):

1. `allocFrame` 16-byte chunk padding broke guest frame-slot adjacency
   (transliterator's sized gap chunks encode exact offsets; 196 TUs affected).
   buildCamera's projection matrix P = afStack_140[5] + adjacent scalar slots
   (elements 5,8..11,14,15 from DAT_332b2c/332c88/332c8c/332c90/3356e0/3356e4)
   read back as scaled identity -> projection multiply silently lost.
   Fix: 4-byte align only (85e812a).
2. buildCamera jal 0x21f748 ComposeCameraMatrix: delay slot `move $4,$sp`
   sets callee $a0 to the caller slot (Ghidra's 1-arg call was right about
   $a0) but $a1..$a3 carry param_2/3/4 live; transliteration dropped them.
   Flag-SET path CopyVector #1 same clobber: copies param_2 -> auStack_1d0.
3. ComposeCameraMatrix jal 0x21f3bc/0x21f3c8 (vu0MulMatrix): machine args
   are (param_1, param_3, auStack_c0) and (auStack_80, param_3, auStack_80)
   — Ghidra emitted (auStack_c0, param_3) / (auStack_80, param_4, auStack_80)
   ($a1 at jal #2 is still $16 = param_3; no $5 write between 0x21f3b0 and
   the jal). d179d05.

After 1+2+3 the block structure matches the oracle (projected columns, unit
lane-3 rotation terms, homogeneous position column). Residual numeric gap:
the oracle's V converges to identity with position exactly (0,0,50,1) — i.e.
ComposeCameraMatrix reads its own never-written frame locals (auStack_b0/
a0/70/60 as the CopyVector sources feeding BuildCameraViewMatrices vectors)
and sees boot-era residue from the task's earlier deeper call tree (spawn at
counter==0). The game reads uninitialized stack deterministically; bit-exact
attract parity therefore needs the per-task EE stack model (real push/pop
frame discipline per task stack, contents persisting across invocations —
the W13 stack model foreshadowed in guest_shim.h's shortcut comment).
Native bump-arena (reset per vblank) gives different residue.

Tooling: RAM slice pairing (native RT_RAM_DUMP vb:N <-> oracle socket
state_dump chunk=EERM at park N-1, both named N), float-pair search, and
per-page diff live in build/scratch/w12-parity/ (never commit). Note oracle
0x177e700 pair (102400,102400) is P x (0,0,50,1): P[14]=DAT_332c90=2.51661e7
appears at 0x177e708 — exact-value float searches must use tolerant windows,
values wobble by rounding pre-settle.

Grading after fixes: boot scanout bit-identical {580,590,640,650}; closeout
sweep 154/154; det pair race@110 identical; matrix 32/32. Onset phase:
native attract 3D starts vb ~268, oracle ~285-295 (camera animation runs
the same 165-frame RunMenuPanelTask window in both; kill at counter >0xa4
confirmed in native call census).

Next: per-task EE stack model (unblocks attract visible-3D parity ~vb 900+),
then pgs golden re-grade + divergence-policy appendix.

## Stack mirror + boot-phase gate (2026-10-20, pushed 0874c89)

Step 1 (d8accba): FrameGuard(machine_frame_size) as first statement of all
1555 gen TU bodies; kernel per-thread frame region save/restore at every
switchcontext site. Step 2 (6c9fb9b): framesizes.json (1521 fns sized by a
whole-.text llvm-objdump prologue `addiu/daddiu $sp,-N` scan; 295 frameless
fns verified zero-chunk), allocFrame carves chunks upward from the pushed
base (offset-exact, no padding; align fix 85e812a), main-thread stack =
[0x1F80000,0x2000000) top-down from the ELF's declared main stack. Result:
native frames land at the oracle's absolute addresses (unique-nonzero-chunk
search: dominant delta 0), RAM diff down to ~700-800 B/vb.

Machine decode correction (supersedes the a378823 note above for BCM's
cross/normalize jal dsts): a jal's delay slot executes BEFORE the callee
body, so registers set in the delay slot ARE visible to the callee as
arguments. At 0x21f4b8-0x21f4e8 that makes Ghidra's original dst slots
correct (cross1 +0x00, normalize2 +0x30, cross2 +0x20 <- +0x30,+0x10);
native now writes exactly the machine slots. (Compose's tail-copy dst fix
stands — those are plain stores, no jal-arg lag.)

Boot-phase gate (0874c89): the oracle's interpreted IOP boot module loads
consume 9 vblank periods before the game's first game-task spawn (pool
empty through park 9, first active dump pair vb10; native vb2). Gate holds
the boot main thread out of the ready queue until the 10th kVblank tick
(kBootMainThread(entry, gate=true); false for kernel-primitive tests).
Task pool now byte-identical at paired dumps vb14..500 incl. the panel
counter (24@300, 154@430) and the 0x208848->0x208a88 transition@500; boot
scanouts stay bit-identical {580,590,640,650}.

Remaining attract-camera gap is pure residue BYTES in the shared depth-3
stack window (base 0x1fffbe0): the oracle's prologue `sd $16-$23/$ra,
0x40..0xc0($sp)` + `swc1 $f20-$f23` stores write saved-register values into
slots that other frames later read as locals (Compose's CopyVector sources
+0x60/+0x70 read GPR-save bytes; oracle rest-state in1-source =
(0,0,0x3f800000,0) = saved regs holding 1.0f bits). Native never writes
saved slots. Next task: model prologue save-stores generically (values
derivable per function from the machine prologue: $4-$9 -> param_k, simple
`move`/`lui` chains -> constants/params, $ra -> call-site pc via a kernel
link-register shim; older opaque $16-$23/$fN saves stay a known residue
class).

## W12 2026-10-20 late: scanout parity reclassified to GS-chain class (poke evidence)

- Grading set: attract+boot scanouts bit-identical at {580,590,640,650,900,1100,1300,1360,1480};
  DIFF only at transition/card frames {1308-1348,1372-1428,1440-1500-ish}, diff confined to band
  y136-148 x209-454 (2811 px of 143360). Oracle frames there hold a stable orange card strip (X),
  native a garbled variant (Y) with the window phase ~3 vb early and ~12 vb short.
- PAIRING CORRECTION: an oracle state_dump earlier named eerm_o1319 was actually taken at oracle
  vblank ~1465 (chained run() arithmetic), which manufactured a phantom "+133 walker iterations"
  and a phantom page-0x335 live-counter drift. True-pair dump (/tmp/eerm_p1319.bin, parked 1318):
  walker counter DAT_0x335900 matches n/o exactly per vblank (1309 vs 1309); total RAM diff 752 B,
  pages {1fff:397, 177e:196, 1787:49, 177f:28, 1f10:27, 175e:11, small tails}. 0x335 CLEAN.
- Frozen-block probe: oracle camera block 0x177e6d0..0x177e870 is frozen (identical oracle dumps
  vb500/600/1318; native identical 500/1300/1319) — nobody writes it in attract. RT_NATIVE_POKE
  (temp probe in main.cpp stepLogic, splices a file into kRam at a vb; STRIP before commit)
  injected the oracle's exact bytes at vb1300: native scanouts at 1308-1348 unchanged, bit-identical
  to unpoked. => the 0x177e residue is NOT read by card rendering; RAM-value parity is not the
  scanout gate.
- 49 B heap 0x17874e0 region decoded: oracle = HLE thread-control-block array (ids 0x41-0x5f,
  16-byte entries); native = RPC endpoint structs (0x01234567 bind number). Both are oracle-HLE
  internal mirrors; the game-visible task pool is byte-identical. Inert residue.
- CONCLUSION: remaining EE-RAM diffs (stack save slots incl. $16-$23 opaque class, camera block,
  HLE mirror blocks) are proven inert for attract rendering. The card-band X-vs-Y divergence and
  its phase belong to the GS/VIF1 chain side: suspect the W3 residual 21 DIR=0 chains
  (DBP=4 C32 640x32 — band y136-148x4=272-592 GS lines fits a 32-line chain) i.e. chain coverage,
  not RAM mirroring. Next: per-frame draw coverage diff (oracle RT_GS_BATCH_LOG vs native
  gs_batchPrim/stream) over vb 1306-1312.

### Poke-2 (stack page): also inert — the card divergence lives OUTSIDE EE RAM

Poked the oracle's true-pair bytes 0x1fff000.. (+4 KiB, covering the Compose/buildCamera window
with all 397 B of stack residue) into native at vb1305, one vb before native's A->Y flip (1307):
scanouts 1308-1348 stay bit-identical to the UNPOKED native run (21cc/Y then A). Combined with
poke-1 (camera block), every EE-RAM diff byte at vb1304-1319 is experimentally inert for the card
behaviour. Yet the card decision differs (native flips at 1307, oracle at ~1310; content Y vs X):
the state driving it is NOT in the EERM space at all. Candidates, in order of suspicion:
1. SIF/IOP -> EE message arrival TIMING (message content lands identically when it arrives; the
   decision branches on arrived-yet). IOP scheduler + SIF DMA model state is not in EE dumps.
2. MMIO register values fed back to the guest (GS_CSR bit13 FIELD parity via sceGsSyncV poll,
   INTC_STAT races at vblank boundary) -- native models these host-side (EEHW chunk on oracle).
3. VU0/VU1 micro-memory + register state carried across frames (VU0M/VU1M/VU0S/VU1S chunks).
Next discriminator: oracle chunkHashes at park 1304 vs native-side equivalents for EERM(done:
pair diff 752 B residue only), VU0M/VU1M, EEHW, GSFE. Native has no chunk-hash emitter beyond the
EE-RAM FNV (RT_STATE_HASH); a temp emitter for native VU0 micro memory + key MMIO shadow regs is
the cheap probe. The old 3c-2 note "oracle processes 9 fewer CLEARs by vb650" is the same
phenomenon: per-vblank GIF emission differs while EE RAM agrees -- an emission-decision input
outside EERM, not a GS defect.

## W12 2026-10-20 late-3: card-band is a fade-accumulation canvas; composite source selection diverges

RT_GS_BATCH_LOG vs the temp [band] probe (RT_NATIVE_DIRECT_LOG="lo,hi" windows [band] in
guest_gs.cpp kick dispatch + [direct] in guest_vif1 kDirect) over vb1303-1312:

- Draw SCHEDULE identical: card activity windows (474,505)(538,569)...(1242,1273)(1306,1312+) on
  both sides; 64-vb cadence, 34-vb idle gaps; text batch = 16 sprites (oracle n=32 vertices).
- Text PACKETS identical: 16 sprites tbp0=6720 psm=20, UV/color/cbp byte-identical at boot (474),
  steady-A (1272) and post-transition (1307). All texture pages (6720/7566/2240/8690) and the CLUT
  page are byte-static across the transition (native VRAM dumps every 3 vb).
- What differs: the full-screen COMPOSITE copies. Oracle per vb: ONE source page copied to BOTH
  fb pages (even vb: page 2240; odd vb: page 0 = fb-self-echo), second copy fst=0 (often ctx=1);
  plus 7566 overlay + 8690 photo. Native: BOTH pages (0 and 2240) each once per frame. The band is
  a fade-accumulation canvas, so native accumulates ~2x and its band flips 4-6 vb early (1306) and
  garbled; oracle flips 1309-1312. Band fb writes land at fb0 y~138 rows (byte 0x56600 region) and
  the same rows of fb1 (base 0x8C000).
- Poke-interpretation refinement: the card task REWRITES its display lists every frame, so poking
  oracle list bytes at 1305 gets overwritten -> "all RAM diffs inert" does NOT exonerate the ported
  card/fade task gen TU (list WRITER). Discriminator: raw DIRECT qword stream at one card vb —
  one copy-pair per frame in the bytes (native parse defect) vs two (gen TU writer defect).
- Frontier parity agent (openai/gpt-6.1-sol, background) dispatched on exactly this discriminator;
  full handoff + repro commands in build/scratch/w12-parity/HANDOFF-cardband.md.

## W12 2026-10-20 late-4: ROOT CAUSE — ported card-list writer corrupts the TEX2_1 entry

Paired RAM diff native vb1306 (/tmp/ram_n1306.bin) vs oracle EERM parked at vblank 1306
(/tmp/eerm_p1305.bin, run{vblanks:1305} parks at 1306): the attract card text-list pages are
byte-identical EXCEPT ONE QWORD, and the copy-chain page differs in one qword:

- 0x336370 (AD entry value; reg qword 0x336378 = 0x16 = TEX2_1 on both sides):
  game   0x0413780001400000  -> fork TEX2 apply: psm=(v>>20)&0x3F = 0x14 (PSMT4H),
         cbp=(v>>37)&0x3FFF = 7104, bits56-60=4; oracle batch log text state psm=20/cbp=7104 EXACT.
  native 0x0000004000008080  -> the TEXA constant (same value as the 0x336360 reg-0x3b entry):
         psm->0, cbp->2 => text samples page 6720 as RGBA32 => the garbled band, and the
         band-transition timing artifact.
- 0x29a840: oracle 0x0033aaf000000000, native 0 (likely uiBatchFlush other-buffer bookkeeping;
  does it reach GS? RE agent dispatched).
- All other setup AD pairs (TEX0 0x2013780e65321a40 psm=0x13 tbp=6720 cbp=7104, CLAMP, ALPHA,
  TEXA, GIF tag qwords incl. PRE/PRIM 0x156 and NLOOP) are byte-identical. The earlier
  "NLOOP 3 vs 84" reading was an artifact of comparing unpaired vblanks (park1304 predates text).

So the GS sink (PATH2/AD parsing) is EXONERATED for this symptom: identical bytes in, identical
apply out (frontier agent's parser agreement was right). The defect is the ported display-list
WRITER: src/native/guest/gen/00203810.cpp (uiBatchFlush) writes 0x4000008080 at 0x336360+iVar in
two sites (lines 19, 33) where the game writes the TEXA constant AND a COMPUTED TEX2_1 value
(the 0x0413780001400000 shares bytes 0x13/0x78 with the canvas TEX0 qword -> derived from canvas
state with psm forced 0x14). RE subagent dispatched to cite the pseudocode derivation; fix goes
into the gen TU with that citation. test-engineer dispatched to pin the band contract
(oracle band sha table: vb1308 a7ca7a63d3c7a0c7; vb1312..1348 static 3e45c5141106e978).

## W12 2026-10-20 late-5: UB fix landed; band timeline now: swap early + corrupted new-card + early revert

Fix applied (uncommitted): gen/00204310.cpp uiDrawTextCell + gen/002046c0.cpp uiDrawNumberFixed 4th param
uint32_t -> uint64_t. Ground truth: pseudocode `ulong param_4` (00204310.c:5, 002046c0.c:7) and ELF 0x204364
`dsll32 $2,$7,0x18` (full 64-bit a3). `param_4 << 0x38` on uint32 was UB; clang DROPPED the TEX2_1 value store,
leaving the stale TEXA constant at slot 0x336370. Forward decls in build/decomp/guest_decls.inc patched to match
(refresh_decls.py regenerates from defs, so widening survives regen). RE answer: the qword is NOT canvas-derived —
it is `bank<<56 | 0x13780001400000` (TEX2 PSMT4, CBP 0x1BC0, CT16, CSA=bank), emitted by uiSetPaletteBank
@0x2039e8 pattern in 11 UI text fns; bank arg for the card is 4. uiBatchFlush TU 00203810 was ported correctly.
Also RE-proven: 0x29a840..44 oracle bytes are ELF .reginfo ($gp 0x33aaf0) inside PT_LOAD — our loader doesn't copy
them; inert for GS; loader-side follow-up: copy whole first PT_LOAD.

Post-fix band hashes (native): 1300-1304 a7ca7a (= oracle OLD card, byte-exact!), 1308-1336 d274e9 (neither card),
1340-1364 a7ca7a again. Oracle: old through 1308, 3e45c5 NEW card 1312-1348+. So per-draw old-card rendering is now
proven identical; remaining defect is card STATE machinery: swap starts ~2 vb early in native, new-card content
corrupted (d274e9), and reverts to old ~40 vb early. RAM pairing at native1306/oracle1305 now shows only: byte
0x335909 (task pool 0x335900 header, pool qword 0x335908 = n 0x0002070700000000 vs o 0x0002070700000100), the
.reginfo 3 bytes, and 64B diff blocks at pages 0,9,a,e,175e,1773,177e(7),177f(2),1787(3),1f10,1fff (stack) —
0x177e/177f/1787 cluster = prime suspect for the attract card/photo task timer state.
Dispatched (claude-code pool): parity-debugger@opus (band chase owner), reverse-engineer@sonnet (pool byte +9
semantics + card cadence code), bug-tester@sonnet (gates on tree with UB fix), test-engineer@sonnet
(tests/native_parity/test_text_list_tex2_ram.py pinning slot 0x336370 at RAM level), port-worker@haiku
(ub-audit.md across gen corpus for the same pseudocode-ulong/uint32-shift UB class).

## W12 2026-10-20 late-6: RE cadence model + UB batch 2 applied; new pacing symptom

RE (verified against pseudocode): 0x335900 is NOT a task-pool header — it is the 0x160-byte gameFrameCtx
(2nd arg to every task callback). Byte +9 = gsFieldParity = (sceGsSyncV(0)==0) (002003b8.c:52-54). The task
pool is gs_tasks_pool 0x177A0D0, 48x0x30; slot +8 = frames-run, incremented once per WALKER VISIT, i.e. per
main-loop ITERATION, not per vblank. The attract "card" = titlePressStart 0x208A88 blink: text only when
(task+8 & 0x20) && +8 < 0x708, 32 on / 32 off, ends at +8 == 0x718; on-windows 474+64k in vblanks PROVE the
old build paced iterations 1:1 to vblanks. SyncV ground truth (0026e458.c): returns interlace-env ? CSR FIELD
bit : 1; native gen/0026e458.cpp reads CSR as *(uint8_t*) at 0x12001000 — byte read, >>0xd&1 == 0 always
(transliterator MMIO width class, also 0026e250/0026ecb0; oracle pair: native +9 stuck 0 = env interlace 0 or
FIELD never set). The linked sceGsSyncV is the GEN TU (libs stub in guest_libs_gs.cpp is anonymous-namespace,
dead for direct calls). The ELF SyncV copy never parks; iteration parking must come from elsewhere — hunt it.

UB batch 2 APPLIED (pseudocode-cited, built clean): 00215220.cpp:88 (ulong)(uint)<<0x20 lost -> sprite-sheet
stack qword high word; 00221f70/002213e8/00221788x2 (ulong)*(byte)<<0x38 in vifFixedRegs/vifBatchRegs csa
bits -> attract demo 3D model regs; 00277bc0 copysign params widened (was uint32 vs ulong/long; kernel_rem_pio2
trig). Band hashes UNCHANGED (old binary post-batch-1 tree: 1300-1304 a7ca7a, 1308-1336 d274e9, 1340+ a7ca7a)
— these UBs are not the band content fix. NEW SYMPTOM with the rebuilt binary: run ENDS at vblanks=1151
(running=false, no error, faults=0) instead of 1365: consistent with titlePressStart +8 hitting 0x718 at
vb~1151 -> iterations free-running (~1.58/vb) once faithful trig/sprite/VIF data changes load+draw mix. Fixing
iteration parking (1 iteration == 1 vblank like the oracle) is now the primary band-cadence suspect: it gates
the blink windows AND the title->attract-demo transition timing. Registry updated (gameFrameCtx/gsFieldParity/
loadInProgressFlag/titlePressStart+sequencer notes). claude RE+test agents closed; parity-debugger still on it.

## W12 2026-10-20 late-7: CARD BAND CLOSED (rgba5551 decode); +9 confirmed real, timing theory dead

Root cause (parity-debugger, coordinator-applied): `rgba5551To8888` in guest_gs.cpp dropped `<<3` on G/B and put
alpha at bit24 instead of bit31 — oracle gs_cpu_backend.cpp:38-47 expands every channel `<<3` (5→8 bit) and alpha
`<<7`. Patched native is FULL-FRAME byte-identical to the oracle at every vblank 1297..1360 (not just the band).
The late-5/late-6 "early swap / corrupted new-card / pacing" symptoms were ALL artifacts of the mis-decode plus
mislabelled `/tmp/so_o*.raw` frames (old oracle driver advanced 4 labels per 3 vblanks). No GIF-carry or iteration
parking defect exists; the run ending at vb1151 was the same label artifact. Per-vblank oracle table rebuilt
correctly (build/scratch/parity/oracle_per_vb_drive.py, dumps /tmp/cbo/f{1296..1359}.raw): new card parks
1306..1337 → native labels 1308..1336 NEW, 1300/1304/1340..1348 OLD; EXPECTED pinned in
tests/native_parity/test_card_band_attract.py (GREEN). tex2 RAM test green too. UB batch 1+2 stand (page-0x336
diff gone in post-fix pair /tmp/eerm_p1305.bin vs /tmp/ram_n1306b.bin).

+9 (0x335909) CONFIRMED real: oracle toggles per-vblank (0@parks 1140/1170 even, 1@1305 odd); native stuck 0.
MIPS ground truth for gsSyncVSetFlagIntc @0x285460 (LE capstone): zeroes sp+0 flag slot, SetVSyncFlag(a0=sp)
(flagAddr=sp+0, tickAddr=sp+8 — transliterator wrongly used a separate allocFrame(4) slot in gen/00285460.cpp),
writes INTC_STAT(0x1000f000)=4, polls INTC_STAT&4 / flag(sp+0) until either fires, acks INTC_STAT, returns
`ld 8($sp)` = u64 tick. Kernel (EeScheduler.cpp:1378/2056): onVblank sets CSR FIELD 0x2000 from tick&1, flag=1,
u64 m_vsyncTick → tickAddr. CONTRADICTION: (tick>>13)&1 is near-constant while +9 toggles per-vblank → the
late-6 model "+9 = (SyncV(0)==0)" needs re-derivation; also native lacks the poll/ack and CSR FIELD toggle.
gen/00285460.cpp is NOT faithful. parity-debugger dispatched (claude-sonnet-5-5, session kept) with this evidence;
also ranking the 177e/177f/1787 + page clusters and the 3 .reginfo bytes as state-hash blockers vs inert.
test-engineer (gpt-6.1-sol) writing tests/native_parity/test_ram_parity_attract.py: full-RAM diff-set tripwire
(diff set must EQUAL the pinned known set — trips on regressions AND on fixes). TEMP probes stripped
(RT_NATIVE_POKE/RT_NATIVE_DIRECT_LOG gone; GS_DUMP/RAM_DUMP seams stay, w2-13 coverage 83/83). bug-tester
re-running full closeout (expect 154/154) on the integrated tree pre-commit.

late-8 (2026-10-20, +9 CLOSED + re-pin): +9 fix verified (native 0x335909 = 0/1/0/1 at vb 1141/1142/1171/1306
= oracle parity per EeScheduler.cpp:1438). Tripwire red revealed the OLD fixture was hooked-oracle
(progressive-ON forces uiEnvCenterPos parity arg to 0, overrides.cpp:179-183): regenerated it PINNED
(RT_PROGRESSIVE_FIELDS=0, driver oracle_eerm_pin.py — oracle test socket dies with its parent, drive it from
a persistent python parent). Repinned via repin_ram_attract.py: 736 B / 12 pages, pages 0x335/0x336 now
byte-exact. card_band + tex2 + tripwire all green on the fixed binary; test_w12_kernel CHECK 0xFFFFFFFFu -> 1u
(test pinned pre-fix shim value; contradicts cited oracle, stated in commit). closeout green (154/154, det pair
identical); matrix re-running. port-worker W12-UB3 triage DONE: 2 signedness FIXS in cap (0021a088
UpdateSteeringDamping param_2/4 < 0/< 5000 on pseudocode long, caller 0021a310 passes int -> int32_t, coordinator
spot-verified 0021a088.c:58/64/75; 00217c68 UpdateCarTireSound param_3 signed < 0x41 chain -> int32_t) + 4 real
64-bit rows out of cap (0027b540 __floatdisf param_1 >> 0x20; 00271cd8 gifEmitBlock8Planes param_5 8-plane mask,
param_6 &0xffffffff<<9, param_2 Qword addr) to apply once GATES_DONE sentinel lands (post-commit). parity-debugger
chasing 0x177e camera-matrix cluster + 0x17874e0-vs-0x1f10000 allocator placement.

late-9 (2026-10-20, heap batch landed): parity-debugger HEAP-ROOTCAUSE.md applied. gen fixes (MIPS-cited,
coordinator verified `sh` stores 00208848 @0x2088ec/0x208988 with capstone, mask word 0x4b864218 present in
00272a70 body): 00208848 halfword stores (u32 store was zeroing 0x29abe6), 00272a70 vu0SinCos a3 branch +
dest-mask 0b0110->0b1100 + callers 00272ae8/90/c38 set g_a3SinCos, ClipScreen vsub masks 0b1011->0b1101
(002732d0/18/78, attract-neutral). Shims applied by hand: guest_libs_sif.cpp SdRpc dummy server from oracle
fixed pool 0x01F10000 stride 0x80 (rpcAllocServerAddr Runtime.h:99) not game heap; guest_mem.cpp setPendingRA
+8 (MIPS jal return, oracle residue 0x2089c8 for jal@0x2089c0 — sweep caught nothing, det pair identical);
guest_mem.cpp loadImage now PT_LOAD segment staging (section walk missed .reginfo 0x29a840-47; ELF32 memsz
at phdr+0x14 — first patch attempt used +0x18=p_flags and failed staging). Census 736/12 -> 498 B / 10 pages
(pinned repin from /tmp/eerm_p1305pin.bin vs /tmp/p10_1306.bin); tripwire header updated. closeout green on
integrated tree (154/154, det pair). OPEN: camera view-matrix output (21/25 rows, stack-residue class,
BuildCameraViewMatrices input probe next), vu0TransformVerts vf0 vsub guard decision (gen/0021d470, needs
oracle vf0-state probe), pages 0/9/a/e + 1fff=294 mask-policy ledger, oracle thread/stack table words at
1787 (18). loader patch provenance refreshed at build/scratch/w12-parity/loader-segment-staging.patch.

late-10 (2026-10-20, cycle-1 shipped + UB3): commit 9400315 pushed (card-band/+9/heap/vu0/loader/tripwire-498). Release-guard PASS ruled tripwire byte triples = runtime-state facts not disc data; user ruling: switch to hash-of-diff fingerprint if ever re-flagged. Guard false-block on unstaged gen/0021f450.cpp (mtime 13:31:45 > closeout build 13:24:25 = live debugger WIP; staged tree WAS tested tree - gates pin by build timestamps, check mtime before staging another agent's file). port-worker UB3 applied, coordinator-verified vs fn pseudocode: UpdateSteeringDamping param_2/4 int32 (fn/0021a088.c:8,:64), UpdateCarTireSound param_3 int32, __floatdisf param_1 uint64 ((long)>>0x20 kept), gifEmitBlock8Planes param_2/5/6 uint64 ((ulong)(int32) sign-extend line matches :15, (uint64)(uint32) zero-extend matches :19, callers 00271e60/002723b0 already pass 64-bit param_2). Decls auto-update via scripts/refresh_decls.py -> build/decomp/guest_decls.inc (generated). gifEmitBlock8Planes is a GLYPH emitter - attract text lists may hit it: cycle-2 MUST re-run attract census + triple before commit. build/w12-ub3 0-error + TITLE smoke.

late-11 (2026-10-20, parallel lanes while debugger flies): W16 portability audit landed (build/scratch/w16-portability/AUDIT.md): native GS is a CPU rasterizer (gsRenderBatches/gsScanoutRgba) - Vulkan only in preview/ + swapchain presenter; GL backend must sit behind the preview/preview.h seam (push constants -> UBO); ps2xRuntime raylib presenter NOT reusable (hard rule); CI: guest_decls.inc required at build (CMakeLists.txt:133,:299) -> CI must run refresh_decls.py; SDL3 not in Ubuntu archives, missing SDL3/Vulkan silently drops rt-app (:213,:297); ucontext threads need Windows/Android replacement. test-engineer pinned cycle-1: tests/native_guest/test_w12_gs_color.cpp (590 checks: rgba5551 = plain <<3 no replicate, 0xFFFF->0x80F8F8F8), test_w12_shim_contracts.cpp (310: pendingRA jal+8, FrameGuard, synthetic-ELF loadImage staging incl. gap-bytes-untouched + reject cases, SdRpc pool 0x01F10000 stride 0x80 rebind reuse). Registered via test_*.cpp glob rename (no CMake edit). Registry: vu0NormalLightMatrix 0x272e08, ComposeCameraMatrix 0x21f340, buildCamera 0x21f698 (recomp widescreen hook wraps this addr - corroborates). mat4Transpose (00272820) debugger rewrite pre-reviewed: old V16 qextu interleaved wrong words = real defect class. Debugger mat4InvertAffine 00272868 registered too. W13 oracle race/town recipe being designed by reverse-engineer (build/scratch/w13-fixture/RACE-RECIPE.md, design-only while debugger holds the oracle).

late-12 (2026-10-20, W13 fixtures prep): RACE-RECIPE.md (build/scratch/w13-fixture/) - reverse-engineer recipe B chosen: scripted pad schedule, same on both sides, oracle logs realised schedule, native replays. No oracle scene-warp cmd (ps2_test_harness.cpp:671-1059); Quick Race route = Start,Down,Crossx3 with 3s gaps (rtharness/quick_race.py:17-35); town route = memory-card checkpoint + location=1/licence=2 edits then Continue (adventure.py:37-54); adventure_first_save checkpoint MISSING from build/regression/checkpoints, copy at build/scratch/parity-checkpoints/. Mac .rtstate states exist (build/scratch/menu-shots/states) but native has no state loader (recipe A = oracle-only reference). Quick Race detector: game_mode 0x33590A==1 && scene3d_handler 0x3356D8==0x21CE48. N1 BUILT by coordinator: RT_PAD_SCRIPT="vb:hexmask[:hold],..." in app/main.cpp boot loop replaces the three hard-wired RT_BOOT_START windows (legacy ladder now synthesizes 3 Press entries; default unchanged = 0xFFFF when no env). Verified: 6 reads in 3-vblank window, 18 in 9-vblank (2 reads/vblank, ports 0+1); tests/native_parity/test_pad_script.py pins both tests. HAZARD (defer, debugger live): oracle_eerm_pin.py sets no RT_DATA_DIR - runs against the user's real memory card; W13 recipe drivers MUST use private data dirs, and re-verify the park pin fixture isn't memcard-sensitive before switching the debugger's driver.

late-13 (2026-10-20, cycle-2: HEAP2 delivered, census 498->301/6): parity-debugger report build/scratch/parity/HEAP2-ROOTCAUSE.md. Camera view matrix PASS, stack-residue theory dead: 6 gen TUs fixed with MIPS-cited diffs (0021f450 normalize-to-wrong-local + fov divisor raw-int bits + 1-byte pointer step; 0021f340 restored matmul args/tail copies; 0021f698 two projection terms read back after unit-matrix reset; 00272868 stale w lane on stored translation row; 00272e08 missing arg on first scale; 00272820 interleave helpers picked wrong words -> plain scalar transpose). Class lesson: several earlier "machine-exact" edits had the DELAY-SLOT order backwards (delay slot runs before the callee). vf0 guard (0021d470): guard correct but dropping the two vsubs wrong - oracle sets VU0 flags on every mad-class op then resets vf0 (vu_translator.cpp:44-60); both vsubs now run with vf0 saved/restored (clip test previously always passed). Plus 24 uiMenuCreate call sites fixed (called with no arg -> wrote through address 0 = page-0 0x28 census): MIPS spot-verified by coordinator at 0x228114 (addu $s1,$a0,$zero) -> jal 0x205c20, no $a0 write before jal. Two sites in 0026be50 still open (need control-flow proof). Follow-up: guest_vu0.h sets flags only for vsub/vsubbc, oracle sets them for every mad-class op (W13, needs test).
Coordinator cycle-2 fixes: (1) DMA reg file side table (guest_libs_dma.cpp dmaRead/dmaWrite no longer mask MMIO into kRam - leaked chcr/qwc into pages 9/a/e, oracle keeps regs separate ps2_memory.cpp:1269). (2) EE float mode: boot thread enters FPCR RZ+FZ (aarch64) / MXCSR RC+FTZ+DAZ (x86) sticky in nativeBoot branch, mirrors ps2_ee_fpu_mode.h enterThread - killed 1-ulp class (177f 26->0, 177e 122->9). (3) N2: lockstep test-socket pad now feeds the guest boot pad (main.cpp boot branch: testSocket.pad().buttons overrides g_bootButtons under lockstep; verified 8 ffef reads for 4 held vblanks, clean on release_pad) - rtharness can drive native guest input. (4) registry entry pad_script/RT_PAD_SCRIPT (w2-13 coverage), data files regenerated via rt-w2-enh_registry.
ACCEPT LEDGER (tripwire re-pinned 301 B / 6 pages): 0x0:4 (0x2f0/0x2f8 oracle-only writes, no writer found - watchpoint build would name it); 0x175e:8+0x1773:4 (game copies stale stack words: sceSdRemote varargs, RPC end_param); 0x177e:9 (gen TUs use plain '/', oracle uses saturating ps2_fpu_div_s - divide-helper POLICY = W13 decision, do not mass-rewrite without it); 0x1787:26 (oracle sceGsResetGraph stand-ins malloc 128B GIF packets via runtime->guestMalloc (GS.cpp:862-880, helpers Support.h:1597-1690) - native has no runtime-heap mirror; W13 task = mirror incl. putDrawEnv/putDispEnv packet order); 0x1fff:250 (oracle prologue callee-saves + crt0 stand-in frames; native host stubs push none - mask-below-SP rejected as too blunt).
GATES on integrated tree (build/w-G0-verify): 0-error build, sweep 156/0, det pair race@110 identical, feature matrix 32/0, parity triple + pad script + ram tripwire all green. First matrix attempt timed out while debugger's oracle sessions were live - re-run clean passed; run matrix when the oracle lane is idle.
