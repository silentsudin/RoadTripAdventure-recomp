// W16.2 pin: GL presenter selection outcomes and backend names (the GL context itself needs a window).
#include "preview/renderer_select.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace rt::native::preview;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    // main.cpp picks the GL presenter exactly when this is Gl; unset/vulkan/auto-with-Vulkan stay Vulkan.
    CHECK(selectBackend(parseRendererMode("gl"), true, true) == Backend::Gl);
    CHECK(selectBackend(parseRendererMode(nullptr), true, true) == Backend::Vulkan);
    CHECK(selectBackend(parseRendererMode("vk"), true, true) == Backend::Vulkan);
    CHECK(selectBackend(parseRendererMode("auto"), false, true) == Backend::Gl);
    CHECK(std::strcmp(backendName(Backend::Gl), "gl") == 0);
    CHECK(std::strcmp(backendName(Backend::Vulkan), "vulkan") == 0);
    CHECK(std::strcmp(backendName(Backend::None), "none") == 0);
    std::puts("gl_presenter_select: ok");
    return 0;
}
