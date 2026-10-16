#pragma once

// Output-mode policy for the native host: the standard progressive
// resolution ladder, preset/WxH parsing, aspect-fit/letterbox math, HUD
// scaling from the 640x448 authored space, and the progressive-present
// invariant.  Design and evidence: docs/native-reference/output-modes.md.
// Pure C++ by the tree's library rule: no SDL, no Vulkan, no guest code, and
// no game data - every value below is either ELF-proven (labelled CONFIRMED,
// cross-cited from the native-reference docs) or authored policy (AUTHORED).
//
// Original display mode (see output-modes.md §1; addresses are SLUS-203.98
// EE vaddrs):
//   * sceGsResetGraph(0,1,2,1) + the two sceGsSetDefDBuffDc alternatives at
//     0x002000d4..0x0020018c; SetGsCrt(1,2,1) via 0x0026e328..0x0026e348 =
//     NTSC, interlaced, frame mode (tuple semantics INFERRED).
//   * The double buffers are FIELDS: 640x224 (0x280x0e0) - the NTSC mode the
//     audience ran - and 512x256 (0x200x0x100) as the PAL alternative.  The
//     root loop swaps them every VBlank (0x0020067c..0x002006d8) and feeds GS
//     CSR bit-13 field parity (0x0026e494..0x0026e4a4) to the display
//     environments; the recomp additionally proves the parity half-line shift
//     (sceGsSetHalfOffset 0x26F340/0x26F3C8, forced to 0 there).
//   * The authored picture those fields combine into is the 640x448 UI space
//     (hud-layout.md: 640x448 CONFIRMED; the +16 centring of the 448-line
//     field in the 480-line frame INFERRED).
//
// The native app presents natively (no GS emulation): one full-frame RGBA
// blit per frame.  There is no field-parity or half-offset path anywhere in
// src/native, so every presented frame is progressive BY CONSTRUCTION.  This
// module pins that as a checkable contract (progressiveContractSatisfied)
// and defines the standard-resolution presentation so the window is a
// deliberate choice rather than an accident of the initial 4:3 window.

#include <array>
#include <cstdint>
#include <string_view>

