#include "PauseMenu.h"

#include "Theme.h"
#include "imgui.h"
#include "platform/Controllers.h"
#include "platform/Input.h"
#include "raylib.h"
#include "runtime/ps2_test_harness.h"
#include "settings/Apply.h"
#include "settings/Capabilities.h"
#include "settings/Settings.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <string>
#include <vector>

namespace rt::ui
{
    namespace
    {
        using namespace rt::settings;
        namespace th = rt::ui::theme;

        // One line of a page: a label and, for settings, a value changed with Left/Right. Headings
        // are labels only and are skipped by the cursor.
        struct Row
        {
            std::string label;
            std::function<std::string()> value;       // empty for actions
            std::function<void(int)> change;          // -1 / +1
            std::function<void()> activate;           // Cross / Enter / click
            std::string hint;                         // shown when focused
            bool heading = false;
            bool preview = false;                     // changes the picture: show the game while focused
            bool note = false;                        // a line of text, not selectable
        };

        Row heading(const char *label) { return {label, {}, {}, {}, {}, true}; }
        Row note(const char *text)
        {
            Row r{text, {}, {}, {}, {}, true};
            r.note = true;
            return r;
        }

        // "Player 1: Keyboard, DualSense 80%   Player 2: none", for the root page's band.
        std::string playersSummary()
        {
            std::string p[2];
            for (const rt::input::DeviceStatus &d : rt::input::devices())
                if (d.player == 0 || d.player == 1)
                {
                    std::string name = d.name;
                    if (d.batteryPercent >= 0)
                        name += " " + std::to_string(d.batteryPercent) + "%";
                    p[d.player] += (p[d.player].empty() ? "" : ", ") + name;
                }
            return "Player 1: " + (p[0].empty() ? std::string("none") : p[0]) + "     Player 2: " +
                   (p[1].empty() ? std::string("none") : p[1]);
        }

        enum class Page { Closed, Root, Options, Controllers, Device, Buttons, QuitConfirm, ResetConfirm };

        Page g_page = Page::Closed;
        int g_selected = 0;
        float g_scroll = 0;          // first visible row
        float g_barY = -1;           // animated selection bar position
        float g_panelY = -1;         // animated panel top (it moves down while previewing)
        double g_openedAt = 0;
        double g_closedAt = -10;
        bool g_toastPending = true;
        double g_toastUntil = 0;
        std::string g_device;        // Device and Buttons pages
        // RT_MENU_SHOT
        const char *g_shot = std::getenv("RT_MENU_SHOT");
        int g_shotFrames = -1;

        template <typename T>
        int indexOf(const std::vector<T> &v, const T &x)
        {
            auto it = std::find(v.begin(), v.end(), x);
            return it == v.end() ? 0 : static_cast<int>(it - v.begin());
        }

        template <typename T>
        void cycle(T &value, const std::vector<T> &values, int dir)
        {
            const int n = static_cast<int>(values.size());
            value = values[(indexOf(values, value) + dir + n) % n];
        }

        std::string percent(float v)
        {
            const int p = static_cast<int>(std::lround(v * 100.0f));
            return p == 0 ? std::string("Off") : std::to_string(p) + "%";
        }

        // 0, 25, 50, 75, 100%.
        float stepQuarter(float v, int dir) { return std::clamp(std::round(v * 4.0f + dir) / 4.0f, 0.0f, 1.0f); }

        void changed(bool applyNow = true)
        {
            saveCurrent();
            if (applyNow)
            {
                applyAll();
                // Let a couple of frames through so the picture behind the menu shows the change.
                if (ps2_test::paused())
                    ps2_test::stepPaused(2);
            }
        }

        void go(Page page, int selected = 0)
        {
            g_page = page;
            g_selected = selected;
            g_scroll = 0;
            g_barY = -1;
        }

        void open(Page page)
        {
            if (g_page == Page::Closed)
            {
                ps2_test::setPaused(true);
                rt::input::blockGameInput(true);
                g_openedAt = GetTime();
                g_panelY = -1;
            }
            go(page);
        }

        void close()
        {
            g_page = Page::Closed;
            g_closedAt = GetTime();
            rt::input::blockGameInput(false);
            ps2_test::setPaused(false);
        }

        std::vector<Row> rootRows()
        {
            return {
                {"Resume", {}, {}, [] { close(); }, playersSummary()},
                {"Options", {}, {}, [] { go(Page::Options, 1); }, "Window and picture settings."},
                {"Controllers", {}, {}, [] { go(Page::Controllers); }, "Who plays, buttons and vibration."},
                {"Quit", {}, {}, [] { go(Page::QuitConfirm, 1); }, playersSummary()},
            };
        }

