// Pure controller-prompt glyph selection for the native Road Trip rebuild.
//
// The FONTE non-ASCII PlayStation codes are deliberately not guessed here:
// see docs/native-reference/glyph-repaint.md for the ELF evidence and the
// deterministic atlas-dump closure step. Until that step produces code/slot
// evidence, PlayStation button prompts use vector Shapes rather than FontCode.
#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace rt::native::gameplay {

enum class GlyphFamily : std::uint8_t { Playstation, XboxStyle, Nintendo, Unknown, Count };

// Numeric SDL3 SDL_GamepadType values. Kept as ints so this pure module does
// not require SDL headers or a host linkage.
GlyphFamily mapSdlPadType(int sdlGamepadTypeValue);

// Includes all native driving bindings (menu.h ControllerAction) plus the
// menu/help actions. Gas/brake are textual trigger labels by contract.
enum class GlyphAction : std::uint8_t {
    Steer, Pitch, Reverse, Jet, WingUp, WingDown, Horn, Lights, View, Navigator,
    Confirm, Back, Pause, Start, Gas, Brake, Count,
};

enum class GlyphKind : std::uint8_t { FontCode, Shape, Text };
enum class GlyphShape : std::uint8_t {
    Circle, Cross, Square, Triangle, LetterA, LetterB, LetterX, LetterY,
    LB, RB, LT, RT, Minus, Plus, Home, Create,
};

struct GlyphSpec {
    GlyphKind kind = GlyphKind::Text;
    std::uint8_t fontCode = 0;       // only meaningful for FontCode
    GlyphShape shape = GlyphShape::Circle;
    const char* textFallback = "?"; // ASCII keyboard/help representation
    bool provisional = false;        // true when static ELF has not grounded it
};

inline constexpr bool kPlaystationFontCodesGrounded = false;
inline constexpr std::size_t kGlyphFamilyCount = static_cast<std::size_t>(GlyphFamily::Count);
inline constexpr std::size_t kGlyphActionCount = static_cast<std::size_t>(GlyphAction::Count);

// The complete action x family decision table. It is exposed for renderer
// glue/tests; resolve() supplies bounds-safe access for runtime callers.
extern const std::array<std::array<GlyphSpec, kGlyphFamilyCount>, kGlyphActionCount> kGlyphSpecs;

GlyphSpec resolve(GlyphAction action, GlyphFamily family);
std::string_view labelFor(const GlyphSpec& spec);

// Native renderer policy: PS and an SDL-unknown pad retain PS-default prompts;
// every known non-PS family needs the prompt presentation rebuilt.
bool isRepaintNeeded(GlyphFamily family);

} // namespace rt::native::gameplay
