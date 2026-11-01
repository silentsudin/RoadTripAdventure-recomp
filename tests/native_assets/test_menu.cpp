// Pins the authored native menu contracts (docs/game-mechanics/menus-and-save.md).
// No game data.
#include "../../src/native/gameplay/menu.h"

#include <cstdio>
#include <cstdlib>

using namespace rt::native::gameplay;

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

static MenuInput pressed(MenuInput input) { return input; }

static void testMenuGridWrapAndDisabledRows() {
    MenuModel menu{{
        {1, "one", true}, {2, "two", true}, {3, "three", false},
        {4, "four", true}, {5, "five", true}, {6, "six", true},
    }, 3, 2};
    CHECK(menu.rows == 3u);
    CHECK(menu.cols == 2u);
    CHECK(menu.cursorRow == 0u && menu.cursorCol == 0u);
    CHECK(menu.navigateGridSelection(-1, 0));
    CHECK(menu.cursorRow == 2u && menu.cursorCol == 0u);  // top -> bottom
    CHECK(menu.navigateGridSelection(0, -1));
    CHECK(menu.cursorCol == 1u);  // left -> right
    menu.cursorRow = 0;
    menu.cursorCol = 0;
    CHECK(menu.navigateGridSelection(1, 0));  // disabled row 1 col 0 -> row 2
    CHECK(menu.cursorRow == 2u);
    CHECK(menu.navigateGridSelection(0, 1));
    CHECK(menu.cursorCol == 1u);
}

static void testMenuScrollOffsetTracksVerticalCursor() {
    MenuModel menu{{
        {0, "0", true}, {1, "1", true}, {2, "2", true}, {3, "3", true},
    }, 4, 1, 2};
    CHECK(menu.scrollOffset == 0u);
    CHECK(menu.navigateGridSelection(1, 0));
    CHECK(menu.scrollOffset == 0u);
    CHECK(menu.navigateGridSelection(1, 0));
    CHECK(menu.cursorRow == 2u && menu.scrollOffset == 1u);
}

static void testMenuFlowSelectsEveryTopLevelMode() {
    struct Case { std::size_t row; MenuMode mode; int gameMode; };
    constexpr Case cases[] = {
        {0, MenuMode::ADVENTURE, 0}, {1, MenuMode::QUICK_RACE, 1},
        {2, MenuMode::TWO_PLAYER, 2}, {3, MenuMode::OPTIONS, -1},
    };
    for (const Case& c : cases) {
        MenuFlow flow;
        CHECK(tickMenuFlow(flow, pressed({.confirm = true}), 1.0f / 60.0f).type == MenuEventType::PAGE_CHANGED);
        for (std::size_t row = 0; row < c.row; ++row)
            CHECK(tickMenuFlow(flow, pressed({.down = true}), 1.0f / 60.0f).type == MenuEventType::PAGE_CHANGED);
        const MenuEvent event = tickMenuFlow(flow, pressed({.confirm = true}), 1.0f / 60.0f);
        if (c.mode == MenuMode::OPTIONS) {
            CHECK(event.type == MenuEventType::PAGE_CHANGED);
            CHECK(flow.mode == MenuMode::OPTIONS);
        } else {
            CHECK(event.type == MenuEventType::MODE_SELECTED);
            CHECK(event.mode == c.mode);
            CHECK(event.gameMode == c.gameMode);
        }
    }
}

static void testMenuFlowCancelReturnsToTitle() {
    MenuFlow flow;
    tickMenuFlow(flow, {.confirm = true}, 0.0f);
    CHECK(flow.screen == MenuScreen::MODE_SELECT);
    CHECK(tickMenuFlow(flow, {.cancel = true}, 0.0f).type == MenuEventType::CANCEL);
    CHECK(flow.screen == MenuScreen::TITLE);
    tickMenuFlow(flow, {.confirm = true}, 0.0f);
    tickMenuFlow(flow, {.down = true}, 0.0f);
    tickMenuFlow(flow, {.down = true}, 0.0f);
    tickMenuFlow(flow, {.down = true}, 0.0f);
    tickMenuFlow(flow, {.confirm = true}, 0.0f);
    CHECK(flow.screen == MenuScreen::OPTIONS);
    CHECK(tickMenuFlow(flow, {.cancel = true}, 0.0f).type == MenuEventType::CANCEL);
    CHECK(flow.screen == MenuScreen::MODE_SELECT);
}

