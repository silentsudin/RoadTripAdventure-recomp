#include "glyphs.h"

namespace rt::native::gameplay {
namespace {
constexpr GlyphSpec shape(GlyphShape value, const char* label, bool provisional = false) {
    return {GlyphKind::Shape, 0, value, label, provisional};
}
constexpr GlyphSpec text(const char* label, bool provisional = false) {
    return {GlyphKind::Text, 0, GlyphShape::Circle, label, provisional};
}
constexpr bool kPsProvisional = !kPlaystationFontCodesGrounded;
} // namespace

// Columns: PlayStation, Xbox-style, Nintendo, SDL unknown (PS-default).
const std::array<std::array<GlyphSpec, kGlyphFamilyCount>, kGlyphActionCount> kGlyphSpecs = {{
    {{text("LS", kPsProvisional), text("LS"), text("LS"), text("LS", kPsProvisional)}},
    {{text("RS", kPsProvisional), text("RS"), text("RS"), text("RS", kPsProvisional)}},
    {{shape(GlyphShape::Circle, "◯", kPsProvisional), shape(GlyphShape::LetterB, "B"), shape(GlyphShape::LetterB, "B"), shape(GlyphShape::Circle, "◯", kPsProvisional)}},
    {{shape(GlyphShape::Cross, "X", kPsProvisional), shape(GlyphShape::LetterA, "A"), shape(GlyphShape::LetterA, "A"), shape(GlyphShape::Cross, "X", kPsProvisional)}},
    {{shape(GlyphShape::LB, "L1", kPsProvisional), shape(GlyphShape::LB, "LB"), shape(GlyphShape::LB, "L"), shape(GlyphShape::LB, "L1", kPsProvisional)}},
    {{shape(GlyphShape::LT, "L2", kPsProvisional), shape(GlyphShape::RB, "RB"), shape(GlyphShape::RB, "R"), shape(GlyphShape::LT, "L2", kPsProvisional)}},
    {{shape(GlyphShape::Triangle, "Triangle", kPsProvisional), shape(GlyphShape::LetterY, "Y"), shape(GlyphShape::LetterX, "X"), shape(GlyphShape::Triangle, "Triangle", kPsProvisional)}},
    {{shape(GlyphShape::RB, "R1", kPsProvisional), shape(GlyphShape::LetterY, "Y"), shape(GlyphShape::LetterX, "X"), shape(GlyphShape::RB, "R1", kPsProvisional)}},
    {{shape(GlyphShape::Circle, "◯", kPsProvisional), shape(GlyphShape::LetterB, "B"), shape(GlyphShape::LetterB, "B"), shape(GlyphShape::Circle, "◯", kPsProvisional)}},
    {{shape(GlyphShape::Minus, "Select", kPsProvisional), shape(GlyphShape::Minus, "Back"), shape(GlyphShape::Minus, "-"), shape(GlyphShape::Minus, "Select", kPsProvisional)}},
    {{shape(GlyphShape::Circle, "◯", kPsProvisional), shape(GlyphShape::LetterA, "A"), shape(GlyphShape::LetterA, "A"), shape(GlyphShape::Circle, "◯", kPsProvisional)}},
    {{shape(GlyphShape::Cross, "X", kPsProvisional), shape(GlyphShape::LetterB, "B"), shape(GlyphShape::LetterB, "B"), shape(GlyphShape::Cross, "X", kPsProvisional)}},
    {{shape(GlyphShape::Minus, "Select", kPsProvisional), shape(GlyphShape::Home, "Menu"), shape(GlyphShape::Home, "Home"), shape(GlyphShape::Minus, "Select", kPsProvisional)}},
    {{shape(GlyphShape::Plus, "Start", kPsProvisional), shape(GlyphShape::Plus, "Start"), shape(GlyphShape::Plus, "+"), shape(GlyphShape::Plus, "Start", kPsProvisional)}},
    {{text("R2", kPsProvisional), text("RT"), text("ZR"), text("R2", kPsProvisional)}},
    {{text("L2", kPsProvisional), text("LT"), text("ZL"), text("L2", kPsProvisional)}},
}};

GlyphFamily mapSdlPadType(int type) {
    switch (type) {
    case 0: return GlyphFamily::Unknown; // SDL_GAMEPAD_TYPE_UNKNOWN
    case 4: case 5: case 6: return GlyphFamily::Playstation; // PS3/PS4/PS5
    case 7: case 8: case 9: case 10: return GlyphFamily::Nintendo; // Switch variants
    default: return GlyphFamily::XboxStyle; // STANDARD, Xbox, Steam, future non-PS
    }
}

GlyphSpec resolve(GlyphAction action, GlyphFamily family) {
    const auto actionIndex = static_cast<std::size_t>(action);
    const auto familyIndex = static_cast<std::size_t>(family);
    if (actionIndex >= kGlyphActionCount || familyIndex >= kGlyphFamilyCount) return text("?");
    return kGlyphSpecs[actionIndex][familyIndex];
}

std::string_view labelFor(const GlyphSpec& spec) { return spec.textFallback; }

bool isRepaintNeeded(GlyphFamily family) {
    return family == GlyphFamily::XboxStyle || family == GlyphFamily::Nintendo;
}

} // namespace rt::native::gameplay
