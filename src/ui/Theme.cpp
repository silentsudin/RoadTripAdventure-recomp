#include "Theme.h"

#include "platform/Host.h"
#include "platform/Input.h"
#include "platform/Paths.h"
#include "raylib.h"

#include <SDL3/SDL_video.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <filesystem>

namespace rt::ui::theme
{
    namespace
    {
        ImFont *g_font = nullptr;

        ImVec2 operator+(ImVec2 a, ImVec2 b) { return ImVec2(a.x + b.x, a.y + b.y); }

        // A frame drawn as nested rounded strokes, outermost first.
        void frame(ImDrawList *dl, ImVec2 min, ImVec2 max, float radius, const ImU32 *colours, const float *widths, int n)
        {
            float inset = 0.0f;
            for (int i = 0; i < n; ++i)
            {
                const float w = px(widths[i]);
                const float half = w * 0.5f;
                dl->AddRect(ImVec2(min.x + inset + half, min.y + inset + half), ImVec2(max.x - inset - half, max.y - inset - half),
                            colours[i], std::max(0.0f, radius - inset - half), 0, w);
                inset += w;
            }
        }

        void shadow(ImDrawList *dl, ImVec2 min, ImVec2 max, float radius)
        {
            for (int i = 3; i >= 1; --i)
            {
                const float spread = px(4.0f * i);
                dl->AddRectFilled(ImVec2(min.x - spread * 0.5f, min.y + px(6) - spread * 0.25f),
                                  ImVec2(max.x + spread * 0.5f, max.y + px(6) + spread),
                                  IM_COL32(0, 0, 0, 40), radius + spread);
            }
        }

        // Which glyph family the player is holding, from the first connected controller: PlayStation
        // shapes for PlayStation pads, Nintendo letters for Nintendo ones, and Xbox letters for the
        // rest: Xbox pads, and the "standard" pads SDL can't name (handhelds such as the AYN Thor and
        // Odin, most Bluetooth and Android pads), which are labelled A/B/X/Y the Xbox way.
        std::string padFamily()
        {
            for (const rt::input::DeviceStatus &d : rt::input::devices())
            {
                if (d.type == "keyboard")
                    continue;
                if (d.type.rfind("ps", 0) == 0)
                    return "ps";
                if (d.type == "switchpro" || d.type == "joycon")
                    return "nintendo";
                return "xbox";
            }
            return "keyboard";
        }
    }

    // Handheld screens: the same pixels are physically much smaller than on a desk monitor, so the
    // UI grows with how small the screen is (Android reports its density; desktops stay at 1).
    float handheldBoost()
    {
        static const float boost = [] {
#if defined(__ANDROID__)
            SDL_Window *w = rt::host::window();
            const SDL_DisplayID display = w ? SDL_GetDisplayForWindow(w) : SDL_GetPrimaryDisplay();
            const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(display);
            const float dpi = SDL_GetDisplayContentScale(display) * 160.0f; // Android density
            if (mode && dpi > 0.0f)
            {
                const float heightInches = float(std::min(mode->w, mode->h)) / dpi;
                return std::clamp(4.5f / heightInches, 1.0f, 1.7f);
            }
#endif
            return 1.0f;
        }();
        return boost;
    }

    float scale() { return std::max(0.5f, ImGui::GetIO().DisplaySize.y / 1080.0f * handheldBoost()); }
    float px(float at1080p) { return at1080p * scale(); }
    ImFont *font() { return g_font ? g_font : ImGui::GetFont(); }

    float fontSize(Size s)
    {
        switch (s)
        {
        case Size::Hint: return px(26);
        case Size::Heading: return px(38);
        case Size::Title: return px(64);
        default: return px(30);
        }
    }

