#include "platform/ControllersWindow.h"
#include "platform/Input.h"

#include "State.h"
#include "raylib.h"

#include <SDL3/SDL.h>
#include "imgui.h"

#include <chrono>
#include <cmath>
#include <optional>
#include <string>
#include <vector>

namespace rt::input
{
    namespace
    {
        using Clock = std::chrono::steady_clock;

        std::string g_selected = kKeyboardId;
        int g_rebinding = -1; // Ps2Button being rebound, or -1
        Clock::time_point g_rebindStart;
        Clock::time_point g_comboSince{};
        bool g_comboHeld = false;
        bool g_dirty = false;
        // What was already held when rebinding started (it must be let go before it counts).
        GamepadSnapshot g_heldPad;
        std::vector<bool> g_heldKeys;

        void startRebind(int b)
        {
            g_rebinding = b;
            g_rebindStart = Clock::now();
            g_heldPad = {};
            for (const Device &d : detail::devices().list())
                if (d.id == g_selected)
                    g_heldPad = Devices::snapshot(d);
            int count = 0;
            const bool *keys = SDL_GetKeyboardState(&count);
            g_heldKeys.assign(keys, keys + count);
        }

        // Back+Start held for 2 s, or the Guide button, on any controller opens the window.
        bool comboPressed()
        {
            bool combo = false, guide = false;
            for (const Device &d : detail::devices().list())
            {
                const GamepadSnapshot s = Devices::snapshot(d);
                combo |= s.buttons[static_cast<int>(PadButton::Back)] && s.buttons[static_cast<int>(PadButton::Start)];
                guide |= s.buttons[static_cast<int>(PadButton::Guide)];
            }
            static bool guideWas = false;
            const bool guidePressed = guide && !guideWas;
            guideWas = guide;
            if (!combo)
            {
                g_comboSince = {};
                g_comboHeld = false;
                return guidePressed;
            }
            const auto now = Clock::now();
            if (g_comboSince == Clock::time_point{})
                g_comboSince = now;
            if (!g_comboHeld && now - g_comboSince > std::chrono::seconds(2))
            {
                g_comboHeld = true;
                return true;
            }
            return guidePressed;
        }

        const Device *selectedDevice()
        {
            for (const Device &d : detail::devices().list())
                if (d.id == g_selected)
                    return &d;
            return nullptr;
        }

        // Rebinding: the next control the player uses on the selected device becomes the binding.
        void pollRebind()
        {
            if (g_rebinding < 0)
                return;
            if (IsKeyPressed(KEY_ESCAPE) || Clock::now() - g_rebindStart > std::chrono::seconds(6))
            {
                g_rebinding = -1;
                return;
            }
            Config &c = detail::config();
            if (g_selected == kKeyboardId)
            {
                int count = 0;
                const bool *keys = SDL_GetKeyboardState(&count);
                g_heldKeys.resize(count, false);
                for (int code = 0; keys && code < count; ++code)
                {
                    if (!keys[code])
                        g_heldKeys[code] = false;
                    if (keys[code] && !g_heldKeys[code] && code != SDL_SCANCODE_ESCAPE)
                    {
                        c.keyButtons[g_rebinding] = {SDL_GetScancodeName(static_cast<SDL_Scancode>(code))};
                        g_rebinding = -1;
                        g_dirty = true;
                        return;
                    }
                }
                return;
            }
            const Device *d = selectedDevice();
            if (!d)
                return;
            const GamepadSnapshot s = Devices::snapshot(*d);
            for (int b = 0; b < kPadButtonCount; ++b)
                g_heldPad.buttons[b] = g_heldPad.buttons[b] && s.buttons[b];
            for (int a = 0; a < kPadAxisCount; ++a)
                if (std::fabs(s.axes[a]) < 0.25f)
                    g_heldPad.axes[a] = 0;
            std::optional<PadSource> got;
            for (int b = 0; b < kPadButtonCount && !got; ++b)
                if (s.buttons[b] && !g_heldPad.buttons[b] && b != static_cast<int>(PadButton::Guide))
                    got = PadSource{PadSource::Kind::Button, static_cast<uint8_t>(b)};
            for (int a = 0; a < kPadAxisCount && !got; ++a)
                if (std::fabs(s.axes[a]) > 0.5f && std::fabs(g_heldPad.axes[a]) < 0.25f)
                    got = PadSource{s.axes[a] > 0 ? PadSource::Kind::AxisPlus : PadSource::Kind::AxisMinus,
                                    static_cast<uint8_t>(a)};
            if (!got)
                return;
            // A controller of its own gets its own profile the first time it is rebound.
            if (!c.gamepads.count(d->id))
                c.gamepads[d->id] = c.gamepad;
            c.gamepads[d->id].buttons[g_rebinding] = {*got};
            g_rebinding = -1;
            g_dirty = true;
        }