        // Named window sizes (the game's 10:7 picture); "Largest" fits the monitor.
        struct WindowSize
        {
            const char *name;
            int w, h;
        };
        std::vector<WindowSize> windowSizes()
        {
            std::vector<WindowSize> sizes = {{"Small", 640, 448}, {"Medium", 960, 672}, {"Large", 1280, 896}};
            const int monitor = GetCurrentMonitor();
            const int mh = GetMonitorHeight(monitor), mw = GetMonitorWidth(monitor);
            int h = static_cast<int>(mh * 0.88f) / 32 * 32;
            int w = h * 10 / 7;
            if (w > mw * 0.95f)
            {
                w = static_cast<int>(mw * 0.95f) / 40 * 40;
                h = w * 7 / 10;
            }
            if (h > 896)
                sizes.push_back({"Largest", w, h});
            return sizes;
        }

        // Options. Features not built yet, or impossible on this device, are not listed: a row
        // appears once any non-default choice is available.
        std::vector<Row> optionRows()
        {
            Settings &s = current();
            std::vector<Row> rows;
            rows.push_back(heading("Display"));
            rows.push_back({"Window",
                            [&s] { return std::string(s.windowMode == WindowMode::Windowed ? "Windowed" : "Borderless full screen"); },
                            [&s](int) {
                                s.windowMode = s.windowMode == WindowMode::Windowed ? WindowMode::Borderless : WindowMode::Windowed;
                                changed();
                            },
                            {}, "Play in a window or fill the screen."});
            if (s.windowMode == WindowMode::Windowed)
            {
                rows.push_back({"Window size",
                                [&s] {
                                    const std::string dims = std::to_string(s.windowWidth) + "x" + std::to_string(s.windowHeight);
                                    for (const WindowSize &z : windowSizes())
                                        if (z.w == s.windowWidth && z.h == s.windowHeight)
                                            return std::string(z.name) + " (" + dims + ")";
                                    return "Custom (" + dims + ")";
                                },
                                [&s](int d) {
                                    const std::vector<WindowSize> sizes = windowSizes();
                                    const int n = static_cast<int>(sizes.size());
                                    int i = -1;
                                    for (int k = 0; k < n; ++k)
                                        if (sizes[k].w == s.windowWidth && sizes[k].h == s.windowHeight)
                                            i = k;
                                    if (i < 0) // Custom: step from the nearest size by height
                                    {
                                        i = 0;
                                        while (i + 1 < n && sizes[i + 1].h <= s.windowHeight)
                                            ++i;
                                        if (d < 0 && sizes[i].h < s.windowHeight)
                                            d = 0;
                                    }
                                    i = std::clamp(i + d, 0, n - 1);
                                    s.windowWidth = sizes[i].w;
                                    s.windowHeight = sizes[i].h;
                                    changed();
                                },
                                {}, "You can also drag the window's edges."});
            }
            if (aspectAvailability(Aspect::R16_9).ok)
            {
                static const std::vector<Aspect> aspects = {Aspect::R4_3, Aspect::R16_10, Aspect::R16_9, Aspect::R21_9, Aspect::R32_9};
                rows.push_back({"Aspect ratio", [&s] { return std::string(s.aspect == Aspect::R4_3 ? "4:3 (original)" : name(s.aspect)); },
                                [&s](int d) { cycle(s.aspect, aspects, d); changed(); }, {},
                                "Widens the 3D view to fill wider screens. 2D screens and the HUD stay 4:3.", false, true});
                static const std::vector<HudMode> huds = {HudMode::FourThree, HudMode::Edges};
                rows.push_back({"HUD position",
                                [&s] { return std::string(s.hud == HudMode::FourThree ? "4:3 centred" : "Screen edges"); },
                                [&s](int d) { cycle(s.hud, huds, d); changed(); }, {},
                                "Speedometer, map and timers: inside the centred 4:3 area or at the screen edges.", false, true});
            }
            if (refreshAvailability(capabilities(), 120).ok)
            {
                static const std::vector<int> rates = {60, 72, 90, 100, 120, 144};
                rows.push_back({"Frame rate", [&s] { return s.refreshRate == 60 ? std::string("60 fps (original)") : std::to_string(s.refreshRate) + " fps (interpolated)"; },
                                [&s](int d) {
                                    do
                                        cycle(s.refreshRate, rates, d);
                                    while (!refreshAvailability(capabilities(), s.refreshRate).ok);
                                    changed();
                                },
                                {}, "The game runs at 60; extra frames are rendered in between from interpolated positions."});
            }

            rows.push_back(heading("Graphics"));
            static const std::vector<int> levels = {1, 2, 4, 8, 16};
            rows.push_back({"Supersampling (SSAA)",
                            [&s] {
                                switch (s.superSampling)
                                {
                                case 1: return std::string("Off (640x448)");
                                case 2: return std::string("2x");
                                case 4: return std::string("4x (1280x896)");
                                case 8: return std::string("8x");
                                default: return std::string("16x (2560x1792)");
                                }
                            },
                            [&s](int d) {
                                const int i = std::clamp(indexOf(levels, s.superSampling) + d, 0, static_cast<int>(levels.size()) - 1);
                                s.superSampling = levels[i];
                                changed();
                            },
                            {}, "Samples per pixel. 4x and up also doubles the output resolution. Higher costs more GPU.", false, true});
            rows.push_back({"Mipmaps", [&s] { return std::string(s.sharpTextures ? "Off (sharpest)" : "On (original)"); },
                            [&s](int) { s.sharpTextures = !s.sharpTextures; changed(); }, {},
                            "Off always samples the full-size texture, so distant roads and signs stay sharp.", false, true});
            if (availability(capabilities(), AntiAliasing::Smaa).ok || availability(capabilities(), Upscaler::Fsr1).ok)
            {
                static const std::vector<AntiAliasing> aas = {AntiAliasing::None, AntiAliasing::Fxaa, AntiAliasing::Smaa, AntiAliasing::Taa};
                rows.push_back({"Anti-aliasing", [&s] { return std::string(s.aa == AntiAliasing::None ? "Off" : name(s.aa)); },
                                [&s](int d) {
                                    do
                                        cycle(s.aa, aas, d);
                                    while (!availability(capabilities(), s.aa).ok);
                                    changed();
                                },
                                {}, "Post-process anti-aliasing on top of supersampling.", false, true});
            }
            return rows;
        }

