#include "Mapping.h"

#include <algorithm>
#include <cmath>

namespace rt::input
{
    namespace
    {
        PadSource button(PadButton b) { return {PadSource::Kind::Button, static_cast<uint8_t>(b)}; }
        PadSource plus(PadAxis a) { return {PadSource::Kind::AxisPlus, static_cast<uint8_t>(a)}; }

        bool sourceActive(const PadSource &s, const GamepadSnapshot &pad, const GamepadProfile &p, AxisLatch &latch)
        {
            if (s.kind == PadSource::Kind::Button)
                return s.index < kPadButtonCount && pad.buttons[s.index];
            if (s.index >= kPadAxisCount)
                return false;
            const float v = pad.axes[s.index];
            bool &on = s.kind == PadSource::Kind::AxisPlus ? latch.plus[s.index] : latch.minus[s.index];
            const float pushed = s.kind == PadSource::Kind::AxisPlus ? v : -v;
            on = on ? pushed > p.triggerOff : pushed > p.triggerOn;
            return on;
        }

        void pushStick(PadOutput &out, StickDir d)
        {
            switch (d)
            {
            case StickDir::LUp: out.ly = -1; break;
            case StickDir::LDown: out.ly = 1; break;
            case StickDir::LLeft: out.lx = -1; break;
            case StickDir::LRight: out.lx = 1; break;
            case StickDir::RUp: out.ry = -1; break;
            case StickDir::RDown: out.ry = 1; break;
            case StickDir::RLeft: out.rx = -1; break;
            case StickDir::RRight: out.rx = 1; break;
            default: break;
            }
        }

        float furthest(float a, float b) { return std::fabs(b) > std::fabs(a) ? b : a; }
    }

    GamepadProfile defaultGamepadProfile()
    {
        GamepadProfile p;
        auto set = [&](Ps2Button b, std::vector<PadSource> s) { p.buttons[static_cast<int>(b)] = std::move(s); };
        set(Ps2Button::Up, {button(PadButton::DpadUp)});
        set(Ps2Button::Down, {button(PadButton::DpadDown)});
        set(Ps2Button::Left, {button(PadButton::DpadLeft)});
        set(Ps2Button::Right, {button(PadButton::DpadRight)});
        set(Ps2Button::Cross, {button(PadButton::South)});
        set(Ps2Button::Circle, {button(PadButton::East)});
        set(Ps2Button::Square, {button(PadButton::West)});
        set(Ps2Button::Triangle, {button(PadButton::North)});
        set(Ps2Button::L1, {button(PadButton::LShoulder)});
        set(Ps2Button::R1, {button(PadButton::RShoulder)});
        set(Ps2Button::L2, {plus(PadAxis::LeftTrigger)});
        set(Ps2Button::R2, {plus(PadAxis::RightTrigger)});
        set(Ps2Button::Start, {button(PadButton::Start)});
        set(Ps2Button::Select, {button(PadButton::Back), button(PadButton::Touchpad)});
        set(Ps2Button::L3, {button(PadButton::LStick)});
        set(Ps2Button::R3, {button(PadButton::RStick)});
        return p;
    }

    std::string padFamily(const std::string &type)
    {
        if (type.rfind("ps", 0) == 0)
            return "ps";
        if (type == "switchpro" || type == "joycon")
            return "nintendo";
        return "xbox";
    }

    GamepadProfile familyProfile(const GamepadProfile &base, const std::string &family)
    {
        if (family != "xbox" && family != "nintendo")
            return base;
        // Where each PlayStation-position face button moves.
        auto moved = [&](PadButton b) {
            const bool xbox = family == "xbox";
            switch (b)
            {
            case PadButton::South: return xbox ? PadButton::South : PadButton::East;
            case PadButton::East: return PadButton::North;
            case PadButton::North: return xbox ? PadButton::East : PadButton::South;
            default: return b;
            }
        };
        GamepadProfile p = base;
        auto remap = [&](std::vector<PadSource> &sources) {
            for (PadSource &s : sources)
                if (s.kind == PadSource::Kind::Button)
                    s.index = static_cast<uint8_t>(moved(static_cast<PadButton>(s.index)));
        };
        for (auto &list : p.buttons)
            remap(list);
        for (auto &list : p.stickDirs)
            remap(list);
        return p;
    }

    void applyDeadzone(float &x, float &y, const StickSettings &s)
    {
        const float r = std::hypot(x, y);
        const float inner = std::clamp(s.inner, 0.0f, 0.9f);
        const float outer = std::clamp(s.outer, inner + 0.05f, 1.0f);
        if (r <= inner)
        {
            x = y = 0;
            return;
        }
        const float scaled = std::min(1.0f, (r - inner) / (outer - inner));
        x = std::clamp(x / r * scaled, -1.0f, 1.0f);
        y = std::clamp(y / r * scaled, -1.0f, 1.0f);
    }

    PadOutput mapGamepad(const GamepadSnapshot &pad, const GamepadProfile &profile, AxisLatch &latch)
    {
        PadOutput out;
        for (int b = 0; b < kPs2ButtonCount; ++b)
        {
            bool pressed = false;
            for (const PadSource &s : profile.buttons[b])
                pressed |= sourceActive(s, pad, profile, latch); // evaluate all: keeps every latch current
            if (pressed)
                out.buttons &= static_cast<uint16_t>(~ps2Mask(static_cast<Ps2Button>(b)));
        }
        out.lx = pad.axes[static_cast<int>(PadAxis::LeftX)];
        out.ly = pad.axes[static_cast<int>(PadAxis::LeftY)];
        out.rx = pad.axes[static_cast<int>(PadAxis::RightX)];
        out.ry = pad.axes[static_cast<int>(PadAxis::RightY)];
        applyDeadzone(out.lx, out.ly, profile.left);
        applyDeadzone(out.rx, out.ry, profile.right);
        for (int d = 0; d < kStickDirCount; ++d)
            for (const PadSource &s : profile.stickDirs[d])
                if (sourceActive(s, pad, profile, latch))
                    pushStick(out, static_cast<StickDir>(d));
        return out;
    }

    PadOutput mapKeyboard(const std::function<bool(int)> &down, const KeyboardProfile &profile)
    {
        PadOutput out;
        for (int b = 0; b < kPs2ButtonCount; ++b)
            for (int key : profile.buttons[b])
                if (down(key))
                {
                    out.buttons &= static_cast<uint16_t>(~ps2Mask(static_cast<Ps2Button>(b)));
                    break;
                }
        for (int d = 0; d < kStickDirCount; ++d)
            for (int key : profile.stickDirs[d])
                if (down(key))
                {
                    pushStick(out, static_cast<StickDir>(d));
                    break;
                }
        return out;
    }

    PadOutput merge(const PadOutput &a, const PadOutput &b)
    {
        PadOutput out;
        out.buttons = static_cast<uint16_t>(a.buttons & b.buttons);
        out.lx = furthest(a.lx, b.lx);
        out.ly = furthest(a.ly, b.ly);
        out.rx = furthest(a.rx, b.rx);
        out.ry = furthest(a.ry, b.ry);
        return out;
    }
}
