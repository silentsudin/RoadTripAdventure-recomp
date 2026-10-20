// W16.3 pin: which draw classes the GL backend owns, and the quad triangulation/NDC mapping it shares with Vulkan.
#include "preview/gl_route.h"

#include <cstdio>
#include <cstdlib>

using namespace rt::native::preview;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    CHECK(glImplements(DrawClass::Flat2D));
    CHECK(glImplements(DrawClass::UiTextured));
    CHECK(glImplements(DrawClass::FlatLit));
    CHECK(glImplements(DrawClass::Scene));  // W16.4 flipped this: the scene is on GL (test contradicted the new ground truth)

    // Pixel (0,0) is the top-left = NDC (-1,-1) (Vulkan y-down, no GL flip); (W,H) is (1,1).
    CHECK(ndcX(0, 640) == -1.0f && ndcY(0, 448) == -1.0f);
    CHECK(ndcX(640, 640) == 1.0f && ndcY(448, 448) == 1.0f);
    CHECK(ndcX(320, 640) == 0.0f);

    std::vector<FlatVertex> v;
    const FlatVertex a{0, 0, 1, 0, 0, 1}, b{1, 0, 0, 1, 0, 1}, c{1, 1, 0, 0, 1, 1}, d{0, 1, 1, 1, 1, 1};
    pushQuad(v, a, b, c, d);
    CHECK(v.size() == 6);
    // previewRender2D order: {a, c, b, a, d, c}
    CHECK(v[0].r == a.r && v[1].b == c.b && v[2].g == b.g && v[3].x == a.x && v[4].y == d.y && v[5].x == c.x);
    static_assert(sizeof(FlatVertex) == 24 && sizeof(GlyphVertex) == 32, "vertex strides mirror preview_vk.cpp");
    std::puts("gl_backend_route: ok");
    return 0;
}
