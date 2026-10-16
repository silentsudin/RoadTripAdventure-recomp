// tests/native_assets/test_perf_glyphs_integration.cpp
//
// INTEGRATION + REGRESSION tests for the two pure gameplay UI support
// features whose unit tests already pass (test_perfstats.cpp, test_glyphs.cpp):
//
//  (f) Perf overlay stats (gameplay/perfstats.{h,cpp},
//      docs/native-reference/perf-overlay.md): driven end-to-end by a
//      SYNTHETIC fixed-step + present schedule simulating the interpolated
//      host cadence (60 completed logic steps + 120 presents per wall second)
//      and the non-interpolated one (60 + 60). The overlay's ROW STRINGS are
//      asserted by exact equality, GEN 2.0 vs 1.0 and the steady 60 logic
//      rate are pinned in both cadences, and the window ring survives 1000
//      simulated seconds with zero drift (per-window rates within 1e-9 of the
//      injected truth). Wall and CPU time are injected (no sleeps, no
//      monotonic clock reads): this is the doc's "tests inject both sources".
//
//  (g) Glyph selection x input layer (gameplay/glyphs.{h,cpp} +
//      gameplay/input.h's ControllerAction set,
//      docs/native-reference/glyph-repaint.md): every driving action the
//      input layer defines must resolve to a usable prompt for every SDL pad
//      type in the documented mapping table (no unresolved holes); the
//      PS vs Xbox confirm/cancel swap, the Nintendo ZL/ZR trigger labels and
//      the keyboard-help label round-trip are pinned.
//
//  (h) Table-invariance regression: the full 16x4 action x family resolution
//      table, dumped through the public resolve()/labelFor() API as a
//      canonical SORTED string, must equal a hardcoded pin derived ONCE from
//      current behaviour. Any silent table edit trips this; an INTENTIONAL
//      edit (e.g. the planned RT_APP_FONT_DUMP grounding closure replacing
//      provisional PS Shapes with exact FontCodes) must regenerate the pin in
//      the same change. The PS font-code grounding itself is UNVALIDATED BY
//      DESIGN and is not a defect (docs/native-reference/glyph-repaint.md
//      "Verdict: UNVALIDATED").

#include "../../src/native/gameplay/glyphs.h"
#include "../../src/native/gameplay/input.h"
#include "../../src/native/gameplay/perfstats.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>
#include <vector>

using namespace rt::native::gameplay;