        const char *choiceName(rt::input::PlayerChoice c, int automatic)
        {
            using rt::input::PlayerChoice;
            switch (c)
            {
            case PlayerChoice::Player1: return "Player 1";
            case PlayerChoice::Player2: return "Player 2";
            case PlayerChoice::Off: return "Not playing";
            default: return automatic == 0 ? "Player 1" : automatic == 1 ? "Player 2" : "Not playing";
            }
        }

        // Left/Right on a device: Player 1, Player 2, Not playing (Auto is what a device starts as).
        void cyclePlayer(const rt::input::DeviceStatus &d, int dir)
        {
            using rt::input::PlayerChoice;
            static const std::vector<PlayerChoice> order = {PlayerChoice::Player1, PlayerChoice::Player2, PlayerChoice::Off};
            PlayerChoice c = rt::input::playerChoice(d.id);
            if (c == PlayerChoice::Auto)
                c = d.player == 0 ? PlayerChoice::Player1 : d.player == 1 ? PlayerChoice::Player2 : PlayerChoice::Off;
            cycle(c, order, dir);
            rt::input::setPlayerChoice(d.id, c);
        }

        std::string deviceLabel(const rt::input::DeviceStatus &d)
        {
            std::string label = d.name;
            if (d.batteryPercent >= 0)
                label += "  " + std::to_string(d.batteryPercent) + "%";
            return label;
        }

        const rt::input::DeviceStatus *findDevice(const std::vector<rt::input::DeviceStatus> &all, const std::string &id)
        {
            for (const auto &d : all)
                if (d.id == id)
                    return &d;
            return nullptr;
        }

        std::vector<Row> controllerRows()
        {
            std::vector<Row> rows;
            rows.push_back(heading("Devices"));
            for (const rt::input::DeviceStatus &d : rt::input::devices())
            {
                const std::string id = d.id;
                rows.push_back({deviceLabel(d),
                                [id] {
                                    const auto all = rt::input::devices();
                                    const auto *x = findDevice(all, id);
                                    return std::string(x ? choiceName(rt::input::playerChoice(id), x->player) : "");
                                },
                                [id](int dir) {
                                    const auto all = rt::input::devices();
                                    if (const auto *x = findDevice(all, id))
                                        cyclePlayer(*x, dir);
                                },
                                [id] {
                                    g_device = id;
                                    // The keyboard has only its keys to set up.
                                    go(id == "keyboard" ? Page::Buttons : Page::Device, id == "keyboard" ? 1 : 0);
                                },
                                d.id == "keyboard" ? "Left/Right: who this plays as. Select: keys."
                                                   : "Left/Right: who this plays as. Select: buttons and sticks."});
            }
            if (!rt::input::anyGamepad())
                rows.push_back(note("Connect a controller and it appears here."));
            else
                rows.push_back({"Vibration", [] { return percent(rt::input::vibrationOverall()); },
                                [](int d) { rt::input::setVibrationOverall(stepQuarter(rt::input::vibrationOverall(), d)); }, {},
                                "How strongly every controller shakes."});
            return rows;
        }

        std::vector<Row> deviceRows()
        {
            std::vector<Row> rows;
            const std::string id = g_device;
            const bool pad = id != "keyboard";
            rows.push_back({"Plays as",
                            [id] {
                                const auto all = rt::input::devices();
                                const auto *x = findDevice(all, id);
                                return std::string(x ? choiceName(rt::input::playerChoice(id), x->player) : "Disconnected");
                            },
                            [id](int dir) {
                                const auto all = rt::input::devices();
                                if (const auto *x = findDevice(all, id))
                                    cyclePlayer(*x, dir);
                            },
                            {}, "Player 2 joins once a device plays as player 2."});
            rows.push_back({"Buttons", {}, {}, [] { go(Page::Buttons); }, "Choose which control presses each button."});
            if (pad)
            {
                for (int right = 0; right < 2; ++right)
                    rows.push_back({right ? "Right stick deadzone" : "Left stick deadzone",
                                    [id, right] { return percent(rt::input::stickDeadzone(id, right)); },
                                    [id, right](int d) {
                                        const float v = std::round(rt::input::stickDeadzone(id, right) * 20.0f + d) / 20.0f;
                                        rt::input::setStickDeadzone(id, right, v);
                                    },
                                    {}, "Raise this if the car drifts when you let go of the stick."});
                rows.push_back({"Vibration", [id] { return percent(rt::input::vibration(id)); },
                                [id](int d) { rt::input::setVibration(id, stepQuarter(rt::input::vibration(id), d)); }, {},
                                "How strongly this controller shakes."});
                rows.push_back({"Test vibration", {}, {}, [id] { rt::input::testVibration(id); }, ""});
            }
            return rows;
        }

