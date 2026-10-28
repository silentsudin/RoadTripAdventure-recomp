# W16 OpenGL backend design (native app: Vulkan + GL3.3 / GLES3.0)

Read-only inventory, 2026-10-20. Scope: `src/native` (rt-app). The recomp product and
PS2Recomp are oracle only and not linkable here (AGENTS.md hard rule). Line numbers
are as of this commit; re-grep before editing.

## 0. Why (issue #20)

`gh issue view 20` (silentsudin/RoadTripAdventure-recomp): "Crashes at the Conspiracy
Entertainment screen on my intel630hd" (body is off-topic filler). Owner's reply
(2026-10-13): the **recomp** needs modern Vulkan features the UHD/HD 630 driver lacks;
an OpenGL mode is wanted later "for compatibility with older devices and those linux
gaming handhelds via something like portmaster". Implications:

- The crash is in the recomp's paraLLEl-GS / hardware-GS device requirements
  (subgroup / 16-bit / compute features), not in anything the native app does. The
  native app asks for **nothing**: Vulkan 1.1 instance, zero device features, zero
  extensions except macOS portability (`preview.cpp:326-376`, `main.cpp:592-630`),
  graphics-only pipelines, no compute. On Intel HD 630 it is likely to run on
  Vulkan today; still, HD 630 has GL 4.6 (Windows/Mesa), so a GL path is an insurance
  tier, not the fix for that specific GPU.
- Hardware the GL path actually unlocks: Vulkan-less GPUs (Ivy Bridge/Haswell on
  Windows, pre-Kepler NVIDIA, old Radeon), Linux handhelds on PortMaster (Mali
  Panfrost/Lima/Adreno with GLES3 only; many have no usable Vulkan), and old Android
  (GLES3.0 but no Vulkan 1.3).
- Device selection hazard to fix regardless: `pds[0]` is taken blindly
  (`preview.cpp:354`) and the presenter picks its own device separately
  (`main.cpp:612-627`); on laptops with iGPU+dGPU or llvmpipe first this may pick a
  different device than the swapchain device. Harmless today only because frames go
  through host RAM (see §3).

## 1. Vulkan surface inventory

| Surface | Where | Notes |
|---|---|---|
| Presenter: instance, SDL surface, device, 1 graphics+present queue | `main.cpp:270-296` struct, `592-640` init | SDL3 `SDL_WINDOW_VULKAN` (`main.cpp:1553`); instance exts from `SDL_Vulkan_GetInstanceExtensions` |
| Swapchain create/recreate, present-mode pick | `main.cpp:543-590`, `1759-1761` | recreate on `SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED` / OUT_OF_DATE |
| `present()` | `main.cpp:641-668` | CPU `rgba` -> host-visible staging buffer (BGRA swizzle if needed) -> `vkCmdCopyBufferToImage` into the swapchain image -> present. **No shader, no render pass.** |
| Scene renderer: own instance+device, 1 queue | `preview.cpp:326-376` (`initImpl`) | Separate Vulkan instance from the presenter's. Offscreen only. |
| Targets | `preview.cpp:381-402` | RGBA8 colour (TRANSFER_SRC) + D32F depth, one render pass, one framebuffer; sized to window |
| Pipelines (5) | `preview.cpp:403-541` | `pipe` flat lit+fog (pos3,col3, depth LESS), `ovPipe` overlay (depth off, route strips), `uiPipe` (pos2,col4 NDC quads), `texPipe` (pos3,col3,uv2 + sampler set 0/b0), `hudPipe` (pos2,uv2,col4 + sampler). All TRIANGLE_LIST, cull none, no GPU blend (compositing is on the CPU, see §2) |
| Push constants | `preview.cpp:448,504,702,718,911,920,946` | 144 B: `mat4 mvp; mat4 view; vec4 fog` (flat + textured only) |
| Shaders | `preview/*.vert|frag` (`#version 450`), SPIR-V inlined as `preview_shaders.h` / `preview_hud_shaders.h` via `scripts/regen_preview_shaders.sh` | 10 stages. Uses only `dFdx/dFdy`, `smoothstep`, `discard`, `texture()`. No compute, no storage buffers, no subgroup ops. |
| Texture upload paths | page images: `preview.cpp:753-860` (`TexEntry`, cached by content hash, `vkQueueWaitIdle` per upload); HUD/FONTE atlas / title backdrop / speed dial share ONE slot `previewUploadFontAtlas` (`preview.h`, `hudAtlasPtr` identity re-upload); car livery via `PreviewTexture` | All RGBA8, host staged, nearest/linear sampler; sampled only |
| Readback | `vkCmdCopyImageToBuffer` + `vkQueueWaitIdle` at `preview.cpp:725,953,1303,1378` | every frame, every pass: colour -> `rgbaOut` (std::vector, top-down) |
| Build | `CMakeLists.txt:182-200` (Android: SDL3 `release-3.4.18` FetchContent static, `libvulkan`), `263-305` (host: `find_package(Vulkan)`, tools `rt-asset-*`, `rt-w25-car-preview`, `rt-app` link `Vulkan::Vulkan`) | Vulkan is a hard requirement of every preview tool and rt-app |
| Android manifest | `android/app/src/main/AndroidManifest.xml:3` | `android.hardware.vulkan.version 0x403000` **required=true**; `minSdk 31`, arm64-v8a only (`build.gradle.kts`). This blocks installing on non-Vulkan-1.3 devices; GL tier needs `required=false` |

