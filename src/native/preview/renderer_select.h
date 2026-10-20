#pragma once
// W16.1: RT_APP_RENDERER=vulkan|gl|auto backend decision (pure, header-only so it is unit-testable).
#include <cctype>
#include <string>

namespace rt::native::preview {

enum class RendererMode { Auto, Vulkan, Gl };
enum class Backend { None, Vulkan, Gl };

// null/empty/unknown -> Auto. Case-insensitive; "opengl"/"gles" are aliases of "gl", "vk" of "vulkan".
inline RendererMode parseRendererMode(const char* s) {
    if (!s) return RendererMode::Auto;
    std::string v;
    for (; *s; ++s) v += static_cast<char>(std::tolower(static_cast<unsigned char>(*s)));
    if (v == "vulkan" || v == "vk") return RendererMode::Vulkan;
    if (v == "gl" || v == "opengl" || v == "gles") return RendererMode::Gl;
    return RendererMode::Auto;
}

// vulkan: Vulkan only. gl: GL if available, else Vulkan (until GL is real).
// auto: Vulkan first, GL as fallback. None only when nothing usable.
inline Backend selectBackend(RendererMode m, bool vulkanAvailable, bool glAvailable) {
    switch (m) {
    case RendererMode::Vulkan: return vulkanAvailable ? Backend::Vulkan : Backend::None;
    case RendererMode::Gl:
        return glAvailable ? Backend::Gl : (vulkanAvailable ? Backend::Vulkan : Backend::None);
    default:
        return vulkanAvailable ? Backend::Vulkan : (glAvailable ? Backend::Gl : Backend::None);
    }
}

inline const char* backendName(Backend b) {
    return b == Backend::Vulkan ? "vulkan" : b == Backend::Gl ? "gl" : "none";
}

} // namespace rt::native::preview
