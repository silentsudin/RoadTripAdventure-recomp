// The second screen (see SecondScreen.h).

#include "ui/SecondScreen.h"

#include "game/GameStats.h"
#include "platform/Host.h"
#include "platform/SecondDisplay.h"
#include "ui/PauseMenu.h"
#include "ui/Theme.h"

#include "imgui.h"
#include "ps2_runtime.h"
#include "runtime/ps2_host_presenter.h"

#if defined(__ANDROID__)
#include <android/native_window.h>
#endif

#include <algorithm>
#include <bitset>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

namespace rt::ui
{
    namespace th = theme;

    namespace
    {
        ImGuiContext *g_context = nullptr;
        double g_lastTime = 0;

        // Colours of this screen (the game's records panels and stamp notebook).
        namespace c
        {
            constexpr ImU32 SkyTop = IM_COL32(0x5E, 0x9B, 0xE0, 0xFF), SkyBottom = IM_COL32(0xB4, 0xD2, 0xF2, 0xFF);
            constexpr ImU32 Cloud = IM_COL32(0xFF, 0xFF, 0xFF, 0xB3);
            constexpr ImU32 Shadow = IM_COL32(0x0A, 0x2A, 0x60, 0x59);
            constexpr ImU32 PanelBody = IM_COL32(0x30, 0x40, 0x90, 0xFF), PanelRimOuter = IM_COL32(0x91, 0xD2, 0xE8, 0xFF),
                            PanelRimInner = IM_COL32(0x1C, 0xA1, 0xE6, 0xFF);
            constexpr ImU32 Label = IM_COL32(0xF6, 0xE6, 0x8F, 0xFF), Value = IM_COL32(0xFF, 0xFF, 0xFF, 0xFF);
            constexpr ImU32 Outline = IM_COL32(0x00, 0x08, 0x20, 0xFF);
            constexpr ImU32 Name = IM_COL32(0x58, 0xE0, 0x48, 0xFF);
            constexpr ImU32 Track = IM_COL32(0x1C, 0x2E, 0x70, 0xFF), Fill = IM_COL32(0x00, 0x98, 0xC8, 0xFF);
            constexpr ImU32 Paper = IM_COL32(0xDC, 0xE6, 0xB8, 0xFF), PaperRim = IM_COL32(0xA8, 0xD0, 0x70, 0xFF),
                            Frame = IM_COL32(0xEE, 0xF2, 0xD0, 0xFF), Ink = IM_COL32(0x1A, 0x3C, 0x8C, 0xFF),
                            InkFaint = IM_COL32(0x4C, 0x5A, 0x34, 0xFF), Silhouette = IM_COL32(0x9D, 0xB2, 0x7A, 0xFF);
            constexpr ImU32 MenuFill = IM_COL32(0x2E, 0x8A, 0x3A, 0xFF), MenuRim = IM_COL32(0x9E, 0xE0, 0x7A, 0xFF);
            constexpr ImU32 TabIdle = IM_COL32(0x30, 0x40, 0x90, 0xFF), TabIdleRim = IM_COL32(0x1C, 0xA1, 0xE6, 0xFF);
            constexpr ImU32 Magenta = IM_COL32(0xF0, 0x48, 0xA8, 0xFF);
        }

        bool inside(ImVec2 p, ImVec2 min, ImVec2 max) { return p.x >= min.x && p.x < max.x && p.y >= min.y && p.y < max.y; }

        // A tap: pressed and released inside.
        bool tapped(ImVec2 min, ImVec2 max)
        {
            const ImGuiIO &io = ImGui::GetIO();
            return io.MouseReleased[0] && inside(io.MousePos, min, max) && inside(io.MouseClickedPos[0], min, max) &&
                   std::abs(io.MousePos.y - io.MouseClickedPos[0].y) < th::px(24); // not the end of a drag
        }
        bool held(ImVec2 min, ImVec2 max)
        {
            const ImGuiIO &io = ImGui::GetIO();
            return io.MouseDown[0] && inside(io.MousePos, min, max) && inside(io.MouseClickedPos[0], min, max);
        }

