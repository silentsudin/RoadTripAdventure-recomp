#pragma once

#include <map>
#include <string>
#include <string_view>

namespace rt::native::app {

struct EnhancementOptions {
    int ssaa = 1;          // Legacy single-sample rendering; app/main.cpp:1418 (legacy window path).
    int target_hz = 0;     // Legacy vsync/no explicit cap; app/main.cpp:650.
    int physics_hz = 60;   // Legacy fixed 60-Hz logic; app/main.cpp:1991.
    bool log_fps = false;  // Legacy logging is opt-in; app/main.cpp:2841.
    bool fps_display = false; // Legacy overlay defaults off; app/main.cpp:1491-1497.
};

EnhancementOptions parseEnhancementOptions(const std::map<std::string, std::string>& env);

class Enhancements {
public:
    static const Enhancements& instance();
    const EnhancementOptions& options() const { return options_; }
    std::string describeAll() const;

private:
    Enhancements();
    EnhancementOptions options_;
};

} // namespace rt::native::app
