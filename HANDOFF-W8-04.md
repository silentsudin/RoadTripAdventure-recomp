# W8-04 handoff — rotated preview quad and tachometer needle

## Files

- `src/native/preview/rot_quad.h` — header-only CPU preview compositor for a
  four-vertex textured quad, plus the gauge-needle adapter.
- `tests/native_w21/rot_quad_test.cpp` — synthetic-texture geometry and invalid
  input tests.
- `tests/native_w21/CMakeLists.txt` — house-style `w21_rot_quad_test` target.

No application-entry or `src/native/CMakeLists.txt` files were changed.

## API

```cpp
namespace rt::native::preview {
struct RgbaSurface { int width; int height; std::span<std::uint8_t> rgba; };
struct TexturedQuad { std::array<float,4> x, y, u, v; };
struct GaugeDialOrigin { float x, y, scaleX = 1.f, scaleY = 1.f; };

bool drawTexturedQuad(RgbaSurface surface, const TexturedQuad& quad,
                      const PreviewTexture& texture);
bool drawGaugeNeedle(RgbaSurface surface, float rpm,
                     const PreviewTexture& texture, GaugeDialOrigin dialOrigin);
}
```

`drawGaugeNeedle` uses the settled `gauge_needle.h` geometry and the documented
RPM law from `gauge-needle-draw.md` (backed there by the gauge handler at
`0x00249E10–0x00249EDC`), then samples the supplied 128×120 CSA-12 page.
`dialOrigin` is the *target-pixel* gauge origin; `scaleX/Y` map the 640×448
design pixels to the target surface.

## Coordinator wiring

Include `preview/rot_quad.h`. Immediately after the existing Meter0 dial-face
`compositeOver(pixels, dialPixels);` and before the speed digits, add:

```cpp
if (hudArt.gaugePage)
    pv::drawGaugeNeedle(
        {static_cast<int>(presenter.extent.width), static_cast<int>(presenter.extent.height), pixels},
        static_cast<float>(renderSnapshot.rpm), *hudArt.gaugePage,
        {484.f * sx, 294.f * sy, sx, sy});
```

This only draws the needle. The documented hub-cap follow-up sprite remains a
separate draw if/when its source cell is exposed.

## Gate evidence

Configured own tree:

```text
RT_SKIP_ANDROID_CHECK=1 RT_SKIP_VRC_PLUGIN=1 cmake -S src/native -B build/w-W8-4 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON
```

The parent CMake file does not yet `add_subdirectory(tests/native_w21)`, and
this worker was explicitly forbidden from editing it. The W21 source was built
in the required private build directory with the same C++20/warning flags:

```text
c++ -std=c++20 -Wall -Wextra -I src/native tests/native_w21/rot_quad_test.cpp \
  -o build/w-W8-4/w21_rot_quad_test
build/w-W8-4/w21_rot_quad_test
theta=+0.00000 centroid=(64.00,34.50) expected=(64.00,34.50) error=0.000 px
theta=+0.78540 centroid=(84.73,43.27) expected=(84.86,43.14) error=0.187 px
theta=-0.78540 centroid=(43.27,43.27) expected=(43.14,43.14) error=0.187 px
theta=+1.57080 centroid=(93.50,64.00) expected=(93.50,64.00) error=0.000 px
theta=-1.57080 centroid=(34.50,64.00) expected=(34.50,64.00) error=0.000 px
theta=+3.14159 centroid=(64.00,93.50) expected=(64.00,93.50) error=0.000 px
checks=31 failed=0
```

Existing W19 target remains green unchanged:

```text
cmake --build build/w-W8-4 --target w19_gauge_needle_test -j4
build/w-W8-4/native_w19/w19_gauge_needle_test
CAUGHT: +15 degree needle-angle perturbation misses the coast golden
checks=12 failed=0
```

## Precision

Coverage is tested at pixel centres across two strip triangles; UVs are
interpolated affinely and sampled bilinearly. It is intentionally a CPU
post-composite path, so it does not reproduce GS sub-pixel coverage or the
original nearest sampling exactly. Invalid/malformed textures, non-finite RPM,
and invalid scale return `false` without writes.
