// The performance overlay (see PerfOverlay.h).

#include "ui/PerfOverlay.h"

#include "debug/PerfStats.h"
#include "ui/Theme.h"

#include "imgui.h"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

namespace rt::ui
{
    namespace th = theme;

    void drawPerfOverlay(rt::settings::PerfOverlay level)
    {
        using rt::settings::PerfOverlay;
        const rt::debug::PerfStats &s = rt::debug::perfStats();
        if (level == PerfOverlay::Off || !s.valid)
            return;

        // Lines: the frame rate first (gold at full speed, orange below), then the details.
        struct Line
        {
            std::string text;
            ImU32 colour;
        };
        std::vector<Line> lines;
        char buf[96];
        std::snprintf(buf, sizeof(buf), "%.0f fps", s.fps);
        const bool full = s.fps >= 59.0;
        lines.push_back({buf, full ? th::col::Heading : IM_COL32(0xFF, 0x8A, 0x3D, 0xFF)});
        if (level == PerfOverlay::Detailed)
        {
            std::snprintf(buf, sizeof(buf), "worst %.1f ms", s.worstFrameMs);
            lines.push_back({buf, s.worstFrameMs > 20.0 ? IM_COL32(0xFF, 0x8A, 0x3D, 0xFF) : th::col::White});
            std::snprintf(buf, sizeof(buf), "VU1 %.0f%%  GS %.0f%%", s.vu1Busy, s.gsBusy);
            lines.push_back({buf, th::col::White});
            std::snprintf(buf, sizeof(buf), "CPU %.0f%%", s.appCpu);
            lines.push_back({buf, th::col::White});
            if (s.gpuBusy >= 0)
            {
                std::snprintf(buf, sizeof(buf), "GPU %d%% @ %d MHz", s.gpuBusy, s.gpuMHz);
                lines.push_back({buf, th::col::White});
            }
            if (s.batteryC > 0)
            {
                std::snprintf(buf, sizeof(buf), "Battery %.0f \xC2\xB0""C", s.batteryC);
                lines.push_back({buf, th::col::White});
            }
        }

        // One row along the bottom edge from the left: the frame rate, then the details, with thin
        // dividers. Low and short, it stays clear of the race HUD (times at the top, the map on the
        // left, the speedometer at the bottom right).
        ImDrawList *dl = ImGui::GetForegroundDrawList();
        const ImGuiViewport *vp = ImGui::GetMainViewport();
        const float big = th::fontSize(th::Size::Body) * 0.9f, small = th::fontSize(th::Size::Hint) * 0.8f;
        const float padX = th::px(14), padY = th::px(8), sep = th::px(22);
        float w = padX * 2, h = big + padY * 2;
        for (size_t i = 0; i < lines.size(); ++i)
            w += th::font()->CalcTextSizeA(i == 0 ? big : small, FLT_MAX, 0, lines[i].text.c_str()).x + (i ? sep : 0);
        const ImVec2 min(vp->Pos.x + th::px(16), vp->Pos.y + vp->Size.y - th::px(12) - h);
        const ImVec2 max(min.x + w, min.y + h);
        dl->AddRectFilled(min, max, IM_COL32(0x0B, 0x12, 0x40, 0xB8), h * 0.5f);
        dl->AddRect(min, max, IM_COL32(0x2C, 0x6C, 0xE8, 0xFF), h * 0.5f, 0, th::px(2));
        float x = min.x + padX;
        const float o = std::max(1.0f, th::px(1.5f));
        for (size_t i = 0; i < lines.size(); ++i)
        {
            const float fs = i == 0 ? big : small;
            if (i > 0)
            {
                const float cx = x + sep * 0.5f - th::px(1);
                dl->AddLine(ImVec2(cx, min.y + padY + th::px(4)), ImVec2(cx, max.y - padY - th::px(4)),
                            IM_COL32(0x2C, 0x6C, 0xE8, 0xFF), th::px(2));
                x += sep;
            }
            const float y = min.y + (h - fs) * 0.5f; // centred on the row
            for (const ImVec2 d : {ImVec2(-o, 0), ImVec2(o, 0), ImVec2(0, -o), ImVec2(0, o)})
                dl->AddText(th::font(), fs, ImVec2(x + d.x, y + d.y), th::col::Outline, lines[i].text.c_str());
            dl->AddText(th::font(), fs, ImVec2(x, y), lines[i].colour, lines[i].text.c_str());
            x += th::font()->CalcTextSizeA(fs, FLT_MAX, 0, lines[i].text.c_str()).x;
        }
    }
}