namespace rt::native::app {

// ------------------------------------------------------------------ authored
// The combined original picture: two 224-line fields (CONFIRMED DBuff
// 0x280x0e0 at frame-timing.md). All native HUD geometry is authored in
// this space (gameplay::kHudUiWidth/kHudUiHeight mirror these values).
inline constexpr int kAuthoredWidthPx = 640;    // full display width
inline constexpr int kAuthoredHeightPx = 448;   // 2 x 224-line field
inline constexpr int kAuthoredFieldLines = 224;  // lines per field buffer

// The 480-line NTSC frame fiction the 448-line picture sat inside, with the
// 16-line centring border top and bottom (hud-layout.md; the centring is the
// one INFERRED mapping step).  Used for analysis/re-anchoring and the 4:3
// full-frame presentation option, not for the default HUD anchor.
inline constexpr int kAuthoredFrameLines = 480;      // 4:3 640x480 frame
inline constexpr int kAuthoredFrameYOffset = 16;     // INFERRED centring

// Square-pixel aspect of the authored picture: 640/448 = 10/7.  On a CRT the
// 640x448 area spanned the full 4:3 display width over 448/480 of its
// height, i.e. 10:7 - an exact match, so a 10:7 host rect reproduces the
// original proportions with zero stretch.
inline constexpr float kAuthoredAspect =
    static_cast<float>(kAuthoredWidthPx) / static_cast<float>(kAuthoredHeightPx);
inline constexpr float kDisplayFrameAspect =
    static_cast<float>(kAuthoredWidthPx) / static_cast<float>(kAuthoredFrameLines);

// --------------------------------------------------------------- resolution
struct Resolution {
    int width = 0;
    int height = 0;
};
constexpr bool operator==(const Resolution& a, const Resolution& b) {
    return a.width == b.width && a.height == b.height;
}
constexpr bool valid(const Resolution& r) { return r.width > 0 && r.height > 0; }
// The sentinel meaning "host desktop display mode at app start" (the
// RT_APP_RESOLUTION "native"/"desktop" preset). The SDL glue resolves it.
constexpr bool isNativeDesktop(const Resolution& r) { return r.width == 0 && r.height == 0; }

// The ladder of standard output resolutions.  Every fixed entry presents
// the authored picture progressively; "native" defers to the desktop mode.
struct OutputPreset {
    const char* name;
    Resolution resolution;
};
inline constexpr std::array<OutputPreset, 6> kOutputPresets{{
    {"native", {0, 0}},         // sentinel: desktop display mode
    {"720p", {1280, 720}},      // HDTV 720p
    {"1080p", {1920, 1080}},    // HDTV 1080p / FHD
    {"1440p", {2560, 1440}},    // QHD
    {"2160p", {3840, 2160}},    // UHD 4K
    {"4k", {3840, 2160}},       // alias of 2160p (kept in the table so both
                                // canonical tokens are visible to callers)
}};

// Any token the preset grammar accepts (including the native/desktop
// sentinel): the companion predicate for parseResolution, because the
// sentinel resolves to the same {0,0} bytes as a failed parse and the host
// needs to tell "explicitly desktop" from "unparseable" to honour its
// invalid-input logging.
bool isPresetName(std::string_view name);

// parseResolution: the RT_APP_RESOLUTION grammar.
//   "<preset>"  -> preset resolution; case-insensitive; accepted tokens are
//                  the kOutputPresets names plus "desktop" (-> native) and
//                  "uhd" (-> 2160p).
//   "<W>x<H>"   -> exact custom size, W/H in [1, 32768], optional surrounding
//                  ASCII spaces, 'x' or 'X' separator, no sign.
//   anything else (empty, missing "x", trailing junk, out-of-range,
//   zero/overflowed dimension) -> {0, 0} and valid() == false.
// (Declared here, defined in output_modes.cpp: the grammar is host policy,
// not constant-evaluation surface; every arithmetic contract above it is
// constexpr in this header.)
Resolution parseResolution(std::string_view text);

// ------------------------------------------------------------- letterboxing
struct RectF {
    float x = 0.f;
    float y = 0.f;
    float width = 0.f;
    float height = 0.f;
};

// Fit a content rectangle of the given aspect (width/height > 0) into a
// window, centred, with no stretch.  `viewport` is the content rect in
// window pixels; the four bars cover the remainder of the window (a bar with
// width == 0 or height == 0 means that side has no margin).  Degenerate
// inputs (non-positive window or content aspect) return an all-zero fit
// deterministically - never NaN.
struct AspectFit {
    RectF viewport;
    RectF barLeft;
    RectF barRight;
    RectF barTop;
    RectF barBottom;
};
constexpr AspectFit fitContentToWindow(int windowWidth, int windowHeight,
                                      float contentAspect) {
    if (windowWidth <= 0 || windowHeight <= 0 || !(contentAspect > 0.f)) return {};
    const float w = static_cast<float>(windowWidth);
    const float h = static_cast<float>(windowHeight);
    const float windowAspect = w / h;
    AspectFit fit;
    if (windowAspect > contentAspect) {
        // Window wider than the content: pillarbox, bars on the left/right.
        const float vw = h * contentAspect;
        const float x = (w - vw) * 0.5f;
        fit.viewport = {x, 0.f, vw, h};
        fit.barLeft = {0.f, 0.f, x, h};
        fit.barRight = {x + vw, 0.f, w - (x + vw), h};
        fit.barTop = {0.f, 0.f, w, 0.f};
        fit.barBottom = {0.f, h, w, 0.f};
    } else {
        // Window taller (or the same): letterbox, bars top/bottom.
        const float vh = w / contentAspect;
        const float y = (h - vh) * 0.5f;
        fit.viewport = {0.f, y, w, vh};
        fit.barTop = {0.f, 0.f, w, y};
        fit.barBottom = {0.f, y + vh, w, h - (y + vh)};
        fit.barLeft = {0.f, 0.f, 0.f, h};
        fit.barRight = {w, 0.f, 0.f, h};
    }
    return fit;
}

// The authored 640x448 picture (10/7) fitted into a window: the default HUD
// anchor and the 4:3-original pillarbox target.
constexpr AspectFit fitAuthoredPicture(int windowWidth, int windowHeight) {
    return fitContentToWindow(windowWidth, windowHeight, kAuthoredAspect);
}

// The 4:3 640x480 display frame fitted into a window: the recomp-style HUD
// narrowing / 2D-screen anchor (its widescreen stance narrows the HUD into a
// centred 4:3 region and shows 2D-backed screens at 4:3).  Combine with
// kAuthoredFrameYOffset to map authored 640x448 coordinates inside the
// frame: x = viewport.x + uiX * (viewport.width / 640),
// y = viewport.y + (uiY + 16) * (viewport.height / 480).
constexpr AspectFit fitDisplayFrame(int windowWidth, int windowHeight) {
    return fitContentToWindow(windowWidth, windowHeight, kDisplayFrameAspect);
}

// ------------------------------------------------------------ HUD placement
// Maps the authored 640x448 HUD/UI space into window pixels with a uniform
// scale (never stretch).  `target` is the window-pixel rectangle the
// authored raster occupies; authored pixel (uiX, uiY) lands at
// (target.x + uiX * scale, target.y + uiY * scale).
enum class HudScaleMode : std::uint8_t {
    Exact,           // min(windowW / 640, windowH / 448), full-fit
    IntegerSnapped,  // floor of the exact factor, clamped up to 1x (crisp
                     // texel grids; the target may exceed a tiny window)
};

// Exact uniform scale for the authored picture. 0 when the window is
// degenerate.  1280x896 -> exactly 2.0 (the integer-perfect configuration,
// matching the recomp's 8x/16x progressive scanout of 1280x896).
constexpr float hudExactScale(int windowWidth, int windowHeight) {
    if (windowWidth <= 0 || windowHeight <= 0) return 0.f;
    const float sx = static_cast<float>(windowWidth) / static_cast<float>(kAuthoredWidthPx);
    const float sy = static_cast<float>(windowHeight) / static_cast<float>(kAuthoredHeightPx);
    return sx < sy ? sx : sy;
}

// Integer-snapped factor: largest whole multiple of the authored space that
// fits, at least 1x.  A window smaller than 640x448 still snaps to 1x and
// the target then extends past the window edges (the app clips).
constexpr int hudIntegerScale(int windowWidth, int windowHeight) {
    const float exact = hudExactScale(windowWidth, windowHeight);
    int whole = static_cast<int>(exact);  // floor (exact >= 0 here)
    if (whole < 1) whole = 1;
    return whole;
}

struct HudPlacement {
    float scale = 0.f;   // authored px -> window px (uniform)
    RectF target{};      // authored 640x448 rect in window pixels
};
constexpr HudPlacement hudPlacement(int windowWidth, int windowHeight,
                                    HudScaleMode mode) {
    HudPlacement p;
    if (mode == HudScaleMode::IntegerSnapped) {
        p.scale = static_cast<float>(hudIntegerScale(windowWidth, windowHeight));
    } else {
        p.scale = hudExactScale(windowWidth, windowHeight);
    }
    p.target.width = p.scale * static_cast<float>(kAuthoredWidthPx);
    p.target.height = p.scale * static_cast<float>(kAuthoredHeightPx);
    p.target.x = (static_cast<float>(windowWidth) - p.target.width) * 0.5f;
    p.target.y = (static_cast<float>(windowHeight) - p.target.height) * 0.5f;
    return p;
}

// ------------------------------------------------------- progressive safety
// The invariant this module pins: one native present is a full progressive
// frame.  (1) It covers both original fields - the composed picture has at
// least twice the field line count (the original 640x448 picture and the
// recomp's 640x448 progressive double-strike both sit exactly at the 2x
// boundary, so >= is the contract).  (2) No display-space half-line/parity
// offset is applied at host present: the retail CRT path (224-line field,
// odd lines shifted half a line) fails here, as does anything riding a
// parity flag; a correct native present passes with height 448 and offset 0.
struct ProgressivePresent {
    int contentLines = kAuthoredHeightPx;  // authored display lines presented
    float parityOffsetPx = 0.f;            // y offset applied to odd/parity lines
                                           // at host present (must be exactly 0;
                                           // 0 is representable in binary FP)
};
constexpr bool progressiveContractSatisfied(const ProgressivePresent& present,
                                            int fieldLines = kAuthoredFieldLines) {
    return present.contentLines >= 2 * fieldLines && present.parityOffsetPx == 0.f;
}

}  // namespace rt::native::app