        const char *kButtonNames[] = {"Up", "Down", "Left", "Right", "Cross", "Circle", "Square", "Triangle",
                                      "L1", "R1", "L2", "R2", "Start", "Select", "L3", "R3"};

        std::vector<Row> buttonRows()
        {
            std::vector<Row> rows;
            const std::string id = g_device;
            Row header = heading("Game button");
            header.value = [id] { return std::string(id == "keyboard" ? "Your key" : "Your control"); };
            rows.push_back(header);
            std::string hint = id == "keyboard" ? "Select, then press a key." : "Select, then press a button.";
            if (rt::input::rebinding() >= 0)
            {
                const int left = static_cast<int>(std::ceil(rt::input::rebindSecondsLeft()));
                hint = std::string(id == "keyboard" ? "Press a key for " : "Press a button for ") + kButtonNames[rt::input::rebinding()] +
                       (id == "keyboard" ? ". Esc cancels (" : ". Guide cancels (") + std::to_string(left) + ")";
            }
            else if (const int other = rt::input::lastRebindSwappedWith(2.5f); other >= 0)
                hint = std::string("Swapped with ") + kButtonNames[other] + ".";
            for (int b = 0; b < 16; ++b)
                rows.push_back({kButtonNames[b],
                                [id, b] { return rt::input::rebinding() == b ? std::string("...") : rt::input::bindingText(id, b); },
                                {}, [id, b] { rt::input::startRebind(id, b); }, hint});
            return rows;
        }

        std::vector<Row> currentRows()
        {
            switch (g_page)
            {
            case Page::Root: return rootRows();
            case Page::Options: return optionRows();
            case Page::Controllers: return controllerRows();
            case Page::Device: return deviceRows();
            case Page::Buttons: return buttonRows();
            default: return {};
            }
        }

        bool selectable(const std::vector<Row> &rows, int i) { return i >= 0 && i < static_cast<int>(rows.size()) && !rows[i].heading; }

        // Moves the cursor to the next selectable row (wrapping).
        int step(const std::vector<Row> &rows, int from, int dir)
        {
            const int n = static_cast<int>(rows.size());
            for (int k = 1; k <= n; ++k)
            {
                const int i = ((from + dir * k) % n + n) % n;
                if (selectable(rows, i))
                    return i;
            }
            return from;
        }

        // Confirm boxes: a question and two choices, the safe one (index 1) first in focus.
        struct Confirm
        {
            const char *question, *detail, *yes, *no;
            std::function<void()> onYes;
        };

        Confirm currentConfirm()
        {
            if (g_page == Page::QuitConfirm)
                return {"Quit Road Trip?", "Anything since you last saved is lost.", "Quit", "Keep playing", [] {
                            close();
                            std::fflush(nullptr);
                            std::_Exit(0);
                        }};
            return {"Reset every option?", "Window and picture go back to how they started.", "Reset", "Cancel", [] {
                        current() = Settings{};
                        current().menuHintShown = true;
                        changed();
                        go(Page::Options, 1);
                    }};
        }

        void goBack()
        {
            switch (g_page)
            {
            case Page::Root: close(); break;
            case Page::Options: go(Page::Root, 1); break;
            case Page::Controllers: go(Page::Root, 2); break;
            case Page::Device: go(Page::Controllers, 1); break;
            case Page::Buttons:
                if (g_device == "keyboard")
                    go(Page::Controllers, 1);
                else
                    go(Page::Device, 1);
                break;
            case Page::QuitConfirm: go(Page::Root, 3); break;
            case Page::ResetConfirm: go(Page::Options, 1); break;
            default: break;
            }
        }

        // RT_MENU_SHOT: open the requested page once the game is running, picture it, quit.
        // RT_MENU_PAGE: root, display, graphics, controllers, device, buttons, quit, reset.
        void shotStep()
        {
            if (!g_shot)
                return;
            const char *atText = std::getenv("RT_MENU_AT");
            const uint64_t at = atText ? std::strtoull(atText, nullptr, 10) : 900;
            if (g_shotFrames < 0 && ps2_test::currentVblank() >= at)
            {
                const std::string page = std::getenv("RT_MENU_PAGE") ? std::getenv("RT_MENU_PAGE") : "root";
                open(Page::Root);
                if (page == "quit")
                    go(Page::QuitConfirm, 1);
                else if (page == "reset")
                    go(Page::ResetConfirm, 1);
                else if (page == "display")
                    go(Page::Options, 1);
                else if (page == "graphics")
                {
                    const auto rows = optionRows();
                    int i = 0;
                    while (i < static_cast<int>(rows.size()) && rows[i].label != "Supersampling (SSAA)")
                        ++i;
                    go(Page::Options, i);
                }
                else if (page == "controllers")
                    go(Page::Controllers, 1);
                else if (page == "device" || page == "buttons")
                {
                    const auto all = rt::input::devices();
                    g_device = all.size() > 1 ? all[1].id : all[0].id;
                    go(page == "device" ? Page::Device : Page::Buttons);
                }
                g_shotFrames = 0;
            }
        }