namespace {
int gChecks = 0;
int gFailures = 0;

#define CHECK(c)                                                                     \
    do {                                                                            \
        ++gChecks;                                                                  \
        if (!(c)) {                                                                 \
            std::fprintf(stderr, "FAIL perf_glyphs_integration check #%d (%s:%d): %s\n", \
                         gChecks, __FILE__, __LINE__, #c);                          \
            ++gFailures;                                                            \
        }                                                                           \
    } while (0)

// ===========================================================================
// (f) Perf overlay: injected deterministic schedule
// ===========================================================================

struct SimSource {
    double wall = 0.0;
    double cpu = 0.0;
};

PerfStats makeStats(SimSource& source) {
    return PerfStats([&source] { return source.wall; }, [&source] { return source.cpu; }, 0.5, 4);
}

struct Cadence {
    int presentsPerSec;    // host presents per wall second
    int logicEvery;        // one completed fixed step per N host iterations
    double renderFpsTruth;
    double logicRateTruth;
    double genFactorTruth;
    double frameMsTruth;
};

// Interpolated cadence per docs/native-reference/perf-overlay.md: a 120 Hz
// host presenting every iteration while the fixed 1/60 s simulation drains
// one step every other iteration — 60 logic + 120 presents per second.
const Cadence kInterpOn{120, 2, 120.0, 60.0, 2.0, 1000.0 / 120.0};
// Non-interpolated: one present and one logic step per iteration (60 + 60).
const Cadence kInterpOff{60, 1, 60.0, 60.0, 1.0, 1000.0 / 60.0};

// Runs `seconds` of the cadence against the module: one present per host
// iteration, a completed logic step every `logicEvery` iterations, one now()
// poll per iteration (the doc's glue order). Wall time is computed fresh each
// iteration as base + S + (i+1)/rate — no accumulation, so no floating-point
// drift is injected by the test itself. CPU is injected as an exact binary
// fraction (3/8) of wall time. `wallBase` is only ever 0.0 or a half-integer
// (both exactly representable), keeping every 0.5 s window boundary exact.
// Returns the number of windows completed (2 per simulated second).
int runCadence(PerfStats& stats, SimSource& src, const Cadence& c, int seconds, double cpuScale,
               bool steadyChecks, double wallBase = 0.0) {
    const int perWindow = c.presentsPerSec / 2;  // iterations per 0.5 s window
    int windowsCompleted = 0;
    for (int S = 0; S < seconds; ++S) {
        for (int i = 0; i < c.presentsPerSec; ++i) {
            src.wall = wallBase + static_cast<double>(S) +
                       static_cast<double>(i + 1) / static_cast<double>(c.presentsPerSec);
            if ((i % c.logicEvery) == 0) stats.noteLogicStep();
            src.cpu = cpuScale * src.wall;
            stats.notePresent();
            static_cast<void>(stats.now());
            if ((i + 1) % perWindow == 0) {  // a 0.5 s window just closed
                ++windowsCompleted;
                const auto w = stats.lastWindow();
                CHECK(w.has_value());
                CHECK(w->warmup == (windowsCompleted == 1));  // only the window after start/reset
                CHECK(w->presents == static_cast<std::uint64_t>(perWindow));
                CHECK(w->logicSteps == static_cast<std::uint64_t>(perWindow / c.logicEvery));
                CHECK(stats.history().size() <= 4);  // the ring never outgrows capacity
                if (steadyChecks) {
                    CHECK(std::fabs(w->renderFps - c.renderFpsTruth) < 1e-9);
                    CHECK(std::fabs(w->logicRate - c.logicRateTruth) < 1e-9);
                    CHECK(std::fabs(w->genFactor - c.genFactorTruth) < 1e-9);
                    CHECK(std::fabs(w->worstFrameMs - c.frameMsTruth) < 1e-6);
                    CHECK(std::fabs(w->cpuPercent - cpuScale * 100.0) < 1e-9);
                }
            }
        }
    }
    return windowsCompleted;
}

void testInterpOnOverlayRowEndToEnd() {
    SimSource src;
    PerfStats stats = makeStats(src);
    CHECK(runCadence(stats, src, kInterpOn, 3, 0.375, true) == 6);

    const auto steady = stats.lastWindow();
    CHECK(steady.has_value() && !steady->warmup);

    OverlayModel overlay(true);
    const std::vector<OverlayPair> row = overlay.row(*steady);
    CHECK(row.size() == 5);
    CHECK(row[0].label == "FPS" && row[0].value == "120");
    CHECK(row[1].label == "GAME" && row[1].value == "60");   // shown: differs from FPS
    CHECK(row[2].label == "GEN" && row[2].value == "2.0");   // shown: interpolating 2 presents/step
    CHECK(row[3].label == "WORST" && row[3].value == "8.3");  // one 120 Hz present interval
    CHECK(row[4].label == "CPU" && row[4].value == "38");     // 37.5% half-up rounds to 38
    const std::string text = overlay.text(*steady);
    CHECK(text == "FPS 120 GAME 60 GEN 2.0 WORST 8.3 CPU 38");
    CHECK(text.size() <= OverlayModel::kMaxTextChars);
    // Steady windows keep producing the identical row: no window-to-window
    // wobble in the rendered string.
    for (const PerfWindow& w : stats.history())
        CHECK(overlay.text(w) == text || w.warmup);
    OverlayModel disabled(false);
    CHECK(disabled.row(*steady).empty());
    CHECK(disabled.text(*steady).empty());
}

void testInterpOffOverlayRowEndToEnd() {
    SimSource src;
    PerfStats stats = makeStats(src);
    CHECK(runCadence(stats, src, kInterpOff, 3, 0.375, true) == 6);

    const auto steady = stats.lastWindow();
    CHECK(steady.has_value() && !steady->warmup);
    CHECK(std::fabs(steady->logicRate - 60.0) < 1e-9);  // logic rate steady 60 here too
    CHECK(std::fabs(steady->genFactor - 1.0) < 1e-9);   // one present per step

    OverlayModel overlay(true);
    const std::vector<OverlayPair> row = overlay.row(*steady);
    CHECK(row.size() == 3);
    CHECK(row[0].label == "FPS" && row[0].value == "60");
    CHECK(row[1].label == "WORST" && row[1].value == "16.7");  // one 60 Hz present interval
    CHECK(row[2].label == "CPU" && row[2].value == "38");
    // GAME is hidden (rounds equal to FPS) and GEN is hidden (== 1.0).
    CHECK(overlay.text(*steady) == "FPS 60 WORST 16.7 CPU 38");
    CHECK(overlay.text(*steady).size() <= OverlayModel::kMaxTextChars);
}

void testInterpRingThousandSecondDrift() {
    // The ring survives 1000 simulated seconds: every window still carries
    // the injected truth (fps within epsilon), nothing is lost or duplicated.
    SimSource src;
    PerfStats stats = makeStats(src);
    const int windows = runCadence(stats, src, kInterpOn, 1000, 0.375, true);
    CHECK(windows == 2000);  // exactly two windows per second, none skipped
    CHECK(stats.totals().presents == 120000);    // 120/s * 1000 s
    CHECK(stats.totals().logicSteps == 60000);  // 60/s * 1000 s
    CHECK(stats.windowSeconds() == 0.5);
    CHECK(stats.history().size() == 4);  // ring at capacity, oldest evicted
    const auto last = stats.lastWindow();
    CHECK(last.has_value() && !last->warmup);
    CHECK(std::fabs(last->renderFps - 120.0) < 1e-9);  // the last window is still exact
    CHECK(std::fabs(last->logicRate - 60.0) < 1e-9);
    CHECK(std::fabs(last->genFactor - 2.0) < 1e-9);
    CHECK(last->presents == stats.history().back().presents);
    CHECK(last->logicSteps == stats.history().back().logicSteps);
    // Long-run averages equal the injected truth, i.e. zero accumulated drift.
    CHECK(std::fabs(static_cast<double>(stats.totals().presents) / 1000.0 - 120.0) < 1e-9);
    CHECK(std::fabs(static_cast<double>(stats.totals().logicSteps) / 1000.0 - 60.0) < 1e-9);
}

void testPerfPauseResumeResetWindows() {
    // Pause in the middle of a healthy cadence, then resume exactly as the
    // doc's glue contract says: call resetWindows() when recovering. The next
    // completed window is warmup-only (excluded from the trend), and the
    // cadence after it is exact again — no resume spikes, no drift.
    SimSource src;
    PerfStats stats = makeStats(src);
    CHECK(runCadence(stats, src, kInterpOn, 3, 0.375, true) == 6);

    src.wall += 37.5;  // a 0.5 s-aligned pause of 37.5 s (stalled host)
    src.cpu = 0.375 * src.wall;
    stats.resetWindows();
    CHECK(stats.history().empty());
    CHECK(!stats.lastWindow().has_value());

    // Resume on a fresh half-second grid anchored at the wall time of resume.
    CHECK(runCadence(stats, src, kInterpOn, 1, 0.375, true, 40.5) == 2);
    CHECK(stats.history().size() == 1);  // only the first full window after warmup is visible
    const auto w = stats.lastWindow();
    CHECK(w.has_value() && !w->warmup);
    CHECK(std::fabs(w->renderFps - 120.0) < 1e-9);  // drift-free resume
    CHECK(std::fabs(w->logicRate - 60.0) < 1e-9);
    CHECK(std::fabs(w->genFactor - 2.0) < 1e-9);
    CHECK(std::fabs(w->cpuPercent - 37.5) < 1e-9);
    // Totals keep accumulating across the pause: 4 s of presents in all.
    CHECK(stats.totals().presents == 4 * 120);
    CHECK(stats.totals().logicSteps == 4 * 60);
}

void testFirstPresentAndPartialWindowEdges() {
    // Edge cadence: nothing completes until the first 0.5 s boundary — the
    // first present of the app sets no worst-frame, no window closes early.
    SimSource src;
    PerfStats stats = makeStats(src);
    src.wall = 0.0;  // PerfStats sampled the sources at construction
    CHECK(!stats.lastWindow().has_value());
    for (int i = 0; i < 59; ++i) {
        src.wall = static_cast<double>(i + 1) / 120.0;
        stats.notePresent();
        stats.noteLogicStep();
        static_cast<void>(stats.now());
        CHECK(!stats.lastWindow().has_value());  // no partial-window closes
    }
    CHECK(stats.totals().presents == 59);
    CHECK(stats.totals().logicSteps == 59);
    src.wall = 60.0 / 120.0;  // the exact first boundary
    stats.noteLogicStep();    // the boundary iteration's drained step (before its present)
    stats.notePresent();
    static_cast<void>(stats.now());
    const auto w = stats.lastWindow();
    CHECK(w.has_value());
    CHECK(w->warmup);
    CHECK(w->presents == 60);  // the boundary present belongs to the window it closes
    CHECK(w->logicSteps == 60);
    CHECK(stats.totals().presents == 60);
    CHECK(stats.totals().logicSteps == 60);
}

// ===========================================================================
// (g) Glyphs x input wiring
// ===========================================================================

// Every driving action the input layer defines (menu.h ControllerAction —
// the enum playerCarInput remaps) has a prompt glyph action in the matrix,
// per docs/native-reference/glyph-repaint.md "Full prompt matrix". This
// correspondence is the cross-module contract under test: a driving action
// without a resolvable prompt is a hole in the driving UI.
struct DriveGlyphBinding {
    ControllerAction drive;
    GlyphAction prompt;
};
const DriveGlyphBinding kDriveGlyphs[] = {
    {ControllerAction::ACCELERATE, GlyphAction::Gas},
    {ControllerAction::BRAKE, GlyphAction::Brake},
    {ControllerAction::REVERSE, GlyphAction::Reverse},
    {ControllerAction::JET, GlyphAction::Jet},
    {ControllerAction::WING_UP, GlyphAction::WingUp},
    {ControllerAction::WING_DOWN, GlyphAction::WingDown},
    {ControllerAction::HORN, GlyphAction::Horn},
    {ControllerAction::VIEW, GlyphAction::View},
    {ControllerAction::LIGHTS, GlyphAction::Lights},
    {ControllerAction::NAVIGATOR, GlyphAction::Navigator},
};

void testEveryDriveActionResolvesForEverySdlPadType() {
    const int sdlTypes[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 99};
    const int kDriveGlyphCount = static_cast<int>(sizeof kDriveGlyphs / sizeof kDriveGlyphs[0]);
    CHECK(kDriveGlyphCount == static_cast<int>(kControllerActionCount));
    bool seenAction[64] = {};
    for (const auto& b : kDriveGlyphs) seenAction[static_cast<std::size_t>(b.drive)] = true;
    for (std::size_t a = 0; a < kControllerActionCount; ++a)
        CHECK(seenAction[a]);  // every driving action has exactly one prompt slot

    for (const int type : sdlTypes) {
        // The documented SDL pad-type mapping (glyph-repaint.md).
        const GlyphFamily family = mapSdlPadType(type);
        const GlyphFamily expectedFamily = (type == 0)   ? GlyphFamily::Unknown
                                           : (type >= 4 && type <= 6) ? GlyphFamily::Playstation
                                           : (type >= 7 && type <= 10) ? GlyphFamily::Nintendo
                                                                       : GlyphFamily::XboxStyle;
        CHECK(family == expectedFamily);
        CHECK(isRepaintNeeded(family) ==
              (family == GlyphFamily::XboxStyle || family == GlyphFamily::Nintendo));

        for (const auto& b : kDriveGlyphs) {
            const GlyphSpec spec = resolve(b.prompt, family);
            CHECK(spec.kind == GlyphKind::Shape || spec.kind == GlyphKind::Text);
            CHECK(spec.kind != GlyphKind::FontCode);  // no invented codes while UNVALIDATED
            const std::string_view label = labelFor(spec);
            CHECK(!label.empty());
            CHECK(label != "?");  // no unresolved holes, on any pad
            // Provisional marking policy: PS-family and SDL-unknown prompts
            // are the unvalidated-by-design PS defaults; known non-PS are not.
            if (family == GlyphFamily::Playstation || family == GlyphFamily::Unknown) {
                CHECK(spec.provisional);
            } else {
                CHECK(!spec.provisional);
            }
        }
    }
}

void testConfirmCancelSwapAndUnknownMirrorsPlaystation() {
    // The swap that matters: PS confirms on circle / cancels on cross, while
    // Xbox-style pads confirm on A / cancel on B. BOTH the shape enum and the
    // label must differ per family, and Nintendo agrees with the Xbox layout.
    const GlyphSpec psConfirm = resolve(GlyphAction::Confirm, GlyphFamily::Playstation);
    const GlyphSpec xboxConfirm = resolve(GlyphAction::Confirm, GlyphFamily::XboxStyle);
    const GlyphSpec psBack = resolve(GlyphAction::Back, GlyphFamily::Playstation);
    const GlyphSpec xboxBack = resolve(GlyphAction::Back, GlyphFamily::XboxStyle);
    CHECK(psConfirm.shape != xboxConfirm.shape);
    CHECK(labelFor(psConfirm) != labelFor(xboxConfirm));
    CHECK(labelFor(psConfirm) == "◯");
    CHECK(labelFor(xboxConfirm) == "A");
    CHECK(psBack.shape != xboxBack.shape);
    CHECK(labelFor(psBack) != labelFor(xboxBack));
    CHECK(labelFor(psBack) == "X");
    CHECK(labelFor(xboxBack) == "B");
    CHECK(resolve(GlyphAction::Confirm, GlyphFamily::Nintendo).shape == xboxConfirm.shape);
    CHECK(resolve(GlyphAction::Back, GlyphFamily::Nintendo).shape == xboxBack.shape);
    CHECK(labelFor(resolve(GlyphAction::Confirm, GlyphFamily::Nintendo)) == "A");
    CHECK(labelFor(resolve(GlyphAction::Back, GlyphFamily::Nintendo)) == "B");

    // SDL-unknown pads retain the PS default prompts: the whole column is
    // identical to Playstation (docs: "Unknown (PS default)").
    for (int a = 0; a < static_cast<int>(GlyphAction::Count); ++a) {
        const auto act = static_cast<GlyphAction>(a);
        const GlyphSpec ps = resolve(act, GlyphFamily::Playstation);
        const GlyphSpec unknown = resolve(act, GlyphFamily::Unknown);
        CHECK(ps.kind == unknown.kind);
        CHECK(ps.fontCode == unknown.fontCode);
        CHECK(ps.shape == unknown.shape);
        CHECK(labelFor(ps) == labelFor(unknown));
        CHECK(ps.provisional == unknown.provisional);
    }
}

void testTriggerLabelsAndKeyboardHelpRoundTrip() {
    // Gas/brake are textual trigger labels by contract; Nintendo's are ZR/ZL.
    const struct {
        GlyphAction a;
        const char* ps;
        const char* xbox;
        const char* nintendo;
    } triggers[] = {
        {GlyphAction::Gas, "R2", "RT", "ZR"},
        {GlyphAction::Brake, "L2", "LT", "ZL"},
    };
    for (const auto& t : triggers) {
        const GlyphSpec ps = resolve(t.a, GlyphFamily::Playstation);
        const GlyphSpec xbox = resolve(t.a, GlyphFamily::XboxStyle);
        const GlyphSpec nintendo = resolve(t.a, GlyphFamily::Nintendo);
        CHECK(ps.kind == GlyphKind::Text);
        CHECK(xbox.kind == GlyphKind::Text);
        CHECK(nintendo.kind == GlyphKind::Text);
        CHECK(labelFor(ps) == t.ps);
        CHECK(labelFor(xbox) == t.xbox);
        CHECK(labelFor(nintendo) == t.nintendo);  // the ZL/ZR labels
    }
    // Stick prompts are short textual labels for every family.
    for (int f = 0; f < static_cast<int>(GlyphFamily::Count); ++f) {
        const auto fam = static_cast<GlyphFamily>(f);
        const GlyphSpec steer = resolve(GlyphAction::Steer, fam);
        const GlyphSpec pitch = resolve(GlyphAction::Pitch, fam);
        CHECK(steer.kind == GlyphKind::Text && labelFor(steer) == "LS");
        CHECK(pitch.kind == GlyphKind::Text && labelFor(pitch) == "RS");
    }

    // Keyboard-help round-trip: labelFor(resolve(...)) must hand the keyboard
    // help exactly the stored ASCII label, printable without any conversion.
    for (int a = 0; a < static_cast<int>(GlyphAction::Count); ++a) {
        for (int f = 0; f < static_cast<int>(GlyphFamily::Count); ++f) {
            const auto act = static_cast<GlyphAction>(a);
            const auto fam = static_cast<GlyphFamily>(f);
            const GlyphSpec spec = resolve(act, fam);
            const std::string_view label = labelFor(spec);
            CHECK(!label.empty());
            if (spec.kind == GlyphKind::Text) {
                for (const char ch : label) CHECK(ch >= 0x21 && ch <= 0x7e);
                CHECK(label == resolve(act, fam).textFallback);  // unchanged bytes
            }
        }
    }
}

// ===========================================================================
// (h) Glyph table invariance pin
// ===========================================================================

const char* glyphActionName(GlyphAction a) {
    switch (a) {
    case GlyphAction::Steer: return "Steer";
    case GlyphAction::Pitch: return "Pitch";
    case GlyphAction::Reverse: return "Reverse";
    case GlyphAction::Jet: return "Jet";
    case GlyphAction::WingUp: return "WingUp";
    case GlyphAction::WingDown: return "WingDown";
    case GlyphAction::Horn: return "Horn";
    case GlyphAction::Lights: return "Lights";
    case GlyphAction::View: return "View";
    case GlyphAction::Navigator: return "Navigator";
    case GlyphAction::Confirm: return "Confirm";
    case GlyphAction::Back: return "Back";
    case GlyphAction::Pause: return "Pause";
    case GlyphAction::Start: return "Start";
    case GlyphAction::Gas: return "Gas";
    case GlyphAction::Brake: return "Brake";
    case GlyphAction::Count: return "?ACTION";
    }
    return "?ACTION";
}

const char* glyphFamilyName(GlyphFamily f) {
    switch (f) {
    case GlyphFamily::Playstation: return "Playstation";
    case GlyphFamily::XboxStyle: return "XboxStyle";
    case GlyphFamily::Nintendo: return "Nintendo";
    case GlyphFamily::Unknown: return "Unknown";
    case GlyphFamily::Count: return "?FAMILY";
    }
    return "?FAMILY";
}

const char* glyphKindName(GlyphKind k) {
    switch (k) {
    case GlyphKind::FontCode: return "FontCode";
    case GlyphKind::Shape: return "Shape";
    case GlyphKind::Text: return "Text";
    }
    return "?KIND";
}

const char* glyphShapeName(GlyphShape s) {
    switch (s) {
    case GlyphShape::Circle: return "Circle";
    case GlyphShape::Cross: return "Cross";
    case GlyphShape::Square: return "Square";
    case GlyphShape::Triangle: return "Triangle";
    case GlyphShape::LetterA: return "LetterA";
    case GlyphShape::LetterB: return "LetterB";
    case GlyphShape::LetterX: return "LetterX";
    case GlyphShape::LetterY: return "LetterY";
    case GlyphShape::LB: return "LB";
    case GlyphShape::RB: return "RB";
    case GlyphShape::LT: return "LT";
    case GlyphShape::RT: return "RT";
    case GlyphShape::Minus: return "Minus";
    case GlyphShape::Plus: return "Plus";
    case GlyphShape::Home: return "Home";
    case GlyphShape::Create: return "Create";
    }
    return "?SHAPE";
}

// Canonical dump of the complete resolution table THROUGH the public API,
// one line per (action, family) cell, lines sorted so the pin is independent
// of enum ordering, joined with '\n'.
std::string canonicalGlyphTableDump() {
    std::vector<std::string> lines;
    for (int a = 0; a < static_cast<int>(GlyphAction::Count); ++a) {
        for (int f = 0; f < static_cast<int>(GlyphFamily::Count); ++f) {
            const GlyphSpec spec = resolve(static_cast<GlyphAction>(a),
                                           static_cast<GlyphFamily>(f));
            std::string line = "A=";
            line += glyphActionName(static_cast<GlyphAction>(a));
            line += " F=";
            line += glyphFamilyName(static_cast<GlyphFamily>(f));
            line += " K=";
            line += glyphKindName(spec.kind);
            line += " S=";
            line += glyphShapeName(spec.shape);
            line += " L=";
            line += spec.textFallback;
            line += " P=";
            line += spec.provisional ? '1' : '0';
            lines.push_back(line);
        }
    }
    std::sort(lines.begin(), lines.end());
    std::string out;
    for (const std::string& l : lines) {
        out += l;
        out += '\n';
    }
    return out;
}

// REGRESSION PIN — READ THIS IF THIS TEST FAILS:
// The string below was derived ONCE from current kGlyphSpecs behaviour
// (2026-10-16, glyphs.cpp table with the PS-family provisional Shapes). It
// pins the ENTIRE action x family resolution table against silent edits.
// If your change to the table is INTENTIONAL (most expectedly: the
// RT_APP_FONT_DUMP closure flips kPlaystationFontCodesGrounded and replaces
// provisional PS Shape/Text entries with grounded FontCodes), regenerate this
// string from the "actual dump" printed in the failure output IN THE SAME
// COMMIT. Do not regenerate it to make an unexplained failure go away.
const char* kPinnedGlyphTable =
    "A=Back F=Nintendo K=Shape S=LetterB L=B P=0\n"
    "A=Back F=Playstation K=Shape S=Cross L=X P=1\n"
    "A=Back F=Unknown K=Shape S=Cross L=X P=1\n"
    "A=Back F=XboxStyle K=Shape S=LetterB L=B P=0\n"
    "A=Brake F=Nintendo K=Text S=Circle L=ZL P=0\n"
    "A=Brake F=Playstation K=Text S=Circle L=L2 P=1\n"
    "A=Brake F=Unknown K=Text S=Circle L=L2 P=1\n"
    "A=Brake F=XboxStyle K=Text S=Circle L=LT P=0\n"
    "A=Confirm F=Nintendo K=Shape S=LetterA L=A P=0\n"
    "A=Confirm F=Playstation K=Shape S=Circle L=\xE2\x97\xAF P=1\n"
    "A=Confirm F=Unknown K=Shape S=Circle L=\xE2\x97\xAF P=1\n"
    "A=Confirm F=XboxStyle K=Shape S=LetterA L=A P=0\n"
    "A=Gas F=Nintendo K=Text S=Circle L=ZR P=0\n"
    "A=Gas F=Playstation K=Text S=Circle L=R2 P=1\n"
    "A=Gas F=Unknown K=Text S=Circle L=R2 P=1\n"
    "A=Gas F=XboxStyle K=Text S=Circle L=RT P=0\n"
    "A=Horn F=Nintendo K=Shape S=LetterX L=X P=0\n"
    "A=Horn F=Playstation K=Shape S=Triangle L=Triangle P=1\n"
    "A=Horn F=Unknown K=Shape S=Triangle L=Triangle P=1\n"
    "A=Horn F=XboxStyle K=Shape S=LetterY L=Y P=0\n"
    "A=Jet F=Nintendo K=Shape S=LetterA L=A P=0\n"
    "A=Jet F=Playstation K=Shape S=Cross L=X P=1\n"
    "A=Jet F=Unknown K=Shape S=Cross L=X P=1\n"
    "A=Jet F=XboxStyle K=Shape S=LetterA L=A P=0\n"
    "A=Lights F=Nintendo K=Shape S=LetterX L=X P=0\n"
    "A=Lights F=Playstation K=Shape S=RB L=R1 P=1\n"
    "A=Lights F=Unknown K=Shape S=RB L=R1 P=1\n"
    "A=Lights F=XboxStyle K=Shape S=LetterY L=Y P=0\n"
    "A=Navigator F=Nintendo K=Shape S=Minus L=- P=0\n"
    "A=Navigator F=Playstation K=Shape S=Minus L=Select P=1\n"
    "A=Navigator F=Unknown K=Shape S=Minus L=Select P=1\n"
    "A=Navigator F=XboxStyle K=Shape S=Minus L=Back P=0\n"
    "A=Pause F=Nintendo K=Shape S=Home L=Home P=0\n"
    "A=Pause F=Playstation K=Shape S=Minus L=Select P=1\n"
    "A=Pause F=Unknown K=Shape S=Minus L=Select P=1\n"
    "A=Pause F=XboxStyle K=Shape S=Home L=Menu P=0\n"
    "A=Pitch F=Nintendo K=Text S=Circle L=RS P=0\n"
    "A=Pitch F=Playstation K=Text S=Circle L=RS P=1\n"
    "A=Pitch F=Unknown K=Text S=Circle L=RS P=1\n"
    "A=Pitch F=XboxStyle K=Text S=Circle L=RS P=0\n"
    "A=Reverse F=Nintendo K=Shape S=LetterB L=B P=0\n"
    "A=Reverse F=Playstation K=Shape S=Circle L=\xE2\x97\xAF P=1\n"
    "A=Reverse F=Unknown K=Shape S=Circle L=\xE2\x97\xAF P=1\n"
    "A=Reverse F=XboxStyle K=Shape S=LetterB L=B P=0\n"
    "A=Start F=Nintendo K=Shape S=Plus L=+ P=0\n"
    "A=Start F=Playstation K=Shape S=Plus L=Start P=1\n"
    "A=Start F=Unknown K=Shape S=Plus L=Start P=1\n"
    "A=Start F=XboxStyle K=Shape S=Plus L=Start P=0\n"
    "A=Steer F=Nintendo K=Text S=Circle L=LS P=0\n"
    "A=Steer F=Playstation K=Text S=Circle L=LS P=1\n"
    "A=Steer F=Unknown K=Text S=Circle L=LS P=1\n"
    "A=Steer F=XboxStyle K=Text S=Circle L=LS P=0\n"
    "A=View F=Nintendo K=Shape S=LetterB L=B P=0\n"
    "A=View F=Playstation K=Shape S=Circle L=\xE2\x97\xAF P=1\n"
    "A=View F=Unknown K=Shape S=Circle L=\xE2\x97\xAF P=1\n"
    "A=View F=XboxStyle K=Shape S=LetterB L=B P=0\n"
    "A=WingDown F=Nintendo K=Shape S=RB L=R P=0\n"
    "A=WingDown F=Playstation K=Shape S=LT L=L2 P=1\n"
    "A=WingDown F=Unknown K=Shape S=LT L=L2 P=1\n"
    "A=WingDown F=XboxStyle K=Shape S=RB L=RB P=0\n"
    "A=WingUp F=Nintendo K=Shape S=LB L=L P=0\n"
    "A=WingUp F=Playstation K=Shape S=LB L=L1 P=1\n"
    "A=WingUp F=Unknown K=Shape S=LB L=L1 P=1\n"
    "A=WingUp F=XboxStyle K=Shape S=LB L=LB P=0\n";

void testGlyphTableInvariancePin() {
    const std::string actual = canonicalGlyphTableDump();
    if (actual != kPinnedGlyphTable) {
        std::fprintf(stderr,
                     "GLYPH TABLE PIN MISMATCH — the full resolution table changed.\n"
                     "If the edit is INTENTIONAL (e.g. the RT_APP_FONT_DUMP grounding\n"
                     "closure), update kPinnedGlyphTable from the actual dump printed\n"
                     "below, in the same commit that changes kGlyphSpecs.\n");
        std::fputs("---- actual dump (kPinnedGlyphTable replacement) ----\n", stderr);
        std::fputs(actual.c_str(), stderr);
        std::fputs("---- end actual dump ----\n", stderr);
    }
    CHECK(std::count(actual.begin(), actual.end(), '\n') ==
          static_cast<std::ptrdiff_t>(kGlyphActionCount * kGlyphFamilyCount));
    CHECK(actual == kPinnedGlyphTable);
}

}  // namespace

int main() {
    testInterpOnOverlayRowEndToEnd();
    testInterpOffOverlayRowEndToEnd();
    testInterpRingThousandSecondDrift();
    testPerfPauseResumeResetWindows();
    testFirstPresentAndPartialWindowEdges();
    testEveryDriveActionResolvesForEverySdlPadType();
    testConfirmCancelSwapAndUnknownMirrorsPlaystation();
    testTriggerLabelsAndKeyboardHelpRoundTrip();
    testGlyphTableInvariancePin();
    std::printf("perf_glyphs_integration: %d checks, %d failures\n", gChecks, gFailures);
    if (gFailures != 0) std::fprintf(stderr, "%d failure(s)\n", gFailures);
    return gFailures == 0 ? 0 : 1;
}
