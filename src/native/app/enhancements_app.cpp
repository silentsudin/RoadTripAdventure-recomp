#include "app/enhancements_app.h"

#include <charconv>
#include <cstdlib>
#include <sstream>

namespace rt::native::app {
namespace {
std::map<std::string, std::string> processEnvironment() {
    std::map<std::string, std::string> values;
    for (const char* name : {"RT_ENHANCE_SSAA", "RT_ENHANCE_TARGET_HZ", "RT_ENHANCE_PHYSICS_HZ",
                             "RT_ENHANCE_LOG_FPS", "RT_ENHANCE_FPS_DISPLAY"}) {
        if (const char* value = std::getenv(name)) values.emplace(name, value);
    }
    return values;
}
bool integer(const std::map<std::string, std::string>& env, const char* name, int fallback, int& out) {
    const auto it = env.find(name);
    if (it == env.end()) { out = fallback; return true; }
    int value = 0;
    const auto result = std::from_chars(it->second.data(), it->second.data() + it->second.size(), value);
    if (result.ec != std::errc{} || result.ptr != it->second.data() + it->second.size() || value < 0) {
        out = fallback; return false;
    }
    out = value; return true;
}
bool flag(const std::map<std::string, std::string>& env, const char* name, bool fallback) {
    const auto it = env.find(name);
    if (it == env.end()) return fallback;
    return it->second == "1" ? true : it->second == "0" ? false : fallback;
}
}

EnhancementOptions parseEnhancementOptions(const std::map<std::string, std::string>& env) {
    EnhancementOptions value;
    integer(env, "RT_ENHANCE_SSAA", 1, value.ssaa);
    integer(env, "RT_ENHANCE_TARGET_HZ", 0, value.target_hz);
    integer(env, "RT_ENHANCE_PHYSICS_HZ", 60, value.physics_hz);
    value.log_fps = flag(env, "RT_ENHANCE_LOG_FPS", false);
    value.fps_display = flag(env, "RT_ENHANCE_FPS_DISPLAY", false);
    return value;
}

Enhancements::Enhancements() : options_(parseEnhancementOptions(processEnvironment())) {}
const Enhancements& Enhancements::instance() { static const Enhancements value; return value; }
std::string Enhancements::describeAll() const {
    const auto& o = options_;
    std::ostringstream out;
    out << "enhancements ssaa=" << o.ssaa << " target_hz=" << o.target_hz
        << " physics_hz=" << o.physics_hz << " log_fps=" << o.log_fps
        << " fps_display=" << o.fps_display;
    return out.str();
}
} // namespace rt::native::app
