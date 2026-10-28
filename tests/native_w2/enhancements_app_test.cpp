#include "app/enhancements_app.h"

#include <array>
#include <iostream>
#include <map>
#include <string>

using rt::native::app::EnhancementOptions;
using rt::native::app::parseEnhancementOptions;

int main() {
    const auto defaults = parseEnhancementOptions({});
    int toggles = 0;
    const auto check = [&](const std::map<std::string, std::string>& env, auto predicate) {
        const auto got = parseEnhancementOptions(env);
        const bool pass = predicate(got);
        toggles += pass;
        return pass;
    };
    bool ok = defaults.ssaa == 1 && defaults.target_hz == 0 && defaults.physics_hz == 60 &&
              !defaults.log_fps && !defaults.fps_display;
    const std::array<std::pair<const char*, int>, 3> ints{{
        {"RT_ENHANCE_SSAA", 4}, {"RT_ENHANCE_TARGET_HZ", 120}, {"RT_ENHANCE_PHYSICS_HZ", 30}}};
    for (const auto& [name, value] : ints) {
        const std::string key = name;  // clang 14 forbids capturing structured bindings; copies keep the checks identical
        const int want = value;
        auto env = std::map<std::string, std::string>{{key, std::to_string(want)}};
        ok &= check(env, [&, key, want](const EnhancementOptions& o) {
            return o.ssaa == (key == std::string("RT_ENHANCE_SSAA") ? want : 1) &&
                   o.target_hz == (key == std::string("RT_ENHANCE_TARGET_HZ") ? want : 0) &&
                   o.physics_hz == (key == std::string("RT_ENHANCE_PHYSICS_HZ") ? want : 60) &&
                   !o.log_fps && !o.fps_display;
        });
    }
    for (const char* name : {"RT_ENHANCE_LOG_FPS", "RT_ENHANCE_FPS_DISPLAY"}) {
        ok &= check({{name, "1"}}, [&](const EnhancementOptions& o) {
            return o.ssaa == 1 && o.target_hz == 0 && o.physics_hz == 60 &&
                   o.log_fps == (std::string(name) == "RT_ENHANCE_LOG_FPS") &&
                   o.fps_display == (std::string(name) == "RT_ENHANCE_FPS_DISPLAY");
        });
    }
    {
        const auto unknown = parseEnhancementOptions({{"RT_ENHANCE_UNKNOWN", "1"}});
        ok &= unknown.ssaa == 1 && unknown.target_hz == 0 && unknown.physics_hz == 60 &&
              !unknown.log_fps && !unknown.fps_display;
        const auto invalid = parseEnhancementOptions({{"RT_ENHANCE_SSAA", "bad"}});
        ok &= invalid.ssaa == 1;
    }
    std::cout << "SCORE defaults=" << (ok ? "ok" : "fail") << " toggles=" << toggles << "/5\n";
    return ok && toggles == 5 ? 0 : 1;
}
