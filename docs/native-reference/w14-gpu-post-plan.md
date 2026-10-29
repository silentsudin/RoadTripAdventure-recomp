# Native visual-quality chain — W14 plan

Status: **plan, not implementation.** The inventory below is measured from this
checkout; every step is gated by the existing native gates (0-error build,
170-binary sweep, `race@110` determinism pair, `rt_feature_matrix.sh`, headless
snapshots). Nothing here is claimed as shipped.

`visual-pipeline.md` already states what exists and what is AUTHORED; this file
is the ordered work list that closes the gap the objective names: motion
vectors plus the upscalers the recomp product has (FXAA/SMAA, FSR 1, TAA,
GSR 2, ASR, MetalFX), frame generation, and the texture-pack glue.

## Measured present architecture (why step 1 exists)

| Fact | Evidence |
|---|---|
| One offscreen RGBA8 colour + D32F depth target; scene = one render pass, then a full host readback | `preview/preview_vk.cpp:413-441`, `994-1060` |
| Every `previewRender2D` call is its own clear/render/readback/wait; every `previewRender2DTextured` too | `preview_vk.cpp:1355-1410`, `1430-1487` |
| GL mirrors it (RGBA8+D32F FBO, synchronous `glReadPixels`) | `preview/preview_gl.cpp:123-140`, `191-202` |
| Overlays are CPU alpha-blended into the scene buffer | `app/main.cpp:1512-1523` (`compositeOver`) |
| Presentation is a CPU upload: Vulkan staging copy, GL `TexSubImage2D` | `app/main.cpp:670-697`; `preview/present_gl.cpp:108-133` |
| Renderer passes + readbacks per frame | PLAYING 1+1; title with text 2+2; normal Explore 6+6; full race HUD **11+11** (`main.cpp:3174-3628`) |

So the chain is not merely absent: the per-element readback is what makes a
temporal chain impossible (colour, depth, motion and the UI mask must stay on
the GPU), and it is also the native build's largest per-frame cost.

## Motion and timing inputs that already exist (CPU)

- `gameplay/motion.{h,cpp}` — `EntityMotion`, `projectEntityMotion`, validity
  rules (no previous pose, >30-unit jump, >45° spin = translation only),
  `cameraOnlyMotion` for static terrain.
- `gameplay/interp.h` — `RenderPose` / `FrameSnapshot` (car position/heading,
  camera views); `gameplay/frame_timing.*` — fixed 60 Hz logic, interpolated
  host render.
- Nothing consumes those for rendering today: **no** velocity attachment, no
  previous matrix, no jitter, no history, no rejection pass.

## Ordered steps

Each step must leave the default path byte-identical (`RT_APP_SNAPSHOT` /
goldens) and must not add an `RT_*` literal without registering it in
`gameplay/enhancements.cpp` plus regenerating
`tests/native_w2/data/enhancement_registry.{csv,json}`.

**Step 1 — GPU HUD composite.** Replace the per-element
`previewRender2D(+Textured)` readbacks with one frame encoder: scene and HUD
submissions into the same colour target (src-over in the pipeline), one
readback only when a snapshot is requested. Keep the CPU composite as the
default until GPU output is proven equal on the race/Explore/title snapshots
(it will differ in blend rounding; record what differs before switching over).
Files: `preview.h`, `preview_backend.h`, `preview_vk.cpp`, `preview_gl.cpp`,
`app/main.cpp`, `present_gl.{h,cpp}`. CPU needle/digit rasterization
(`main.cpp:3357-3411`) needs a GPU equivalent or stays a late CPU overlay.

**Step 2 — scene attachments.** Make the scene target sampleable: RGBA8
colour, D32F depth, RG16F motion (MRT from the scene vertex shaders, fed by
`RenderPose`/`cameraOnlyMotion` + the exact jittered previous/current
view-projection), R8 UI mask (HUD fragments write 1). Terrain needs
camera-only vectors; AI traffic has no render poses yet (`gameplay/ai.cpp`
PARTIAL). Gate every attachment off by default.

**Step 3 — FSR 1 (spatial).** EASU + RCAS in the present path behind
`display.upscaler`; needs only colour, so it can land before step 2 and is the
first real "upscaler supported" claim. Default off must keep the legacy
extent and byte stream.

**Step 4 — TAA, then temporal upscalers.** Halton(2,3) 8-phase projection
jitter, history reprojection with step 2's motion, depth disocclusion,
neighbourhood rejection, history reset on resize/camera cut/teleport, history
weight 0 on UI mask. FSR 2-class/GSR 2/ASR need their own conventions on top
(jitter cancellation, reverse-Z infinite depth, motion units/sign, reactive
policy, exposure, resize) — capability-gated, never a silent fallback.

**Step 5 — frame generation (120/240 Hz).** Simulation stays 60 Hz; the
native loop already interpolates the host pose, so the first version re-renders
the scene from the interpolated pose at the display cadence. A shadow-frame
path like the recomp's is much larger here (the native renderer has no
retained batch stream); do not promise it.

**Step 6 — platform backends.** MetalFX needs a Vulkan↔Metal ownership path
for the preview device, which the CPU-presenter design does not have; Windows
upscaler plugins need the same stable image/command ABI. Neither can be
advertised as working before a real-device visual check.

## Settings and env

- Present today but unwired: `display.aa`, `display.upscaler`,
  `display.frame_rate`, `display.frame_generation`
  (`gameplay/settings.cpp:532-536`, `HostGluePending`).
- Texture packs: the app glue landed 2026-10-28 (`assets/texture_pack.*`,
  `preview/texture_pack_bridge.h`) and is driven by `RT_TEXTURE_PACK` /
  `RT_TEXTURE_DUMP`; only the Options "Texture pack" row (a dynamic folder
  list) is still open — see `hd-texture-packs.md`, "As implemented".
- Env spellings to mirror the recomp where one exists (`RT_APP_*` in
  `enhancements.cpp`), always defaulting to the legacy path.
