#include "app/Autopilot.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>

namespace {
int checks = 0;
int failed = 0;

void check(bool condition, const char* name) {
    ++checks;
    if (!condition) {
        ++failed;
        std::cerr << "FAIL " << name << '\n';
    }
}

void testDisabledNeverSuppliesInput() {
    rt::native::app::Autopilot autopilot;
    check(autopilot.step(rt::native::gameplay::SessionState::EXPLORE).driveInput == nullptr,
          "disabled explore has no drive input");
    check(!autopilot.step(rt::native::gameplay::SessionState::RESULTS).continueResults,
          "disabled results never continues");
}

void testExploreSuppliesCenteredThrottle() {
    rt::native::app::Autopilot autopilot({.enabled = true});
    const auto step = autopilot.step(rt::native::gameplay::SessionState::EXPLORE);
    check(step.driveInput != nullptr, "enabled explore has drive input");
    check(step.driveInput && step.driveInput->gas, "explore throttle is held");
    check(step.driveInput && step.driveInput->steer == 0, "explore steering is centered");
}

void testResultsUsesConfiguredHold() {
    rt::native::app::Autopilot autopilot({.enabled = true, .resultsHoldFrames = 2});
    check(!autopilot.step(rt::native::gameplay::SessionState::RESULTS).continueResults,
          "first results frame holds");
    check(autopilot.step(rt::native::gameplay::SessionState::RESULTS).continueResults,
          "second results frame continues");
    (void)autopilot.step(rt::native::gameplay::SessionState::EXPLORE);
    check(!autopilot.step(rt::native::gameplay::SessionState::RESULTS).continueResults,
          "leaving results resets hold");
}
}  // namespace

int main() {
    std::filesystem::path elf;
    if (const char* value = std::getenv("RT_ELF")) elf = value;
    else if (const char* disc = std::getenv("RT_DISC")) elf = std::filesystem::path(disc) / "SLUS_203.98";
    else if (const char* home = std::getenv("HOME"))
        elf = std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc/SLUS_203.98";
    if (elf.empty() || !std::filesystem::exists(elf)) {
        std::cout << "SKIP missing ELF\n";
        return EXIT_SUCCESS;
    }
    testDisabledNeverSuppliesInput();
    testExploreSuppliesCenteredThrottle();
    testResultsUsesConfiguredHold();
    std::cout << "checks=" << checks << " failed=" << failed << '\n';
    return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
