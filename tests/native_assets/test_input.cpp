// Pins authored player input mapping. No game data.
#include "../../src/native/gameplay/input.h"

#include <cstdio>
#include <string>

using namespace rt::native::gameplay;

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

int main() {
    ControllerConfig cfg;
    AnalogAxes ax;
    CarInput n = playerCarInput(PadState{}, ax, cfg);
    CHECK(n.steer == 0 && !n.gas && !n.brake && !n.reverse);

    CHECK(playerCarInput(padWith(PhysicalButton::CROSS), ax, cfg).gas);
    CHECK(playerCarInput(padWith(PhysicalButton::SQUARE), ax, cfg).brake);
    CHECK(playerCarInput(padWith(PhysicalButton::CIRCLE), ax, cfg).reverse);
    CHECK(!playerCarInput(padWith(PhysicalButton::CROSS), ax, cfg).brake);

    AnalogAxes t; t.triggerGas = 1.f;
    CHECK(playerCarInput({}, t, cfg).gas);
    t = {}; t.triggerBrake = 1.f;
    CHECK(playerCarInput({}, t, cfg).brake);
    t = {}; t.triggerGas = 0.4f;
    CHECK(!playerCarInput({}, t, cfg).gas);

    AnalogAxes s; s.stickX = 0.5f;
    CHECK(playerCarInput({}, s, cfg).steer == 1);
    s.stickX = -0.5f;
    CHECK(playerCarInput({}, s, cfg).steer == -1);
    s.stickX = 0.1f;
    CHECK(playerCarInput({}, s, cfg).steer == 0);

    CHECK(!cfg.setButtonConfig(ControllerAction::ACCELERATE, PhysicalButton::TRIANGLE)); // already VIEW
    CHECK(cfg.setButtonConfig(ControllerAction::ACCELERATE, PhysicalButton::L3));
    PhysicalButton nb = cfg.getButtonConfig(ControllerAction::ACCELERATE);
    CHECK(nb == PhysicalButton::L3);
    CHECK(playerCarInput(padWith(nb), ax, cfg).gas);
    CHECK(!playerCarInput(padWith(PhysicalButton::CROSS), ax, cfg).gas);

    CarInput u = playerCarInput(padWith(PhysicalButton::START, PhysicalButton::SELECT), ax, cfg);
    CHECK(!u.gas && !u.brake && !u.reverse && u.steer == 0);

    // The player's drive bits (gameplay/input.h): keyboard or pad, gas/brake/reverse
    // through the same bindings the player path reads; nothing pressed, no bit.
    CHECK(playerCarInput(padFromButtons(true, false, false, cfg), AnalogAxes{}, cfg).gas);
    CHECK(playerCarInput(padFromButtons(false, true, false, cfg), AnalogAxes{}, cfg).brake);
    CHECK(playerCarInput(padFromButtons(false, false, true, cfg), AnalogAxes{}, cfg).reverse);
    const CarInput idle = playerCarInput(padFromButtons(false, false, false, cfg), AnalogAxes{}, cfg);
    CHECK(!idle.gas && !idle.brake && !idle.reverse && idle.steer == 0);

    // Which input drives the authored loop (gameplay/input.h): the harness pad
    // in lockstep runs, the autopilot while it holds, the loop's AI path in
    // headless captures, and the player in a windowed session - in that order.
    CHECK(driveSource(true, true, true) == DriveSource::Harness);
    CHECK(driveSource(true, false, false) == DriveSource::Harness);
    CHECK(driveSource(false, true, true) == DriveSource::Autopilot);
    CHECK(driveSource(false, true, false) == DriveSource::Autopilot);
    CHECK(driveSource(false, false, true) == DriveSource::Ai);
    CHECK(driveSource(false, false, false) == DriveSource::Player);
    CHECK(std::string(driveSourceName(DriveSource::Harness)) == "harness");
    CHECK(std::string(driveSourceName(DriveSource::Autopilot)) == "autopilot");
    CHECK(std::string(driveSourceName(DriveSource::Ai)) == "ai");
    CHECK(std::string(driveSourceName(DriveSource::Player)) == "player");

    if (g_fail == 0) std::printf("input tests passed\n");
    return g_fail ? 1 : 0;
}
