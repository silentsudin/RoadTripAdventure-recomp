// Deterministic contract tests for native controller-prompt glyph selection.
#include "../../src/native/gameplay/glyphs.h"

#include <cstdio>

using namespace rt::native::gameplay;

namespace {
int gChecks = 0;
int gFailures = 0;

void check(bool value) {
    ++gChecks;
    if (!value) {
        std::fprintf(stderr, "FAIL glyphs check #%d\n", gChecks);
        ++gFailures;
    }
}

void testCompleteResolvableMatrix() {
    for (int family = 0; family < static_cast<int>(GlyphFamily::Count); ++family) {
        for (int action = 0; action < static_cast<int>(GlyphAction::Count); ++action) {
            const GlyphSpec spec = resolve(static_cast<GlyphAction>(action),
                                           static_cast<GlyphFamily>(family));
            check(spec.kind == GlyphKind::FontCode || spec.kind == GlyphKind::Shape ||
                  spec.kind == GlyphKind::Text);
            check(spec.textFallback != nullptr && spec.textFallback[0] != '\0');
            check(spec.kind != GlyphKind::FontCode || spec.fontCode != 0);
        }
    }
}

void testSdlFamilyMap() {
    check(mapSdlPadType(0) == GlyphFamily::Unknown);
    check(mapSdlPadType(1) == GlyphFamily::XboxStyle); // SDL_GAMEPAD_TYPE_STANDARD
    check(mapSdlPadType(2) == GlyphFamily::XboxStyle); // XBOX360
    check(mapSdlPadType(3) == GlyphFamily::XboxStyle); // XBOXONE
    check(mapSdlPadType(4) == GlyphFamily::Playstation); // PS3
    check(mapSdlPadType(5) == GlyphFamily::Playstation); // PS4
    check(mapSdlPadType(6) == GlyphFamily::Playstation); // PS5
    for (int type = 7; type <= 10; ++type) check(mapSdlPadType(type) == GlyphFamily::Nintendo);
    check(mapSdlPadType(11) == GlyphFamily::XboxStyle); // GAMECUBE, non-Switch
    check(mapSdlPadType(12) == GlyphFamily::XboxStyle); // STEAM, non-PS
    check(mapSdlPadType(13) == GlyphFamily::XboxStyle); // COUNT/sentinel is non-PS
    check(mapSdlPadType(99) == GlyphFamily::XboxStyle); // future/non-PS default
}

void testFamilyDifferencesAndTriggerLabels() {
    const GlyphSpec psConfirm = resolve(GlyphAction::Confirm, GlyphFamily::Playstation);
    const GlyphSpec xboxConfirm = resolve(GlyphAction::Confirm, GlyphFamily::XboxStyle);
    const GlyphSpec nintendoConfirm = resolve(GlyphAction::Confirm, GlyphFamily::Nintendo);
    check(psConfirm.shape == GlyphShape::Circle);
    check(labelFor(psConfirm) == "◯");
    check(labelFor(xboxConfirm) == "A");
    check(labelFor(nintendoConfirm) == "A");
    check(labelFor(resolve(GlyphAction::Back, GlyphFamily::Playstation)) == "X");
    check(labelFor(resolve(GlyphAction::Back, GlyphFamily::XboxStyle)) == "B");
    check(labelFor(resolve(GlyphAction::Back, GlyphFamily::Nintendo)) == "B");
    check(labelFor(resolve(GlyphAction::Brake, GlyphFamily::Playstation)) == "L2");
    check(labelFor(resolve(GlyphAction::Gas, GlyphFamily::Playstation)) == "R2");
    check(labelFor(resolve(GlyphAction::Brake, GlyphFamily::XboxStyle)) == "LT");
    check(labelFor(resolve(GlyphAction::Gas, GlyphFamily::XboxStyle)) == "RT");
    check(labelFor(resolve(GlyphAction::Brake, GlyphFamily::Nintendo)) == "ZL");
    check(labelFor(resolve(GlyphAction::Gas, GlyphFamily::Nintendo)) == "ZR");
}

void testGroundingAndRepaintPolicy() {
    // Static ELF strings establish no non-ASCII FONTE button code in this pass.
    // The module must therefore retain Shapes instead of inventing FontCode slots.
    check(!kPlaystationFontCodesGrounded);
    for (int action = 0; action < static_cast<int>(GlyphAction::Count); ++action) {
        const GlyphSpec spec = resolve(static_cast<GlyphAction>(action), GlyphFamily::Playstation);
        check(spec.kind != GlyphKind::FontCode);
        check(spec.provisional);
    }
    check(!isRepaintNeeded(GlyphFamily::Playstation));
    check(!isRepaintNeeded(GlyphFamily::Unknown));
    check(isRepaintNeeded(GlyphFamily::XboxStyle));
    check(isRepaintNeeded(GlyphFamily::Nintendo));
}
} // namespace

int main() {
    testCompleteResolvableMatrix();
    testSdlFamilyMap();
    testFamilyDifferencesAndTriggerLabels();
    testGroundingAndRepaintPolicy();
    std::printf("glyphs: %d checks, %d failures\n", gChecks, gFailures);
    return gFailures == 0 ? 0 : 1;
}
