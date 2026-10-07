// The first-run setup screens (see SetupScreen.h).

#include "ui/SetupScreen.h"

#include "platform/ProgressEta.h"
#include "ui/Theme.h"

#include "imgui.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <deque>
#include <fstream>
#include <initializer_list>
#include <string>
#include <vector>

namespace rt::ui
{
    namespace th = theme;

    namespace
    {
        ProgressEta g_eta;

        // Button focus: which screen it belongs to (reset to that screen's default on a new one).
        const void *g_focusScreen = nullptr;
        int g_focus = 0;

        constexpr ImU32 kFocusFill = IM_COL32(0x2E, 0x5B, 0xFF, 0xFF), kFocusRim = IM_COL32(0xFF, 0xE1, 0x4D, 0xFF);
        constexpr ImU32 kIdleFill = IM_COL32(0x24, 0x30, 0x7A, 0xFF), kIdleRim = IM_COL32(0x0B, 0x12, 0x40, 0xFF);
        constexpr ImU32 kWarning = IM_COL32(0xFF, 0xE1, 0x4D, 0xFF);
        constexpr ImU32 kLogFill = IM_COL32(0x0B, 0x12, 0x40, 0xFF), kLogText = IM_COL32(0xE6, 0xEC, 0xFF, 0xFF);

        // The panel: at most 1.2x the screen height wide (lines a reader can take in; 1296 px at
        // 1080p, whatever the UI scale), a little above centre.
        float panelWidth()
        {
            const ImGuiViewport *vp = ImGui::GetMainViewport();
            return std::min(vp->Size.x - th::px(80), vp->Size.y * 1.2f);
        }
        struct Panel
        {
            ImVec2 min, max;
            float pad, inner;
        };
        void background(double now);
        Panel panel(float contentHeight)
        {
            background(ImGui::GetTime());
            const ImGuiViewport *vp = ImGui::GetMainViewport();
            Panel p;
            const float width = panelWidth();
            p.pad = th::px(56);
            p.inner = width - p.pad * 2;
            const float height = contentHeight + p.pad * 2;
            p.min = ImVec2(vp->Pos.x + (vp->Size.x - width) * 0.5f,
                           vp->Pos.y + std::max(th::px(24), (vp->Size.y - height) * 0.5f - vp->Size.y * 0.04f));
            p.max = ImVec2(p.min.x + width, p.min.y + height);
            th::dialogPanel(ImGui::GetForegroundDrawList(), p.min, p.max, "Road Trip");
            return p;
        }

        float wrappedHeight(th::Size size, const std::string &s, float width)
        {
            return th::font()->CalcTextSizeA(th::fontSize(size), FLT_MAX, width, s.c_str()).y;
        }

