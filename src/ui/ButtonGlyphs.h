#pragma once

// The game's own button glyphs for the pad in use. Road Trip draws PlayStation shapes (the font
// atlas's ○ ✕ □ △ icons, L1/L2/R1/R2 and the "BACK △" / "OK ✕" buttons); with an Xbox-style or
// Nintendo pad those are repainted, as the GS decodes the atlas, with the pad's letters in the
// glyphs' own colours, for the layout the input layer gives those pads (confirm A, back B: ✕ A,
// △ B; ○ Y on Xbox, X on Nintendo; □ the other; LB/LT/RB/RT or L/ZL/R/ZR). Only coordinates and
// drawing code: no game art. Hardware GS only (it decodes textures on the CPU); RT_GLYPH_DEBUG=1
// logs the atlas fingerprint check.
namespace rt::ui
{
    // Once per host frame: follows the pad family (theme::padFamilyName) and re-installs the
    // backend's decode hook when it changes.
    void updateButtonGlyphs();
}
