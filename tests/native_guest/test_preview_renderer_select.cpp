// W16.1 pin: RT_APP_RENDERER parsing and backend selection.
#include "preview/renderer_select.h"

#include <cstdio>
#include <cstdlib>

using namespace rt::native::preview;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    CHECK(parseRendererMode(nullptr) == RendererMode::Auto);
    CHECK(parseRendererMode("") == RendererMode::Auto);
    CHECK(parseRendererMode("bogus") == RendererMode::Auto);
    CHECK(parseRendererMode("auto") == RendererMode::Auto);
    CHECK(parseRendererMode("vulkan") == RendererMode::Vulkan);
    CHECK(parseRendererMode("VK") == RendererMode::Vulkan);
    CHECK(parseRendererMode("gl") == RendererMode::Gl);
    CHECK(parseRendererMode("OpenGL") == RendererMode::Gl);
    CHECK(parseRendererMode("gles") == RendererMode::Gl);

    CHECK(selectBackend(RendererMode::Vulkan, true, true) == Backend::Vulkan);
    CHECK(selectBackend(RendererMode::Vulkan, false, true) == Backend::None);
    CHECK(selectBackend(RendererMode::Gl, true, true) == Backend::Gl);
    CHECK(selectBackend(RendererMode::Gl, true, false) == Backend::Vulkan);
    CHECK(selectBackend(RendererMode::Gl, false, false) == Backend::None);
    CHECK(selectBackend(RendererMode::Auto, true, true) == Backend::Vulkan);
    CHECK(selectBackend(RendererMode::Auto, false, true) == Backend::Gl);
    CHECK(selectBackend(RendererMode::Auto, false, false) == Backend::None);
    std::puts("preview_renderer_select: ok");
    return 0;
}
