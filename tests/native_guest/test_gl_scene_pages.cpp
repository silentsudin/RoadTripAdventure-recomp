// W16.4 pin: page-slot mapping, sampler table and sky-band triangulation shared by the Vulkan and GL scene paths.
#include "preview/gl_route.h"

#include <cstdio>
#include <cstdlib>

using namespace rt::native::preview;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    // Slot 0 = white dummy for unresolved (-1) and any out-of-range id; page n -> slot n+1.
    CHECK(pageSlot(-1, 4) == 0);
    CHECK(pageSlot(-7, 4) == 0);
    CHECK(pageSlot(0, 4) == 1);
    CHECK(pageSlot(3, 4) == 4);
    CHECK(pageSlot(4, 4) == 0);
    CHECK(pageSlot(0, 0) == 0);

    for (SamplerClass c : {SamplerClass::PageTexture, SamplerClass::FontAtlas}) {
        const SamplerDesc d = samplerFor(c);
        CHECK(d.nearest && d.repeat && !d.mips);  // texSampler in preview_vk.cpp
    }

    std::vector<float> v;
    SkyBandGl flat{0.2f, 0.2f, {1, 0, 0, 1}, {0, 1, 0, 1}};
    skyBandVertices(v, flat);
    CHECK(v.empty());  // zero-height band draws nothing, like drawSkyBands
    SkyBandGl b{-1.f, 0.f, {1, 0, 0, 1}, {0, 0, 1, 1}};
    skyBandVertices(v, b);
    CHECK(v.size() == 6 * 6);
    // {a, e, c, a, d, e}: a=(-1,y0,c0) e=(1,y1,c1) c=(1,y0,c0) d=(-1,y1,c1)
    CHECK(v[0] == -1.f && v[1] == -1.f && v[2] == 1.f);                   // a
    CHECK(v[6] == 1.f && v[7] == 0.f && v[10] == 1.f && v[8] == 0.f);     // e: y1, colour c1 (b=1)
    CHECK(v[12] == 1.f && v[13] == -1.f);                                 // c
    CHECK(v[24] == -1.f && v[25] == 0.f);                                 // d
    std::puts("gl_scene_pages: ok");
    return 0;
}
