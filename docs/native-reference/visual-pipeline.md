# Native visual-quality pipeline

Status: **CONFIRMED** items describe this checkout's native renderer.
**AUTHORED** items are a proposed GPU wave; they are not implemented by this
document or by `motion.*`. The sibling recomp's VU1-matrix vectors, FSR/GSR/ASR,
Halton-jitter TAA, and UI-mask treatment mentioned in `CLAUDE.md:123`,
`CLAUDE.md:132-133`, and `CLAUDE.md:151` are **AUTHORED-for-native inspiration**
only, not native code or an implementation claim.

## Current inventory

* **CONFIRMED — colour/depth render pass:** `preview.cpp:347-350` creates RGBA8
  colour and D32_SFLOAT depth images. `preview.cpp:352-375` builds one subpass
  with both attachments; the colour finishes in transfer-src layout for CPU
  readback. The scene pipeline depth-tests and writes (`preview.cpp:406-409`).
  Therefore depth exists in the offscreen preview renderer, although it is not
  currently copied to a sampled/readable temporal attachment.
* **CONFIRMED — no MSAA:** colour and depth images are single sample
  (`preview.cpp:216-219`, `preview.cpp:347-350`), and the graphics pipeline is
  `VK_SAMPLE_COUNT_1_BIT` (`preview.cpp:406-407`).
* **CONFIRMED — HUD is separable:** 3D scene geometry is submitted before its
  own readback (`preview.cpp:907-1120`). Screen quads are an independent
  `previewRender2D` pass (`preview.cpp:1125-1179`), and font HUD has a distinct
  depth-off pipeline (`preview.cpp:184-193`, `preview.cpp:486-507`). The app
  combines HUD and scene on CPU with `compositeOver` (`main.cpp:799-810`) after
  scene/HUD calls (`main.cpp:1333-1345`; text is separately composed at
  `main.cpp:1373-1377`). A GPU wave should retain this as a
  UI-mask or, preferably, a late HUD composite: text and digits must not enter
  temporal history.
* **CONFIRMED — presentation is a copy:** `main.cpp:317-345` maps the RGBA
  readback into staging and copies it to a transfer-destination-only swapchain
  image. It is not yet a direct multi-attachment swapchain render path.

## Motion vectors and temporal plumbing

**AUTHORED:** `gameplay::EntityMotion` is the CPU contract in
`src/native/gameplay/motion.{h,cpp}`. Feed it from the prior/current completed
`RenderPose` snapshots in `gameplay/interp.h:34-62`: the car's `carPosition`
and `heading` directly populate position/yaw, while camera vectors build the
previous/current view-projection matrices. The fixed 60-Hz source and
interpolated-host-render policy are **CONFIRMED** in `frame-timing.md:92-124`.

The module provides a column-major CPU projection and this exact vertex-shader
formula (also retained beside the API):

```glsl
vec4 prevClip = uPrevViewProjection * (uPrevModel * vec4(inPosition, 1.0));
vec4 curClip  = uCurViewProjection  * (uCurModel  * vec4(inPosition, 1.0));
vec2 prevNdc = prevClip.xy / prevClip.w;
vec2 curNdc  = curClip.xy / curClip.w;
outMotionUv = 0.5 * (curNdc - prevNdc); // current minus previous
```

New entities (`hasPrevious=false`), a jump farther than the configurable
30-native-unit default, and non-projectable clip coordinates return zero
motion and invalid history. Large rotation (default >45 degrees per fixed
step) is translation-only: it retains root displacement but avoids unstable
coarse-mesh spin vectors. `cameraOnlyMotion` covers static terrain using the
previous/current view-projection pair.

**AUTHORED TAA:** jitter the *projection* with an 8-phase Halton(2,3) sequence
in render pixels, store the exact jittered previous/current matrices used for
the vector pass, then reproject history by `outMotionUv`. Reject or sharply
reduce history for invalid vector flags, depth disagreement/disocclusion,
large velocity, and neighbourhood/luma disagreement. Composite the HUD after
TAA; if a future unified pass forces HUD into the colour target, write a
one-bit UI mask and force its history weight to zero. HUD text/digits are never
temporally blended.

