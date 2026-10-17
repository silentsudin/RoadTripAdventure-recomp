// Pure preview-lighting contracts: no Vulkan device or disc assets required.
#include "../../src/native/gameplay/daynight.h"
#include "../../src/native/preview/lighting.h"

#include <cmath>
#include <cstdio>

using namespace rt::native;

namespace {
int failures = 0;
#define CHECK(expr) do { if (!(expr)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #expr); ++failures; } } while (0)
bool near(float a, float b, float epsilon = 1e-5f) { return std::fabs(a - b) <= epsilon; }
}

int main() {
    // updateTimeOfDayLight is fully daytime at 09:00 (81000 ticks): its fog
    // distance establishes the render-space [40, 80] smooth fog ramp.
    gameplay::DayNightOutput day;
    gameplay::updateTimeOfDayLight(0.f, 81000u, day);
    CHECK(preview::hasSceneLighting(day));
    CHECK(near(preview::fogFactor(0.f, day.fogFar), 0.f));
    CHECK(near(preview::fogFactor(day.fogFar * .5f, day.fogFar), 0.f));
    CHECK(near(preview::fogFactor(day.fogFar * .75f, day.fogFar), .5f));
    CHECK(near(preview::fogFactor(day.fogFar, day.fogFar), 1.f));
    CHECK(preview::fogFactor(day.fogFar * .6f, day.fogFar) > preview::fogFactor(day.fogFar * .55f, day.fogFar));
    CHECK(preview::fogFactor(day.fogFar * .9f, day.fogFar) > preview::fogFactor(day.fogFar * .6f, day.fogFar));

    const auto rgba = preview::argbToRgba(0x80409fffu);
    CHECK(near(rgba[0], 64.f / 255.f));
    CHECK(near(rgba[1], 159.f / 255.f));
    CHECK(near(rgba[2], 1.f));
    CHECK(near(rgba[3], 1.f));

    const auto selected = preview::selectSceneLighting(day);
    CHECK(selected.enabled);
    CHECK(near(selected.fogFarRender, 80.f));
    CHECK(near(selected.clear[0], 1.f));  // authored day endpoint is 0x80ffffff
    CHECK(near(selected.clear[1], 1.f));
    CHECK(near(selected.clear[2], 1.f));

    // A default-constructed snapshot has no authored matrices, so preserve
    // the legacy dark clear rather than treating its default night values as
    // initialized lighting.
    const gameplay::DayNightOutput uninitialized;
    const auto fallback = preview::selectSceneLighting(uninitialized);
    CHECK(!fallback.enabled);
    CHECK(near(fallback.clear[0], .08f));
    CHECK(near(fallback.clear[1], .09f));
    CHECK(near(fallback.clear[2], .12f));

    if (failures) return 1;
    std::puts("native_preview_lighting_test: all checks passed");
    return 0;
}
