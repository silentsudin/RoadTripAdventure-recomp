// Deterministic contract tests for the native performance-overlay model.
#include "../../src/native/gameplay/perfstats.h"

#include <cmath>
#include <cstdio>
#include <string>

using namespace rt::native::gameplay;

namespace {
int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

struct Source {
    double wall = 0.0;
    double cpu = 0.0;
    [[nodiscard]] double now() const { return wall; }
    [[nodiscard]] double cpuSeconds() const { return cpu; }
};

PerfStats makeStats(Source& source, std::size_t history = 4) {
    return PerfStats([&source] { return source.now(); }, [&source] { return source.cpuSeconds(); }, 0.5, history);
}

void advanceHealthyWindow(PerfStats& stats, Source& source, int presents, int logicSteps,
                         double cpuSeconds = 0.0) {
    const double start = source.wall;
    for (int i = 0; i < presents; ++i) {
        source.wall = start + (i + 1) * (0.5 / presents);
        stats.notePresent();
    }
    for (int i = 0; i < logicSteps; ++i) stats.noteLogicStep();
    source.cpu = cpuSeconds;
    static_cast<void>(stats.now());
}

void testExactRateMathAndWarmup() {
    Source source;
    auto stats = makeStats(source);
    advanceHealthyWindow(stats, source, 30, 30, 0.10);
    const auto warmup = stats.lastWindow();
    CHECK(warmup.has_value());
    CHECK(warmup->warmup);
    CHECK(std::fabs(warmup->renderFps - 60.0) < 1e-9);
    CHECK(std::fabs(warmup->logicRate - 60.0) < 1e-9);
    CHECK(stats.history().empty());  // The startup interval is intentionally not graphed.

    advanceHealthyWindow(stats, source, 30, 30, 0.20);
    const auto window = stats.lastWindow();
    CHECK(window.has_value() && !window->warmup);
    CHECK(std::fabs(window->renderFps - 60.0) < 1e-9);
    CHECK(std::fabs(window->cpuPercent - 20.0) < 1e-9);
}

void testLogicRateAndFrameGenerationAreIndependentOfPresents() {
    Source source;
    auto stats = makeStats(source);
    advanceHealthyWindow(stats, source, 1, 0);  // consume warmup
    advanceHealthyWindow(stats, source, 60, 30);
    const auto window = *stats.lastWindow();
    CHECK(std::fabs(window.renderFps - 120.0) < 1e-9);
    CHECK(std::fabs(window.logicRate - 60.0) < 1e-9);
    CHECK(std::fabs(window.genFactor - 2.0) < 1e-9);
}

void testWorstFrameCrossesAWindowBoundary() {
    Source source;
    auto stats = makeStats(source);
    source.wall = 0.49; stats.notePresent();
    source.wall = 0.50; static_cast<void>(stats.now());  // suppress initial partial warmup
    source.wall = 0.51; stats.notePresent();  // 20 ms since the pre-boundary present
    source.wall = 1.00; static_cast<void>(stats.now());
    const auto window = *stats.lastWindow();
    CHECK(!window.warmup);
    CHECK(std::fabs(window.worstFrameMs - 20.0) < 1e-8);
}

void testRolloverAndHistoryOverwriteOldest() {
    Source source;
    auto stats = makeStats(source, 2);
    advanceHealthyWindow(stats, source, 1, 1);  // warmup
    advanceHealthyWindow(stats, source, 10, 1);
    advanceHealthyWindow(stats, source, 20, 1);
    advanceHealthyWindow(stats, source, 30, 1);
    const auto& history = stats.history();
    CHECK(history.size() == 2);
    CHECK(std::fabs(history[0].renderFps - 40.0) < 1e-9);
    CHECK(std::fabs(history[1].renderFps - 60.0) < 1e-9);
    CHECK(stats.totals().presents == 61);
    CHECK(stats.totals().logicSteps == 4);
}

void testResetAfterPauseDropsPartialWindow() {
    Source source;
    auto stats = makeStats(source);
    source.wall = 0.25; stats.notePresent();
    source.wall = 20.25; source.cpu = 10.0;
    stats.resetWindows();
    source.wall = 20.75; source.cpu = 10.1;
    static_cast<void>(stats.now());  // reset starts another warmup period
    CHECK(stats.history().empty());
    CHECK(std::fabs(stats.lastWindow()->cpuPercent - 20.0) < 1e-9);
    CHECK(std::fabs(stats.lastWindow()->worstFrameMs) < 1e-9);
}

void testCpuMayExceedOneCore() {
    Source source;
    auto stats = makeStats(source);
    advanceHealthyWindow(stats, source, 1, 1);
    advanceHealthyWindow(stats, source, 1, 1, 1.25);  // 1.25 CPU seconds / 0.5 wall seconds
    CHECK(std::fabs(stats.lastWindow()->cpuPercent - 250.0) < 1e-9);
}

void testOverlayFormattingAndDisable() {
    PerfWindow window;
    window.renderFps = 1000.0;
    window.logicRate = 59.5;
    window.genFactor = 1.681;
    window.worstFrameMs = 14.25;
    window.cpuPercent = 34.5;
    OverlayModel enabled(true);
    const auto row = enabled.row(window);
    CHECK(!row.empty());
    CHECK(row[0].label == "FPS" && row[0].value == "999");
    CHECK(row[1].label == "GAME" && row[1].value == "60");
    CHECK(row[2].label == "GEN" && row[2].value == "1.7");
    CHECK(row[3].label == "WORST" && row[3].value == "14.3");
    CHECK(row[4].label == "CPU" && row[4].value == "35");
    CHECK(enabled.text(window) == "FPS 999 GAME 60 GEN 1.7 WORST 14.3 CPU 35");
    CHECK(enabled.text(window).size() <= OverlayModel::kMaxTextChars);
    OverlayModel disabled(false);
    CHECK(disabled.row(window).empty());
    CHECK(disabled.text(window).empty());
}
}  // namespace

int main() {
    testExactRateMathAndWarmup();
    testLogicRateAndFrameGenerationAreIndependentOfPresents();
    testWorstFrameCrossesAWindowBoundary();
    testRolloverAndHistoryOverwriteOldest();
    testResetAfterPauseDropsPartialWindow();
    testCpuMayExceedOneCore();
    testOverlayFormattingAndDisable();
    if (g_fail) return 1;
    std::puts("perfstats tests passed");
    return 0;
}