        bool isConfirm() { return g_page == Page::QuitConfirm || g_page == Page::ResetConfirm; }
    }

    void updatePauseMenu()
    {
        shotStep();
        const rt::input::MenuInput in = rt::input::menuInput();
        // Rebinding takes every control until it is done (or Escape / the time-out cancels it).
        if (rt::input::rebinding() >= 0)
        {
            rt::input::pollRebind();
            return;
        }
        if (in.toggleMenu)
        {
            if (g_page == Page::Closed)
                open(Page::Root);
            else
                close();
            return;
        }
        if (g_page == Page::Closed)
            return;
        if (isConfirm())
        {
            if (in.left || in.right || in.up || in.down)
                g_selected = 1 - std::clamp(g_selected, 0, 1);
            if (in.confirm)
            {
                if (g_selected == 0)
                    currentConfirm().onYes();
                else
                    goBack();
            }
            else if (in.back)
                goBack();
            return;
        }
        std::vector<Row> rows = currentRows();
        const int n = static_cast<int>(rows.size());
        if (n == 0)
        {
            if (in.back)
                goBack();
            return;
        }
        if (!selectable(rows, g_selected))
            g_selected = step(rows, std::clamp(g_selected, 0, n - 1), 1);
        if (in.up)
            g_selected = step(rows, g_selected, -1);
        if (in.down)
            g_selected = step(rows, g_selected, 1);
        Row &row = rows[g_selected];
        if ((in.left || in.right) && row.change)
            row.change(in.left ? -1 : 1);
        if (in.confirm)
        {
            if (row.activate)
                row.activate();
            else if (row.change)
                row.change(1);
        }
        else if (in.back)
            goBack();
        else if (in.extra)
        {
            if (g_page == Page::Options)
                go(Page::ResetConfirm, 1);
            else if (g_page == Page::Buttons)
                rt::input::resetBindings(g_device);
        }
    }

    bool pauseMenuWantsFrame()
    {
        if (g_toastPending && !current().menuHintShown && ps2_test::currentVblank() > 120)
        {
            g_toastPending = false;
            g_toastUntil = GetTime() + 8.0;
            current().menuHintShown = true;
            saveCurrent();
        }
        const double now = GetTime();
        return g_page != Page::Closed || now < g_toastUntil || now < g_closedAt + 0.2;
    }

    namespace
    {
        // The button prompts for the current page, in a blue prompt box under the panel's right edge.
        void drawPrompts(ImDrawList *dl, ImVec2 panelMax, float panelLeft, bool canChange)
        {
            struct P
            {
                const char *button, *label;
            };
            std::vector<P> ps;
            if (isConfirm())
                ps = {{"cross", "Select"}, {"triangle", "Back"}};
            else
            {
                ps.push_back({"cross", "Select"});
                if (canChange)
                    ps.push_back({"", "Change"});
                ps.push_back({"triangle", g_page == Page::Root ? "Resume" : "Back"});
                if (g_page == Page::Options)
                    ps.push_back({"square", "Reset all"});
                if (g_page == Page::Buttons)
                    ps.push_back({"square", "Default buttons"});
            }
            // Measured first, then drawn in a box right-aligned under the panel.
            const float gap = th::px(26), padX = th::px(20), h = th::px(58);
            auto drawAll = [&](float x, float y) {
                for (const P &p : ps)
                {
                    if (!*p.button)
                    {
                        // Left/Right: two small arrows and the label.
                        th::arrow(dl, ImVec2(x + th::px(8), y + th::px(20)), false);
                        th::arrow(dl, ImVec2(x + th::px(30), y + th::px(20)), true);
                        const ImVec2 ls = th::measure(th::Size::Hint, p.label);
                        th::text(dl, ImVec2(x + th::px(48), y + (th::px(40) - ls.y) * 0.5f), th::Size::Hint, th::col::Silver, p.label,
                                 th::col::OutlineBlue);
                        x += th::px(48) + ls.x + gap;
                    }
                    else
                        x += th::prompt(dl, ImVec2(x, y), p.button, p.label) + gap;
                }
                return x - gap;
            };
            // Width: the same layout drawn off screen, fully clipped.
            dl->PushClipRect(ImVec2(-2, -2), ImVec2(-1, -1));
            const float width = drawAll(-100000.0f, -100000.0f) + 100000.0f;
            dl->PopClipRect();
            const float w = width + padX * 2;
            const ImVec2 bmax(panelMax.x, panelMax.y + th::px(14) + h);
            const ImVec2 bmin(std::max(panelLeft, bmax.x - w), panelMax.y + th::px(14));
            th::promptBox(dl, bmin, bmax);
            drawAll(bmin.x + padX, bmin.y + (h - th::px(40)) * 0.5f);
        }

