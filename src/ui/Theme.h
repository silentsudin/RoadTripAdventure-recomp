#pragma once

// The recomp UI's look, taken from Road Trip's own menus (colours sampled from the game; see
// .claude/agents/choroq-style-critic.md): olive dialog panels in an orange frame with a name tab,
// a blue gradient selection bar with a gold horn cursor, blue-rimmed prompt boxes, chunky rounded
// type with dark outlines. Sizes are given at 1080p and scale with the window height.

#include "imgui.h"

#include <string>

namespace rt::ui::theme
{
    // Colours (ImU32, IM_COL32 order).
    namespace col
    {
        constexpr ImU32 Olive = IM_COL32(0x48, 0x58, 0x00, 0xF2);
        constexpr ImU32 OliveRim = IM_COL32(0x23, 0x2A, 0x08, 0xFF);
        constexpr ImU32 FrameOuter = IM_COL32(0xC0, 0x5B, 0x1E, 0xFF);
        constexpr ImU32 Frame = IM_COL32(0xF8, 0x60, 0x18, 0xFF);
        constexpr ImU32 FrameLight = IM_COL32(0xF8, 0xBF, 0x84, 0xFF);
        constexpr ImU32 Cream = IM_COL32(0xD6, 0xCA, 0x77, 0xFF);
        constexpr ImU32 Gold = IM_COL32(0xE5, 0xD2, 0x7C, 0xFF);
        constexpr ImU32 Silver = IM_COL32(0xE8, 0xE8, 0xE8, 0xFF);
        constexpr ImU32 BarTop = IM_COL32(0x2C, 0x6C, 0xE8, 0xFF);
        constexpr ImU32 BarBottom = IM_COL32(0x08, 0x38, 0xB0, 0xFF);
        constexpr ImU32 BarCapGold = IM_COL32(0xF8, 0xC8, 0x38, 0xFF);
        constexpr ImU32 BarCapShine = IM_COL32(0xFF, 0xF0, 0xA0, 0xFF);
        constexpr ImU32 Horn = IM_COL32(0xF2, 0xC2, 0x30, 0xFF);
        constexpr ImU32 HornShade = IM_COL32(0xC0, 0x8A, 0x18, 0xFF);
        constexpr ImU32 HornLine = IM_COL32(0x4A, 0x30, 0x00, 0xFF);
        constexpr ImU32 Rivet = IM_COL32(0x3A, 0xA8, 0xC8, 0xFF);
        constexpr ImU32 RivetRing = IM_COL32(0x0B, 0x3C, 0x55, 0xFF);
        constexpr ImU32 HintBand = IM_COL32(0x14, 0x6F, 0xA6, 0xFF);
        // List menus (Q's Factory and shop lists): a sky-blue body in the orange frame.
        constexpr ImU32 ListBody = IM_COL32(0x00, 0x98, 0xC8, 0xFF);
        constexpr ImU32 ListBevelLight = IM_COL32(0x8E, 0xD4, 0xF0, 0xFF);
        constexpr ImU32 ListBevelDark = IM_COL32(0x0F, 0x5E, 0x92, 0xFF);
        constexpr ImU32 ListText = IM_COL32(0xF6, 0xE6, 0x8F, 0xFF);
        constexpr ImU32 ListSelected = IM_COL32(0xEC, 0xEC, 0xEC, 0xFF);
        constexpr ImU32 Heading = IM_COL32(0xF8, 0xC8, 0x38, 0xFF);
        constexpr ImU32 Black = IM_COL32(0x00, 0x00, 0x00, 0xFF);
        constexpr ImU32 White = IM_COL32(0xFF, 0xFF, 0xFF, 0xFF);
        constexpr ImU32 Prompt = IM_COL32(0x30, 0x40, 0x90, 0xF2);
        constexpr ImU32 PromptRimOuter = IM_COL32(0x91, 0xD2, 0xE8, 0xFF);
        constexpr ImU32 PromptRimInner = IM_COL32(0x1C, 0xA1, 0xE6, 0xFF);
        constexpr ImU32 Outline = IM_COL32(0x1A, 0x1A, 0x10, 0xFF);
        constexpr ImU32 OutlineBlue = IM_COL32(0x00, 0x08, 0x20, 0xFF);
        constexpr ImU32 Disabled = IM_COL32(0x9F, 0x8C, 0x47, 0xFF);
        constexpr ImU32 Scrim = IM_COL32(0x00, 0x0A, 0x28, 0x99);
    }

    enum class Size { Hint, Body, Heading, Title };

    // Loads the bundled fonts (Resources/fonts) and sets ImGui's style. After rlImGuiSetup.
    void initialize();
    float scale(); // window height / 1080
    float px(float at1080p);
    ImFont *font();
    float fontSize(Size s);

    // Text with the game's dark outline.
    void text(ImDrawList *dl, ImVec2 pos, Size size, ImU32 colour, const char *s, ImU32 outline = col::Outline);
    ImVec2 measure(Size size, const char *s);

    // Panels and pieces. List panels (menus) are sky blue with small radii and corner rivets like
    // the game's shop and factory lists; speech-style dialogs are olive and rounder.
    void dialogPanel(ImDrawList *dl, ImVec2 min, ImVec2 max, const char *nameTab = nullptr, bool list = true);
    void promptBox(ImDrawList *dl, ImVec2 min, ImVec2 max);
    // `capHeight` > 0: the gold cap that tall, centred (tall rows), instead of the bar's height.
    void selectionBar(ImDrawList *dl, ImVec2 min, ImVec2 max, float capHeight = 0.0f);
    void horn(ImDrawList *dl, ImVec2 tip, float height, float time);
    void pillTab(ImDrawList *dl, ImVec2 min, ImVec2 max, bool active, const char *label);
    void scrim(ImDrawList *dl, ImVec2 min, ImVec2 max, float amount = 1.0f);
    // A small filled arrow (value can change), pointing left or right.
    void arrow(ImDrawList *dl, ImVec2 centre, bool right);
    // The sunken band the focused row's explanation sits in.
    void hintBand(ImDrawList *dl, ImVec2 min, ImVec2 max);

    // A button glyph ("cross", "circle", "square", "triangle", "l1", "r1", "guide", "select",
    // "start", or a key name) as the connected pad shows it, followed by `label`; returns the width.
    // On the keyboard cross is Enter, triangle Esc and square R.
    float prompt(ImDrawList *dl, ImVec2 pos, const char *button, const char *label);
    // A physical control as the pad in hand shows it: face buttons as round badges (A/B/X/Y in
    // their colours, ✕ ○ □ △ on PlayStation), shoulders, triggers and the rest as pills, keys as
    // keycaps. family: "keyboard" (key = its name), "ps", "xbox", "nintendo", or "text" (key = a
    // word drawn between badges); kind 0 = a button
    // (index: SDL gamepad button), 1 / 2 = an axis pushed + / - (index: SDL gamepad axis).
    // Returns the width; dl == nullptr only measures.
    float control(ImDrawList *dl, ImVec2 pos, const std::string &family, int kind, int index, const std::string &key);

    // The glyph family of the first connected controller: "ps", "xbox", "nintendo", or "keyboard".
    std::string padFamilyName();
}