`title_ui.cpp` and `ui_menu.cpp` contain **no Vulkan calls** (grep): they only build
`ScreenQuad`/`HudQuad` lists that main.cpp hands to `previewRender2D`/`2DTextured`.
`car_render.cpp` likewise uses the preview API.

## 2. Seam inventory (draws -> GL tiers)

Every draw goes through the six `previewRender*` entry points (`preview.h`) and returns
**RGBA bytes on the CPU**; `main.cpp` (call sites ~2819-3425) composites layers on the
CPU (src-over) and calls `present()` once. This is the seam.

| Class | Entry point / pipeline | GL3.3-core | GLES3.0 |
|---|---|---|---|
| (c) Host 3D scene: terrain, car, route/nav strips, sky bands | `previewRenderScene` -> `pipe`/`texPipe`/`ovPipe` (`preview.cpp:632-966`) | VAO + VBO/IBO (re-uploaded per frame; `glBufferData` orphaning), FBO RGBA8 + `DEPTH_COMPONENT32F`, uniform block replaces push constants (144 B), `GL_REPEAT` page textures | same; GLES3.0 has `GL_DEPTH_COMPONENT32F`, `dFdx/dFdy` core (no `OES_standard_derivatives` needed at `#version 300 es`), `highp` float in frag required |
| (a) Host UI/menu quads | `previewRender2D` -> `uiPipe` (`:1258-1310`) | trivial: pos2+col4, no sampler | trivial |
| (a)/(d) Textured HUD, FONTE ascii glyphs, title backdrop, speed dial | `previewUploadFontAtlas` + `previewRender2DTextured` -> `hudPipe` (`:1317-1385`) | one `GL_TEXTURE_2D` RGBA8 `GL_NEAREST`; discard `a<0.02` | same |
| (d) text via FONTE path | glyph UVs from `assets::fonteSlotUv`, quads built by `race_hud.h`/`text_metrics.h` — backend-agnostic CPU code | no change | no change |
| (b) Scanout blit/composite | **Not a renderer draw today.** `rtguest::gsScanoutRgba` (`guest_gs.h:77`) is only used by the `RT_NATIVE_GS_DUMP` dump seam (`main.cpp:2393-2430`); W13 step 3 wants it composited as a texture | `glTexSubImage2D` of the GS RAM image + fullscreen triangle | same |
| Present | `present()` (`main.cpp:641`) | `SDL_GL_SwapWindow` after `glTexSubImage2D` + fullscreen blit (or `glBlitFramebuffer` from a texture-backed FBO) | same via EGL (SDL3 creates it) |

Convention deltas the GL port must absorb **inside the shaders** (keeps readback
bit-identical in row order and avoids `glClipControl`, which is GL4.5-only):