static void testPauseMenuEnterLeaveAndFrames() {
    PauseMenu pause;
    CHECK(pause.pageShown == PausePage::RESUME);
    CHECK(tickPauseMenu(pause, {.down = true}).type == PauseEventType::PAGE_CHANGED);
    CHECK(pause.pageShown == PausePage::AUDIO_SETTINGS);
    CHECK(pause.framesOnPage == 0u);
    CHECK(tickPauseMenu(pause, {}).type == PauseEventType::NONE);
    CHECK(pause.framesOnPage == 1u);
    CHECK(tickPauseMenu(pause, {.confirm = true}).type == PauseEventType::PAGE_CHANGED);
    CHECK(pause.pageShown == PausePage::AUDIO_SETTINGS);
    CHECK(tickPauseMenu(pause, {.cancel = true}).type == PauseEventType::PAGE_CHANGED);
    CHECK(pause.pageShown == PausePage::RESUME);
    CHECK(tickPauseMenu(pause, {.confirm = true}).type == PauseEventType::EXIT);
    CHECK(pause.leave);
}

static void testControllerButtonMasksAndDefaults() {
    CHECK(buttonMask(PhysicalButton::NONE) == 0x0000u);
    CHECK(buttonMask(PhysicalButton::L2) == 0x0001u);
    CHECK(buttonMask(PhysicalButton::R2) == 0x0002u);
    CHECK(buttonMask(PhysicalButton::L1) == 0x0004u);
    CHECK(buttonMask(PhysicalButton::R1) == 0x0008u);
    CHECK(buttonMask(PhysicalButton::TRIANGLE) == 0x0010u);
    CHECK(buttonMask(PhysicalButton::CIRCLE) == 0x0020u);
    CHECK(buttonMask(PhysicalButton::CROSS) == 0x0040u);
    CHECK(buttonMask(PhysicalButton::SQUARE) == 0x0080u);
    CHECK(buttonMask(PhysicalButton::SELECT) == 0x0100u);
    CHECK(buttonMask(PhysicalButton::L3) == 0x0200u);
    CHECK(buttonMask(PhysicalButton::R3) == 0x0400u);
    CHECK(buttonMask(PhysicalButton::START) == 0x0800u);
    CHECK(buttonMask(PhysicalButton::DPAD_UP) == 0x1000u);
    CHECK(buttonMask(PhysicalButton::DPAD_RIGHT) == 0x2000u);
    CHECK(buttonMask(PhysicalButton::DPAD_DOWN) == 0x4000u);
    CHECK(buttonMask(PhysicalButton::DPAD_LEFT) == 0x8000u);

    ControllerConfig config;
    config.resetDefaults();
    CHECK(config.getButtonConfig(ControllerAction::ACCELERATE) == PhysicalButton::CROSS);
    CHECK(config.getButtonConfig(ControllerAction::BRAKE) == PhysicalButton::SQUARE);
    CHECK(config.getButtonConfig(ControllerAction::REVERSE) == PhysicalButton::CIRCLE);
    CHECK(config.getButtonConfig(ControllerAction::JET) == PhysicalButton::R2);
    CHECK(config.getButtonConfig(ControllerAction::WING_UP) == PhysicalButton::L1);
    CHECK(config.getButtonConfig(ControllerAction::WING_DOWN) == PhysicalButton::L2);
    CHECK(config.getButtonConfig(ControllerAction::HORN) == PhysicalButton::R1);
    CHECK(config.getButtonConfig(ControllerAction::VIEW) == PhysicalButton::TRIANGLE);
    CHECK(config.getButtonConfig(ControllerAction::LIGHTS) == PhysicalButton::R1);
    CHECK(config.getButtonConfig(ControllerAction::NAVIGATOR) == PhysicalButton::SELECT);
    SaveData save;
    saveControllerConfig(config, save);
    ControllerConfig loaded;
    CHECK(loadControllerConfig(loaded, save));
    CHECK(loaded.getButtonConfig(ControllerAction::ACCELERATE) == PhysicalButton::CROSS);
    CHECK(config.setButtonConfig(ControllerAction::ACCELERATE, PhysicalButton::START));
    CHECK(config.getButtonConfig(ControllerAction::ACCELERATE) == PhysicalButton::START);
    CHECK(!config.setButtonConfig(ControllerAction::BRAKE, PhysicalButton::START));
    config.resetDefaults();
    CHECK(config.setButtonConfig(ControllerAction::LIGHTS, PhysicalButton::R1));
}

