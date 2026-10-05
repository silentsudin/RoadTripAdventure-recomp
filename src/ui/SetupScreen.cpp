// The first-run setup screen (see SetupScreen.h).

#include "ui/SetupScreen.h"

#include "platform/ProgressEta.h"
#include "ui/Theme.h"

#include "imgui.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <deque>
#include <fstream>
#include <string>
#include <vector>

namespace rt::ui
{
    namespace th = theme;

    namespace
    {
        ProgressEta g_eta;

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

        // Text wrapped to `width`, drawn from `pos`; returns the height used.
        float wrapped(ImDrawList *dl, ImVec2 pos, th::Size size, ImU32 colour, const std::string &s, float width)
        {
            const float fs = th::fontSize(size);
            const ImVec2 extent = th::font()->CalcTextSizeA(fs, FLT_MAX, width, s.c_str());
            dl->AddText(th::font(), fs, pos, colour, s.c_str(), nullptr, width);
            return extent.y;
        }

        // The screen's one action as a button (setup runs before the input layer, so no button glyphs:
        // a tap on it, or any confirm button or key, presses it). Bottom right of the panel.
        bool actionButton(ImDrawList *dl, ImVec2 panelMax, float pad, const char *label)
        {
            const ImVec2 size = th::measure(th::Size::Body, label);
            const ImVec2 max(panelMax.x - pad, panelMax.y - pad + th::px(8));
            const ImVec2 min(max.x - size.x - th::px(72), max.y - th::px(64));
            th::pillTab(dl, min, max, true, label);
            return (ImGui::IsMouseClicked(0) && ImGui::IsMouseHoveringRect(min, max)) || ImGui::IsKeyPressed(ImGuiKey_Enter) ||
                   ImGui::IsKeyPressed(ImGuiKey_KeypadEnter) || ImGui::IsKeyPressed(ImGuiKey_GamepadFaceDown) ||
                   ImGui::IsKeyPressed(ImGuiKey_GamepadFaceRight) || ImGui::IsKeyPressed(ImGuiKey_GamepadStart);
        }

        // The game's blue bar with a gold cap; `fraction` < 0 draws a moving stripe (unmeasured work).
        void bar(ImDrawList *dl, ImVec2 min, ImVec2 max, float fraction, double now)
        {
            const float r = (max.y - min.y) * 0.5f;
            dl->AddRectFilled(ImVec2(min.x - th::px(3), min.y - th::px(3)), ImVec2(max.x + th::px(3), max.y + th::px(3)),
                              th::col::OutlineBlue, r + th::px(3));
            dl->AddRectFilled(min, max, th::col::ListBevelDark, r);
            float x0 = min.x, x1 = min.x + (max.x - min.x) * std::clamp(fraction, 0.0f, 1.0f);
            if (fraction < 0.0f)
            {
                const float w = (max.x - min.x) * 0.25f;
                const float t = static_cast<float>(std::fmod(now * 0.6, 1.0));
                x0 = min.x - w + (max.x - min.x + w) * t;
                x1 = x0 + w;
                x0 = std::max(x0, min.x);
                x1 = std::min(x1, max.x);
            }
            if (x1 - x0 < 1.0f)
                return;
            dl->PushClipRect(ImVec2(x0, min.y), ImVec2(x1, max.y), true);
            const ImVec2 mid(max.x, (min.y + max.y) * 0.5f);
            dl->AddRectFilledMultiColor(min, ImVec2(max.x, mid.y), th::col::BarTop, th::col::BarTop, th::col::BarBottom,
                                        th::col::BarBottom);
            dl->AddRectFilled(ImVec2(min.x, mid.y), max, th::col::BarBottom);
            dl->PopClipRect();
            // Rounded ends over the clipped fill.
            dl->AddRect(min, max, th::col::ListBevelDark, r, 0, th::px(2));
            const float capW = th::px(10);
            if (fraction >= 0.0f && fraction < 1.0f)
            {
                dl->AddRectFilled(ImVec2(x1 - capW, min.y), ImVec2(x1, max.y), th::col::BarCapGold, th::px(3));
                dl->AddRectFilled(ImVec2(x1 - capW + th::px(2), min.y + th::px(3)), ImVec2(x1 - th::px(2), min.y + th::px(7)),
                                  th::col::BarCapShine, th::px(2));
            }
        }
    }

