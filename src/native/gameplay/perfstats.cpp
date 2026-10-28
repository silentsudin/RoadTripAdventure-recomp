#include "gameplay/perfstats.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <sstream>

namespace rt::native::gameplay {
namespace {
double nonNegative(double value) { return value > 0.0 ? value : 0.0; }

long rounded(double value) {
    return static_cast<long>(std::floor(nonNegative(value) + 0.5));
}

std::string integerText(double value) {
    return std::to_string(std::min<long>(999, rounded(value)));
}

std::string decimalText(double value) {
    std::ostringstream out;
    out.setf(std::ios::fixed);
    out.precision(1);
    out << std::min(999.0, std::floor(nonNegative(value) * 10.0 + 0.5) / 10.0);
    return out.str();
}
}  // namespace

PerfStats::PerfStats(Source wallTime, Source cpuTime, double windowSeconds,
                     std::size_t historyCapacity)
    : wallTime_(std::move(wallTime)), cpuTime_(std::move(cpuTime)),
      windowSeconds_(windowSeconds > 0.0 ? windowSeconds : 0.5),
      historyCapacity_(historyCapacity) {
    windowStart_ = wallTime_();
    cpuStart_ = cpuTime_();
}

void PerfStats::noteLogicStep() {
    ++windowLogicSteps_;
    ++totals_.logicSteps;
}

void PerfStats::notePresent() {
    const double stamp = wallTime_();
    if (haveLastPresent_) worstFrameMs_ = std::max(worstFrameMs_, nonNegative(stamp - lastPresent_) * 1000.0);
    lastPresent_ = stamp;
    haveLastPresent_ = true;
    ++windowPresents_;
    ++totals_.presents;
}

double PerfStats::now() {
    const double stamp = wallTime_();
    // A normal caller polls once per host loop, so each close is exactly one
    // configured interval.  Repeated closes retain the zero-event windows if
    // an app was stalled long enough to cross more than one boundary.
    while (stamp >= windowStart_ + windowSeconds_) finishWindow(windowStart_ + windowSeconds_);
    return stamp;
}

void PerfStats::finishWindow(double end) {
    PerfWindow window;
    window.presents = windowPresents_;
    window.logicSteps = windowLogicSteps_;
    window.renderFps = static_cast<double>(windowPresents_) / windowSeconds_;
    window.logicRate = static_cast<double>(windowLogicSteps_) / windowSeconds_;
    window.genFactor = windowLogicSteps_ == 0 ? 0.0
                                               : static_cast<double>(windowPresents_) / windowLogicSteps_;
    window.worstFrameMs = worstFrameMs_;
    window.cpuPercent = nonNegative(cpuTime_() - cpuStart_) / windowSeconds_ * 100.0;
    window.warmup = suppressNextWindow_;
    lastWindow_ = window;
    if (suppressNextWindow_) suppressNextWindow_ = false;
    else appendVisible(window);
    windowStart_ = end;
    cpuStart_ = cpuTime_();
    windowPresents_ = 0;
    windowLogicSteps_ = 0;
    worstFrameMs_ = 0.0;
}

void PerfStats::appendVisible(PerfWindow window) {
    if (historyCapacity_ == 0) return;
    if (history_.size() == historyCapacity_) history_.erase(history_.begin());
    history_.push_back(window);
}

void PerfStats::resetWindows() {
    windowStart_ = wallTime_();
    cpuStart_ = cpuTime_();
    haveLastPresent_ = false;
    suppressNextWindow_ = true;
    windowPresents_ = 0;
    windowLogicSteps_ = 0;
    worstFrameMs_ = 0.0;
    history_.clear();
    lastWindow_.reset();
}

const std::vector<PerfWindow>& PerfStats::history() const { return history_; }
std::optional<PerfWindow> PerfStats::lastWindow() const { return lastWindow_; }
const PerfTotals& PerfStats::totals() const { return totals_; }
double PerfStats::windowSeconds() const { return windowSeconds_; }

std::vector<OverlayPair> OverlayModel::row(const PerfWindow& window) const {
    if (!enabled_) return {};
    std::vector<OverlayPair> out{{"FPS", integerText(window.renderFps)}};
    if (rounded(window.renderFps) != rounded(window.logicRate))
        out.push_back({"GAME", integerText(window.logicRate)});
    if (std::fabs(window.genFactor - 1.0) >= 0.05)
        out.push_back({"GEN", decimalText(window.genFactor)});
    out.push_back({"WORST", decimalText(window.worstFrameMs)});
    out.push_back({"CPU", integerText(window.cpuPercent)});
    return out;
}

std::string OverlayModel::text(const PerfWindow& window) const {
    const auto pairs = row(window);
    std::string out;
    for (const auto& pair : pairs) {
        if (!out.empty()) out += ' ';
        out += pair.label;
        out += ' ';
        out += pair.value;
    }
    return out;
}

namespace {
template <typename... A>
std::string fmt(const char* format, A... args) {
    char buf[96];
    std::snprintf(buf, sizeof(buf), format, args...);
    return buf;
}
}  // namespace

PerfOverlayLevel perfOverlayLevelFromValue(const std::string& value) {
    if (value == "Frame rate") return PerfOverlayLevel::FrameRate;
    if (value == "Detailed") return PerfOverlayLevel::Detailed;
    return PerfOverlayLevel::Off;
}

std::vector<OverlaySegment> recompOverlayRow(const PerfWindow& w, PerfOverlayLevel level) {
    std::vector<OverlaySegment> out;
    if (level == PerfOverlayLevel::Off) return out;
    const bool full = w.logicRate >= 59.0;
    out.push_back({fmt("%.0f fps", w.renderFps), !full});
    if (w.renderFps > w.logicRate + 1.0) out.push_back({fmt("game %.0f", w.logicRate), !full});
    if (level != PerfOverlayLevel::Detailed) return out;
    out.push_back({fmt("worst %.1f ms", w.worstFrameMs), w.worstFrameMs > 20.0});
    // No VU1/GS counters natively: shown only if a source ever fills them.
    if (w.vu1Busy >= 0 && w.gsBusy >= 0) out.push_back({fmt("VU1 %.0f%%  GS %.0f%%", w.vu1Busy, w.gsBusy), false});
    out.push_back({fmt("CPU %.0f%%", w.cpuPercent), false});
    if (w.gpuBusy >= 0) out.push_back({fmt("GPU %d%% @ %d MHz", w.gpuBusy, w.gpuMHz), false});
    // FONTE has no degree sign: approved substitution (ui-parity.md text-parity).
    if (w.batteryC > 0) out.push_back({fmt("Battery %.0f  deg C", w.batteryC), false});
    return out;
}

}  // namespace rt::native::gameplay