- Vulkan clip: y down, z 0..1. GL: y up, z -1..1. Fix in each 3D vertex shader:
  `gl_Position.y = -clip.y; gl_Position.z = 0.5*(clip.z + clip.w);`. With y negated,
  `glReadPixels` returns rows in the same top-down order the Vulkan copy produces, so
  `rgbaOut` and every downstream CPU compositor / golden stays unchanged. The 2D
  NDC passes (`uiPipe`, `hudPipe`) need the same y negate.
- Primitive winding / front face irrelevant (cull none).
- `vUV = inUV * clip.w` / `uv/vW` perspective trick works unchanged.
- Push constants -> one `std140` UBO (GL3.3 and ES3.0 both have UBOs).
- `#version 450` -> `#version 330 core` / `#version 300 es` (+ `precision highp float;`),
  `layout(location=)` on inputs/outputs is OK in 330 with `GL_ARB_explicit_attrib_location`
  (core since 3.3 — yes) and in ES 3.0; `layout(set/binding)` and `push_constant`
  must be rewritten. Authoring: keep one GLSL source per stage with `#if` guards
  rather than SPIRV-Cross, to avoid a build-time dependency (10 small shaders).
- Anything that used compute: **none** in the native renderer. (The recomp's
  paraLLEl-GS is compute-heavy; that is the reason it can't go GL and is irrelevant
  here.) GL3.3 / ES3.0 therefore lose nothing.

## 3. Is the scanout path backend-agnostic? Presenter-only vs full reimplementation

Yes, structurally: the app already treats the GPU as a **CPU-RGBA producer** plus a
**CPU-RGBA presenter**.

- Presenter-only GL work: ~150-250 LOC (window GL context via SDL3, one texture,
  fullscreen triangle, resize handling, vsync interval) replacing `Presenter` +
  `present()` (~130 LOC of Vulkan). Gets rt-app windowed on GL **with the Vulkan
  scene renderer still in the process** — useless on its own for no-Vulkan hardware
  (preview still needs a Vulkan device), but it is step 1 and covers any "Vulkan
  swapchain broken but device fine" case.
- Full GL requires reimplementing `preview.cpp` (1411 lines; real work is the five
  pipelines + page/atlas cache + readback ≈ 600-800 LOC GL) because rt-app's scene
  renderer *is* `preview.cpp`. Headless mode (`RT_APP_HEADLESS`, `main.cpp:1507-1510`)
  also needs it: offscreen GL needs a context — use SDL3 hidden window
  (`SDL_WINDOW_OPENGL|HIDDEN`) or EGL surfaceless/pbuffer; Linux CI without a
  display needs `EGL_PLATFORM=surfaceless` (Mesa) — document, don't promise.
- The GS (`guest_gs*.cpp`, CPU rasterizer) is backend-free: it writes guest RAM / its
  own VRAM image. No GL work; the scanout composite is `glTexSubImage2D` of that
  RGBA buffer, identical to the HUD atlas path.

**Total GL cost ≈ 1000-1400 LOC new (+ ~300 LOC of shaders), ~100 LOC of main.cpp
glue, ~60 LOC CMake.** No change to gameplay, HUD builders, title_ui, ui_menu,
car_mesh/car_render geometry, or the CPU compositor.

## 4. Recommended seam (smallest complete diff)

**Do not** write a parallel `preview_gl.cpp` that copies the `Renderer` API twice
behind `#ifdef`s, and **do not** introduce a deep scene-graph abstraction. The
existing public API in `preview.h` is already the right seam (opaque
`struct Renderer*`, free functions, RGBA out). Smallest diff:

1. Keep `preview.h` signatures verbatim. Rename the current `preview.cpp` ->
   `preview_vk.cpp` (git mv, no edits besides the include/namespace-internal rename of
   `Renderer`'s impl), add `preview_gl.cpp` implementing the same free functions in
   namespace `rt::native::preview::gl` (or file-local) with its own `struct Renderer`.
2. Add `preview_dispatch.cpp` (~120 LOC) that defines the public `previewInit/
   previewRender*/previewDestroy/previewUploadFontAtlas` and forwards to a vtable
   chosen once at `previewInit` (a `struct Backend { fn pointers }`; `Renderer*` is a
   tagged wrapper). rt-app and the tools see no change. (Tools `rt-asset-*` can
   default to Vulkan and only `rt-app` gets the choice.)
3. Selection (`RT_APP_RENDERER=vulkan|gl|auto`, default `auto`): try Vulkan device
   creation; on any failure (`vkCreateInstance`/no device/no SDL Vulkan surface) fall
   back to GL; `gl` forces GL. `auto` ordering also checks the Android manifest/
   feature. SDL window flags decided before `SDL_CreateWindow`: `SDL_WINDOW_VULKAN`
   vs `SDL_WINDOW_OPENGL` (requires `SDL_GL_SetAttribute` for 3.3 core / ES3.0 profile
   *before* window creation — so the probe must run before the window; on `auto` do a
   2-step: probe Vulkan via a throwaway `vkCreateInstance`+enumerate, then create the
   window with the right flag).
4. Presenter: `Presenter` becomes `Presenter{vk|gl}` with `present(...)` dispatching;
   headless keeps the null presenter.
5. **UI-parity mandate:** the recomp's Graphics page has a `Renderer` row with
   `Hardware`/`Accurate (paraLLEl-GS)` only (`ui-parity.md:52`). A "GL" value would be a
   row/label the recomp never showed. So: **no visible row**. Backend selection is
   `RT_APP_RENDERER`/`files/env.txt` + an automatic fallback + a `[display] renderer`
   settings.toml key that is honoured but not shown (the recomp's values
   `hardware|accurate` map to "native default"; add `gl` as a hidden value). If the
   user wants a visible choice, it must first be added to the deviation ledger in
   `ui-parity.md` as an acceptable deviation. Renderer-pixel differences between VK and
   GL fall under the existing "renderer-backend pixels" T1/T2 tolerance (`:120,140`);
   goldens remain hashes of the VK output, GL is graded with the T1 metric (≤2 levels,
   1-px fringe) against VK frames, not a new golden set.

Rejected alternatives: (a) one GLSL-ES/ANGLE layer over Vulkan (adds a huge dependency
and defeats the old-hardware goal); (b) Vulkan-on-GL (Zink) — not shippable; (c)
raylib presenter like the recomp (raylib only reads back RGBA and draws it; the native
presenter is already that, so raylib would add a dependency for the 150-LOC part and
still not provide the scene renderer).

## 5. Android, EGL/GLES3, Thor second display

- SDL3 (`release-3.4.18`, static, FetchContent, `CMakeLists.txt:187-191`) already
  compiles its Android EGL/GLES backends (`-DGL_GLEXT_PROTOTYPES` appears in the
  Android compile commands). Nothing in the native app calls them yet; `main` links
  only `libvulkan` + `liblog` (`:193-197`). GL path adds `GLESv3` + `EGL` to
  `target_link_libraries` and `SDL_GL_CONTEXT_PROFILE_ES` 3.0 attributes.
- Gradle: `minSdk 31` / arm64-v8a only today; GLES3.0 devices below SDK 31 are out of
  scope unless minSdk is lowered (a separate, user-level decision). Manifest must drop
  `required=true` for `android.hardware.vulkan.version` (set `false`), add
  `<uses-feature android:glEsVersion="0x00030000" android:required="true"/>`.
- **Device-test identity** (AGENTS.md): any Android GL run uses
  `io.github.silentsudin.roadtrip.dev` / "Road Trip DEV", never the release id; env
  switches come from `files/env.txt` (so `RT_APP_RENDERER=gl` on device is an env.txt
  line).
- raylib-in-FetchContent precedent is the **recomp's** (`third_party/PS2Recomp/
  ps2xRuntime/cmake/patch_raylib_audio.cmake`; not in root `cmake/` and not usable by
  native). Native stays on SDL3 only: GL contexts, audio, input all SDL3 already.
- Thor dual-screen: the native app has **no** second-display code (`grep -i
  second|presentation` over `src/native` finds nothing). The recomp does it in
  `RoadTripActivity.java` + `SecondDisplay.cpp` with a second Vulkan swapchain. For GL
  the second screen would be a second `SDL_Window`/EGL surface on the presentation
  display (SDL3 supports one window per display; shared-context for the map texture) —
  **out of W16 scope**; design it when the native second screen (map/notebook/radio per
  `ui-parity.md:146`) is built, and design it backend-neutral: the second screen is an
  RGBA texture from the CPU either way, so both backends reduce to "present CPU RGBA to
  window N".

## 6. W16 work packages (ordered; each independently gated)

Gate for every WP: native build 0 errors (Vulkan unchanged), native sweep, feature
matrix (`scripts/rt_feature_matrix.sh`), determinism pair; plus the WP-specific check
below. Vulkan output must stay **bit-identical** across WP1-2 (hash compare of
`RT_APP_SNAPSHOT`).

| WP | Content | LOC | Gate |
|---|---|---|---|
| W16.0 | Device-selection hardening: pick device by type (discrete>integrated>cpu), single-source it, log choice; Android manifest `required=false` prep | ~80 | VK snapshot hashes unchanged; log shows device |
| W16.1 | `preview_dispatch` + rename `preview.cpp` -> `preview_vk.cpp`; `RT_APP_RENDERER` parse; `Presenter` dispatch skeleton; CMake splits Vulkan from "preview core" so `find_package(Vulkan)` becomes optional (`RT_NATIVE_VULKAN`/`RT_NATIVE_GL` options) | ~250 | VK-only build + sweep + matrix green; hashes identical |
| W16.2 | GL presenter + GL context bring-up (SDL3, 3.3 core desktop / ES3.0 Android), `glTexSubImage2D` + fullscreen triangle, resize, vsync; still using VK renderer (hybrid, desktop-only dev step) | ~250 | windowed smoke on macOS (macOS GL is 4.1 core, deprecated but present) and Linux; `RT_APP_SNAPSHOT` equal to VK presenter |
| W16.3 | GL renderer: shaders (330/300 es), FBO, flat `pipe` + `uiPipe` + readback; title/menu screens through `previewRender2D` | ~450 + shaders | title and menu frames within T1 of VK; no VK call in `-DRT_NATIVE_VULKAN=OFF` build |
| W16.4 | GL textured terrain/atlas/HUD (`texPipe`, `hudPipe`, page cache, shared HUD slot identity rule), overlay pass | ~450 + shaders | town/race/HUD frames within T1 vs VK across the feature-matrix states |
| W16.5 | Headless GL (hidden window / EGL surfaceless), perf (readback via PBO ring, remove per-pass finish), `rt-app` tools keep VK | ~200 | `RT_APP_HEADLESS` sweep on GL; frame time ≤ VK + 2 ms on test GPU |
| W16.6 | Android GLES3.0: EGL link, manifest, env.txt `RT_APP_RENDERER`, Thor dev-app run | ~100 + gradle | Thor (`...roadtrip.dev`) boots title -> town on GL and on VK; saves/stuff backed up per memory rules |
| W16.7 | `auto` fallback (VK probe fail -> GL) incl. Windows Intel HD 630 / Linux handheld (PortMaster, GLES3 Mali) smoke on real hardware, docs + ledger entry for renderer-pixel tolerance | ~100 | issue-#20-class machine reaches the Conspiracy screen and title; file result on #20 |

Not in W16: compute-based upscalers/AA/FSR (recomp-only features, GPU-wave-pending
rows stay hidden per parity), second-display GL presenter (follows native second
screen), GL4.5-only features (`glClipControl`, DSA, `GL_ARB_buffer_storage`: keep the
3.3/ES3.0 subset; DSA/buffer storage may be used behind capability checks later).

## 7. Risks / unknowns not verified (read-only pass)

- Texture sampler details (filter/mips for page images) were not read line-by-line;
  GL must match VK sampler state (`preview.cpp:770-860`) or terrain differs at T1.
- `dFdx/dFdy` face normal in flat shading: GL derivative quality hints
  (`GL_FRAGMENT_SHADER_DERIVATIVE_HINT`) can differ on tile GPUs; compare early in W16.3.
- macOS GL is capped at 4.1 core and Apple has deprecated it; treat macOS GL as a dev
  convenience only (MoltenVK path already works there).
- Per-pass `vkQueueWaitIdle` + full readback every frame is the existing cost model;
  GL `glReadPixels` stalls similarly — PBO double-buffering (WP5) adds a frame of
  latency only if enabled; keep synchronous by default for golden parity.

## 8. W16.1 as implemented (2026-10-20) — deviations from §4/§6

- `preview.cpp` renamed `preview_vk.cpp`; all CMake users updated. No separate
  `preview_dispatch.cpp`/vtable yet: with no GL backend a forwarding layer would be dead code.
  The decision lives in header-only `preview/renderer_select.h` (`parseRendererMode`,
  `selectBackend`), consumed at the top of `previewInit` (`preview_vk.cpp`). W16.2/W16.3 should
  introduce the vtable dispatch TU when the first GL entry point exists.
- `RT_APP_RENDERER` = `vulkan|vk|gl|opengl|gles|auto` (case-insensitive; unset/unknown = auto).
  GL availability is hard-wired false: `gl` logs "GL renderer not implemented (W16.2-W16.4),
  using Vulkan" on stderr and uses Vulkan; `vulkan` logs the choice. Unset is silent.
- `RT_APP_RENDERER` is registered in the W2-13 enhancement registry
  (`gameplay/enhancements.cpp`, `renderer_backend`; csv/json regenerated). Any new RT_* getenv
  needs this or `w2_enhancements_test` fails.
- Not done (still W16.2+): optional `find_package(Vulkan)`, `RT_NATIVE_VULKAN/GL` options,
  `Presenter` dispatch.

## 8b. W16.2 as implemented (2026-10-20) — GL presenter, deviations from §4/§6

- New `preview/present_gl.{h,cpp}` (GL 3.3 core fwd-compat / GLES 3.0 under `__ANDROID__`): own GL typedefs,
  entry points via `SDL_GL_GetProcAddress` (no system GL header or link dependency), one RGBA8 NEAREST
  texture + gl_VertexID fullscreen triangle (y flip in the vertex shader), vsync 1, 1:1 viewport.
  `RT_APP_RENDERER=gl` -> `SDL_WINDOW_OPENGL` window + GL presenter; scene renderer is still Vulkan
  (`previewInit` now accepts `Backend::Gl` and logs it). `auto` creates the Vulkan window first and, only if
  presenter init fails, recreates the window with OPENGL (not a throwaway-instance probe as §4.3 proposed).
- No presenter vtable/`preview_dispatch.cpp`: main.cpp holds a `GlPresenter*` and branches at its four sites
  (create, resize, present, destroy); a vtable for two backends at four call sites would be dead abstraction.
  Headless keeps the null presenter (GL headless is W16.5).
- New debug getenv `RT_APP_PRESENT_DUMP=<ppm>` (registered): GL presenter reads back the back buffer after
  the draw and writes it as P6; compare with `RT_APP_SNAPSHOT` of the same run to verify the presenter.
- Not done: `find_package(Vulkan)` optional / `RT_NATIVE_VULKAN|GL` CMake options, Android link of GLESv3/EGL.

## 8c. W16.3 as implemented (2026-10-20) — GL flat + UI classes, deviations from §2/§4

- Dispatch is a registered ops table, not a vtable TU: `preview/preview_backend.h` (`GlBackendOps`, `registerGlBackend`),
  consumed in `preview_vk.cpp` (`previewInit` chooses; `previewRender2D`, `previewUploadFontAtlas`,
  `previewRender2DTextured` and the flat-lit `previewRender` forward when `Renderer::gl` is set). rt-app registers
  `glBackendOps()` (`preview_gl.cpp`) only after the GL presenter's context exists; tools linking only preview_vk.cpp
  stay Vulkan. A GL-backed `Renderer` has no Vulkan device; `previewRenderScene` (terrain/car/sky/overlay) creates one
  lazily (`ensureVulkan`, one log line) until W16.4. So with `RT_APP_RENDERER=gl`: flat 2D quads, FONTE/backdrop/dial
  glyph quads and the flat-lit model draw on GL; the 3D scene is still Vulkan.
- **y convention corrected**: §2 said "negate clip.y". For an FBO that is read back, do NOT negate: Vulkan NDC y=-1 is
  the image top, and GL window row 0 (= first `glReadPixels` row) is NDC y=-1, so the unmodified Vulkan NDC already
  yields top-down rows (and the same pixel rows). Negating would make `glReadPixels` bottom-up. (The W16.2 presenter
  flips in its own vertex shader because it samples a top-down CPU texture; that stays.)
- **z convention corrected**: z' = 2*z - w (not 0.5*(z+w)) so GL's [-1,1] clip maps to window depth z/w = Vulkan's.
- Uniforms are plain `uMvp/uView/uFog` uniforms, not a UBO (3 uniforms; no gain from a block).
- GL state is set at the start of every pass (dither/blend/cull/scissor off, depth per pass) because the presenter
  shares the context; each pass ends with FBO 0 bound. `GL_DITHER` must be disabled or flat colours are not exact.
- GL functions: `preview/gl_api.h` is the shared SDL-loaded table (presenter + renderer).
- Not on GL yet (W16.4): `texPipe` terrain (page cache, REPEAT), `ovPipe` overlay, sky bands, scene depth pass;
  `ScreenQuad::texture` is ignored by Vulkan's `previewRender2D` too (unchanged here).

## 8d. W16.4 as implemented (2026-10-20) — GL scene, Vulkan fallback removed

- `GlBackendOps::renderScene` (`preview_backend.h`, `preview_gl.cpp glRenderScene`) draws one scene frame in ONE depth pass:
  sky bands (ui program, depth off) -> flat `pipe` (lit program, depth LESS) -> page-batched textured `texPipe` (new `tex`
  program, `kTexVert/kTexFrag`: vUV = uv*clip.w, divided by vW per fragment, tex.a<0.02 discard, modulate x2, fog) -> depth-off
  overlay. Both previewRenderScene branches (banded-colour via `renderGeometry`, textured via `renderGeometryTextured`)
  funnel through `glDrawScene` (preview_vk.cpp) when `R.gl`; `ensureVulkan`/`vkReady` are deleted.
- Page textures: `textureFor()` (RGBA8, one object per FNV-1a content hash, forever like Vulkan's texCache) with the sampler
  table `samplerFor()` (NEAREST/REPEAT/no mips; HUD atlas shares it). Page-slot bucketing is now the shared `pageSlot()`.
  `DrawRange` gained a `tex` pointer (the GL side needs the image, not a Vulkan entry); `ensurePageTexture` is skipped on GL.
- No-Vulkan guarantee: `g_glRenderers>0` makes `initImpl` refuse (stderr `ASSERT`), `previewVulkanDevicesCreated()` counts
  VkDevices; destroy logs `Vulkan devices created this process: N` (0 in gl mode). The Vulkan loader is still linked, but
  libMoltenVK is never loaded at runtime in gl mode (lsof proof in build/scratch/w16-gl/w164-noVulkan-proof.txt).
- Conventions unchanged from §8c (no clip.y negate, z' = 2z-w, GL_DITHER off, fixed state per pass). Sky is drawn with
  depth test disabled (GL does not write depth with the test off, matching the Vulkan uiPipe's disabled depth state).
- `glImplements(Scene)` is true; test_gl_backend_route.cpp's one `!glImplements(Scene)` assertion was flipped (the test
  contradicted the new ground truth; the only existing test edited).
- Parity (macOS, same binary, 800x600): title/menu bit-exact; race 16 px / explore 55 px of 480,000 differ (worst 128 on
  one channel at isolated pixels, most <=8; stable across reruns). Not depth ties from the shader order and not the uv
  division form (both variants tried, identical output): attributed to GL-vs-MoltenVK(MSL) float rounding in varyings /
  nearest-texel boundaries; not bit-exact. Treat as renderer-pixel tolerance (ledger item for W16.7).

## 8e. W16.5 as implemented (2026-10-20) — headless GL, readback ledger

- `RT_APP_HEADLESS=1 RT_APP_RENDERER=gl` (explicit gl only; auto/unset stay on the dummy-driver Vulkan path) keeps the real
  video driver, opens a HIDDEN `SDL_WINDOW_OPENGL` window for the context and never swaps (`app/main.cpp headlessGl`). macOS
  has no surfaceless EGL/CGL path through SDL, so a window-server-less sandbox cannot use GL headless.
- Readback stays one synchronous `glReadPixels` per pass: the callers composite passes on the CPU right away, so a PBO
  double-buffer would be a frame stale and a single fused read needs the `previewRender*` API restructured. Measured
  ~1.06 ms/pass (800x600, M3 Max), GL wall time already ~Vulkan's; the per-destroy `[preview-gl] readback:` line is the ledger.

## 8f. W16.7 as implemented (2026-10-20) — auto fallback, resize, ES validation (desktop)

- `RT_APP_RENDERER=auto` now sets `RT_APP_RENDERER=gl` (overwrite) when the Vulkan presenter init fails and the window is
  recreated as OpenGL: before, the scene renderer re-resolved `auto` to Vulkan and created a Vulkan device on a machine whose
  Vulkan had just failed. Explicit `=vulkan` stays a clean error exit (rc 1).
- Test-only hooks (registered, `OracleOnly`): `RT_APP_TEST_VKFAIL=1` forces `init(Presenter)` to fail before any Vulkan
  object exists; `RT_APP_TEST_RESIZE=<W>x<H>@<frame>` calls `SDL_SetWindowSize` once at that host frame (windowed only).
  Resize follows the existing `SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED` -> `recreateSwapchain` path on both presenters.
- Shaders (desktop 330 core and ES 300 prologues) validated with glslc via a location/version shim (`build/scratch/w16-gl/es-check/`);
  Android CMake branch now builds `present_gl.cpp`/`preview_gl.cpp` and links `GLESv3` + `EGL`. No NDK build/device run.

## 8g. Linux build (2026-10-28) — first verified Linux compile and run

Recipe: `scripts/native/linux_w16.sh` + `docker/Dockerfile.native-w16` (Debian bookworm, clang,
CMake/Ninja, SDL 3.4.18 built from source, Vulkan loader/headers, Mesa lavapipe and Mesa GL/EGL,
X11 + Xvfb). The host's extracted disc folder is mounted read-only at `/disc` and passed as
`RT_DISC` — no game data is baked into the image, as everywhere else.

Verified: the app, `rt_guest` (all 1542 translated TUs) and the test binaries build with clang 14 /
libstdc++ 12; 8/8 disc-free tests pass (`native_assets_settings_test`, `w14_ui_parity`, …); the
headless race smoke runs on lavapipe and is run-to-run byte-identical; the Vulkan presenter and the
GL presenter both come up under Xvfb with software Mesa (`RT_APP_RENDERER=vulkan|gl`).

Cross-platform picture, with the same disc: vs the macOS golden `closeout-a.ppm` 1,403 of 480,000
pixels differ (0.29 %, worst 245 on thin glyph/needle edges; the differing cells are the HUD text,
the speedometer ticks and the horizon edge). That is FONTE/gauge quad rounding and float codegen,
not structure — the same class as the GL-vs-Vulkan ledger in 8a. Linux run-to-run is exact.

Portability fixes the Linux lane forced (each fixes a clang 14 / libstdc++ error): `<unordered_map>`
in `guest_kernel.cpp`; a bare label ending a block needed a statement in three translated TUs
(`transliterate.py` now emits the `;` so a regen keeps it); `<cstdio>` and `<algorithm>` in
`gameplay/camera.cpp` and `gameplay/results.cpp`; structured-binding lambda captures replaced with
copies in `tests/native_w2/enhancements_app_test.cpp`; and parallel key/value vectors in
`tests/native_w4/race_hud_layout_test.cpp` (libstdc++ rejects `std::pair` with an incomplete
element inside its own struct).

Not covered by the container: physical Linux GPU drivers, handheld GLES behaviour, audio devices
and the second-screen presentation display. `RT_NATIVE_VULKAN`/`RT_NATIVE_GL` CMake options from
§ 8 do not exist; renderer choice is the runtime `RT_APP_RENDERER`, which is what the lanes test.

## 8h. Android on GL-only hardware (2026-10-28)

The native manifest required `android.hardware.vulkan.version 0x403000` (Vulkan 1.3) with
`required="true"`, which hid the app from devices without Vulkan — the old-Intel class in
issue #20 — even though the renderer only asks for Vulkan 1.1 (`preview_vk.cpp`
`VK_API_VERSION_1_1`) and there is a GL path with an automatic fallback
(`RT_APP_RENDERER=auto`). Now: version `0x401000` (1.1), `required="false"`. Whether the GL
path actually runs on such a device is unverified here (no such machine); it is the first
thing to check when one is available.