    bool drawSetupScreen(const TaskProgress &progress, double now)
    {
        ImDrawList *dl = ImGui::GetForegroundDrawList();
        const ImGuiViewport *vp = ImGui::GetMainViewport();
        const float width = std::min(vp->Size.x - th::px(80), th::px(1180));
        const float pad = th::px(48);
        const float inner = width - pad * 2;

        if (progress.failed)
        {
            const std::string message = withoutDetails(progress.error());
            const auto tail = logTail(progress.error(), 12);
            const float msgH = th::font()->CalcTextSizeA(th::fontSize(th::Size::Body), FLT_MAX, inner, message.c_str()).y;
            const float lineH = th::fontSize(th::Size::Hint) * 0.8f;
            const float height = pad * 2 + th::px(64) + msgH + (tail.empty() ? 0.0f : th::px(24) + lineH * tail.size()) +
                                 th::px(80);
            const ImVec2 min(vp->Pos.x + (vp->Size.x - width) * 0.5f, vp->Pos.y + std::max(th::px(20), (vp->Size.y - height) * 0.5f));
            const ImVec2 max(min.x + width, min.y + height);
            th::dialogPanel(dl, min, max, "Setup");
            float y = min.y + pad;
            th::text(dl, ImVec2(min.x + pad, y), th::Size::Heading, th::col::Heading, "Setup didn't finish", th::col::Black);
            y += th::px(64);
            y += wrapped(dl, ImVec2(min.x + pad, y), th::Size::Body, th::col::White, message, inner);
            if (!tail.empty())
            {
                y += th::px(24);
                const ImVec2 lmin(min.x + pad - th::px(12), y - th::px(8)), lmax(max.x - pad + th::px(12), y + lineH * tail.size() + th::px(8));
                dl->AddRectFilled(lmin, lmax, th::col::ListBevelDark, th::px(8));
                dl->PushClipRect(lmin, lmax, true);
                for (const auto &line : tail)
                {
                    dl->AddText(th::font(), lineH, ImVec2(min.x + pad, y), IM_COL32(0xCF, 0xE8, 0xF5, 0xFF), line.c_str());
                    y += lineH;
                }
                dl->PopClipRect();
            }
            return actionButton(dl, max, pad, "Close") || ImGui::IsKeyPressed(ImGuiKey_Escape);
        }

        const double left = g_eta.update(progress, now);
        const uint64_t done = progress.done.load(), total = progress.total.load();
        const float fraction = total ? static_cast<float>(std::min<double>(1.0, static_cast<double>(done) / total)) : -1.0f;

        const float height = pad * 2 + th::px(64 + 54 + 30 + 34 + 30 + 60);
        const ImVec2 min(vp->Pos.x + (vp->Size.x - width) * 0.5f, vp->Pos.y + (vp->Size.y - height) * 0.5f);
        const ImVec2 max(min.x + width, min.y + height);
        th::dialogPanel(dl, min, max, "Setup");
        float y = min.y + pad;
        th::text(dl, ImVec2(min.x + pad, y), th::Size::Heading, th::col::Heading, "Getting Road Trip ready", th::col::Black);
        y += th::px(64);

        char step[64] = "";
        if (progress.steps > 1)
            std::snprintf(step, sizeof(step), "Step %d of %d:  ", std::max(1, progress.step.load()), progress.steps.load());
        const std::string phase = step + progress.phase();
        th::text(dl, ImVec2(min.x + pad, y), th::Size::Body, th::col::ListText, phase.c_str());
        y += th::px(54);

        bar(dl, ImVec2(min.x + pad, y), ImVec2(max.x - pad, y + th::px(30)), fraction, now);
        y += th::px(30 + 16);

        // The percentage on the left, the time left on the right (elapsed time while unmeasured).
        char pct[32];
        if (fraction >= 0.0f)
            std::snprintf(pct, sizeof(pct), "%d%%", static_cast<int>(fraction * 100.0f));
        else
        {
            const int s = static_cast<int>(g_eta.phaseSeconds(now));
            std::snprintf(pct, sizeof(pct), "%d:%02d", s / 60, s % 60);
        }
        th::text(dl, ImVec2(min.x + pad, y), th::Size::Body, th::col::White, pct);
        std::string eta = ProgressEta::describe(left);
        if (eta.empty())
            eta = fraction >= 0.0f ? "Estimating time left..." : "Working...";
        const ImVec2 es = th::measure(th::Size::Body, eta.c_str());
        th::text(dl, ImVec2(max.x - pad - es.x, y), th::Size::Body, th::col::Silver, eta.c_str());
        y += th::px(34 + 30);

        const std::string detail = progress.detail();
        const std::string hint = detail.empty() ? "This happens once, the first time Road Trip starts. Keep it open." : detail;
        dl->AddText(th::font(), th::fontSize(th::Size::Hint), ImVec2(min.x + pad, y), IM_COL32(0xCF, 0xE8, 0xF5, 0xFF),
                    hint.c_str());
        return false;
    }

    bool drawWelcomeScreen(const std::string &note)
    {
        ImDrawList *dl = ImGui::GetForegroundDrawList();
        const ImGuiViewport *vp = ImGui::GetMainViewport();
        const float width = std::min(vp->Size.x - th::px(80), th::px(1180));
        const float pad = th::px(48);
        const float inner = width - pad * 2;
        const std::string body = "Road Trip is rebuilt from your own copy of the game. Choose your Road Trip (USA) disc "
                                 "image: the .bin or .iso file you made from the disc.";
        const float bodyH = th::font()->CalcTextSizeA(th::fontSize(th::Size::Body), FLT_MAX, inner, body.c_str()).y;
        const float noteH = note.empty() ? 0.0f : th::px(24) + th::font()->CalcTextSizeA(th::fontSize(th::Size::Hint), FLT_MAX, inner, note.c_str()).y;
        const float height = pad * 2 + th::px(64) + bodyH + noteH + th::px(110);
        const ImVec2 min(vp->Pos.x + (vp->Size.x - width) * 0.5f, vp->Pos.y + (vp->Size.y - height) * 0.5f);
        const ImVec2 max(min.x + width, min.y + height);
        th::dialogPanel(dl, min, max, "Setup");
        float y = min.y + pad;
        th::text(dl, ImVec2(min.x + pad, y), th::Size::Heading, th::col::Heading, "Welcome to Road Trip", th::col::Black);
        y += th::px(64);
        y += wrapped(dl, ImVec2(min.x + pad, y), th::Size::Body, th::col::White, body, inner);
        if (!note.empty())
        {
            y += th::px(24);
            wrapped(dl, ImVec2(min.x + pad, y), th::Size::Hint, th::col::Gold, note, inner);
        }
        return actionButton(dl, max, pad, "Choose disc image");
    }
}