        // Text with the game's navy outline.
        void outlined(ImDrawList *dl, ImVec2 pos, float size, ImU32 colour, const char *s, float o = 0)
        {
            o = o > 0 ? o : std::max(1.0f, th::px(3));
            static const ImVec2 dirs[8] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-0.7f, -0.7f}, {0.7f, -0.7f}, {-0.7f, 0.7f}, {0.7f, 0.7f}};
            for (const ImVec2 &d : dirs)
                dl->AddText(th::font(), size, ImVec2(pos.x + d.x * o, pos.y + d.y * o), c::Outline, s);
            dl->AddText(th::font(), size, pos, colour, s);
        }
        ImVec2 measure(float size, const char *s) { return th::font()->CalcTextSizeA(size, FLT_MAX, 0, s); }
        float body() { return th::px(32); }
        float heading() { return th::px(44); }
        float tag() { return th::px(24); }

        // Daylight sky with a few soft clouds (drawn shapes).
        void backdrop(ImDrawList *dl, ImVec2 size)
        {
            dl->AddRectFilledMultiColor(ImVec2(0, 0), size, c::SkyTop, c::SkyTop, c::SkyBottom, c::SkyBottom);
        }

        // The game's blue records panel.
        void panel(ImDrawList *dl, ImVec2 min, ImVec2 max)
        {
            const float r = th::px(24);
            dl->AddRectFilled(ImVec2(min.x, min.y + th::px(8)), ImVec2(max.x, max.y + th::px(8)), c::Shadow, r);
            dl->AddRectFilled(min, max, c::PanelRimOuter, r);
            dl->AddRectFilled(ImVec2(min.x + th::px(3), min.y + th::px(3)), ImVec2(max.x - th::px(3), max.y - th::px(3)), c::PanelRimInner,
                              r - th::px(3));
            dl->AddRectFilled(ImVec2(min.x + th::px(6), min.y + th::px(6)), ImVec2(max.x - th::px(6), max.y - th::px(6)), c::PanelBody,
                              r - th::px(6));
        }

        // The green prompt pill (Menu).
        bool menuButton(ImDrawList *dl, ImVec2 min, ImVec2 max)
        {
            const bool down = held(min, max);
            const float r = (max.y - min.y) * 0.5f;
            dl->AddRectFilled(min, max, down ? c::MenuRim : c::MenuFill, r);
            dl->AddRect(min, max, c::MenuRim, r, 0, th::px(4));
            const char *label = pauseMenuOpen() ? "Close" : "Menu";
            const ImVec2 sz = measure(body(), label);
            outlined(dl, ImVec2((min.x + max.x - sz.x) * 0.5f, (min.y + max.y - sz.y) * 0.5f), body(), c::Value, label);
            return tapped(min, max);
        }

        std::string withCommas(uint64_t v)
        {
            std::string s = std::to_string(v);
            for (int i = static_cast<int>(s.size()) - 3; i > 0; i -= 3)
                s.insert(static_cast<size_t>(i), ",");
            return s;
        }

        std::string ordinalSuffix(int n)
        {
            return (n % 100 >= 11 && n % 100 <= 13) ? "th" : n % 10 == 1 ? "st" : n % 10 == 2 ? "nd" : n % 10 == 3 ? "rd" : "th";
        }

        // Words split into at most two lines that fit `width`.
        std::vector<std::string> wrap2(const std::string &text, float size, float width)
        {
            if (measure(size, text.c_str()).x <= width)
                return {text};
            size_t best = std::string::npos;
            for (size_t i = text.find(' '); i != std::string::npos; i = text.find(' ', i + 1))
                if (measure(size, text.substr(0, i).c_str()).x <= width)
                    best = i;
            if (best == std::string::npos)
                return {text};
            return {text.substr(0, best), text.substr(best + 1)};
        }

        // ---- the map ----

        // The game's own map (drawn apart from the picture by the hardware GS, without its bezel),
        // as large as fits (its layer is drawn at 4x, sampled smoothly), in a gold plate rim.
        // Where the map went on screen (for markers).
        struct MapPlacement
        {
            ImVec2 a, b; // the map image's corners
        };

        bool drawMap(ImDrawList *dl, ImVec2 min, ImVec2 max, ps2x::HostPresenter &presenter, bool centre, ImVec2 *placed = nullptr,
                     bool inset = false, MapPlacement *where = nullptr)
        {
            float uv[4], aspect = 1.0f;
            if (!presenter.mapRegion(uv, aspect) || aspect <= 0.0f)
                return false;
            // Widescreen draws the HUD (the map with it) narrower by (4:3) / the display's shape,
            // to look right once stretched: undo that here.
            aspect /= std::min(1.0f, (4.0f / 3.0f) / std::max(presenter.displayAspect(), 0.1f));
            if (!inset) // the margin below widens the shown part
            {
                const float bw = std::max(1.0f, (uv[2] - uv[0]) * 640.0f), bh = std::max(1.0f, (uv[3] - uv[1]) * 224.0f);
                aspect *= ((bw + 4.0f) / bw) / ((bh + 2.0f) / bh);
            }
            const float rim = th::px(6);
            const float w = max.x - min.x - rim * 2, h = max.y - min.y - rim * 2;
            const float dw = std::floor(std::min(w, h * aspect)), dh = std::floor(dw / aspect);
            const ImVec2 a(centre ? std::floor(min.x + rim + (w - dw) * 0.5f) : min.x + rim, std::floor(min.y + rim + (h - dh) * 0.5f));
            const ImVec2 b(a.x + dw, a.y + dh);
            dl->AddRectFilled(ImVec2(a.x - rim, a.y - rim), ImVec2(b.x + rim, b.y + rim), IM_COL32(0xE8, 0xC8, 0x30, 0xFF), th::px(16));
            dl->AddRect(ImVec2(a.x - rim * 0.5f, a.y - rim * 0.5f), ImVec2(b.x + rim * 0.5f, b.y + rim * 0.5f), IM_COL32(0xFF, 0xF0, 0xA0, 0xFF),
                        th::px(12), 0, th::px(3));
            // Inside the rim's rounded corners; the town map a field pixel in from its box (the
            // game's bezel and scissor edge show there; a race map's box is its own extent).
            // A race map's box is the track's extent plus 4 GS pixels (fixed for the race by the
            // GS); a little more of the layer's background keeps the track off the rim.
            const float du = inset ? 1.0f / 640.0f : -2.0f / 640.0f, dv = inset ? 1.0f / 224.0f : -1.0f / 224.0f;
            dl->AddImageRounded(static_cast<ImTextureID>(ps2x::HostPresenter::kMapTexture), a, b, ImVec2(uv[0] + du, uv[1] + dv),
                                ImVec2(uv[2] - du, uv[3] - dv), IM_COL32_WHITE, th::px(12));
            if (placed)
                *placed = ImVec2(b.x + rim, b.y + rim);
            if (where)
                *where = {a, b};
            return true;
        }

        // ---- places on the town map ----

        // Marks on the town map: Q's Factory, the shops, Quick-Pic Shops not used yet and houses
        // not visited yet, where the game's minimap puts them (projected like the game does; the
        // town map's box is its scissor, 46..158 x 157..207 shown, whatever widescreen did to it).
        void drawPlaces(ImDrawList *dl, const MapPlacement &m, const rt::game::Stats &st, const std::vector<rt::game::Place> &places)
        {
            constexpr float kX0 = 46, kX1 = 158, kY0 = 157, kY1 = 207;
            dl->PushClipRect(m.a, m.b, true);
            const float r = th::px(13);
            for (const auto &pl : places)
            {
                using K = rt::game::Place::Kind;
                if ((pl.kind == K::PhotoBooth || pl.kind == K::House) && pl.done)
                    continue;
                if (pl.kind == K::Other)
                    continue;
                float fx, fy;
                rt::game::mapPoint(st, pl.x, pl.z, fx, fy);
                if (fx < kX0 - 4 || fx > kX1 + 4 || fy < kY0 - 4 || fy > kY1 + 4)
                    continue;
                const ImVec2 c(m.a.x + (fx - kX0) / (kX1 - kX0) * (m.b.x - m.a.x), m.a.y + (fy - kY0) / (kY1 - kY0) * (m.b.y - m.a.y));
                switch (pl.kind)
                {
                case K::Factory:
                {
                    dl->AddCircleFilled(c, r * 1.25f, IM_COL32(0xE8, 0x84, 0x0C, 0xFF), 20);
                    dl->AddCircle(c, r * 1.25f, IM_COL32(0xFF, 0xD8, 0x78, 0xFF), 20, th::px(3));
                    const ImVec2 sz = measure(tag(), "Q");
                    outlined(dl, ImVec2(c.x - sz.x * 0.5f, c.y - sz.y * 0.5f), tag(), c::Value, "Q", th::px(2));
                    break;
                }
                case K::Shop:
                {
                    const char t[2] = {pl.letter, 0};
                    dl->AddRectFilled(ImVec2(c.x - r, c.y - r), ImVec2(c.x + r, c.y + r), IM_COL32(0x1C, 0xA1, 0xE6, 0xFF), th::px(6));
                    dl->AddRect(ImVec2(c.x - r, c.y - r), ImVec2(c.x + r, c.y + r), c::Value, th::px(6), 0, th::px(2));
                    const ImVec2 sz = measure(tag(), t);
                    outlined(dl, ImVec2(c.x - sz.x * 0.5f, c.y - sz.y * 0.5f), tag(), c::Value, t, th::px(2));
                    break;
                }
                case K::PhotoBooth:
                {
                    // A little camera: body, lens, flash.
                    const ImU32 pink = IM_COL32(0xF0, 0x48, 0xA8, 0xFF);
                    dl->AddRectFilled(ImVec2(c.x - r, c.y - r * 0.7f), ImVec2(c.x + r, c.y + r * 0.75f), pink, th::px(4));
                    dl->AddRectFilled(ImVec2(c.x - r * 0.4f, c.y - r), ImVec2(c.x + r * 0.2f, c.y - r * 0.6f), pink, th::px(2));
                    dl->AddCircleFilled(ImVec2(c.x, c.y + r * 0.05f), r * 0.45f, c::Value, 16);
                    dl->AddCircleFilled(ImVec2(c.x, c.y + r * 0.05f), r * 0.25f, IM_COL32(0x1A, 0x3C, 0x8C, 0xFF), 16);
                    break;
                }
                default: // a house still to visit
                    dl->AddCircleFilled(c, r * 0.55f, c::Value, 16);
                    dl->AddCircle(c, r * 0.55f, IM_COL32(0x1A, 0x3C, 0x8C, 0xFF), 16, th::px(2));
                    break;
                }
            }
            dl->PopClipRect();
        }

        // ---- the column beside the map ----

        // Label over a large value.
        float g_statSize = 60; // th::px units of a stat's value (the town column is more compact)

        float stat(ImDrawList *dl, float x, float y, float w, const char *label, const std::string &value, ImU32 colour = c::Value,
                   const std::string &note = {}, ImU32 noteColour = c::Value)
        {
            outlined(dl, ImVec2(x, y), body(), c::Label, label);
            const float big = th::px(g_statSize);
            const ImVec2 sz = measure(big, value.c_str());
            const float size = sz.x > w ? big * w / sz.x : big;
            outlined(dl, ImVec2(x, y + body() * 1.1f), size, colour, value.c_str(), th::px(4));
            float next = y + body() * 1.1f + big + th::px(18);
            if (!note.empty())
            {
                outlined(dl, ImVec2(x, next - th::px(10)), body(), noteColour, note.c_str());
                next += body() + th::px(8);
            }
            return next;
        }

        float columnHeading(ImDrawList *dl, float x, float y, float w, const std::string &text)
        {
            for (const auto &l : wrap2(text, heading(), w))
            {
                outlined(dl, ImVec2(x, y), heading(), c::Name, l.c_str());
                y += heading() * 1.1f;
            }
            return y + th::px(14);
        }

        constexpr ImU32 kGold = IM_COL32(0xF8, 0xC8, 0x38, 0xFF);

        // During a race: the course, the lap (the top HUD shows only its number), the last lap
        // against the best before it, and the best (the top shows the total and this lap).
        void raceColumn(ImDrawList *dl, float x, float y, float w, const rt::game::Stats &st)
        {
            if (!st.course.empty())
                y = columnHeading(dl, x, y, w, st.course);
            if (st.countdown >= 2 && st.countdown <= 6)
            {
                stat(dl, x, y, w, "Get ready", st.countdown == 6 ? "GO!" : "...", kGold);
                return;
            }
            if (st.laps > 0)
                y = stat(dl, x, y, w, st.finished ? "Finished" : "Lap",
                         st.finished ? std::to_string(st.place) + ordinalSuffix(st.place) : std::to_string(st.lap) + " / " + std::to_string(st.laps));
            if (st.lastLapFrames)
            {
                std::string note;
                ImU32 noteColour = c::Value;
                if (st.bestBeforeLast)
                {
                    const int32_t d = static_cast<int32_t>(st.lastLapFrames) - static_cast<int32_t>(st.bestBeforeLast);
                    char buf[32];
                    std::snprintf(buf, sizeof(buf), "%s%d\"%02d", d < 0 ? "-" : "+", std::abs(d) / 60, std::abs(d) % 60 * 100 / 60);
                    note = d < 0 ? std::string("New best ") + buf : buf;
                    noteColour = d < 0 ? IM_COL32(0x58, 0xE0, 0x48, 0xFF) : IM_COL32(0xFF, 0x70, 0x60, 0xFF);
                }
                y = stat(dl, x, y, w, "Last lap", rt::game::raceTime(st.lastLapFrames), c::Value, note, noteColour);
            }
            // A new best flashes for a moment.
            static uint32_t lastBest = 0;
            static float bestAt = -10;
            const float now = static_cast<float>(ImGui::GetTime());
            if (st.bestLapFrames && st.bestLapFrames != lastBest)
            {
                if (lastBest && st.bestLapFrames < lastBest)
                    bestAt = now;
                lastBest = st.bestLapFrames;
            }
            const bool flash = now - bestAt < 2.5f && std::fmod(now - bestAt, 0.5f) < 0.3f;
            if (now - bestAt < 2.5f)
            {
                const float top = y - th::px(8), bottom = y + body() * 1.1f + th::px(60) + th::px(12);
                dl->AddRectFilled(ImVec2(x - th::px(12), top), ImVec2(x + w, bottom), IM_COL32(0xF8, 0xC8, 0x38, flash ? 0xA0 : 0x50), th::px(14));
            }
            stat(dl, x, y, w, now - bestAt < 2.5f ? "New best lap!" : "Best lap",
                 st.bestLapFrames ? rt::game::raceTime(st.bestLapFrames) : "--'--\"--", flash ? c::Value : kGold);
        }

        // Driving in a town: where, when, and what's left to find there.
        void townColumn(ImDrawList *dl, float x, float y, float w, float bottom, rt::game::Stats st, const std::vector<rt::game::Place> &places)
        {
            g_statSize = 46;
            struct Restore
            {
                ~Restore() { g_statSize = 60; }
            } restore;
            const float each = body() * 1.1f + th::px(g_statSize) + th::px(18); // one stat's height
            for (const auto &pl : places)
                if (pl.here && pl.kind == rt::game::Place::Kind::PhotoBooth)
                    ++st.townPhotos, st.townPhotosTaken += pl.done;
            y = columnHeading(dl, x, y, w, st.townLabel);
            char clock[16];
            std::snprintf(clock, sizeof(clock), "%d:%02d %s", st.hour % 12 == 0 ? 12 : st.hour % 12, st.minute, st.hour < 12 ? "am" : "pm");
            // As many as fit, most useful first.
            if (y + each <= bottom)
                y = stat(dl, x, y, w, "Time", clock);
            if (st.townPhotos && y + each <= bottom)
                y = stat(dl, x, y, w, "Photos here", std::to_string(st.townPhotosTaken) + " / " + std::to_string(st.townPhotos),
                         st.townPhotosTaken == st.townPhotos ? kGold : c::Value);
            if (y + each <= bottom)
                y = stat(dl, x, y, w, "Houses to visit", std::to_string(st.housesLeft), st.housesLeft ? c::Value : kGold);
            if (y + each <= bottom)
                stat(dl, x, y, w, "Coins found", std::to_string(st.coins) + " / 100", kGold);
        }

        // The place, large (a race without a map).
        void bigPlace(ImDrawList *dl, ImVec2 min, ImVec2 max, const rt::game::Stats &st)
        {
            const std::string num = std::to_string(st.place), suffix = ordinalSuffix(st.place), of = "of " + std::to_string(st.entrants);
            const float big = th::px(220), small = big * 0.4f;
            const ImVec2 nsz = measure(big, num.c_str()), ssz = measure(small, suffix.c_str());
            const float x = (min.x + max.x - nsz.x - ssz.x) * 0.5f, y = (min.y + max.y) * 0.5f - nsz.y * 0.6f;
            const ImU32 colour = st.place == 1 ? kGold : c::Magenta;
            outlined(dl, ImVec2(x, y), big, colour, num.c_str(), th::px(6));
            outlined(dl, ImVec2(x + nsz.x, y + th::px(18)), small, colour, suffix.c_str(), th::px(4));
            const ImVec2 osz = measure(heading(), of.c_str());
            outlined(dl, ImVec2((min.x + max.x - osz.x) * 0.5f, y + nsz.y), heading(), c::Value, of.c_str());
        }

        // ---- the notebook ----

        bool g_stampsGot = true; // Got (else To get)
        // Stamps earned while we watched, newest first (they lead the Got list), and the moment
        // the newest arrived (the notebook shows it being stamped).
        std::vector<int> g_recent;
        float g_newStampAt = -10;
        float g_scroll = 0, g_scrollShownUntil = 0;

        struct StampIcons
        {
            uint64_t ink = 0, mask = 0;
        };
        // Stamp n's picture: as inked, and as a white mask for silhouettes (nearest-sampled).
        StampIcons stampIcons(ps2x::HostPresenter &presenter, int n)
        {
            static StampIcons icons[100];
            static bool tried[100] = {};
            if (n < 1 || n > 100)
                return {};
            if (!tried[n - 1])
            {
                tried[n - 1] = true;
                std::vector<uint8_t> rgba = rt::game::stampIcon(n);
                if (!rgba.empty())
                {
                    icons[n - 1].ink = presenter.createUiTexture(rgba.data(), 64, 64, true);
                    for (size_t i = 0; i < rgba.size(); i += 4)
                        rgba[i] = rgba[i + 1] = rgba[i + 2] = 0xFF;
                    icons[n - 1].mask = presenter.createUiTexture(rgba.data(), 64, 64, true);
                }
            }
            return icons[n - 1];
        }

        // A label over a value in a dark chip (the notebook's header).
        void chip(ImDrawList *dl, ImVec2 min, ImVec2 max, const std::string &label, const std::string &value, ImU32 colour = c::Value)
        {
            dl->AddRectFilled(min, max, c::Track, th::px(16));
            const float ls = th::px(24), vs = body();
            const float w = max.x - min.x - th::px(20);
            auto fit = [&](const std::string &t, float size) {
                const float tw = measure(size, t.c_str()).x;
                return tw > w ? size * w / tw : size;
            };
            const float l = fit(label, ls), v = fit(value, vs);
            const float top = min.y + (max.y - min.y - l - v * 1.05f) * 0.5f;
            outlined(dl, ImVec2(min.x + th::px(10), top), l, c::Label, label.c_str(), th::px(2));
            outlined(dl, ImVec2(min.x + th::px(10), top + l * 1.05f), v, colour, value.c_str());
        }

        // Text cut with an ellipsis to fit.
        std::string ellipsize(const std::string &text, float size, float width)
        {
            if (measure(size, text.c_str()).x <= width)
                return text;
            std::string t = text;
            while (!t.empty() && measure(size, (t + "...").c_str()).x > width)
                t.pop_back();
            return t + "...";
        }

        void drawNotebook(ImDrawList *dl, ImVec2 cmin, ImVec2 cmax, const rt::game::Stats &st, PS2Runtime &runtime)
        {
            if (!st.adventure)
            {
                const char *msg = "Start or load an Adventure to fill your notebook.";
                const ImVec2 sz = measure(body(), msg);
                outlined(dl, ImVec2((cmin.x + cmax.x - sz.x) * 0.5f, (cmin.y + cmax.y - sz.y) * 0.5f), body(), c::Label, msg);
                return;
            }
            // The driver's name (miles on the right), then four chips.
            float y = cmin.y;
            outlined(dl, ImVec2(cmin.x, y), heading(), c::Name, st.playerName.c_str());
            char miles[32];
            std::snprintf(miles, sizeof(miles), "%.0f miles driven", st.mileageMetres / 1609.344);
            const ImVec2 msz = measure(body(), miles);
            outlined(dl, ImVec2(cmax.x - msz.x, y + (heading() - body()) * 0.5f), body(), c::Label, miles);
            y += heading() * 1.2f;
            const float chipH = th::px(64), chipGap = th::px(12);
            const float chipW = (cmax.x - cmin.x - chipGap * 4) / 5;
            const std::string currency = st.currency.empty() ? std::string("Money") : st.currency;
            const std::string values[5][2] = {{"Licence", rt::game::licenceName(st.licence)},
                                              {ellipsize(currency, th::px(24), chipW - th::px(20)), withCommas(st.money)},
                                              {"Coins", std::to_string(st.coins) + " / 100"},
                                              {"Towns", std::to_string(st.townsVisited) + " / 10"},
                                              {"Photos", std::to_string(st.photos) + " / 100"}};
            for (int i = 0; i < 5; ++i)
            {
                const float x = cmin.x + i * (chipW + chipGap);
                chip(dl, ImVec2(x, y), ImVec2(x + chipW, y + chipH), values[i][0], values[i][1], i >= 2 ? kGold : c::Value);
            }
            y += chipH + th::px(16);

            // Got / To get, one segmented control, and the bar.
            const int got = static_cast<int>(st.stamps.count());
            const float segH = th::px(72), r = segH * 0.5f;
            const ImVec2 smin(cmin.x, y), smax(cmax.x, y + segH);
            const float mid = (smin.x + smax.x) * 0.5f;
            dl->AddRectFilled(smin, smax, c::Track, r);
            const ImVec2 selMin(g_stampsGot ? smin.x : mid, smin.y), selMax(g_stampsGot ? mid : smax.x, smax.y);
            dl->AddRectFilled(selMin, selMax, th::col::BarCapGold, r);
            // The game's selection bar: blue between gold end caps.
            const ImVec2 inMin(selMin.x + th::px(10), selMin.y + th::px(4)), inMax(selMax.x - th::px(10), selMax.y - th::px(4));
            dl->AddRectFilled(inMin, inMax, th::col::BarBottom, r - th::px(4));
            dl->AddRectFilled(inMin, ImVec2(inMax.x, (inMin.y + inMax.y) * 0.5f), th::col::BarTop, r - th::px(4), ImDrawFlags_RoundCornersTop);
            const std::string a = "Got " + std::to_string(got), b = "To get " + std::to_string(100 - got);
            const ImVec2 asz = measure(body(), a.c_str()), bsz = measure(body(), b.c_str());
            outlined(dl, ImVec2((smin.x + mid - asz.x) * 0.5f, smin.y + (segH - asz.y) * 0.5f), body(), g_stampsGot ? c::Value : c::Label, a.c_str());
            outlined(dl, ImVec2((mid + smax.x - bsz.x) * 0.5f, smin.y + (segH - bsz.y) * 0.5f), body(), g_stampsGot ? c::Label : c::Value, b.c_str());
            if (tapped(smin, ImVec2(mid, smax.y)) && !g_stampsGot)
                g_stampsGot = true, g_scroll = 0;
            if (tapped(ImVec2(mid, smin.y), smax) && g_stampsGot)
                g_stampsGot = false, g_scroll = 0;
            y = smax.y + th::px(16);

            // The cards (drag to scroll).
            const ImVec2 lmin(cmin.x, y), lmax(cmax.x, cmax.y);
            std::vector<int> shown;
            if (g_stampsGot)
                for (int n : g_recent)
                    if (st.stamps[static_cast<size_t>(n - 1)])
                        shown.push_back(n);
            for (int i = 0; i < 100; ++i)
                if (st.stamps[static_cast<size_t>(i)] == g_stampsGot && std::find(shown.begin(), shown.end(), i + 1) == shown.end())
                    shown.push_back(i + 1);
            if (shown.empty())
            {
                const char *msg = g_stampsGot ? "No stamps yet: they come from people, places and races." : "Every stamp collected!";
                const ImVec2 sz = measure(body(), msg);
                outlined(dl, ImVec2((lmin.x + lmax.x - sz.x) * 0.5f, lmin.y + th::px(40)), body(), c::Label, msg);
                return;
            }
            const float cardH = th::px(112), gap = th::px(10);
            const ImGuiIO &io = ImGui::GetIO();
            const float now = static_cast<float>(ImGui::GetTime());
            if (io.MouseDown[0] && inside(io.MouseClickedPos[0], lmin, lmax) && io.MouseDelta.y != 0)
            {
                g_scroll -= io.MouseDelta.y;
                g_scrollShownUntil = now + 0.6f;
            }
            const float content = shown.size() * (cardH + gap) - gap, view = lmax.y - lmin.y;
            g_scroll = std::clamp(g_scroll, 0.0f, std::max(0.0f, content - view));
            ps2x::HostPresenter &presenter = *runtime.presenter();
            dl->PushClipRect(lmin, lmax, true);
            for (size_t k = 0; k < shown.size(); ++k)
            {
                const float top = lmin.y + k * (cardH + gap) - g_scroll;
                if (top + cardH < lmin.y || top > lmax.y)
                    continue;
                const int n = shown[k];
                const bool earned = g_stampsGot;
                // A notebook card: paper with a green rim, the stamp in its frame on the left.
                const ImVec2 kmin(lmin.x, top), kmax(lmax.x, top + cardH);
                dl->AddRectFilled(kmin, kmax, c::PaperRim, th::px(20));
                dl->AddRectFilled(ImVec2(kmin.x + th::px(6), kmin.y + th::px(6)), ImVec2(kmax.x - th::px(6), kmax.y - th::px(6)), c::Paper,
                                  th::px(14));
                const float icon = th::px(96);
                const ImVec2 fmin(kmin.x + th::px(16), kmin.y + (cardH - icon) * 0.5f), fmax(fmin.x + icon, fmin.y + icon);
                dl->AddRectFilled(ImVec2(fmin.x - th::px(4), fmin.y - th::px(4)), ImVec2(fmax.x + th::px(4), fmax.y + th::px(4)), c::PaperRim,
                                  th::px(18));
                dl->AddRectFilled(fmin, fmax, c::Frame, th::px(14));
                const StampIcons ic = stampIcons(presenter, n);
                // The newest stamp is pressed onto the page: ink fades in as the stamp comes down.
                const float since = static_cast<float>(ImGui::GetTime()) - g_newStampAt;
                const bool fresh = earned && !g_recent.empty() && n == g_recent.front() && since < 3.5f;
                if (fresh)
                {
                    const float glow = 0.5f + 0.5f * std::sin(since * 6.0f);
                    dl->AddRect(ImVec2(kmin.x - th::px(4), kmin.y - th::px(4)), ImVec2(kmax.x + th::px(4), kmax.y + th::px(4)),
                                IM_COL32(0xF8, 0xC8, 0x38, static_cast<int>(120 + 135 * glow)), th::px(22), 0, th::px(6));
                }
                if (fresh && ic.ink && ic.mask)
                {
                    const float t = std::clamp(since / 0.8f, 0.0f, 1.0f);
                    const float grow = 1.0f + 0.35f * (1.0f - t) * (1.0f - t);
                    const ImVec2 c((fmin.x + fmax.x) * 0.5f, (fmin.y + fmax.y) * 0.5f), half((fmax.x - fmin.x) * 0.5f * grow, (fmax.y - fmin.y) * 0.5f * grow);
                    dl->AddImage(static_cast<ImTextureID>(ic.mask), fmin, fmax, ImVec2(0, 0), ImVec2(1, 1), c::Silhouette);
                    dl->AddImage(static_cast<ImTextureID>(ic.ink), ImVec2(c.x - half.x, c.y - half.y), ImVec2(c.x + half.x, c.y + half.y),
                                 ImVec2(0, 0), ImVec2(1, 1), IM_COL32(0xFF, 0xFF, 0xFF, static_cast<int>(255 * t)));
                }
                else if (earned && ic.ink)
                    dl->AddImage(static_cast<ImTextureID>(ic.ink), fmin, fmax);
                else if (!earned && ic.mask)
                    dl->AddImage(static_cast<ImTextureID>(ic.mask), fmin, fmax, ImVec2(0, 0), ImVec2(1, 1), c::Silhouette);
                // The page number on the card's corner.
                const std::string num = std::to_string(n);
                const ImVec2 nsz = measure(tag(), num.c_str());
                const float tagW = std::max(th::px(44), nsz.x + th::px(16));
                const ImVec2 tmin(kmax.x - th::px(6) - tagW, kmin.y + th::px(6)), tmax(kmax.x - th::px(6), kmin.y + th::px(6) + th::px(34));
                dl->AddRectFilled(tmin, tmax, c::PaperRim, th::px(10));
                dl->AddText(th::font(), tag(), ImVec2((tmin.x + tmax.x - nsz.x) * 0.5f, (tmin.y + tmax.y - nsz.y) * 0.5f), c::Ink, num.c_str());
                // The caption, up to two lines.
                std::string caption = rt::game::stampCaption(runtime, n);
                if (caption.empty())
                    caption = "Stamp " + num;
                const float tx = fmax.x + th::px(24);
                const auto lines = wrap2(caption, body(), kmax.x - th::px(70) - tx);
                const float lh = body() * 1.25f;
                float ty = top + (cardH - lh * lines.size()) * 0.5f;
                for (const auto &l : lines)
                {
                    dl->AddText(th::font(), body(), ImVec2(tx, ty), earned ? c::Ink : c::InkFaint, l.c_str());
                    // Notebook ruling under each line.
                    dl->AddRectFilled(ImVec2(tx, ty + body() + th::px(4)), ImVec2(kmax.x - th::px(20), ty + body() + th::px(6)), c::PaperRim);
                    ty += lh;
                }
            }
            // Fade the bottom edge into the panel, and the scroll thumb while scrolling.
            if (g_scroll < content - view - 1.0f)
                dl->AddRectFilledMultiColor(ImVec2(lmin.x, lmax.y - th::px(20)), lmax, IM_COL32(0x30, 0x40, 0x90, 0x00),
                                            IM_COL32(0x30, 0x40, 0x90, 0x00), c::PanelBody, c::PanelBody);
            dl->PopClipRect();
            if (now < g_scrollShownUntil && content > view)
            {
                const float th_ = std::max(th::px(48), view * view / content);
                const float ty = lmin.y + (view - th_) * (g_scroll / (content - view));
                dl->AddRectFilled(ImVec2(lmax.x + th::px(8), ty), ImVec2(lmax.x + th::px(16), ty + th_), th::col::BarCapGold, th::px(4));
            }
        }

        // ---- now playing ----

        std::string g_lastSong;
        float g_songChangedAt = -10;

        // A strip along the bottom of the map page while the town radio plays: the station, then
        // the song (when we know it). Glows gold for a moment when the song changes.
        void nowPlayingStrip(ImDrawList *dl, ImVec2 min, ImVec2 max, const rt::game::NowPlaying &np)
        {
            const std::string song = np.title.empty() ? np.artist : np.title + "  \xE2\x80\x94  " + np.artist;
            const float now = static_cast<float>(ImGui::GetTime());
            if (np.station + song != g_lastSong)
                g_lastSong = np.station + song, g_songChangedAt = now;
            const float glow = std::clamp(1.0f - (now - g_songChangedAt) / 2.0f, 0.0f, 1.0f);
            const float r = (max.y - min.y) * 0.5f;
            dl->AddRectFilled(min, max, c::Track, r);
            if (glow > 0)
                dl->AddRect(min, max, IM_COL32(0xF8, 0xC8, 0x38, static_cast<int>(255 * glow)), r, 0, th::px(4));
            // A radio: a body with a dial and an aerial.
            const float h = max.y - min.y, x = min.x + r * 0.6f, cy = (min.y + max.y) * 0.5f;
            const ImVec2 bmin(x, cy - h * 0.18f), bmax(x + h * 0.7f, cy + h * 0.26f);
            dl->AddLine(ImVec2(bmin.x + h * 0.12f, bmin.y), ImVec2(bmin.x + h * 0.42f, cy - h * 0.38f), kGold, th::px(3));
            dl->AddRectFilled(bmin, bmax, kGold, th::px(6));
            dl->AddCircleFilled(ImVec2(bmax.x - h * 0.2f, (bmin.y + bmax.y) * 0.5f), h * 0.11f, c::Track, 12);
            float tx = bmax.x + th::px(18);
            const float ty = cy - body() * 0.5f;
            outlined(dl, ImVec2(tx, ty), body(), kGold, np.station.c_str());
            tx += measure(body(), np.station.c_str()).x + th::px(18);
            if (!song.empty())
            {
                const std::string fit = ellipsize(song, body(), max.x - r * 0.6f - tx);
                outlined(dl, ImVec2(tx, ty), body(), c::Value, fit.c_str());
            }
        }

        // ---- the screen ----

        enum class Page { Map, Notebook };
        Page g_auto = Page::Notebook; // what the game's state calls for
        Page g_chosen = Page::Notebook;
        bool g_overridden = false; // a tap chose another page; until the context changes

        // The active tab: the game's golden plate with two rivets, joined to the panel.
        void activeTab(ImDrawList *dl, ImVec2 min, ImVec2 max, const char *label)
        {
            const float r = th::px(28);
            const ImU32 top = IM_COL32(0xF8, 0xB0, 0x30, 0xFF), bottom = IM_COL32(0xE8, 0x84, 0x0C, 0xFF);
            dl->AddRectFilled(min, max, bottom, r, ImDrawFlags_RoundCornersTop);
            dl->AddRectFilled(min, ImVec2(max.x, (min.y + max.y) * 0.5f), top, r, ImDrawFlags_RoundCornersTop);
            dl->AddRectFilled(ImVec2(min.x + r * 0.5f, min.y), ImVec2(max.x - r * 0.5f, min.y + th::px(4)), IM_COL32(0xFF, 0xD8, 0x78, 0xFF));
            const ImVec2 sz = measure(body(), label);
            const float cy = (min.y + max.y - th::px(8)) * 0.5f;
            outlined(dl, ImVec2((min.x + max.x - sz.x) * 0.5f, cy - sz.y * 0.5f), body(), c::Value, label);
            for (float x : {min.x + th::px(22), max.x - th::px(22)})
            {
                dl->AddCircleFilled(ImVec2(x, cy), th::px(7), IM_COL32(0x48, 0xB8, 0xF0, 0xFF), 16);
                dl->AddCircle(ImVec2(x, cy), th::px(7), IM_COL32(0x1A, 0x3C, 0x8C, 0xFF), 16, th::px(2));
            }
        }

        void idleTab(ImDrawList *dl, ImVec2 min, ImVec2 max, const char *label, bool dim)
        {
            const float r = th::px(28);
            dl->AddRectFilled(min, max, IM_COL32(0x26, 0x34, 0x78, 0xFF), r, ImDrawFlags_RoundCornersTop);
            dl->AddRect(min, ImVec2(max.x, max.y + r), c::TabIdleRim, r, ImDrawFlags_RoundCornersTop, th::px(4));
            const ImVec2 sz = measure(body(), label);
            outlined(dl, ImVec2((min.x + max.x - sz.x) * 0.5f, (min.y + max.y - th::px(8) - sz.y) * 0.5f), body(),
                     dim ? IM_COL32(0x8A, 0x96, 0xC8, 0xFF) : c::Label, label);
        }

        void draw(PS2Runtime &runtime)
        {
            ImDrawList *dl = ImGui::GetForegroundDrawList();
            const ImVec2 size = ImGui::GetIO().DisplaySize;
            backdrop(dl, size);
            ps2x::HostPresenter &presenter = *runtime.presenter();
            const rt::game::Stats st = rt::game::readStats(runtime);
            float uv[4], aspect;
            // The attract demo leaves both screens alone: its map stays on top.
            const bool haveMap = !st.demo && presenter.mapRegion(uv, aspect);
            const float now = static_cast<float>(ImGui::GetTime());

            // A stamp earned just now (one new bit; loading a save changes many): show it.
            static std::bitset<100> seen;
            static bool seenValid = false;
            if (st.adventure)
            {
                const std::bitset<100> added = st.stamps & ~seen;
                if (seenValid && added.count() == 1)
                    for (int i = 0; i < 100; ++i)
                        if (added[static_cast<size_t>(i)])
                        {
                            g_recent.erase(std::remove(g_recent.begin(), g_recent.end(), i + 1), g_recent.end());
                            g_recent.insert(g_recent.begin(), i + 1);
                            g_newStampAt = now;
                            g_stampsGot = true;
                            g_scroll = 0;
                        }
                seen = st.stamps;
                seenValid = true;
            }
            else
                seenValid = false;
            const bool showingNewStamp = now - g_newStampAt < 3.5f;

            // The page follows the game (the map while driving or racing), unless a tap chose. A
            // change of context has to hold for a moment first: cuts and loads don't flash pages.
            const Page wanted = haveMap || st.racing ? Page::Map : Page::Notebook;
            static Page candidate = Page::Notebook;
            static float candidateSince = 0;
            if (wanted != candidate)
                candidate = wanted, candidateSince = now;
            if (candidate != g_auto && now - candidateSince > 0.75f)
                g_auto = candidate, g_overridden = false;
            // The new stamp takes the screen for a moment, whatever was on it (the map stays down).
            const Page page = showingNewStamp ? Page::Notebook : g_overridden ? g_chosen : g_auto;
            // Keep the map on the second screen unless the player chose the notebook over it.
            presenter.setWantMap(!st.demo && !(g_overridden && g_chosen == Page::Notebook));

            const float pad = th::px(24), rowH = th::px(84);
            const ImVec2 pmin(pad, pad + rowH), pmax(size.x - pad, size.y - pad);
            const float tabW = th::px(240), tabGap = th::px(16), tabTop = pad + th::px(8);
            const std::pair<Page, const char *> tabs[2] = {{Page::Map, "Map"}, {Page::Notebook, "Notebook"}};
            auto tabRect = [&](int i, bool active) {
                const float x = pmin.x + th::px(24) + i * (tabW + tabGap);
                return std::make_pair(ImVec2(x, active ? pad : tabTop), ImVec2(x + tabW, active ? pmin.y + th::px(6) : pmin.y + th::px(2)));
            };
            // Idle tabs tuck behind the panel; the active one is drawn over its rim.
            for (int i = 0; i < 2; ++i)
                if (tabs[i].first != page)
                {
                    const auto [tmin, tmax] = tabRect(i, false);
                    idleTab(dl, tmin, tmax, tabs[i].second, tabs[i].first == Page::Map && !haveMap && !st.racing);
                    if (tapped(tmin, tmax))
                        g_chosen = tabs[i].first, g_overridden = true;
                }
            panel(dl, pmin, pmax);
            for (int i = 0; i < 2; ++i)
                if (tabs[i].first == page)
                {
                    const auto [tmin, tmax] = tabRect(i, true);
                    activeTab(dl, tmin, tmax, tabs[i].second);
                }
            // Menu: the same height and baseline as the idle tabs, at the panel's right inset.
            const float menuW = th::px(168);
            const ImVec2 mmax(pmax.x - th::px(24), pmin.y - th::px(8));
            if (menuButton(dl, ImVec2(mmax.x - menuW, tabTop), mmax))
                togglePauseMenu();

            const float inset = th::px(30);
            const ImVec2 cmin(pmin.x + inset, pmin.y + inset), cmaxAll(pmax.x - inset, pmax.y - inset);
            if (page == Page::Notebook)
            {
                drawNotebook(dl, cmin, cmaxAll, st, runtime);
                return;
            }
            // Map page: the map on the left at full height, a column on the right (race or town),
            // and in town the radio along the bottom.
            const rt::game::NowPlaying np = st.inTown ? rt::game::nowPlaying(runtime) : rt::game::NowPlaying{};
            ImVec2 cmax = cmaxAll;
            if (np.playing)
            {
                const float stripH = th::px(64);
                nowPlayingStrip(dl, ImVec2(cmin.x, cmax.y - stripH), cmax, np);
                cmax.y -= stripH + th::px(16);
            }
            const float colW = th::px(250);
            const bool column = st.racing || st.inTown;
            ImVec2 mapEnd;
            MapPlacement where;
            const bool drawn =
                drawMap(dl, cmin, ImVec2(column ? cmax.x - colW - th::px(24) : cmax.x, cmax.y), presenter, !column, &mapEnd, !st.racing, &where);
            // The town's buildings, once per town (the tables don't change).
            static int placesTile = -1;
            static std::vector<rt::game::Place> places;
            static float placesAt = 0;
            if (st.inTown && (st.tile != placesTile || now - placesAt > 1.0f)) // done-ness changes as you play
            {
                places = rt::game::placesAround(runtime, st);
                placesTile = st.tile;
                placesAt = now;
            }
            if (drawn && st.inTown)
                drawPlaces(dl, where, st, places);
            if (!drawn && st.racing)
                bigPlace(dl, cmin, ImVec2(cmax.x - colW - th::px(24), cmax.y), st);
            else if (!drawn)
            {
                const char *msg = "The map shows here while you drive in a town or race.";
                const ImVec2 sz = measure(body(), msg);
                outlined(dl, ImVec2((cmin.x + cmax.x - sz.x) * 0.5f, (cmin.y + cmax.y - sz.y) * 0.5f), body(), c::Label, msg);
                return;
            }
            if (!column)
                return;
            // The column takes the room the map leaves (centred in it).
            const float left = drawn ? mapEnd.x + th::px(32) : cmax.x - colW;
            const float w = std::min(colW + th::px(160), cmax.x - left);
            if (st.racing)
                raceColumn(dl, left, cmin.y, w, st);
            else
                townColumn(dl, left, cmin.y, w, cmax.y, st, places);
        }
    }

    void updateSecondScreen(PS2Runtime &runtime)
    {
        ps2x::HostPresenter *presenter = runtime.presenter();
        if (!presenter)
            return;
        if (void *window = nullptr; rt::seconddisplay::takeWindow(window))
        {
            presenter->setSecondScreen(window); // takes its own reference
#if defined(__ANDROID__)
            if (window)
                ANativeWindow_release(static_cast<ANativeWindow *>(window));
#endif
        }
        const auto touches = rt::seconddisplay::takeTouches();
        int w = 0, h = 0;
        if (!presenter->secondScreenSize(w, h) || !ImGui::GetCurrentContext())
        {
            presenter->setWantMap(false); // the game's map stays on the main screen
            return;
        }

        ImGuiContext *previous = ImGui::GetCurrentContext();
        if (!g_context)
        {
            const ImGuiBackendFlags flags = ImGui::GetIO().BackendFlags;
            g_context = ImGui::CreateContext(ImGui::GetIO().Fonts); // the theme's fonts
            ImGui::SetCurrentContext(g_context);
            ImGuiIO &io = ImGui::GetIO();
            io.IniFilename = nullptr;
            io.BackendFlags |= flags & (ImGuiBackendFlags_RendererHasVtxOffset | ImGuiBackendFlags_RendererHasTextures);
        }
        ImGui::SetCurrentContext(g_context);
        ImGuiIO &io = ImGui::GetIO();
        io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
        io.DisplayFramebufferScale = ImVec2(1, 1);
        const double now = rt::host::now();
        io.DeltaTime = g_lastTime > 0 ? std::clamp(static_cast<float>(now - g_lastTime), 0.001f, 0.1f) : 1.0f / 60.0f;
        g_lastTime = now;
        for (const auto &t : touches)
        {
            if (t.pointer != 0)
                continue; // one finger
            io.AddMouseSourceEvent(ImGuiMouseSource_TouchScreen);
            io.AddMousePosEvent(t.x, t.y);
            if (t.kind != rt::seconddisplay::Touch::Move)
                io.AddMouseButtonEvent(0, t.kind == rt::seconddisplay::Touch::Down);
        }
        ImGui::NewFrame();
        draw(runtime);
        ImGui::Render();
        ImGui::SetCurrentContext(previous);
        presenter->submitSecondScreenUi(g_context);
    }
}
