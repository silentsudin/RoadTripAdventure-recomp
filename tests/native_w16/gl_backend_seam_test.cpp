// W16 independent seam pins: parser edge cases and GL dispatch must not create Vulkan.
#include "preview/preview_backend.h"
#include "preview/renderer_select.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace rt::native::preview;

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); return 1; } } while (0)

namespace {

int creates = 0;
int destroys = 0;

void* create(std::uint32_t width, std::uint32_t height, std::string& error) {
    if (width != 64 || height != 32 || !error.empty()) std::abort();
    ++creates;
    return reinterpret_cast<void*>(1);
}

void destroy(void* state) {
    if (state != reinterpret_cast<void*>(1)) std::abort();
    ++destroys;
}

const GlBackendOps kFakeGl{create, destroy, nullptr, nullptr, nullptr, nullptr, nullptr};

} // namespace

int main() {
    // Existing parser test covers canonical aliases and unset/unknown. These are the
    // externally surprising cases: case folds, but whitespace is not trimmed.
    CHECK(parseRendererMode("VULKAN") == RendererMode::Vulkan);
    CHECK(parseRendererMode("gLeS") == RendererMode::Gl);
    CHECK(parseRendererMode("AUTO") == RendererMode::Auto);
    CHECK(parseRendererMode(" gl") == RendererMode::Auto);
    CHECK(parseRendererMode("gl ") == RendererMode::Auto);
    CHECK(parseRendererMode("\tvulkan") == RendererMode::Auto);

    // This uses the public dispatch seam, not a duplicate selectBackend calculation:
    // a registered GL backend is selected by previewInit and no VkDevice is created.
    CHECK(previewVulkanDevicesCreated() == 0);
    registerGlBackend(&kFakeGl);
    setenv("RT_APP_RENDERER", "gl", 1);
    std::string error;
    std::string device;
    Renderer* renderer = previewInit(64, 32, error, &device);
    CHECK(renderer != nullptr);
    CHECK(error.empty());
    CHECK(device == "opengl");
    CHECK(creates == 1);
    CHECK(previewVulkanDevicesCreated() == 0);
    previewDestroy(renderer);
    CHECK(destroys == 1);
    CHECK(previewVulkanDevicesCreated() == 0);
    registerGlBackend(nullptr);
    unsetenv("RT_APP_RENDERER");
    std::puts("w16_gl_backend_seam: ok");
    return 0;
}