        void drawConfirm(ImDrawList *dl, ImVec2 vmin, ImVec2 vmax, float appear)
        {
            const Confirm c = currentConfirm();
            const float w = std::min(th::px(600), (vmax.x - vmin.x) * 0.9f), h = th::px(220);
            const ImVec2 min((vmin.x + vmax.x - w) * 0.5f, (vmin.y + vmax.y - h) * 0.5f + (1 - appear) * th::px(30));
            const ImVec2 max(min.x + w, min.y + h);
            th::dialogPanel(dl, min, max, c.question);
            ImVec2 ds = th::measure(th::Size::Hint, c.detail);
            th::text(dl, ImVec2((min.x + max.x - ds.x) * 0.5f, min.y + th::px(52)), th::Size::Hint, th::col::ListText, c.detail,
                     th::col::Black);
            // Two choices side by side; the bar sits behind the focused one.
            const char *labels[2] = {c.yes, c.no};
            // Two equal bars centred as a pair; the focused one is the selection bar.
            const float cw = th::px(220), ch = th::px(56), cy = max.y - th::px(40) - ch, gap = th::px(24);
            const float x0 = (min.x + max.x - (cw * 2 + gap)) * 0.5f;
            for (int i = 0; i < 2; ++i)
            {
                const ImVec2 a(x0 + i * (cw + gap), cy), b(a.x + cw, cy + ch);
                if (ImGui::IsMouseHoveringRect(a, b) && (ImGui::GetIO().MouseDelta.x != 0 || ImGui::GetIO().MouseDelta.y != 0))
                    g_selected = i;
                const bool on = g_selected == i;
                if (!on)
                    dl->AddRectFilled(a, b, th::col::ListBevelDark, th::px(8));
                if (on)
                {
                    th::selectionBar(dl, a, b);
                    th::horn(dl, ImVec2(a.x + th::px(2), (a.y + b.y) * 0.5f), th::px(26), static_cast<float>(GetTime()));
                }
                const ImVec2 ls = th::measure(th::Size::Body, labels[i]);
                th::text(dl, ImVec2((a.x + b.x - ls.x) * 0.5f - th::px(8), (a.y + b.y - ls.y) * 0.5f), th::Size::Body,
                         on ? th::col::ListSelected : th::col::ListText, labels[i], on ? th::col::OutlineBlue : th::col::Black);
                if (ImGui::IsMouseHoveringRect(a, b) && ImGui::IsMouseClicked(0))
                {
                    if (i == 0)
                        c.onYes();
                    else
                        goBack();
                    return;
                }
            }
            drawPrompts(dl, max, min.x, false);
        }

        void drawToast(ImDrawList *dl, ImVec2 vmax)
        {
            // First launch: how to find the menu, with the player's own buttons.
            const float h = th::px(66);
            const bool pad = rt::input::anyGamepad();
            const float y = vmax.y - h - th::px(32);
            // Measure, then draw right-aligned.
            auto layout = [&](float x0, bool draw) {
                float x = x0;
                const float gy = y + (h - th::px(40)) * 0.5f, ty = y + (h - th::fontSize(th::Size::Hint)) * 0.5f;
                auto word = [&](const char *s) {
                    if (draw)
                        th::text(dl, ImVec2(x, ty), th::Size::Hint, th::col::Silver, s, th::col::OutlineBlue);
                    x += th::measure(th::Size::Hint, s).x + th::px(10);
                };
                auto glyph = [&](const char *b) {
                    if (draw)
                        x += th::prompt(dl, ImVec2(x, gy), b, "") + th::px(10);
                    else
                    {
                        dl->PushClipRect(ImVec2(-2, -2), ImVec2(-1, -1));
                        x += th::prompt(dl, ImVec2(-100000, -100000), b, "") + th::px(10);
                        dl->PopClipRect();
                    }
                };
                if (pad)
                {
                    word("Menu:");
                    glyph("guide");
                    word("or hold");
                    glyph("select");
                    word("+");
                    glyph("start");
                }
                else
                {
                    word("Menu:");
                    glyph("Esc");
                }
                return x - x0;
            };
            const float w = layout(0, false) + th::px(36);
            const ImVec2 min(vmax.x - w - th::px(32), y), max(vmax.x - th::px(32), y + h);
            th::promptBox(dl, min, max);
            layout(min.x + th::px(22), true);
        }
    }

