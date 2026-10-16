// Output-mode contract tests. No game data: pins the standard progressive
// resolution ladder, the RT_APP_RESOLUTION grammar, aspect-fit/letterbox math
// (4:3-in-16:9 and 16:9-in-4:3, hand-computed), HUD scaling from the 640x448
// authored space (x2 at 1280x896, exact ratios at 1080p/4K, integer snap),
// and the progressive-present invariant. Expected rects/ratios are derived
// by hand in docs/native-reference/output-modes.md.
#include "../../src/native/app/output_modes.h"

#include <cstdio>

using namespace rt::native::app;

namespace {

int g_checks = 0;
int g_fail = 0;

void check(bool ok) {
    ++g_checks;
    if (!ok) {
        std::fprintf(stderr, "FAIL output_modes check #%d\n", g_checks);
        ++g_fail;
    }
}

bool near(float a, float b, float eps = 1e-3f) {
    const float d = a - b < 0.f ? b - a : a - b;
    return d <= eps;
}

bool rectNear(const RectF& r, float x, float y, float w, float h, float eps = 1e-3f) {
    return near(r.x, x, eps) && near(r.y, y, eps) && near(r.width, w, eps) &&
           near(r.height, h, eps);
}

Resolution res(int w, int h) { return Resolution{w, h}; }

// ------------------------------------------------------------ preset ladder

void testPresetTable() {
    // Exact standard values, nothing invented.
    check(kOutputPresets.size() == 6);
    check(std::string_view(kOutputPresets[0].name) == "native");
    check(isNativeDesktop(kOutputPresets[0].resolution));
    check(std::string_view(kOutputPresets[1].name) == "720p" &&
          kOutputPresets[1].resolution == res(1280, 720));
    check(std::string_view(kOutputPresets[2].name) == "1080p" &&
          kOutputPresets[2].resolution == res(1920, 1080));
    check(std::string_view(kOutputPresets[3].name) == "1440p" &&
          kOutputPresets[3].resolution == res(2560, 1440));
    check(std::string_view(kOutputPresets[4].name) == "2160p" &&
          kOutputPresets[4].resolution == res(3840, 2160));
    check(std::string_view(kOutputPresets[5].name) == "4k" &&
          kOutputPresets[5].resolution == res(3840, 2160));

    // Authored-space constants the ladder is anchored to (frame-timing.md:
    // 224-line NTSC fields; hud-layout.md: 640x448 space, +16 in 480).
    check(kAuthoredWidthPx == 640);
    check(kAuthoredHeightPx == 448);
    check(kAuthoredFieldLines == 224);
    check(kAuthoredHeightPx == 2 * kAuthoredFieldLines);
    check(kAuthoredFrameLines == 480);
    check(kAuthoredFrameYOffset == 16);
    check(near(kAuthoredAspect, 640.f / 448.f));
    check(near(kAuthoredAspect, 10.f / 7.f));  // square-pixel 10:7
    check(near(kDisplayFrameAspect, 4.f / 3.f));
}

// ------------------------------------------------------------------ parsing

void testParsePresetTokens() {
    const Resolution p720 = parseResolution("720p");
    check(p720 == res(1280, 720));
    const Resolution p1080 = parseResolution("1080p");
    check(p1080 == res(1920, 1080));
    const Resolution p1440 = parseResolution("1440p");
    check(p1440 == res(2560, 1440));
    check(parseResolution("2160p") == res(3840, 2160));
    check(parseResolution("4k") == res(3840, 2160));

    // Case-insensitivity and documented aliases.
    check(parseResolution("4K") == res(3840, 2160));
    check(parseResolution("UHD") == res(3840, 2160));
    check(parseResolution("1080P") == res(1920, 1080));
    check(isNativeDesktop(parseResolution("native")));
    check(isNativeDesktop(parseResolution("NATIVE")));
    check(isNativeDesktop(parseResolution("desktop")));
    check(!valid(parseResolution("native")));  // sentinel, not a concrete size

    // Surrounding whitespace is trimmed.
    check(parseResolution("  720p  ") == res(1280, 720));
    check(parseResolution("\t1080p\n") == res(1920, 1080));

    // isPresetName separates the {0,0} sentinel from a failed parse, which
    // share bytes; the host uses it for its invalid-input logging.
    check(isPresetName("720p") && isPresetName("4K") && isPresetName("uhd"));
    check(isPresetName("native") && isPresetName("desktop"));
    check(isPresetName("  NATIVE  "));
    check(!isPresetName("1920x1080"));  // valid input, but not a preset token
    check(!isPresetName(""));
    check(!isPresetName("wide"));
    check(!isPresetName("1080p-ish"));
    check(!isPresetName("nano"));
}

void testParseWxH() {
    check(parseResolution("1920x1080") == res(1920, 1080));
    check(parseResolution("1280x720") == res(1280, 720));
    check(parseResolution("640x448") == res(640, 448));  // authored space, 1:1
    check(parseResolution("1280X896") == res(1280, 896));  // capital X
    check(parseResolution("1x1") == res(1, 1));
    check(parseResolution("32768x32768") == res(32768, 32768));  // range bound
    check(parseResolution("01280x0896") == res(1280, 896));      // leading zeros
    check(parseResolution("  1920x1080  ") == res(1920, 1080));
}

void testParseInvalid() {
    const Resolution invalid{};
    check(parseResolution("") == invalid && !valid(parseResolution("")));
    check(parseResolution("   ") == invalid);
    check(parseResolution("1080") == invalid);          // no separator
    check(parseResolution("x1080") == invalid);         // no width
    check(parseResolution("1080x") == invalid);         // no height
    check(parseResolution("1920xx1080") == invalid);    // double separator
    check(parseResolution("1920x1080x") == invalid);    // trailing junk
    check(parseResolution("1920xx") == invalid);
    check(parseResolution("0x480") == invalid);         // zero dimension
    check(parseResolution("480x0") == invalid);
    check(parseResolution("-640x480") == invalid);      // signed
    check(parseResolution("+640x480") == invalid);
    check(parseResolution("1920 x 1080") == invalid);   // inner space breaks shape
    check(parseResolution("32769x1") == invalid);       // over the range bound
    check(parseResolution("1x32769") == invalid);
    check(parseResolution("99999999999x2") == invalid); // saturating overflow
    check(parseResolution("2x99999999999") == invalid);
    check(parseResolution("720pm") == invalid);         // not a preset token
    check(parseResolution("wide") == invalid);
    check(parseResolution("interlaced") == invalid);    // not on the menu, ever
}

void testParseRoundTrip() {
    // Every table preset resolves by name.
    for (const OutputPreset& preset : kOutputPresets) {
        const Resolution parsed = parseResolution(preset.name);
        if (isNativeDesktop(preset.resolution)) {
            check(isNativeDesktop(parsed));
        } else {
            check(parsed == preset.resolution);
        }
    }
}

// -------------------------------------------------------- aspect-fit / bars

void testFitContent4x3Inside16x9() {
    // 4:3 content in a 1920x1080 16:9 window: pillarbox, 240 px each side.
    const AspectFit fit = fitContentToWindow(1920, 1080, 4.f / 3.f);
    check(rectNear(fit.viewport, 240.f, 0.f, 1440.f, 1080.f));
    check(rectNear(fit.barLeft, 0.f, 0.f, 240.f, 1080.f));
    check(rectNear(fit.barRight, 1680.f, 0.f, 240.f, 1080.f));
    check(fit.barTop.height == 0.f && fit.barBottom.height == 0.f);
    // The bars tile the window exactly.
    check(near(fit.barLeft.width + fit.viewport.width + fit.barRight.width, 1920.f));
}

void testFitContent16x9Inside4x3() {
    // 16:9 content in an 800x600 4:3 window: letterbox, 75 px top and bottom.
    const AspectFit fit = fitContentToWindow(800, 600, 16.f / 9.f);
    check(rectNear(fit.viewport, 0.f, 75.f, 800.f, 450.f));
    check(rectNear(fit.barTop, 0.f, 0.f, 800.f, 75.f));
    check(rectNear(fit.barBottom, 0.f, 525.f, 800.f, 75.f));
    check(fit.barLeft.width == 0.f && fit.barRight.width == 0.f);
}

void testFitContentSameAspect() {
    // 16:9 content in a 16:9 window: no bars of any side.
    const AspectFit fit = fitContentToWindow(1920, 1080, 16.f / 9.f);
    check(rectNear(fit.viewport, 0.f, 0.f, 1920.f, 1080.f));
    check(fit.barLeft.width == 0.f && fit.barRight.width == 0.f);
    check(fit.barTop.height == 0.f && fit.barBottom.height == 0.f);
}

void testFitContentDegenerate() {
    // Zeroed, never NaN, for non-positive windows/aspect.
    const AspectFit z = fitContentToWindow(0, 0, 4.f / 3.f);
    check(z.viewport.width == 0.f && z.viewport.height == 0.f);
    check(z.barLeft.width == 0.f && z.barTop.height == 0.f);
    const AspectFit tall = fitContentToWindow(1920, 0, 4.f / 3.f);
    check(tall.viewport.height == 0.f);
    const AspectFit badAspect = fitContentToWindow(1920, 1080, 0.f);
    check(badAspect.viewport.width == 0.f);
    const AspectFit negative = fitContentToWindow(1920, 1080, -1.f);
    check(negative.viewport.width == 0.f);
}

void testFitAuthoredPicture() {
    // 1280x896 IS the authored 10:7 picture at x2: exact fit, zero bars.
    const AspectFit fit = fitAuthoredPicture(1280, 896);
    check(rectNear(fit.viewport, 0.f, 0.f, 1280.f, 896.f));
    check(fit.barLeft.width == 0.f && fit.barRight.width == 0.f);
    check(fit.barTop.height == 0.f && fit.barBottom.height == 0.f);

    // 640x448 authored content in the 640x480 4:3 frame window centres at
    // y=16 - reproducing the INFERRED +16 centring as pure aspect math.
    const AspectFit frame = fitAuthoredPicture(640, 480);
    check(rectNear(frame.viewport, 0.f, 16.f, 640.f, 448.f, 1e-2f));
    check(near(frame.barTop.height, 16.f, 1e-2f));
    check(near(frame.barBottom.height, 16.f, 1e-2f));
    check(frame.barLeft.width == 0.f && frame.barRight.width == 0.f);

    // 16:9 window: the authored column is 10:7, wider than 4:3 but narrower
    // than 16:9 -> pillarboxed between 4:3 and the window edges.
    const AspectFit wide = fitAuthoredPicture(1920, 1080);
    check(rectNear(wide.viewport, 188.57143f, 0.f, 1542.8571f, 1080.f, 1e-2f));
    check(near(wide.barLeft.width, 188.57143f, 1e-2f));
    check(near(wide.barRight.width, 188.57143f, 1e-2f));
    check(wide.barTop.height == 0.f && wide.barBottom.height == 0.f);
}

void testFitDisplayFrame() {
    // The 4:3 640x480 frame in a 16:9 window (recomp HUD-narrowing anchor).
    const AspectFit fit = fitDisplayFrame(1920, 1080);
    check(rectNear(fit.viewport, 240.f, 0.f, 1440.f, 1080.f, 1e-2f));
    check(near(fit.barLeft.width, 240.f, 1e-2f));
    check(near(fit.barRight.width, 240.f, 1e-2f));
    // Frame mapping check: authored (uiX,uiY)=(0,0) inside the frame with the
    // +16 convention lands at x=240, y=25 at 1080p (scale = 1080/480 = 2.25).
    const float sx = fit.viewport.width / 640.f;
    const float sy = fit.viewport.height / 480.f;
    check(near(sx, 2.25f, 1e-2f) && near(sy, 2.25f, 1e-2f));
    check(near(fit.viewport.x + 0.f * sx, 240.f, 1e-2f));
    check(near(fit.viewport.y + (0.f + 16.f) * sy, 36.f, 1e-2f));
}

// ------------------------------------------------------------- HUD placement

void testHudScaleExact() {
    // The integer-perfect configuration: 1280x896 is x2 of the authored
    // 640x448 space, exactly (recomp 8x/16x progressive scanout precedent).
    const HudPlacement x2 = hudPlacement(1280, 896, HudScaleMode::Exact);
    check(x2.scale == 2.f);
    check(x2.target.x == 0.f && x2.target.y == 0.f);
    check(x2.target.width == 1280.f && x2.target.height == 896.f);

    const HudPlacement x2snapped = hudPlacement(1280, 896, HudScaleMode::IntegerSnapped);
    check(x2snapped.scale == 2.f);
    check(x2snapped.target.width == 1280.f);

    // Standard ladder exact ratios (authored 448-line basis):
    //   720/448 = 45/28, 1080/448 = 135/56, 1440/448 = 45/14, 2160/448 = 135/28.
    check(hudExactScale(1280, 720) == 45.f / 28.f);   // == 720/448, height-capped
    check(hudExactScale(1920, 1080) == 135.f / 56.f);
    check(hudExactScale(2560, 1440) == 45.f / 14.f);
    check(hudExactScale(3840, 2160) == 135.f / 28.f);

    // Column geometry at 1080p: 640*135/56 = 1542+6/7 px wide, centred.
    const HudPlacement p1080 = hudPlacement(1920, 1080, HudScaleMode::Exact);
    check(near(p1080.target.width, 86400.f / 56.f, 1e-2f));
    check(near(p1080.target.height, 1080.f, 1e-2f));
    check(near(p1080.target.x, 2640.f / 14.f, 1e-2f));
    check(near(p1080.target.y, 0.f, 1e-2f));

    // 4K: 640*135/28 = 3085+5/7 px wide, centred with 377+1/7 px bars each side.
    const HudPlacement uhd = hudPlacement(3840, 2160, HudScaleMode::Exact);
    check(near(uhd.target.width, 43200.f / 14.f, 1e-2f));
    check(near(uhd.target.height, 2160.f, 1e-2f));
    check(near(uhd.target.x, 21120.f / 56.f, 1e-2f));
    check(near(uhd.target.y, 0.f, 1e-2f));
    check(near(uhd.scale, 135.f / 28.f, 1e-6f));
}

void testHudScaleHalfWindow() {
    // A half-size window of the authored space itself (320x240 = the CRT
    // frame scaled 0.5, still aspect-corrected by fit, dyadic-exact factors).
    const HudPlacement half = hudPlacement(320, 240, HudScaleMode::Exact);
    check(half.scale == 0.5f);  // 320/640, capped below 240/448
    check(half.target.x == 0.f && near(half.target.y, 8.f, 1e-2f));
    check(half.target.width == 320.f && near(half.target.height, 224.f));

    // Extreme aspect (tall window): width-limited, exact dyadic factor again.
    const HudPlacement tall = hudPlacement(500, 2000, HudScaleMode::Exact);
    check(tall.scale == 0.78125f);  // 500/640 = 25/32, exact in f32
    check(tall.target.x == 0.f);
    check(tall.target.y == 825.f);
    check(tall.target.width == 500.f && tall.target.height == 350.f);
}

void testHudScaleIntegerSnapped() {
    // Ladder snaps: 720p->1x, 1080p->2x, 1440p->3x, 4K->4x.
    check(hudIntegerScale(1280, 720) == 1);
    check(hudIntegerScale(1920, 1080) == 2);
    check(hudIntegerScale(2560, 1440) == 3);
    check(hudIntegerScale(3840, 2160) == 4);
    check(hudIntegerScale(1280, 896) == 2);
    check(hudIntegerScale(640, 448) == 1);

    // 1080p snapped x2 places the authored raster at (320, 92) 1280x896.
    const HudPlacement x2 = hudPlacement(1920, 1080, HudScaleMode::IntegerSnapped);
    check(x2.scale == 2.f);
    check(x2.target.x == 320.f && x2.target.y == 92.f);
    check(x2.target.width == 1280.f && x2.target.height == 896.f);

    // 1440p snapped x3: (320, 48) 1920x1344 - the 48 px bars are exactly the
    // original 16-line borders at x3 (16*3 = 48; 3.33% of frame height).
    const HudPlacement x3 = hudPlacement(2560, 1440, HudScaleMode::IntegerSnapped);
    check(x3.scale == 3.f);
    check(x3.target.x == 320.f && x3.target.y == 48.f);
    check(x3.target.width == 1920.f && x3.target.height == 1344.f);

    // 4K snapped x4: (640, 184) 2560x1792.
    const HudPlacement x4 = hudPlacement(3840, 2160, HudScaleMode::IntegerSnapped);
    check(x4.scale == 4.f);
    check(x4.target.x == 640.f && x4.target.y == 184.f);
    check(x4.target.width == 2560.f && x4.target.height == 1792.f);

    // Tiny window clamps up to 1x; the target then extends past the window
    // and the app clips (documented edge, deterministic offsets).
    const HudPlacement tiny = hudPlacement(320, 240, HudScaleMode::IntegerSnapped);
    check(tiny.scale == 1.f);
    check(tiny.target.x == -160.f && tiny.target.y == -104.f);
    check(tiny.target.width == 640.f && tiny.target.height == 448.f);

    // Degenerate windows: zero exact scale, never NaN; snap stays at 1x.
    const HudPlacement zero = hudPlacement(0, 0, HudScaleMode::Exact);
    check(zero.scale == 0.f && zero.target.width == 0.f && zero.target.height == 0.f);
    check(zero.target.x == 0.f && zero.target.y == 0.f);
    const HudPlacement neg = hudPlacement(-100, 100, HudScaleMode::IntegerSnapped);
    check(neg.scale == 1.f);
    check(neg.target.x == -370.f);
}

// --------------------------------------------------- progressive invariant

void testProgressiveContract() {
    // The native default: the full authored picture, no parity offset.
    check(progressiveContractSatisfied(ProgressivePresent{}));
    check(progressiveContractSatisfied({kAuthoredHeightPx, 0.f}));
    // A lone field is not a frame.
    check(!progressiveContractSatisfied({224, 0.f}));
    check(!progressiveContractSatisfied({kAuthoredHeightPx - 1, 0.f}));
    // Barely-combined frames (>=, the 2x boundary) still pass.
    check(progressiveContractSatisfied({kAuthoredHeightPx + 1, 0.f}));
    check(progressiveContractSatisfied({896, 0.f}));   // 2x scanout height
    check(progressiveContractSatisfied({2 * kAuthoredFieldLines, 0.f}));

    // The retail CRT service this tree sheds: parity half-line offsets fail,
    // positive or negative (that failure IS the interlaced signature).
    check(!progressiveContractSatisfied({448, 0.5f}));
    check(!progressiveContractSatisfied({448, -0.5f}));
    check(!progressiveContractSatisfied({896, 0.5f}));

    // Field-line parameterisation: the PAL-shape DBuff alternative
    // (512x256 fields at 0x200x0x100) composes to 512 lines.
    check(!progressiveContractSatisfied({256, 0.f}, 256));
    check(progressiveContractSatisfied({512, 0.f}, 256));
}

}  // namespace

int main() {
    testPresetTable();
    testParsePresetTokens();
    testParseWxH();
    testParseInvalid();
    testParseRoundTrip();
    testFitContent4x3Inside16x9();
    testFitContent16x9Inside4x3();
    testFitContentSameAspect();
    testFitContentDegenerate();
    testFitAuthoredPicture();
    testFitDisplayFrame();
    testHudScaleExact();
    testHudScaleHalfWindow();
    testHudScaleIntegerSnapped();
    testProgressiveContract();
    if (g_fail) return 1;
    std::printf("output_modes: all %d checks passed\n", g_checks);
    return 0;
}