static void testFixedMenuInputResolution() {
    const PadState state{buttonMask(PhysicalButton::CROSS) | buttonMask(PhysicalButton::CIRCLE) |
                         buttonMask(PhysicalButton::DPAD_UP) | buttonMask(PhysicalButton::DPAD_DOWN) |
                         buttonMask(PhysicalButton::DPAD_LEFT) | buttonMask(PhysicalButton::DPAD_RIGHT)};
    const MenuInput input = resolveMenuInput(state);
    CHECK(input.confirm && input.cancel && input.up && input.down && input.left && input.right);
}

static void testPadMenuEdgesAndHoldGesture() {
    using namespace rt::native::gameplay;
    // One action per press: holding fires once, releasing and pressing fires again.
    std::uint16_t previous = 0;
    const std::uint16_t down = buttonMask(PhysicalButton::DPAD_DOWN);
    const std::uint16_t cross = buttonMask(PhysicalButton::CROSS);
    CHECK(menuInputEdges(down, previous).down);
    CHECK(!menuInputEdges(down, previous).down);            // held: no repeat
    CHECK(!menuInputEdges(0, previous).down);
    CHECK(menuInputEdges(down, previous).down);            // released then pressed
    CHECK(!menuInputEdges(0, previous).down);                // release
    const MenuInput both = menuInputEdges(static_cast<std::uint16_t>(cross | down), previous);
    CHECK(both.confirm && both.down && !both.up && !both.cancel);
    CHECK(!menuInputEdges(static_cast<std::uint16_t>(buttonMask(PhysicalButton::CIRCLE)), previous).confirm);

    // Back + Start held 0.3 s fires once, then re-arms after release.
    float held = 0.f;
    CHECK(!menuGestureReady(true, 0.1f, held));
    CHECK(!menuGestureReady(true, 0.1f, held));
    CHECK(menuGestureReady(true, 0.1f, held));              // 0.3 s reached
    CHECK(!menuGestureReady(true, 0.1f, held));            // stays false while held
    CHECK(!menuGestureReady(false, 0.1f, held));            // release re-arms
    CHECK(!menuGestureReady(true, 0.1f, held));
    CHECK(menuGestureReady(true, 0.25f, held));
    // A press shorter than the hold never fires.
    held = 0.f;
    CHECK(!menuGestureReady(true, 0.1f, held));
    CHECK(!menuGestureReady(false, 1.f, held));
    CHECK(!menuGestureReady(true, 0.1f, held));
}

int main() {
    testMenuGridWrapAndDisabledRows();
    testMenuScrollOffsetTracksVerticalCursor();
    testMenuFlowSelectsEveryTopLevelMode();
    testMenuFlowCancelReturnsToTitle();
    testPauseMenuEnterLeaveAndFrames();
    testControllerButtonMasksAndDefaults();
    testFixedMenuInputResolution();
    testPadMenuEdgesAndHoldGesture();
    if (g_fail != 0) return EXIT_FAILURE;
    std::puts("menu tests passed");
    return EXIT_SUCCESS;
}