    void drawPauseMenu()
    {
        ImDrawList *dl = ImGui::GetForegroundDrawList();
        const ImGuiViewport *vp = ImGui::GetMainViewport();
        const ImVec2 vmin = vp->Pos, vmax(vp->Pos.x + vp->Size.x, vp->Pos.y + vp->Size.y);
        const double now = GetTime();
        const float dt = ImGui::GetIO().DeltaTime;

        if (g_page == Page::Closed)
        {
            if (now < g_closedAt + 0.2) // fade the scrim out after Resume
                th::scrim(dl, vmin, vmax, static_cast<float>(1.0 - (now - g_closedAt) / 0.2));
            if (now < g_toastUntil)
                drawToast(dl, vmax);
            return;
        }

        // Slide in over 0.18 s.
        const float t = std::clamp(static_cast<float>((now - g_openedAt) / 0.18), 0.0f, 1.0f);
        const float appear = 1.0f - (1.0f - t) * (1.0f - t);

        if (isConfirm())
        {
            th::scrim(dl, vmin, vmax, appear);
            drawConfirm(dl, vmin, vmax, appear);
            if (g_shot && g_shotFrames >= 0)
                ++g_shotFrames;
            return;
        }

        const std::vector<Row> rows = currentRows();
        const int n = static_cast<int>(rows.size());
        const int sel = n ? std::clamp(g_selected, 0, n - 1) : 0;
        // A focused picture option moves the panel to the bottom and lifts the scrim, so the change
        // can be seen.
        const bool preview = n && rows[sel].preview;
        th::scrim(dl, vmin, vmax, appear * (preview ? 0.3f : 1.0f));

        const bool wide = g_page != Page::Root;
        const float rowH = th::px(62);
        const float width = std::min(th::px(wide ? 880 : 520), vp->Size.x * 0.92f);
        const float pad = th::px(30);
        const float hintH = th::px(62);
        const float promptsH = th::px(90);
        // While previewing, the panel is a strip with just the focused row and its hint.
        const int visible = preview ? 1 : std::max(1, std::min(n, static_cast<int>(vp->Size.y * 0.62f / rowH)));
        const float bodyH = rowH * visible;
        bool anyHint = false;
        for (const Row &r : rows)
            anyHint |= !r.hint.empty();
        const float height = pad + bodyH + (anyHint ? th::px(10) + hintH : 0.0f) + pad * 0.6f;
        // Keep the cursor in view (with the heading above it when there is room).
        if (preview)
            g_scroll = static_cast<float>(sel);
        else if (sel < g_scroll + 0.5f)
            g_scroll = static_cast<float>(std::max(0, sel - (sel > 0 && rows[sel - 1].heading ? 1 : 0)));
        if (sel > g_scroll + visible - 1)
            g_scroll = static_cast<float>(sel - visible + 1);
        g_scroll = std::clamp(g_scroll, 0.0f, static_cast<float>(std::max(0, n - visible)));
        const int first = static_cast<int>(g_scroll);

        const float targetY = preview ? vmax.y - height - promptsH : vp->Pos.y + (vp->Size.y - height - promptsH * 0.5f) * 0.5f;
        g_panelY = g_panelY < 0 ? targetY : g_panelY + (targetY - g_panelY) * std::min(1.0f, dt * 14.0f);
        const ImVec2 min(vp->Pos.x + (vp->Size.x - width) * 0.5f, g_panelY + (1 - appear) * th::px(40));
        const ImVec2 max(min.x + width, min.y + height);
        std::string title = "Menu";
        if (g_page == Page::Options)
            title = "Options";
        else if (g_page == Page::Controllers)
            title = "Controllers";
        else if (g_page == Page::Device || g_page == Page::Buttons)
        {
            const auto all = rt::input::devices();
            const auto *d = findDevice(all, g_device);
            title = d ? d->name : "Controller";
            if (g_page == Page::Buttons)
                title += ": buttons";
        }
        th::dialogPanel(dl, min, max, title.c_str());

        // Rows: mouse hover selects, click activates, clicking the value side changes it.
        const float top = min.y + pad;
        const float left = min.x + th::px(30), right = max.x - th::px(30);
        auto rowTop = [&](int i) { return top + rowH * static_cast<float>(i - first); };
        for (int i = first; i < std::min(n, first + visible); ++i)
        {
            const ImVec2 a(left, rowTop(i)), b(right, rowTop(i) + rowH);
            if (rows[i].heading)
                continue;
            if (ImGui::IsMouseHoveringRect(a, b) && (ImGui::GetIO().MouseDelta.x != 0 || ImGui::GetIO().MouseDelta.y != 0))
                g_selected = i;
            if (ImGui::IsMouseHoveringRect(a, b) && ImGui::IsMouseClicked(0))
            {
                Row r = rows[i];
                if (r.activate)
                    r.activate();
                else if (r.change)
                    r.change(ImGui::GetIO().MousePos.x < (a.x + b.x) * 0.5f ? -1 : 1);
                break;
            }
        }
        if (ImGui::IsMouseHoveringRect(min, max) && ImGui::GetIO().MouseWheel != 0)
            g_scroll = std::clamp(g_scroll - ImGui::GetIO().MouseWheel, 0.0f, static_cast<float>(std::max(0, n - visible)));

        dl->PushClipRect(ImVec2(min.x, top), ImVec2(max.x, top + bodyH), true);
        const float target = rowTop(sel);
        g_barY = g_barY < 0 ? target : g_barY + (target - g_barY) * std::min(1.0f, dt * 18.0f);
        if (n && !rows[sel].heading)
        {
            const ImVec2 bmin(left + th::px(56), g_barY + th::px(7)), bmax(right, g_barY + rowH - th::px(7));
            th::selectionBar(dl, bmin, bmax);
            th::horn(dl, ImVec2(bmin.x + th::px(2), (bmin.y + bmax.y) * 0.5f), th::px(28), static_cast<float>(now));
        }
        for (int i = first; i < std::min(n, first + visible); ++i)
        {
            const float y0 = rowTop(i);
            if (rows[i].note)
            {
                const ImVec2 ns = th::measure(th::Size::Hint, rows[i].label.c_str());
                dl->AddText(th::font(), th::fontSize(th::Size::Hint),
                            ImVec2((left + right - ns.x) * 0.5f, y0 + (rowH - ns.y) * 0.5f), IM_COL32(0xCF, 0xE8, 0xF5, 0xFF),
                            rows[i].label.c_str());
                continue;
            }
            if (rows[i].heading)
            {
                // Gold text with a black outline, then a rounded rule to the right edge.
                const ImVec2 hs = th::measure(th::Size::Hint, rows[i].label.c_str());
                const float hy = y0 + rowH - hs.y - th::px(8);
                th::text(dl, ImVec2(left + th::px(8), hy), th::Size::Hint, th::col::Heading, rows[i].label.c_str(), th::col::Black);
                float ruleEnd = right - th::px(8);
                if (rows[i].value)
                {
                    const std::string v = rows[i].value();
                    const ImVec2 vs = th::measure(th::Size::Hint, v.c_str());
                    th::text(dl, ImVec2(right - th::px(40) - vs.x, hy), th::Size::Hint, th::col::Heading, v.c_str(), th::col::Black);
                    ruleEnd = right - th::px(52) - vs.x;
                }
                const float ry = hy + hs.y * 0.55f;
                dl->AddLine(ImVec2(left + th::px(20) + hs.x, ry), ImVec2(ruleEnd, ry), th::col::ListBevelDark, th::px(3));
                continue;
            }
            const bool on = i == sel;
            const float y = y0 + (rowH - th::fontSize(th::Size::Body)) * 0.5f;
            const ImU32 colour = on ? th::col::ListSelected : th::col::ListText;
            const ImU32 outline = on ? th::col::OutlineBlue : th::col::Black;
            th::text(dl, ImVec2(left + th::px(84), y), th::Size::Body, colour, rows[i].label.c_str(), outline);
            if (rows[i].value)
            {
                const std::string v = rows[i].value();
                const ImVec2 size = th::measure(th::Size::Body, v.c_str());
                const float vx = right - th::px(on && rows[i].change ? 72 : 40) - size.x;
                // Bindings (an action's control) in white so they read apart from the action.
                const ImU32 vc = on ? th::col::ListSelected : g_page == Page::Buttons ? th::col::White : th::col::ListText;
                th::text(dl, ImVec2(vx, y), th::Size::Body, vc, v.c_str(), outline);
                if (on && rows[i].change)
                {
                    const float cy = y0 + rowH * 0.5f;
                    th::arrow(dl, ImVec2(vx - th::px(20), cy), false);
                    th::arrow(dl, ImVec2(right - th::px(54), cy), true);
                }
            }
        }
        dl->PopClipRect();
        // More rows above or below: small arrows at the list's edge (not on the preview strip).
        if (first > 0 && !preview)
            dl->AddTriangleFilled(ImVec2(max.x - th::px(60), top - th::px(4)), ImVec2(max.x - th::px(48), top - th::px(16)),
                                  ImVec2(max.x - th::px(36), top - th::px(4)), th::col::Heading);
        if (first + visible < n && !preview)
            dl->AddTriangleFilled(ImVec2(max.x - th::px(60), top + bodyH + th::px(2)), ImVec2(max.x - th::px(36), top + bodyH + th::px(2)),
                                  ImVec2(max.x - th::px(48), top + bodyH + th::px(14)), th::col::Heading);

        // The focused row's explanation in the sunken band.
        const ImVec2 hmin(left, top + bodyH + th::px(16)), hmax(right, top + bodyH + th::px(16) + hintH - th::px(6));
        if (anyHint)
            th::hintBand(dl, hmin, hmax);
        if (anyHint && n && !rows[sel].hint.empty())
        {
            const ImVec2 hs = th::measure(th::Size::Hint, rows[sel].hint.c_str());
            th::text(dl, ImVec2(hmin.x + th::px(18), (hmin.y + hmax.y - hs.y) * 0.5f), th::Size::Hint, th::col::ListSelected,
                     rows[sel].hint.c_str(), th::col::Black);
        }
        drawPrompts(dl, max, min.x, n && rows[sel].change != nullptr);

        if (g_shot && g_shotFrames >= 0)
            ++g_shotFrames;
    }

    void menuShotAfterFrame()
    {
        if (g_shot && g_shotFrames >= 20)
        {
            Image image = LoadImageFromScreen();
            ExportImage(image, g_shot);
            UnloadImage(image);
            std::fflush(nullptr);
            std::_Exit(0);
        }
    }
}
