// Application flow state machine (W2-02). Pure logic: no SDL, no GPU, no
// heap. Top-level state names are the RT_APP_STATE values honoured by
// app/main.cpp (TITLE, MENU, PLAYING) plus BOOT and ATTRACT. Session names
// match gameplay::SessionState (session.h) under FlowSession so this header
// does not collide with it. Guest-visible words follow
// docs/native-reference/oracle-golden-frames.md and app-flow.md.
#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace rt::native::gameplay {

enum class FlowState : std::uint8_t { BOOT, TITLE, ATTRACT, MENU, PLAYING };

// Pages inside MENU. MODE_SELECT is the main menu list (menu.h MenuScreen).
enum class FlowMenuPage : std::uint8_t { MODE_SELECT, ADVENTURE_SAVE, OPTIONS, RESULTS_VIEW };

// Pages inside PLAYING (session.h SessionState).
enum class FlowSession : std::uint8_t { EXPLORE, RACE, RESULTS };

// Overlays on EXPLORE. DIALOGUE and SHOP are the town script's dialogueTask
// (0x23B168) and shop spawns; FACTORY_MENU is Q's Factory list (scene 6);
// FACTORY_EXIT_DIALOGUE is the "Come again!" dialogue that leaves the factory
// into the town (scene 2).
enum class FlowOverlay : std::uint8_t { NONE, DIALOGUE, SHOP, FACTORY_MENU, FACTORY_EXIT_DIALOGUE };

// Values of the game_mode byte at 0x33590A (menu.h MenuMode).
enum class FlowGameMode : std::uint8_t { ADVENTURE = 0, QUICK_RACE = 1, TWO_PLAYER = 2, ATTRACT_DEMO = 8 };

enum class FlowEvent : std::uint8_t {
    BootComplete,
    Start,
    IdleTimeout,
    AnyKey,
    NavUp,
    NavDown,
    Confirm,
    Cancel,
    Escape,
    StartRace,
    RaceFinished,
    ResultsConfirm,
    OpenDialogue,
    DialogueDone,
    OpenShop,
    CloseShop,
};
inline constexpr std::uint8_t kFlowEventCount = 16;

// RT_APP_STATE / RT_APP_MODE direct launches (main.cpp initialAppState and the
// RT_APP_MODE=race / explore overrides).
enum class DirectLaunch : std::uint8_t { TITLE, MENU_MODE_SELECT, PLAYING_RACE, PLAYING_EXPLORE };

// Guest-equivalent words. Every field is discrete; an interpolating renderer
// reads the pair from FlowView and picks a side with its own alpha.
struct FlowWords {
    FlowState state = FlowState::BOOT;
    FlowMenuPage page = FlowMenuPage::MODE_SELECT;
    std::uint8_t cursor = 0;
    FlowSession session = FlowSession::EXPLORE;
    FlowOverlay overlay = FlowOverlay::NONE;
    std::uint8_t gameMode = 0;          // 0x33590A
    std::uint8_t scene3dActive = 0;     // 0x3356C4
    std::uint32_t scene3dHandler = 0;   // 0x3356D8: 0x21CF80 town, 0x21CE48 race
    std::uint32_t driveFlags = 0;       // 0x335928
    std::uint8_t location = 0;          // 0x335923 (1 = Peach Town)
    std::uint32_t progressScene = 0;    // progress.scene (0x0177F760+0x538)
    std::uint32_t idleTicks = 0;        // logic ticks since the last accepted event

    friend bool operator==(const FlowWords&, const FlowWords&) = default;
};

inline constexpr std::uint32_t kFlowHandlerTown = 0x0021CF80u;
inline constexpr std::uint32_t kFlowHandlerRace = 0x0021CE48u;
inline constexpr std::uint32_t kFlowDriveFlagsRace = 0x124u;
inline constexpr std::uint32_t kFlowDriveFlagsAttract = 0x10104u;
inline constexpr std::uint32_t kFlowDriveFlagsTown = 0x4u;
inline constexpr std::uint32_t kFlowSceneTown = 2u;
inline constexpr std::uint32_t kFlowSceneFactory = 6u;
inline constexpr std::uint32_t kFlowSceneRaceOver = 3u;
inline constexpr std::uint32_t kFlowSceneRacing = 10u;
inline constexpr std::uint32_t kFlowTickHz = 60u;
inline constexpr std::uint32_t kFlowTitleIdleTicks = 45u * kFlowTickHz;

struct FlowResult {
    bool accepted = false;
    FlowState from = FlowState::BOOT;
    FlowState to = FlowState::BOOT;
};

struct FlowStep {
    FlowResult input;          // rejected when no input was supplied
    bool attractTimeout = false;
};

struct FlowView {
    FlowWords previous;
    FlowWords current;
    float alpha = 1.f;

    // Discrete fields cannot be blended. Snap at the midpoint.
    [[nodiscard]] FlowWords presented() const { return alpha < .5f ? previous : current; }
};

class AppFlow {
public:
    AppFlow() = default;

    static AppFlow directLaunch(DirectLaunch launch);
    static DirectLaunch directLaunchForState(std::string_view stateName, std::string_view modeName);

    // One fixed 60 Hz logic tick. The optional input is applied after the
    // tick's idle counter advances. The title idle timeout fires automatically
    // at kFlowTitleIdleTicks.
    FlowStep step(std::optional<FlowEvent> input = std::nullopt);

    // Applies one discrete event without advancing time. Illegal pairs are
    // rejected and leave the words untouched.
    FlowResult apply(FlowEvent event);

    [[nodiscard]] const FlowWords& current() const { return words_; }
    [[nodiscard]] const FlowWords& previous() const { return previous_; }
    [[nodiscard]] FlowView view(float alpha = 1.f) const;

private:
    FlowWords words_;
    FlowWords previous_;
};

// FNV-1a over the canonical little-endian encoding of every field, in
// declaration order.
[[nodiscard]] std::uint64_t hashFlowWords(const FlowWords& words);

[[nodiscard]] const char* flowStateName(FlowState state);

}  // namespace rt::native::gameplay
