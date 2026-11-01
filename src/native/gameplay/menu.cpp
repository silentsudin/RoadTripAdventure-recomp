// Authored host-side implementation of the documented menu state machinery.
#include "gameplay/menu.h"

#include <algorithm>

namespace rt::native::gameplay {
namespace {

constexpr std::size_t actionIndex(ControllerAction action) {
    return static_cast<std::size_t>(action);
}

bool pressed(const PadState& state, PhysicalButton button) {
    return (state.pressedButtons & buttonMask(button)) != 0;
}

bool isKnownPhysicalButton(std::uint16_t word) {
    switch (static_cast<PhysicalButton>(word)) {
    case PhysicalButton::L2: case PhysicalButton::R2: case PhysicalButton::L1: case PhysicalButton::R1:
    case PhysicalButton::TRIANGLE: case PhysicalButton::CIRCLE: case PhysicalButton::CROSS: case PhysicalButton::SQUARE:
    case PhysicalButton::SELECT: case PhysicalButton::L3: case PhysicalButton::R3: case PhysicalButton::START:
    case PhysicalButton::DPAD_UP: case PhysicalButton::DPAD_RIGHT: case PhysicalButton::DPAD_DOWN: case PhysicalButton::DPAD_LEFT:
        return true;
    case PhysicalButton::NONE:
        return false;
    }
    return false;
}

bool isHornLightsPair(std::size_t first, std::size_t second) {
    return (first == actionIndex(ControllerAction::HORN) && second == actionIndex(ControllerAction::LIGHTS)) ||
           (first == actionIndex(ControllerAction::LIGHTS) && second == actionIndex(ControllerAction::HORN));
}

bool enabledAt(const MenuModel& menu, std::size_t row, std::size_t col) {
    const std::size_t index = row * menu.cols + col;
    return index < menu.items.size() && menu.items[index].enabled;
}

void updateScroll(MenuModel& menu) {
    if (menu.cols != 1 || menu.visibleRows == 0 || menu.rows <= menu.visibleRows) {
        menu.scrollOffset = 0;
        return;
    }
    if (menu.cursorRow < menu.scrollOffset) menu.scrollOffset = menu.cursorRow;
    if (menu.cursorRow >= menu.scrollOffset + menu.visibleRows)
        menu.scrollOffset = menu.cursorRow - menu.visibleRows + 1;
}

}  // namespace

bool MenuModel::navigateGridSelection(int dRow, int dCol) {
    if (rows == 0 || cols == 0 || items.empty() || (dRow == 0 && dCol == 0)) return false;
    const std::size_t oldRow = cursorRow;
    const std::size_t oldCol = cursorCol;
    const int rowStep = dRow == 0 ? 0 : (dRow > 0 ? 1 : -1);
    const int colStep = dCol == 0 ? 0 : (dCol > 0 ? 1 : -1);
    for (std::size_t attempt = 0; attempt < rows * cols; ++attempt) {
        cursorRow = static_cast<std::size_t>((static_cast<int>(cursorRow) + rowStep + static_cast<int>(rows)) % static_cast<int>(rows));
        cursorCol = static_cast<std::size_t>((static_cast<int>(cursorCol) + colStep + static_cast<int>(cols)) % static_cast<int>(cols));
        if (enabledAt(*this, cursorRow, cursorCol)) {
            updateScroll(*this);
            return cursorRow != oldRow || cursorCol != oldCol;
        }
    }
    cursorRow = oldRow;
    cursorCol = oldCol;
    return false;
}

MenuEvent tickMenuFlow(MenuFlow& flow, const MenuInput& input, float dt) {
    flow.timeOnScreen += std::max(dt, 0.0f);
    if (flow.screen == MenuScreen::TITLE) {
        if (input.confirm) {
            flow.screen = MenuScreen::MODE_SELECT;
            flow.mode = MenuMode::TITLE;
            flow.topLevelCursor = 0;
            flow.timeOnScreen = 0.0f;
            return {MenuEventType::PAGE_CHANGED, MenuMode::TITLE, -1};
        }
        return {};
    }
    if (flow.screen == MenuScreen::OPTIONS) {
        if (input.cancel) {
            flow.screen = MenuScreen::MODE_SELECT;
            flow.mode = MenuMode::TITLE;
            flow.timeOnScreen = 0.0f;
            return {MenuEventType::CANCEL, MenuMode::TITLE, -1};
        }
        return input.confirm ? MenuEvent{MenuEventType::CONFIRM, MenuMode::OPTIONS, -1} : MenuEvent{};
    }
    if (input.cancel) {
        flow.screen = MenuScreen::TITLE;
        flow.mode = MenuMode::TITLE;
        flow.timeOnScreen = 0.0f;
        return {MenuEventType::CANCEL, MenuMode::TITLE, -1};
    }
    if (input.up) {
        flow.topLevelCursor = (flow.topLevelCursor + 3) % 4;
        return {MenuEventType::PAGE_CHANGED, MenuMode::TITLE, -1};
    }
    if (input.down) {
        flow.topLevelCursor = (flow.topLevelCursor + 1) % 4;
        return {MenuEventType::PAGE_CHANGED, MenuMode::TITLE, -1};
    }
    if (!input.confirm) return {};
    static constexpr MenuMode kChoices[] = {
        MenuMode::ADVENTURE, MenuMode::QUICK_RACE, MenuMode::TWO_PLAYER, MenuMode::OPTIONS};
    flow.mode = kChoices[flow.topLevelCursor];
    flow.timeOnScreen = 0.0f;
    if (flow.mode == MenuMode::OPTIONS) {
        flow.screen = MenuScreen::OPTIONS;
        return {MenuEventType::PAGE_CHANGED, flow.mode, -1};
    }
    return {MenuEventType::MODE_SELECTED, flow.mode, static_cast<int>(flow.mode)};
}

PauseEvent tickPauseMenu(PauseMenu& pause, const MenuInput& input) {
    if (pause.leave) return {PauseEventType::EXIT, pause.pageShown};
    if (input.cancel) {
        if (pause.inSubmenu) {
            pause.inSubmenu = false;
            pause.pageRequested = PausePage::RESUME;
            pause.pageShown = PausePage::RESUME;
            pause.framesOnPage = 0;
            return {PauseEventType::PAGE_CHANGED, pause.pageShown};
        }
        pause.leave = true;
        return {PauseEventType::EXIT, pause.pageShown};
    }
    if (!pause.inSubmenu && (input.up || input.down)) {
        const auto count = static_cast<int>(PausePage::COUNT);
        const auto current = static_cast<int>(pause.pageShown);
        const int delta = input.up ? -1 : 1;
        pause.pageRequested = static_cast<PausePage>((current + delta + count) % count);
        pause.pageShown = pause.pageRequested;
        pause.framesOnPage = 0;
        return {PauseEventType::PAGE_CHANGED, pause.pageShown};
    }
    if (!pause.inSubmenu && input.confirm) {
        if (pause.pageShown == PausePage::RESUME) {
            pause.leave = true;
            return {PauseEventType::EXIT, pause.pageShown};
        }
        pause.inSubmenu = true;
        pause.pageRequested = pause.pageShown;
        pause.framesOnPage = 0;
        return {PauseEventType::PAGE_CHANGED, pause.pageShown};
    }
    ++pause.framesOnPage;
    return {};
}

ControllerConfig::ControllerConfig() { resetDefaults(); }

PhysicalButton ControllerConfig::getButtonConfig(ControllerAction action) const {
    return static_cast<PhysicalButton>(bindings_[actionIndex(action)]);
}

bool ControllerConfig::setButtonConfig(ControllerAction action, PhysicalButton button) {
    if (!isKnownPhysicalButton(buttonMask(button))) return false;
    const std::size_t index = actionIndex(action);
    const std::uint16_t word = buttonMask(button);
    for (std::size_t i = 0; i < bindings_.size(); ++i)
        if (i != index && bindings_[i] == word && !isHornLightsPair(index, i)) return false;
    bindings_[index] = word;
    return true;
}

void ControllerConfig::resetDefaults() {
    bindings_ = {buttonMask(PhysicalButton::CROSS), buttonMask(PhysicalButton::SQUARE),
                 buttonMask(PhysicalButton::CIRCLE), buttonMask(PhysicalButton::R2),
                 buttonMask(PhysicalButton::L1), buttonMask(PhysicalButton::L2),
                 buttonMask(PhysicalButton::R1), buttonMask(PhysicalButton::TRIANGLE),
                 buttonMask(PhysicalButton::R1), buttonMask(PhysicalButton::SELECT)};
}

bool ControllerConfig::loadSaveWords(const std::array<std::uint16_t, kControllerActionCount>& words) {
    for (const std::uint16_t word : words)
        if (!isKnownPhysicalButton(word)) return false;
    bindings_ = words;
    return true;
}

void saveControllerConfig(const ControllerConfig& config, SaveData& save) {
    save.controllerRemap = config.saveWords();
}

bool loadControllerConfig(ControllerConfig& config, const SaveData& save) {
    return config.loadSaveWords(save.controllerRemap);
}

MenuInput resolveMenuInput(const PadState& input) {
    return {
        .up = pressed(input, PhysicalButton::DPAD_UP),
        .down = pressed(input, PhysicalButton::DPAD_DOWN),
        .left = pressed(input, PhysicalButton::DPAD_LEFT),
        .right = pressed(input, PhysicalButton::DPAD_RIGHT),
        .confirm = pressed(input, PhysicalButton::CROSS),
        .cancel = pressed(input, PhysicalButton::CIRCLE),
    };
}

MenuInput menuInputEdges(std::uint16_t pressed, std::uint16_t& previous) {
    const std::uint16_t edges = static_cast<std::uint16_t>(pressed & ~previous);
    previous = pressed;
    return {
        .up = (edges & buttonMask(PhysicalButton::DPAD_UP)) != 0,
        .down = (edges & buttonMask(PhysicalButton::DPAD_DOWN)) != 0,
        .left = (edges & buttonMask(PhysicalButton::DPAD_LEFT)) != 0,
        .right = (edges & buttonMask(PhysicalButton::DPAD_RIGHT)) != 0,
        .confirm = (edges & buttonMask(PhysicalButton::CROSS)) != 0,
        .cancel = (edges & buttonMask(PhysicalButton::CIRCLE)) != 0,
    };
}

bool menuGestureReady(bool pressed, float dt, float& held) {
    if (!pressed) {
        held = 0.f;
        return false;
    }
    if (held >= kMenuHoldSeconds) return false;  // already fired for this hold
    held += dt;
    return held >= kMenuHoldSeconds;
}

}  // namespace rt::native::gameplay
