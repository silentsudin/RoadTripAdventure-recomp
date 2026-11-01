#pragma once
// Authored menu / pause / controller-config models for Road Trip Adventure.
//
// These are host-side state machines derived from the documented menu cluster:
// UiMenuWindow 0x205350, NavigateGridSelection 0x206D28, StartMenuFlow
// 0x2052D0, UpdateMenuFlowTask 0x205088, and pause_state 0x1727B90.  They do
// not render, access guest memory, or require PS2 types.

#include "gameplay/save.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rt::native::gameplay {

struct MenuItem {
    int id = 0;
    std::string label;
    bool enabled = true;
};

struct MenuModel {
    std::vector<MenuItem> items;
    std::size_t rows = 0;
    std::size_t cols = 1;
    std::size_t visibleRows = 0;
    std::size_t cursorRow = 0;
    std::size_t cursorCol = 0;
    std::size_t scrollOffset = 0;

    // NavigateGridSelection 0x206D28: wrap the coordinate and scan in the
    // requested direction until an enabled item is found.
    bool navigateGridSelection(int dRow, int dCol);
};

struct MenuInput {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool confirm = false;
    bool cancel = false;
};

// Values match game_mode 0x33590A where one is documented. TITLE and OPTIONS
// are flow screens rather than game_mode values.
enum class MenuMode : int {
    ADVENTURE = 0,
    QUICK_RACE = 1,
    TWO_PLAYER = 2,
    TWO_PLAYER_VERSUS = 3,
    TWO_PLAYER_COOP = 6,
    ATTRACT_DEMO = 8,
    TITLE = -1,
    OPTIONS = -2,
};

enum class MenuScreen { TITLE, MODE_SELECT, OPTIONS };
enum class MenuEventType { NONE, MODE_SELECTED, CONFIRM, CANCEL, PAGE_CHANGED };

struct MenuEvent {
    MenuEventType type = MenuEventType::NONE;
    MenuMode mode = MenuMode::TITLE;
    int gameMode = -1;
};

struct MenuFlow {
    MenuScreen screen = MenuScreen::TITLE;
    MenuMode mode = MenuMode::TITLE;
    std::size_t topLevelCursor = 0;
    float timeOnScreen = 0.0f;
};

MenuEvent tickMenuFlow(MenuFlow& flow, const MenuInput& input, float dt);

// The 12 entries represented by pause_pages 0x2EAD60. The latter five are
// generic slots retained so the authored page identity stays table-compatible
// without guessing undocumented names.
enum class PausePage : std::uint8_t {
    RESUME = 0,
    AUDIO_SETTINGS,
    CONTROLLER_CONFIG,
    WORLD_MAP,
    GARAGE,
    RESTART,
    QUIT,
    STATUS,
    OPTIONS,
    HELP,
    RESERVED_10,
    RESERVED_11,
    COUNT,
};

enum class PauseEventType { NONE, PAGE_CHANGED, EXIT };
struct PauseEvent { PauseEventType type = PauseEventType::NONE; PausePage page = PausePage::RESUME; };

struct PauseMenu {
    PausePage pageShown = PausePage::RESUME;      // pause_state +0
    PausePage pageRequested = PausePage::RESUME;  // pause_state +4
    std::uint32_t framesOnPage = 0;               // pause_state +8
    bool leave = false;                           // pause_state +C
    bool inSubmenu = false;
};

PauseEvent tickPauseMenu(PauseMenu& pause, const MenuInput& input);

// The ten actions use precisely SaveData::controllerRemap's documented word
// order: gas, brake, reverse, jet, wing up/down, horn, view, lights, navigator.
enum class ControllerAction : std::size_t {
    ACCELERATE = 0, BRAKE, REVERSE, JET, WING_UP, WING_DOWN, HORN, VIEW, LIGHTS, NAVIGATOR,
};

enum class PhysicalButton : std::uint16_t {
    NONE = 0,
    L2 = 0x0001, R2 = 0x0002, L1 = 0x0004, R1 = 0x0008,
    TRIANGLE = 0x0010, CIRCLE = 0x0020, CROSS = 0x0040, SQUARE = 0x0080,
    SELECT = 0x0100, L3 = 0x0200, R3 = 0x0400, START = 0x0800,
    DPAD_UP = 0x1000, DPAD_RIGHT = 0x2000, DPAD_DOWN = 0x4000, DPAD_LEFT = 0x8000,
};

constexpr std::uint16_t buttonMask(PhysicalButton button) {
    return static_cast<std::uint16_t>(button);
}

struct PadState { std::uint16_t pressedButtons = 0; };

class ControllerConfig {
public:
    ControllerConfig();
    [[nodiscard]] PhysicalButton getButtonConfig(ControllerAction action) const;
    // Returns false for an unbound/unknown button or a duplicate, except for
    // the documented lights=horn shared R1 binding.
    bool setButtonConfig(ControllerAction action, PhysicalButton button);
    void resetDefaults();
    [[nodiscard]] const std::array<std::uint16_t, kControllerActionCount>& saveWords() const { return bindings_; }
    bool loadSaveWords(const std::array<std::uint16_t, kControllerActionCount>& words);

private:
    std::array<std::uint16_t, kControllerActionCount> bindings_{};
};

void saveControllerConfig(const ControllerConfig& config, SaveData& save);
bool loadControllerConfig(ControllerConfig& config, const SaveData& save);
// Menu navigation is fixed (not part of the +0x12E8 driving remap).
MenuInput resolveMenuInput(const PadState& input);

// One action per press from the pad (d-pad, CROSS confirm, CIRCLE cancel),
// like the keyboard's key-down events: a held button fires once and again only
// after it is released (resolveMenuInput above is the held state).
MenuInput menuInputEdges(std::uint16_t pressed, std::uint16_t& previous);

// The recomp's in-game menu gesture (README.md:135 "Guide, or Back + Start
// held"): true on the frame a hold reaches kMenuHoldSeconds, and again only
// after the buttons are released.
constexpr float kMenuHoldSeconds = 0.3f;
bool menuGestureReady(bool pressed, float dt, float& held);

}  // namespace rt::native::gameplay