    void initialize()
    {
        ImGuiIO &io = ImGui::GetIO();
        const auto path = rt::paths::bundleResources() / "fonts" / "Fredoka-SemiBold.ttf";
        std::error_code ec;
        if (g_font)
            ; // already (the setup screen ran first)
        else if (std::filesystem::exists(path, ec))
            g_font = io.Fonts->AddFontFromFileTTF(path.string().c_str(), 30.0f);
        else
            std::fprintf(stderr, "[ui] font missing: %s\n", path.string().c_str());

        // Stock ImGui windows that remain (Controllers, debug panel) take the game's colours too.
        ImGuiStyle &s = ImGui::GetStyle();
        s.WindowRounding = 18;
        s.FrameRounding = 14;
        s.PopupRounding = 12;
        s.GrabRounding = 12;
        s.TabRounding = 12;
        s.WindowBorderSize = 0;
        s.FrameBorderSize = 0;
        s.WindowPadding = ImVec2(20, 18);
        s.FramePadding = ImVec2(12, 6);
        s.ItemSpacing = ImVec2(12, 8);
        ImVec4 *c = s.Colors;
        auto v = [](ImU32 u) { return ImGui::ColorConvertU32ToFloat4(u); };
        c[ImGuiCol_WindowBg] = v(col::Olive);
        c[ImGuiCol_PopupBg] = v(IM_COL32(0x3A, 0x47, 0x00, 0xF8));
        c[ImGuiCol_Text] = v(col::Cream);
        c[ImGuiCol_TextDisabled] = v(col::Disabled);
        c[ImGuiCol_TitleBg] = c[ImGuiCol_TitleBgCollapsed] = v(col::FrameOuter);
        c[ImGuiCol_TitleBgActive] = v(col::Frame);
        c[ImGuiCol_FrameBg] = v(IM_COL32(0x2E, 0x39, 0x00, 0xFF));
        c[ImGuiCol_FrameBgHovered] = c[ImGuiCol_FrameBgActive] = v(IM_COL32(0x3A, 0x47, 0x00, 0xFF));
        c[ImGuiCol_Button] = c[ImGuiCol_Header] = c[ImGuiCol_Tab] = v(col::Prompt);
        c[ImGuiCol_ButtonHovered] = c[ImGuiCol_HeaderHovered] = c[ImGuiCol_TabHovered] = v(col::BarTop);
        c[ImGuiCol_ButtonActive] = c[ImGuiCol_HeaderActive] = c[ImGuiCol_TabSelected] = v(col::BarBottom);
        c[ImGuiCol_SliderGrab] = c[ImGuiCol_SliderGrabActive] = c[ImGuiCol_CheckMark] = v(col::Horn);
        c[ImGuiCol_Separator] = v(col::FrameOuter);
        c[ImGuiCol_Border] = v(col::Frame);
        c[ImGuiCol_NavCursor] = v(col::Horn);
    }

    ImVec2 measure(Size size, const char *s)
    {
        return font()->CalcTextSizeA(fontSize(size), FLT_MAX, 0.0f, s);
    }