        // Text wrapped to `width` with the game's dark outline (readable on the panel's blue);
        // returns the height used.
        float wrapped(ImDrawList *dl, ImVec2 pos, th::Size size, ImU32 colour, const std::string &s, float width)
        {
            const float fs = th::fontSize(size);
            const float o = std::max(1.0f, th::px(2.0f));
            static const ImVec2 dirs[8] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-0.7f, -0.7f}, {0.7f, -0.7f}, {-0.7f, 0.7f}, {0.7f, 0.7f}};
            for (const ImVec2 &d : dirs)
                dl->AddText(th::font(), fs, ImVec2(pos.x + d.x * o, pos.y + d.y * o), th::col::Outline, s.c_str(), nullptr, width);
            dl->AddText(th::font(), fs, pos, colour, s.c_str(), nullptr, width);
            return wrappedHeight(size, s, width);
        }

        // The screen's buttons, right-aligned at the bottom of the panel, left to right. The focused
        // one has the gold rim and the horn cursor; left/right (D-pad, stick, arrow keys) moves the
        // focus, A, Start or Enter presses it, a tap presses the one tapped. Returns the pressed
        // index, or -1.
        int buttons(const Panel &p, std::initializer_list<const char *> labels, int defaultFocus, double now)
        {
            ImDrawList *dl = ImGui::GetForegroundDrawList();
            const int n = static_cast<int>(labels.size());
            const void *screen = labels.begin()[0]; // the labels' storage identifies the screen
            if (g_focusScreen != screen)
            {
                g_focusScreen = screen;
                g_focus = defaultFocus;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_GamepadDpadLeft) || ImGui::IsKeyPressed(ImGuiKey_GamepadLStickLeft) ||
                ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
                g_focus = std::max(0, g_focus - 1);
            if (ImGui::IsKeyPressed(ImGuiKey_GamepadDpadRight) || ImGui::IsKeyPressed(ImGuiKey_GamepadLStickRight) ||
                ImGui::IsKeyPressed(ImGuiKey_RightArrow))
                g_focus = std::min(n - 1, g_focus + 1);

            const float h = th::px(68), gap = th::px(40), padX = th::px(40);
            float x = p.max.x - p.pad;
            const float y1 = p.max.y - p.pad + th::px(10), y0 = y1 - h;
            std::vector<std::pair<ImVec2, ImVec2>> rects(n);
            for (int i = n - 1; i >= 0; --i)
            {
                const float w = th::measure(th::Size::Body, labels.begin()[i]).x + padX * 2;
                rects[i] = {ImVec2(x - w, y0), ImVec2(x, y1)};
                x -= w + gap;
            }
            int pressed = -1;
            for (int i = 0; i < n; ++i)
            {
                const auto [a, b] = rects[i];
                if (ImGui::IsMouseClicked(0) && ImGui::IsMouseHoveringRect(a, b, false))
                {
                    g_focus = i;
                    pressed = i;
                }
                const bool focused = i == g_focus;
                const float r = h * 0.5f;
                dl->AddRectFilled(a, b, focused ? kFocusFill : kIdleFill, r);
                dl->AddRect(a, b, focused ? kFocusRim : kIdleRim, r, 0, th::px(focused ? 3.0f : 2.0f));
                const ImVec2 size = th::measure(th::Size::Body, labels.begin()[i]);
                th::text(dl, ImVec2((a.x + b.x - size.x) * 0.5f, (a.y + b.y - size.y) * 0.5f), th::Size::Body,
                         focused ? th::col::White : th::col::Silver, labels.begin()[i]);
                if (focused)
                    th::horn(dl, ImVec2(a.x - th::px(10), (a.y + b.y) * 0.5f), th::px(28), static_cast<float>(now));
            }
            if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter) ||
                ImGui::IsKeyPressed(ImGuiKey_GamepadFaceDown) || ImGui::IsKeyPressed(ImGuiKey_GamepadStart))
                pressed = g_focus;
            return pressed;
        }

        // The last lines of the log named after "Details: " in the error, if any.
        std::vector<std::string> logTail(const std::string &error, size_t lines)
        {
            const size_t at = error.find("Details: ");
            if (at == std::string::npos)
                return {};
            std::ifstream in(error.substr(at + 9));
            std::deque<std::string> tail;
            for (std::string line; std::getline(in, line);)
            {
                if (line.empty())
                    continue;
                tail.push_back(line);
                if (tail.size() > lines)
                    tail.pop_front();
            }
            return {tail.begin(), tail.end()};
        }

        std::string withoutDetails(const std::string &error)
        {
            const size_t at = error.find("\n\nDetails: ");
            return at == std::string::npos ? error : error.substr(0, at);
        }

        // Behind the panels: a sunny road drawn from shapes (sky, drifting clouds, hills, grass and a
        // road with moving centre dashes), so setup doesn't sit in a black void. Original, no game art.
        void background(double now)
        {
            ImDrawList *dl = ImGui::GetBackgroundDrawList();
            const ImGuiViewport *vp = ImGui::GetMainViewport();
            const ImVec2 o = vp->Pos, sz = vp->Size;
            const float horizon = o.y + sz.y * 0.62f;
            dl->AddRectFilledMultiColor(o, ImVec2(o.x + sz.x, horizon), IM_COL32(0x4F, 0xB4, 0xF2, 0xFF),
                                        IM_COL32(0x4F, 0xB4, 0xF2, 0xFF), IM_COL32(0xCD, 0xEB, 0xFF, 0xFF),
                                        IM_COL32(0xCD, 0xEB, 0xFF, 0xFF));
            // Clouds: three puffs each, drifting right and wrapping.
            for (int i = 0; i < 4; ++i)
            {
                const float speed = sz.x * (0.010f + 0.004f * i);
                const float x = std::fmod(static_cast<float>(now) * speed + sz.x * (0.13f + 0.29f * i), sz.x * 1.3f) - sz.x * 0.15f;
                const float y = o.y + sz.y * (0.08f + 0.09f * (i % 3)), r = sz.y * (0.035f + 0.008f * (i % 2));
                for (const ImVec2 d : {ImVec2(0, 0), ImVec2(r * 1.1f, -r * 0.35f), ImVec2(r * 2.1f, 0)})
                    dl->AddCircleFilled(ImVec2(o.x + x + d.x, y + d.y), r, IM_COL32(0xFF, 0xFF, 0xFF, 0xE6));
            }
            // Rolling hills on the horizon, then the grass.
            for (int i = 0; i < 5; ++i)
            {
                const float cx = o.x + sz.x * (i * 0.25f), r = sz.y * (0.16f + 0.04f * (i % 2));
                dl->AddCircleFilled(ImVec2(cx, horizon + r * 0.55f), r, IM_COL32(0x7C, 0xCC, 0x55, 0xFF), 48);
            }
            dl->AddRectFilled(ImVec2(o.x, horizon), ImVec2(o.x + sz.x, o.y + sz.y), IM_COL32(0x5D, 0xBB, 0x3A, 0xFF));
            // The road: a wedge to a vanishing point, white edges, dashes coming towards the viewer.
            const float cx = o.x + sz.x * 0.5f, bottom = o.y + sz.y, halfB = sz.x * 0.36f, halfT = sz.x * 0.015f;
            auto at = [&](float t, float side) // t: 0 horizon .. 1 bottom
            {
                const float y = horizon + (bottom - horizon) * t;
                return ImVec2(cx + side * (halfT + (halfB - halfT) * t), y);
            };
            const ImVec2 road[4] = {at(0, -1), at(0, 1), at(1, 1), at(1, -1)};
            dl->AddConvexPolyFilled(road, 4, IM_COL32(0x8A, 0x8A, 0x8A, 0xFF));
            dl->AddLine(at(0, -1), at(1, -1), IM_COL32(0xFF, 0xFF, 0xFF, 0xFF), th::px(6));
            dl->AddLine(at(0, 1), at(1, 1), IM_COL32(0xFF, 0xFF, 0xFF, 0xFF), th::px(6));
            const float phase = static_cast<float>(std::fmod(now * 0.9, 1.0));
            for (int i = 0; i < 7; ++i)
            {
                // Perspective: equal steps in depth are squeezed towards the horizon.
                const float t0 = std::pow((i + phase) / 7.0f, 2.0f), t1 = std::pow((i + phase + 0.45f) / 7.0f, 2.0f);
                const float w0 = th::px(3) + sz.x * 0.012f * t0, w1 = th::px(3) + sz.x * 0.012f * std::min(t1, 1.0f);
                const ImVec2 a = at(t0, 0), b = at(std::min(t1, 1.0f), 0);
                const ImVec2 dash[4] = {ImVec2(a.x - w0, a.y), ImVec2(a.x + w0, a.y), ImVec2(b.x + w1, b.y), ImVec2(b.x - w1, b.y)};
                dl->AddConvexPolyFilled(dash, 4, IM_COL32(0xFF, 0xFF, 0xFF, 0xFF));
            }
        }

        // The progress bar: gold fill on a dark track with a black outline, eased so it never jumps,
        // the horn riding the fill's edge; `fraction` < 0 sweeps a stripe (unmeasured work).
        float g_shownFraction = 0.0f;
        void bar(ImDrawList *dl, ImVec2 min, ImVec2 max, float fraction, double now)
        {
            const float r = (max.y - min.y) * 0.5f;
            dl->AddRectFilled(ImVec2(min.x - th::px(4), min.y - th::px(4)), ImVec2(max.x + th::px(4), max.y + th::px(4)),
                              th::col::Black, r + th::px(4));
            dl->AddRectFilled(min, max, kLogFill, r);
            float x0 = min.x, x1;
            if (fraction < 0.0f)
            {
                const float w = (max.x - min.x) * 0.25f;
                const float t = static_cast<float>(std::fmod(now * 0.6, 1.0));
                x0 = std::max(min.x - w + (max.x - min.x + w) * t, min.x);
                x1 = std::min(x0 + w, max.x);
                g_shownFraction = 0.0f;
            }
            else
            {
                // Ease towards the real value (about 250 ms), never backwards.
                const float k = std::min(1.0f, ImGui::GetIO().DeltaTime / 0.25f * 3.0f);
                g_shownFraction = fraction < g_shownFraction ? fraction : g_shownFraction + (fraction - g_shownFraction) * k;
                x1 = min.x + (max.x - min.x) * std::clamp(g_shownFraction, 0.0f, 1.0f);
            }
            // The fill is a pill of its own, round at both ends however short (at least as long as
            // it is tall): deeper gold below, bright above, a highlight along the top, as the game's
            // gauges have.
            float right = x1;
            if (x1 - x0 >= 1.0f)
            {
                right = std::min(max.x, std::max(x1, x0 + 2.0f * r));
                const ImVec2 a(x0, min.y), b(right, max.y);
                dl->AddRectFilled(a, b, IM_COL32(0xF5, 0xB8, 0x00, 0xFF), r);
                dl->AddRectFilled(a, ImVec2(right, (min.y + max.y) * 0.5f + th::px(2)), IM_COL32(0xFF, 0xE1, 0x4D, 0xFF), r,
                                  ImDrawFlags_RoundCornersTop);
                if (right - x0 > r * 1.2f + th::px(6))
                    dl->AddRectFilled(ImVec2(x0 + r * 0.6f, min.y + th::px(6)), ImVec2(right - r * 0.6f, min.y + th::px(11)),
                                      IM_COL32(0xFF, 0xF4, 0xB0, 0xC0), th::px(3));
            }
            dl->AddRect(min, max, th::col::Black, r, 0, th::px(2));
            if (fraction > 0.0f && x1 - x0 >= 1.0f)
                // (The bell stays inside the track at the end.)
                th::horn(dl, ImVec2(std::min(right + th::px(6), max.x - th::px(6)), (min.y + max.y) * 0.5f), th::px(36),
                         static_cast<float>(now));
        }

        constexpr float kHeadingH = 68, kButtonsH = 110;
    }

    SetupAction drawSetupScreen(const TaskProgress &progress, double now, const char *heading)
    {
        ImDrawList *dl = ImGui::GetForegroundDrawList();
        const float inner = panelWidth() - th::px(56) * 2;

        if (progress.failed)
        {
            const auto tail = logTail(progress.error(), 6);
            const std::string message = withoutDetails(progress.error()) +
                                        (tail.empty() ? " Try again." : " Try again; if it keeps failing, the details below are "
                                                                        "saved in the app's build log.");
            const float lineH = th::fontSize(th::Size::Hint) * 0.75f;
            const float logH = tail.empty() ? 0.0f : th::px(24) + lineH * tail.size() + th::px(16);
            const Panel p = panel(th::px(kHeadingH) + wrappedHeight(th::Size::Body, message, inner) + logH + th::px(kButtonsH));
            float y = p.min.y + p.pad;
            th::text(dl, ImVec2(p.min.x + p.pad, y), th::Size::Heading, th::col::Heading, "Setup didn't finish", th::col::Black);
            y += th::px(kHeadingH);
            y += wrapped(dl, ImVec2(p.min.x + p.pad, y), th::Size::Body, th::col::White, message, p.inner);
            if (!tail.empty())
            {
                y += th::px(24);
                const ImVec2 lmin(p.min.x + p.pad, y), lmax(p.max.x - p.pad, y + lineH * tail.size() + th::px(16));
                dl->AddRectFilled(lmin, lmax, kLogFill, th::px(12));
                dl->PushClipRect(ImVec2(lmin.x + th::px(12), lmin.y), ImVec2(lmax.x - th::px(12), lmax.y), true);
                float ly = y + th::px(8);
                for (const auto &line : tail)
                {
                    dl->AddText(th::font(), lineH, ImVec2(lmin.x + th::px(16), ly), kLogText, line.c_str());
                    ly += lineH;
                }
                dl->PopClipRect();
            }
            switch (buttons(p, {"Close", "Try again"}, 1, now))
            {
            case 0: return SetupAction::Close;
            case 1: return SetupAction::Retry;
            default: return SetupAction::None;
            }
        }

        const double left = g_eta.update(progress, now);
        const uint64_t done = progress.done.load(), total = progress.total.load();
        const float fraction = total ? static_cast<float>(std::min<double>(1.0, static_cast<double>(done) / total)) : -1.0f;

        const Panel p = panel(th::px(kHeadingH + 56 + 44 + 18 + 40 + 30 + 34));
        float y = p.min.y + p.pad;
        th::text(dl, ImVec2(p.min.x + p.pad, y), th::Size::Heading, th::col::Heading, heading ? heading : "Getting Road Trip ready",
                 th::col::Black);
        y += th::px(kHeadingH);

        char step[64] = "";
        if (progress.steps > 1)
            std::snprintf(step, sizeof(step), "Step %d of %d: ", std::max(1, progress.step.load()), progress.steps.load());
        const std::string phase = step + progress.phase();
        th::text(dl, ImVec2(p.min.x + p.pad, y), th::Size::Body, th::col::White, phase.c_str());
        y += th::px(56);

        bar(dl, ImVec2(p.min.x + p.pad, y), ImVec2(p.max.x - p.pad, y + th::px(44)), fraction, now);
        y += th::px(44 + 18);

        // The percentage on the left, the time left on the right (elapsed time while unmeasured).
        char pct[32];
        if (fraction >= 0.0f)
            std::snprintf(pct, sizeof(pct), "%d%%", static_cast<int>(fraction * 100.0f));
        else
        {
            const int s = static_cast<int>(g_eta.phaseSeconds(now));
            std::snprintf(pct, sizeof(pct), "%d:%02d", s / 60, s % 60);
        }
        th::text(dl, ImVec2(p.min.x + p.pad, y), th::Size::Body, th::col::White, pct);
        // The time left once there is an estimate (none for short steps: only the percentage).
        std::string eta = fraction >= 1.0f ? std::string() : ProgressEta::describe(left);
        if (eta.empty() && fraction < 0.0f)
            eta = "Working...";
        if (!eta.empty())
        {
            const ImVec2 es = th::measure(th::Size::Body, eta.c_str());
            th::text(dl, ImVec2(p.max.x - p.pad - es.x, y), th::Size::Body, th::col::White, eta.c_str());
        }
        y += th::px(40 + 30);

        const std::string detail = progress.detail();
        wrapped(dl, ImVec2(p.min.x + p.pad, y), th::Size::Hint, th::col::White,
                detail.empty() ? "This only happens once. You can switch to other apps meanwhile." : detail, p.inner);
        return SetupAction::None;
    }

    WelcomeAction drawWelcomeScreen(const std::string &note)
    {
        ImDrawList *dl = ImGui::GetForegroundDrawList();
        const float inner = panelWidth() - th::px(56) * 2;
        const std::string body = "Road Trip needs your own copy of the game. Choose the disc image you made from it "
                                 "(.bin, .iso or .chd).";
        const float noteH = note.empty() ? 0.0f : th::px(24) + wrappedHeight(th::Size::Hint, note, inner);
        const Panel p = panel(th::px(kHeadingH) + wrappedHeight(th::Size::Body, body, inner) + noteH + th::px(kButtonsH));
        float y = p.min.y + p.pad;
        th::text(dl, ImVec2(p.min.x + p.pad, y), th::Size::Heading, th::col::Heading, "Welcome!", th::col::Black);
        y += th::px(kHeadingH);
        y += wrapped(dl, ImVec2(p.min.x + p.pad, y), th::Size::Body, th::col::White, body, p.inner);
        if (!note.empty())
        {
            y += th::px(24);
            wrapped(dl, ImVec2(p.min.x + p.pad, y), th::Size::Hint, kWarning, note, p.inner);
        }
        return buttons(p, {"Choose disc image"}, 0, ImGui::GetTime()) == 0 ? WelcomeAction::Choose : WelcomeAction::None;
    }

    WelcomeAction drawImageWarning(ImageWarning kind)
    {
        ImDrawList *dl = ImGui::GetForegroundDrawList();
        const float inner = panelWidth() - th::px(56) * 2;
        const char *heading = "This disc image looks different";
        std::string body = "It isn't the usual copy of Road Trip (USA), so it may be damaged or from another release. "
                           "It might still work.";
        if (kind == ImageWarning::Damaged)
        {
            heading = "This disc image is damaged";
            body = "Part of this .chd no longer matches what was recorded when it was made. Make a new one from your disc "
                   "if you can.";
        }
        else if (kind == ImageWarning::Unverified)
        {
            heading = "This .chd can't be checked";
            body = "It isn't damaged, but it was made in a way that can't be compared with the known good copy. It will "
                   "most likely work.";
        }
        const Panel p = panel(th::px(kHeadingH) + wrappedHeight(th::Size::Body, body, inner) + th::px(kButtonsH));
        float y = p.min.y + p.pad;
        th::text(dl, ImVec2(p.min.x + p.pad, y), th::Size::Heading, th::col::Heading, heading, th::col::Black);
        y += th::px(kHeadingH);
        wrapped(dl, ImVec2(p.min.x + p.pad, y), th::Size::Body, th::col::White, body, p.inner);
        // The safe choice has the focus: another image, unless this one is most likely fine.
        if (kind == ImageWarning::Unverified)
        {
            const int b = buttons(p, {"Choose another", "Use it"}, 1, ImGui::GetTime());
            return b == 0 ? WelcomeAction::Choose : b == 1 ? WelcomeAction::UseAnyway : WelcomeAction::None;
        }
        const int b = buttons(p, {"Use it anyway", "Choose another"}, 1, ImGui::GetTime());
        return b == 0 ? WelcomeAction::UseAnyway : b == 1 ? WelcomeAction::Choose : WelcomeAction::None;
    }

    void drawFadeOut(float amount)
    {
        const ImGuiViewport *vp = ImGui::GetMainViewport();
        const int a = static_cast<int>(255.0f * std::clamp(amount, 0.0f, 1.0f));
        ImGui::GetForegroundDrawList()->AddRectFilled(vp->Pos, ImVec2(vp->Pos.x + vp->Size.x, vp->Pos.y + vp->Size.y),
                                                      IM_COL32(0, 0, 0, a));
    }
}
