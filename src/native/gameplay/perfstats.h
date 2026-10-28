// Host performance-window accounting.  This is deliberately independent of
// SDL/Vulkan: the app injects monotonic wall time and cumulative process CPU
// time, while tests inject synthetic values.
#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace rt::native::gameplay {

struct PerfWindow {
    double renderFps = 0.0;     // successful host presents per wall second
    double logicRate = 0.0;     // completed 1/60-second simulation steps/sec
    double genFactor = 0.0;     // presents / logic steps (0 if no steps)
    double worstFrameMs = 0.0;  // maximum present-to-present interval
    double cpuPercent = 0.0;    // process CPU seconds / wall seconds * 100
    std::uint64_t presents = 0;
    std::uint64_t logicSteps = 0;
    bool warmup = false;
    // Recomp-overlay fields the native app has no counters for yet; negative =
    // unknown, and unknown segments are never drawn.
    double vu1Busy = -1.0, gsBusy = -1.0, batteryC = -1.0;
    int gpuBusy = -1, gpuMHz = -1;
};

// Options > Performance overlay values (settings.cpp display.perf_overlay).
enum class PerfOverlayLevel { Off, FrameRate, Detailed };
struct OverlaySegment {
    std::string text;
    bool warn = false;  // recomp orange (below full speed / worst frame > 20 ms)
};

struct PerfTotals {
    std::uint64_t presents = 0;
    std::uint64_t logicSteps = 0;
};

class PerfStats {
public:
    using Source = std::function<double()>;

    // Sources return seconds. cpuTime must be cumulative process user+system
    // time (getrusage(RUSAGE_SELF)); it may advance faster than wall time on
    // multi-core workloads, so cpuPercent is intentionally not capped at 100.
    explicit PerfStats(Source wallTime, Source cpuTime, double windowSeconds = 0.5,
                       std::size_t historyCapacity = 4);

    void noteLogicStep();
    void notePresent();
    // Poll once per host iteration, after its logic/present work. Completes
    // every elapsed half-second window and returns the current wall timestamp.
    [[nodiscard]] double now();
    // Drops partial measurements after a pause/resume. The next completed
    // interval is warmup-only, preventing resume spikes in the visible trend.
    void resetWindows();

    [[nodiscard]] const std::vector<PerfWindow>& history() const;
    [[nodiscard]] std::optional<PerfWindow> lastWindow() const;
    [[nodiscard]] const PerfTotals& totals() const;
    [[nodiscard]] double windowSeconds() const;

private:
    void finishWindow(double end);
    void appendVisible(PerfWindow window);

    Source wallTime_;
    Source cpuTime_;
    double windowSeconds_ = 0.5;
    std::size_t historyCapacity_ = 4;
    double windowStart_ = 0.0;
    double cpuStart_ = 0.0;
    double lastPresent_ = 0.0;
    bool haveLastPresent_ = false;
    bool suppressNextWindow_ = true;
    std::uint64_t windowPresents_ = 0;
    std::uint64_t windowLogicSteps_ = 0;
    double worstFrameMs_ = 0.0;
    PerfTotals totals_{};
    std::vector<PerfWindow> history_;
    std::optional<PerfWindow> lastWindow_;
};

struct OverlayPair {
    std::string label;
    std::string value;
};

class OverlayModel {
public:
    // A fully populated row is normally 40-47 ASCII bytes; this leaves room
    // for the existing FONTE HUD text builder without dynamic wrapping.
    static constexpr std::size_t kMaxTextChars = 47;

    explicit OverlayModel(bool enabled = false) : enabled_(enabled) {}
    void setEnabled(bool enabled) { enabled_ = enabled; }
    [[nodiscard]] bool enabled() const { return enabled_; }
    [[nodiscard]] std::vector<OverlayPair> row(const PerfWindow& window) const;
    [[nodiscard]] std::string text(const PerfWindow& window) const;

private:
    bool enabled_ = false;
};

// The recomp PerfOverlay.cpp:19-96 row: segments in its order, same templates.
[[nodiscard]] std::vector<OverlaySegment> recompOverlayRow(const PerfWindow& window, PerfOverlayLevel level);
[[nodiscard]] PerfOverlayLevel perfOverlayLevelFromValue(const std::string& value);

}  // namespace rt::native::gameplay