    void text(ImDrawList *dl, ImVec2 pos, Size size, ImU32 colour, const char *s, ImU32 outline)
    {
        const float fs = fontSize(size);
        const float o = std::max(1.0f, px(2.0f));
        static const ImVec2 dirs[8] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-0.7f, -0.7f}, {0.7f, -0.7f}, {-0.7f, 0.7f}, {0.7f, 0.7f}};
        for (const ImVec2 &d : dirs)
            dl->AddText(font(), fs, ImVec2(pos.x + d.x * o, pos.y + d.y * o), outline, s);
        dl->AddText(font(), fs, pos, colour, s);
    }

    void dialogPanel(ImDrawList *dl, ImVec2 min, ImVec2 max, const char *nameTab, bool list)
    {
        const float r = px(list ? 14 : 28);
        shadow(dl, min, max, r);
        dl->AddRectFilled(min, max, list ? col::ListBody : col::Olive, r);
        if (list)
        {
            // A 3 px bevel inside the frame: light on the top and left, dark on the bottom and right.
            const float in = px(12), b = px(3);
            dl->AddRectFilled(ImVec2(min.x + in, min.y + in), ImVec2(max.x - in, min.y + in + b), col::ListBevelLight);
            dl->AddRectFilled(ImVec2(min.x + in, min.y + in), ImVec2(min.x + in + b, max.y - in), col::ListBevelLight);
            dl->AddRectFilled(ImVec2(min.x + in, max.y - in - b), ImVec2(max.x - in, max.y - in), col::ListBevelDark);
            dl->AddRectFilled(ImVec2(max.x - in - b, min.y + in), ImVec2(max.x - in, max.y - in), col::ListBevelDark);
        }
        static const ImU32 colours[] = {col::FrameOuter, col::Frame, col::FrameLight};
        static const float widths[] = {3, 6, 3};
        frame(dl, min, max, r, colours, widths, 3);
        if (list)
        {
            // Small teal rivets sit on the frame's corners, as on the game's lists.
            const float in = px(6);
            const ImVec2 corners[4] = {{min.x + in, min.y + in}, {max.x - in, min.y + in}, {min.x + in, max.y - in}, {max.x - in, max.y - in}};
            for (const ImVec2 &c : corners)
            {
                dl->AddCircleFilled(c, px(4.5f), col::Rivet);
                dl->AddCircle(c, px(4.5f), col::RivetRing, 0, px(1.5f));
            }
        }
        if (nameTab)
        {
            const ImVec2 size = measure(Size::Heading, nameTab);
            const float h = px(52), padX = px(26);
            const ImVec2 tmin(min.x + px(40), min.y - h * 0.55f);
            const ImVec2 tmax(tmin.x + size.x + padX * 2, tmin.y + h);
            dl->AddRectFilled(tmin, tmax, col::Frame, h * 0.5f);
            dl->AddRect(tmin, tmax, col::FrameOuter, h * 0.5f, 0, px(3));
            text(dl, ImVec2(tmin.x + padX, tmin.y + (h - size.y) * 0.5f), Size::Heading, col::Silver, nameTab);
        }
    }

    void promptBox(ImDrawList *dl, ImVec2 min, ImVec2 max)
    {
        const float r = px(6);
        shadow(dl, min, max, r);
        dl->AddRectFilled(min, max, col::Prompt, r);
        static const ImU32 colours[] = {col::PromptRimOuter, col::PromptRimInner};
        static const float widths[] = {3, 3};
        frame(dl, min, max, r, colours, widths, 2);
    }

    void selectionBar(ImDrawList *dl, ImVec2 min, ImVec2 max)
    {
        // As the game's: a thin orange edge, a two-tone blue body (lighter top half) and a gold
        // capsule on the right end.
        const float h = max.y - min.y;
        dl->AddRectFilled(min, max, col::Frame, px(8));
        const float in = px(3), mid = (min.y + max.y) * 0.5f;
        dl->AddRectFilled(ImVec2(min.x + in, min.y + in), ImVec2(max.x - in, mid), col::BarTop, px(6), ImDrawFlags_RoundCornersTop);
        dl->AddRectFilled(ImVec2(min.x + in, mid), ImVec2(max.x - in, max.y - in), col::BarBottom, px(6), ImDrawFlags_RoundCornersBottom);
        dl->AddRectFilled(ImVec2(max.x - h * 0.55f, min.y), max, col::BarCapGold, h * 0.5f);
        dl->AddRectFilled(ImVec2(max.x - h * 0.45f, min.y + px(5)), ImVec2(max.x - px(6), min.y + h * 0.38f), col::BarCapShine,
                          h * 0.2f);
    }

    void horn(ImDrawList *dl, ImVec2 tip, float height, float time)
    {
        // An original gold trumpet: mouthpiece on the left, a thin tube, a flared bell whose rim
        // touches `tip` (the selection bar's left end). Bobs gently.
        const float bob = std::sin(time * 6.283f * 2.0f) * px(2.0f);
        const float h = height, w = height * 1.7f;
        const ImVec2 o(tip.x - w * 1.0f, tip.y - h * 0.5f + bob);
        const float line = px(2.5f);
        dl->AddCircleFilled(o + ImVec2(w * 0.06f, h * 0.5f), h * 0.14f, col::HornShade);
        dl->AddCircle(o + ImVec2(w * 0.06f, h * 0.5f), h * 0.14f, col::HornLine, 0, line);
        const ImVec2 tube[4] = {o + ImVec2(w * 0.08f, h * 0.42f), o + ImVec2(w * 0.55f, h * 0.36f), o + ImVec2(w * 0.55f, h * 0.64f),
                                o + ImVec2(w * 0.08f, h * 0.58f)};
        dl->AddConvexPolyFilled(tube, 4, col::Horn);
        dl->AddPolyline(tube, 4, col::HornLine, ImDrawFlags_Closed, px(2));
        const ImVec2 bell[4] = {o + ImVec2(w * 0.50f, h * 0.38f), o + ImVec2(w * 0.92f, 0), o + ImVec2(w * 0.92f, h),
                                o + ImVec2(w * 0.50f, h * 0.62f)};
        dl->AddConvexPolyFilled(bell, 4, col::Horn);
        dl->AddPolyline(bell, 4, col::HornLine, ImDrawFlags_Closed, line);
        dl->AddEllipseFilled(o + ImVec2(w * 0.92f, h * 0.5f), ImVec2(w * 0.08f, h * 0.52f), col::HornShade);
        dl->AddEllipse(o + ImVec2(w * 0.92f, h * 0.5f), ImVec2(w * 0.08f, h * 0.52f), col::HornLine, 0, 0, line);
    }

    void pillTab(ImDrawList *dl, ImVec2 min, ImVec2 max, bool active, const char *label)
    {
        // Active: the orange name-tab family; inactive: the blue prompt family.
        const float r = (max.y - min.y) * 0.5f;
        dl->AddRectFilled(min, max, active ? col::Frame : col::Prompt, r);
        dl->AddRect(min, max, active ? col::FrameOuter : col::PromptRimInner, r, 0, px(3));
        const ImVec2 size = measure(Size::Body, label);
        text(dl, ImVec2((min.x + max.x - size.x) * 0.5f, (min.y + max.y - size.y) * 0.5f), Size::Body,
             active ? col::Silver : col::Gold, label, active ? col::Outline : col::OutlineBlue);
    }

    void scrim(ImDrawList *dl, ImVec2 min, ImVec2 max, float amount)
    {
        const ImU32 a = static_cast<ImU32>(0x99 * std::clamp(amount, 0.0f, 1.0f));
        dl->AddRectFilled(min, max, (col::Scrim & 0x00FFFFFFu) | (a << 24));
    }

    void arrow(ImDrawList *dl, ImVec2 c, bool right)
    {
        const float w = px(12), h = px(18);
        const float s = right ? 1.0f : -1.0f;
        const ImVec2 a(c.x - s * w * 0.5f, c.y - h * 0.5f), b(c.x + s * w * 0.5f, c.y), d(c.x - s * w * 0.5f, c.y + h * 0.5f);
        if (right)
            dl->AddTriangleFilled(a, b, d, col::Horn);
        else
            dl->AddTriangleFilled(a, d, b, col::Horn);
        dl->AddTriangle(a, b, d, col::Outline, px(2));
    }

    void hintBand(ImDrawList *dl, ImVec2 min, ImVec2 max)
    {
        dl->AddRectFilled(min, max, col::HintBand, px(10));
        dl->AddRectFilled(ImVec2(min.x + px(6), min.y), ImVec2(max.x - px(6), min.y + px(3)), col::ListBevelDark);
    }

    std::string padFamilyName() { return padFamily(); }

    float prompt(ImDrawList *dl, ImVec2 pos, const char *button, const char *label)
    {
        const std::string fam = padFamily();
        const std::string b = button;
        const float d = px(40);
        const ImVec2 c(pos.x + d * 0.5f, pos.y + d * 0.5f);
        float used = 0;
        auto badge = [&](ImU32 fill, const char *letter) {
            dl->AddCircleFilled(c, d * 0.5f, fill);
            dl->AddCircle(c, d * 0.5f, col::Outline, 0, px(2));
            const ImVec2 ls = font()->CalcTextSizeA(px(22), FLT_MAX, 0.0f, letter);
            dl->AddText(font(), px(22), ImVec2(c.x - ls.x * 0.5f, c.y - ls.y * 0.5f), col::Silver, letter);
            used = d;
        };
        auto key = [&](const char *name) {
            const ImVec2 ls = font()->CalcTextSizeA(px(22), FLT_MAX, 0.0f, name);
            const ImVec2 kmax(pos.x + ls.x + px(28), pos.y + d);
            dl->AddRectFilled(pos, kmax, IM_COL32(0xA8, 0xA8, 0xA8, 0xFF), px(8));
            dl->AddRectFilled(pos, ImVec2(kmax.x, kmax.y - px(3)), IM_COL32(0xF4, 0xF4, 0xF4, 0xFF), px(8));
            dl->AddRect(pos, kmax, col::Outline, px(8), 0, px(2));
            dl->AddText(font(), px(22), ImVec2(pos.x + px(14), pos.y + (d - ls.y) * 0.5f), col::Outline, name);
            used = kmax.x - pos.x;
        };
        if (fam == "keyboard")
            key(b == "cross" ? "Enter" : b == "triangle" ? "Esc" : b == "square" ? "R" : b == "l1" ? "Q" : b == "r1" ? "E" : b.c_str());
        else if (b == "guide")
            key(fam == "xbox" ? "Xbox" : fam == "nintendo" ? "Home" : "PS");
        else if (b == "select")
            key(fam == "xbox" ? "View" : fam == "nintendo" ? "-" : "Create");
        else if (b == "start")
            key(fam == "xbox" ? "Menu" : fam == "nintendo" ? "+" : "Options");
        else if (b == "l1" || b == "r1")
            key(fam == "xbox" ? (b == "l1" ? "LB" : "RB") : fam == "nintendo" ? (b == "l1" ? "L" : "R") : (b == "l1" ? "L1" : "R1"));
        else if (fam == "xbox" || fam == "nintendo")
        {
            // The game's buttons in the pad's own layout (input Mapping.h familyProfile): ✕ (confirm)
            // is A and △ (back) is B on both; ○ is Y on Xbox, X on Nintendo; □ the other.
            const bool xbox = fam == "xbox";
            const char *letter = b == "cross" ? "A" : b == "triangle" ? "B" : b == "circle" ? (xbox ? "Y" : "X") : (xbox ? "X" : "Y");
            const ImU32 colour = letter[0] == 'A'   ? IM_COL32(0x3C, 0xA0, 0x3C, 0xFF)
                                 : letter[0] == 'B' ? IM_COL32(0xC8, 0x3C, 0x3C, 0xFF)
                                 : letter[0] == 'X' ? IM_COL32(0x3C, 0x64, 0xC8, 0xFF)
                                                    : IM_COL32(0xC8, 0xA0, 0x28, 0xFF);
            badge(colour, letter);
        }
        else
        {
            dl->AddCircleFilled(c, d * 0.5f, IM_COL32(0x20, 0x20, 0x28, 0xFF));
            const float s = d * 0.22f, t = px(2.5f);
            if (b == "cross")
            {
                dl->AddLine(ImVec2(c.x - s, c.y - s), ImVec2(c.x + s, c.y + s), IM_COL32(0x8F, 0xB4, 0xF0, 0xFF), t);
                dl->AddLine(ImVec2(c.x - s, c.y + s), ImVec2(c.x + s, c.y - s), IM_COL32(0x8F, 0xB4, 0xF0, 0xFF), t);
            }
            else if (b == "circle")
                dl->AddCircle(c, s * 1.1f, IM_COL32(0xF0, 0x7A, 0x7A, 0xFF), 0, t);
            else if (b == "square")
                dl->AddRect(ImVec2(c.x - s, c.y - s), ImVec2(c.x + s, c.y + s), IM_COL32(0xE6, 0x8F, 0xD6, 0xFF), 0, 0, t);
            else
                dl->AddTriangle(ImVec2(c.x, c.y - s * 1.1f), ImVec2(c.x + s * 1.1f, c.y + s * 0.8f),
                                ImVec2(c.x - s * 1.1f, c.y + s * 0.8f), IM_COL32(0x5E, 0xD8, 0xB8, 0xFF), t);
            used = d;
        }
        if (!label || !*label)
            return used;
        const ImVec2 ls = measure(Size::Hint, label);
        text(dl, ImVec2(pos.x + used + px(10), pos.y + (d - ls.y) * 0.5f), Size::Hint, col::Silver, label, col::OutlineBlue);
        return used + px(10) + ls.x;
    }
}