        void stickPreview(const char *label, float x, float y)
        {
            ImDrawList *draw = ImGui::GetWindowDrawList();
            const ImVec2 at = ImGui::GetCursorScreenPos();
            const float r = 36.0f;
            const ImVec2 c(at.x + r, at.y + r);
            draw->AddCircle(c, r, IM_COL32(160, 160, 160, 255));
            draw->AddCircleFilled(ImVec2(c.x + x * r, c.y + y * r), 5.0f, IM_COL32(80, 200, 255, 255));
            ImGui::Dummy(ImVec2(2 * r, 2 * r));
            ImGui::SameLine();
            ImGui::Text("%s\n%.2f, %.2f", label, x, y);
        }

        std::string bindingText(int b)
        {
            const Config &c = detail::config();
            std::string out;
            if (g_selected == kKeyboardId)
                for (const std::string &k : c.keyButtons[b])
                    out += (out.empty() ? "" : ", ") + k;
            else
                for (const PadSource &s : c.profileFor(g_selected).buttons[b])
                    out += (out.empty() ? "" : ", ") + padSourceName(s);
            return out.empty() ? "-" : out;
        }
    }

    void updateControllersWindow()
    {
        if (IsKeyPressed(KEY_F2) || comboPressed())
            detail::menuOpen() = !detail::menuOpen();
        if (detail::menuOpen())
            pollRebind();
        else
            g_rebinding = -1;
    }

    bool controllersWindowOpen() { return detail::menuOpen(); }

    void drawControllersWindow()
    {
        bool &open = detail::menuOpen();
        if (!open)
            return;
        ImGui::SetNextWindowSize(ImVec2(620, 520), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Controllers (F2)", &open))
        {
            ImGui::End();
            return;
        }
        Config &c = detail::config();

        ImGui::SeparatorText("Devices");
        for (const DeviceStatus &d : devices())
        {
            ImGui::PushID(d.id.c_str());
            if (ImGui::RadioButton("##sel", g_selected == d.id))
                g_selected = d.id;
            ImGui::SameLine();
            std::string label = d.name + "  [" + d.type + "]";
            if (d.batteryPercent >= 0)
                label += "  " + std::to_string(d.batteryPercent) + "%";
            ImGui::TextUnformatted(label.c_str());
            ImGui::SameLine(420);
            const char *choices[] = {"Auto", "Player 1", "Player 2", "Off"};
            auto manual = c.players.find(d.id);
            int choice = manual == c.players.end() ? 0 : manual->second < 0 ? 3 : manual->second + 1;
            ImGui::SetNextItemWidth(110);
            if (ImGui::Combo("##player", &choice, choices, 4))
            {
                if (choice == 0)
                    c.players.erase(d.id);
                else
                    c.players[d.id] = choice == 3 ? -1 : choice - 1;
                detail::applyConfig();
                g_dirty = true;
            }
            if (choice == 0)
            {
                ImGui::SameLine();
                ImGui::TextDisabled(d.player >= 0 ? "(P%d)" : "(-)", d.player + 1);
            }
            ImGui::PopID();
        }
        ImGui::TextDisabled("Player 2 is unplugged until a device plays as player 2.");

        ImGui::SeparatorText("Buttons");
        ImGui::TextDisabled("%s: click Rebind, then press the control (Esc cancels).",
                            g_selected == kKeyboardId ? "Keyboard" : "Controller");
        if (ImGui::BeginTable("buttons", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp))
        {
            for (int b = 0; b < kPs2ButtonCount; ++b)
            {
                ImGui::PushID(b);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(ps2Name(static_cast<Ps2Button>(b)));
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(g_rebinding == b ? "press a control..." : bindingText(b).c_str());
                ImGui::TableNextColumn();
                if (ImGui::SmallButton("Rebind"))
                    startRebind(b);
                ImGui::PopID();
            }
            ImGui::EndTable();
        }

        if (const Device *d = selectedDevice())
        {
            ImGui::SeparatorText("Sticks");
            // Edits make this controller's own profile (the shared one stays as it is).
            GamepadProfile p = c.profileFor(d->id);
            bool changed = ImGui::SliderFloat("Left deadzone", &p.left.inner, 0.0f, 0.5f, "%.2f");
            changed |= ImGui::SliderFloat("Right deadzone", &p.right.inner, 0.0f, 0.5f, "%.2f");
            changed |= ImGui::SliderFloat("Vibration (this controller)", &p.rumble, 0.0f, 1.0f, "%.2f");
            if (changed)
            {
                c.gamepads[d->id] = p;
                g_dirty = true;
            }
            AxisLatch latch = d->latch;
            const PadOutput o = mapGamepad(Devices::snapshot(*d), p, latch);
            stickPreview("Left", o.lx, o.ly);
            ImGui::SameLine(260);
            stickPreview("Right", o.rx, o.ry);
            if (ImGui::Button("Test vibration"))
                detail::testRumble(d->id);
        }

        ImGui::SeparatorText("General");
        g_dirty |= ImGui::SliderFloat("Vibration", &c.rumble, 0.0f, 1.0f, "%.2f");
        if (ImGui::Button("Save") || (g_dirty && !open))
        {
            detail::save();
            g_dirty = false;
        }
        ImGui::SameLine();
        ImGui::TextDisabled(g_dirty ? "unsaved changes (saved when the window closes)" : "saved");
        ImGui::End();
        if (!open && g_dirty)
        {
            detail::save();
            g_dirty = false;
        }
    }
}
