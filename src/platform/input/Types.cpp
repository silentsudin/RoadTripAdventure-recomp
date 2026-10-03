#include "Types.h"

#include <algorithm>
#include <cmath>

namespace rt::input
{
    namespace
    {
        struct Ps2Def
        {
            const char *name;
            uint16_t mask;
        };
        constexpr Ps2Def kPs2[kPs2ButtonCount] = {
            {"up", 0x0010}, {"down", 0x0040}, {"left", 0x0080}, {"right", 0x0020},
            {"cross", 0x4000}, {"circle", 0x2000}, {"square", 0x8000}, {"triangle", 0x1000},
            {"l1", 0x0400}, {"r1", 0x0800}, {"l2", 0x0100}, {"r2", 0x0200},
            {"start", 0x0008}, {"select", 0x0001}, {"l3", 0x0002}, {"r3", 0x0004},
        };
        constexpr const char *kStickDirs[kStickDirCount] = {
            "lstick_up", "lstick_down", "lstick_left", "lstick_right",
            "rstick_up", "rstick_down", "rstick_left", "rstick_right",
        };
        constexpr const char *kPadButtons[kPadButtonCount] = {
            "south", "east", "west", "north", "back", "guide", "start", "lstick", "rstick",
            "lshoulder", "rshoulder", "dpup", "dpdown", "dpleft", "dpright", "misc1",
            "rpaddle1", "lpaddle1", "rpaddle2", "lpaddle2", "touchpad", "misc2", "misc3", "misc4",
            "misc5", "misc6",
        };
        constexpr const char *kPadAxes[kPadAxisCount] = {"leftx", "lefty", "rightx", "righty", "lefttrigger",
                                                        "righttrigger"};
    }

    uint16_t ps2Mask(Ps2Button b) { return kPs2[static_cast<int>(b)].mask; }
    const char *ps2Name(Ps2Button b) { return kPs2[static_cast<int>(b)].name; }

    std::optional<Ps2Button> ps2FromName(std::string_view name)
    {
        for (int i = 0; i < kPs2ButtonCount; ++i)
            if (name == kPs2[i].name)
                return static_cast<Ps2Button>(i);
        return std::nullopt;
    }

    const char *stickDirName(StickDir d) { return kStickDirs[static_cast<int>(d)]; }

    std::optional<StickDir> stickDirFromName(std::string_view name)
    {
        for (int i = 0; i < kStickDirCount; ++i)
            if (name == kStickDirs[i])
                return static_cast<StickDir>(i);
        return std::nullopt;
    }

    std::string padSourceName(PadSource s)
    {
        if (s.kind == PadSource::Kind::Button)
            return kPadButtons[s.index];
        const std::string axis = kPadAxes[s.index];
        // Triggers only go one way: no sign in their names.
        if (s.index >= static_cast<int>(PadAxis::LeftTrigger))
            return axis;
        return axis + (s.kind == PadSource::Kind::AxisPlus ? "+" : "-");
    }

    std::optional<PadSource> padSourceFromName(std::string_view name)
    {
        for (int i = 0; i < kPadButtonCount; ++i)
            if (name == kPadButtons[i])
                return PadSource{PadSource::Kind::Button, static_cast<uint8_t>(i)};
        for (int i = 0; i < kPadAxisCount; ++i)
        {
            const std::string_view axis = kPadAxes[i];
            if (name == axis && i >= static_cast<int>(PadAxis::LeftTrigger))
                return PadSource{PadSource::Kind::AxisPlus, static_cast<uint8_t>(i)};
            if (name.size() == axis.size() + 1 && name.substr(0, axis.size()) == axis)
            {
                if (name.back() == '+')
                    return PadSource{PadSource::Kind::AxisPlus, static_cast<uint8_t>(i)};
                if (name.back() == '-')
                    return PadSource{PadSource::Kind::AxisMinus, static_cast<uint8_t>(i)};
            }
        }
        return std::nullopt;
    }

    uint8_t stickByte(float v)
    {
        v = std::clamp(v, -1.0f, 1.0f);
        return static_cast<uint8_t>(std::lround(127.5f + v * 127.5f));
    }
}
