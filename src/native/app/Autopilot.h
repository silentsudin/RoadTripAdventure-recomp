// Deterministic input policy for opt-in headless capture runs.
#pragma once

#include "gameplay/input.h"
#include "gameplay/session.h"

#include <algorithm>

namespace rt::native::app {

struct AutopilotConfig {
    bool enabled = false;
    int resultsHoldFrames = 1;
};

struct AutopilotStep {
    const gameplay::CarInput* driveInput = nullptr;
    bool continueResults = false;
};

class Autopilot {
public:
    explicit Autopilot(AutopilotConfig config = {}) : config_(config) {
        config_.resultsHoldFrames = std::max(1, config_.resultsHoldFrames);
        exploreInput_.gas = true;
    }

    [[nodiscard]] AutopilotStep step(gameplay::SessionState state) {
        if (!config_.enabled) return {};
        if (state == gameplay::SessionState::EXPLORE) {
            resultsFrames_ = 0;
            return {.driveInput = &exploreInput_};
        }
        if (state == gameplay::SessionState::RESULTS)
            return {.continueResults = ++resultsFrames_ >= config_.resultsHoldFrames};
        resultsFrames_ = 0;
        return {};
    }

private:
    AutopilotConfig config_;
    int resultsFrames_ = 0;
    gameplay::CarInput exploreInput_;
};

}  // namespace rt::native::app
