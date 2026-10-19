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