## Upscaling choices on MoltenVK/macOS

| option | status / requirements | decision |
|---|---|---|
| FSR 1.0 | **AUTHORED portable baseline.** Spatial EASU+RCAS is shader code and requires only colour input; it has no motion/depth dependency. | First portable upscale option for Vulkan/MoltenVK. |
| MetalFX | **OPEN / deliberately not promised.** MetalFX is a Metal framework API, while this renderer presently owns only Vulkan/MoltenVK objects. Sharing images/command scheduling needs explicit Metal interop and lifetime/synchronization validation; no such path is present. | Accept the env spelling, but select FSR 1.0 fallback unless a separately validated Metal implementation exists. |
| FSR2-class / ASR temporal upscaler | **AUTHORED, gated.** Needs jittered colour plus reliable depth, motion vectors, reactive/transparency policy, history resources, and exposure/resize handling. Current depth attachment exists but is not yet a sampled temporal input. | Do not enable until the GPU wave exports depth and vectors and validates disocclusion/UI handling. |
| Custom Vulkan TAA | **AUTHORED.** Vulkan-only compute/fragment resolve over colour, depth, vectors, and history. | Preferred first temporal path once those attachments exist; retains identical behaviour on macOS and other Vulkan hosts. |

## AA and recommended ordering

**AUTHORED menu:** MSAA 4x/8x means multisampled colour *and depth* attachments
plus a resolve; it reduces geometry edges but costs bandwidth/storage and does
not solve texture/shader aliasing. FXAA and SMAA are single-pass post filters;
they work without vectors/depth, but should be alternatives to TAA rather than
stacked by default. TAA uses the 8-phase Halton sequence, motion/depth history
rejection above, and late/UI-mask composite.

Recommended capability order is **MSAA resolve -> FXAA/SMAA -> motion-vector
render -> TAA -> spatial upscale -> temporal upscale**. Treat FXAA/SMAA and
TAA/temporal upscale as selected alternatives, not all simultaneous stages.
MSAA resolve must precede any single-sample post effect. Motion vectors are
rendered with the scene; TAA and temporal-upscale stages are gated on exported
depth as well as vectors. FSR 1.0 spatial upscale can operate when depth is
absent. The present preview's CPU readback/copy architecture means this is a
future direct-GPU graph, not a claim about the current path.

## GPU-wave environment contract and glue sketch

**AUTHORED contract:**

```text
RT_APP_UPSCALE=<off|fsr1:WxH|metalfx>
RT_APP_AA=<off|msaa4|msaa8|fxaa|smaa|taa>
RT_APP_MOTION_VECTORS=1   # vector debug visualization
```

Invalid values log a one-line warning and use `off`; `metalfx` logs its
fallback to `fsr1:WxH` (or `off` when no target size was supplied) unless the
future validated interop capability is present. Resolution changes, AA changes,
camera cuts, teleports, and any temporal-mode toggle clear history.

```cpp
// main.cpp — pseudocode only: parse options at startup/resize.
VisualOptions v = parseVisualOptions(env);
renderer.recreate(v.internalExtent, v.aa); // clears temporal history
// Each completed 60-Hz step retains previous/current RenderPose snapshots.

// preview.cpp — pseudocode only: direct GPU renderer replaces CPU readback.
renderSceneColourDepthAndVelocity(curPose, prevPose, jitteredCurVP, jitteredPrevVP);
if (v.aa == TAA) resolveTaa(colour, depth, velocity, history, uiMask);
else if (v.aa == FXAA || v.aa == SMAA) resolveSpatialAa(colour);
if (v.upscale == FSR1) fsr1EasuRcas(colour);
compositeHudLate(hud); // never writes velocity/history
presentGpuImage();
```

No GPU pass, environment parsing, or renderer integration is added by this
change.
