#include "gameplay/app_flow.h"

#include <algorithm>

namespace rt::native::gameplay {
namespace {

constexpr std::uint8_t kMenuItemLast = 4;
constexpr std::uint8_t kFactoryDriveAroundItem = 4;
constexpr std::uint32_t kDriveFinishedBit = 0x08u;
constexpr std::uint32_t kDriveRacingBit = 0x20u;
constexpr std::uint8_t kAttractGameMode = static_cast<std::uint8_t>(FlowGameMode::ATTRACT_DEMO);

void enterTitle(FlowWords& w) {
    w.state = FlowState::TITLE;
    w.page = FlowMenuPage::MODE_SELECT;
    w.cursor = 0;
    w.session = FlowSession::EXPLORE;
    w.overlay = FlowOverlay::NONE;
    w.gameMode = static_cast<std::uint8_t>(FlowGameMode::ADVENTURE);
    w.scene3dActive = 0;
    w.scene3dHandler = 0;
    w.driveFlags = 0;
}

void enterMenu(FlowWords& w, FlowMenuPage page, std::uint8_t cursor) {
    w.state = FlowState::MENU;
    w.page = page;
    w.cursor = cursor;
    w.session = FlowSession::EXPLORE;
    w.overlay = FlowOverlay::NONE;
    w.gameMode = static_cast<std::uint8_t>(FlowGameMode::ADVENTURE);
    w.scene3dActive = 0;
    w.scene3dHandler = 0;
    w.driveFlags = 0;
}

void enterAttract(FlowWords& w) {
    w.state = FlowState::ATTRACT;
    w.page = FlowMenuPage::MODE_SELECT;
    w.cursor = 0;
    w.session = FlowSession::EXPLORE;
    w.overlay = FlowOverlay::NONE;
    w.gameMode = kAttractGameMode;
    w.scene3dActive = 2;
    w.scene3dHandler = kFlowHandlerRace;
    w.driveFlags = kFlowDriveFlagsAttract;
}

// Race entry. Quick Race and 2P come from the menu and do not touch
// progress.scene (oracle note 1). The Adventure race entered from town sets 10.
void enterRace(FlowWords& w, FlowGameMode mode) {
    w.state = FlowState::PLAYING;
    w.page = FlowMenuPage::MODE_SELECT;
    w.cursor = 0;
    w.session = FlowSession::RACE;
    w.overlay = FlowOverlay::NONE;
    w.gameMode = static_cast<std::uint8_t>(mode);
    w.scene3dActive = 2;
    w.scene3dHandler = kFlowHandlerRace;
    w.driveFlags = kFlowDriveFlagsRace;
    if (mode == FlowGameMode::ADVENTURE) w.progressScene = kFlowSceneRacing;
}

void enterTownDriving(FlowWords& w) {
    w.state = FlowState::PLAYING;
    w.page = FlowMenuPage::MODE_SELECT;
    w.cursor = 0;
    w.session = FlowSession::EXPLORE;
    w.overlay = FlowOverlay::NONE;
    w.gameMode = static_cast<std::uint8_t>(FlowGameMode::ADVENTURE);
    w.scene3dActive = 2;
    w.scene3dHandler = kFlowHandlerTown;
    w.driveFlags = kFlowDriveFlagsTown;
    w.progressScene = kFlowSceneTown;
    w.location = 1;
}

// Q's Factory list. Not driving, so drive flags stay 0 (derived: the oracle
// records the car as not driving but does not record drive_flags here).
void enterFactory(FlowWords& w) {
    w.state = FlowState::PLAYING;
    w.page = FlowMenuPage::MODE_SELECT;
    w.cursor = 0;
    w.session = FlowSession::EXPLORE;
    w.overlay = FlowOverlay::FACTORY_MENU;
    w.gameMode = static_cast<std::uint8_t>(FlowGameMode::ADVENTURE);
    w.scene3dActive = 2;
    w.scene3dHandler = kFlowHandlerTown;
    w.driveFlags = 0;
    w.progressScene = kFlowSceneFactory;
    w.location = 1;
}

bool applyMenu(FlowWords& w, FlowEvent event) {
    switch (w.page) {
    case FlowMenuPage::MODE_SELECT:
        switch (event) {
        case FlowEvent::NavUp:
            if (w.cursor == 0) return false;
            --w.cursor;
            return true;
        case FlowEvent::NavDown:
            if (w.cursor >= kMenuItemLast) return false;
            ++w.cursor;
            return true;
        case FlowEvent::Confirm:
            switch (w.cursor) {
            case 0: w.page = FlowMenuPage::ADVENTURE_SAVE; return true;
            case 1: enterRace(w, FlowGameMode::QUICK_RACE); return true;
            case 2: enterRace(w, FlowGameMode::TWO_PLAYER); return true;
            case 3: w.page = FlowMenuPage::OPTIONS; return true;
            case 4: w.page = FlowMenuPage::RESULTS_VIEW; return true;
            default: return false;
            }
        case FlowEvent::Cancel:
            enterTitle(w);
            return true;
        default:
            return false;
        }
    case FlowMenuPage::ADVENTURE_SAVE:
        if (event == FlowEvent::Confirm) {
            enterFactory(w);
            return true;
        }
        if (event == FlowEvent::Cancel) {
            w.page = FlowMenuPage::MODE_SELECT;
            w.cursor = 0;
            return true;
        }
        return false;
    case FlowMenuPage::OPTIONS:
    case FlowMenuPage::RESULTS_VIEW:
        if (event == FlowEvent::Cancel) {
            w.page = FlowMenuPage::MODE_SELECT;
            return true;
        }
        return false;
    }
    return false;
}

bool applyExplore(FlowWords& w, FlowEvent event) {
    switch (w.overlay) {
    case FlowOverlay::NONE:
        switch (event) {
        case FlowEvent::StartRace:
            enterRace(w, FlowGameMode::ADVENTURE);
            return true;
        case FlowEvent::OpenDialogue:
            w.overlay = FlowOverlay::DIALOGUE;
            return true;
        case FlowEvent::OpenShop:
            w.overlay = FlowOverlay::SHOP;
            return true;
        default:
            return false;
        }
    case FlowOverlay::DIALOGUE:
        if (event != FlowEvent::DialogueDone) return false;
        w.overlay = FlowOverlay::NONE;
        return true;
    case FlowOverlay::SHOP:
        if (event != FlowEvent::CloseShop) return false;
        w.overlay = FlowOverlay::NONE;
        return true;
    case FlowOverlay::FACTORY_MENU:
        switch (event) {
        case FlowEvent::NavUp:
            if (w.cursor == 0) return false;
            --w.cursor;
            return true;
        case FlowEvent::NavDown:
            if (w.cursor >= kMenuItemLast) return false;
            ++w.cursor;
            return true;
        case FlowEvent::Confirm:
            if (w.cursor != kFactoryDriveAroundItem) return false;
            w.overlay = FlowOverlay::FACTORY_EXIT_DIALOGUE;
            w.cursor = 0;
            return true;
        default:
            return false;
        }
    case FlowOverlay::FACTORY_EXIT_DIALOGUE:
        if (event != FlowEvent::DialogueDone) return false;
        enterTownDriving(w);
        return true;
    }
    return false;
}

bool applyPlaying(FlowWords& w, FlowEvent event) {
    if (event == FlowEvent::Escape) {
        enterMenu(w, FlowMenuPage::MODE_SELECT, 0);
        return true;
    }
    switch (w.session) {
    case FlowSession::EXPLORE:
        return applyExplore(w, event);
    case FlowSession::RACE:
        if (event != FlowEvent::RaceFinished) return false;
        w.session = FlowSession::RESULTS;
        w.driveFlags = (w.driveFlags & ~kDriveRacingBit) | kDriveFinishedBit;
        if (w.gameMode == static_cast<std::uint8_t>(FlowGameMode::ADVENTURE)) w.progressScene = kFlowSceneRaceOver;
        return true;
    case FlowSession::RESULTS:
        if (event != FlowEvent::ResultsConfirm) return false;
        enterTownDriving(w);
        return true;
    }
    return false;
}

}  // namespace

AppFlow AppFlow::directLaunch(DirectLaunch launch) {
    AppFlow flow;
    FlowWords& w = flow.words_;
    switch (launch) {
    case DirectLaunch::TITLE:
        enterTitle(w);
        break;
    case DirectLaunch::MENU_MODE_SELECT:
        enterMenu(w, FlowMenuPage::MODE_SELECT, 0);
        break;
    case DirectLaunch::PLAYING_RACE:
        enterRace(w, FlowGameMode::ADVENTURE);
        break;
    case DirectLaunch::PLAYING_EXPLORE:
        enterTownDriving(w);
        break;
    }
    flow.previous_ = w;
    return flow;
}

DirectLaunch AppFlow::directLaunchForState(std::string_view stateName, std::string_view modeName) {
    if (stateName == "MENU") return DirectLaunch::MENU_MODE_SELECT;
    if (stateName == "PLAYING") {
        return modeName == "explore" ? DirectLaunch::PLAYING_EXPLORE : DirectLaunch::PLAYING_RACE;
    }
    return DirectLaunch::TITLE;
}

FlowStep AppFlow::step(std::optional<FlowEvent> input) {
    previous_ = words_;
    if (words_.state == FlowState::TITLE) ++words_.idleTicks;
    FlowStep out;
    if (input) out.input = apply(*input);
    if (words_.state == FlowState::TITLE && words_.idleTicks >= kFlowTitleIdleTicks) {
        out.attractTimeout = apply(FlowEvent::IdleTimeout).accepted;
    }
    return out;
}

FlowResult AppFlow::apply(FlowEvent event) {
    FlowResult result{.accepted = false, .from = words_.state, .to = words_.state};
    switch (words_.state) {
    case FlowState::BOOT:
        if (event == FlowEvent::BootComplete) {
            enterTitle(words_);
            result.accepted = true;
        }
        break;
    case FlowState::TITLE:
        if (event == FlowEvent::Start) {
            enterMenu(words_, FlowMenuPage::MODE_SELECT, 0);
            result.accepted = true;
        } else if (event == FlowEvent::IdleTimeout) {
            enterAttract(words_);
            result.accepted = true;
        }
        break;
    case FlowState::ATTRACT:
        if (event == FlowEvent::AnyKey) {
            enterTitle(words_);
            result.accepted = true;
        }
        break;
    case FlowState::MENU:
        result.accepted = applyMenu(words_, event);
        break;
    case FlowState::PLAYING:
        result.accepted = applyPlaying(words_, event);
        break;
    }
    result.to = words_.state;
    if (result.accepted) words_.idleTicks = 0;
    return result;
}

FlowView AppFlow::view(float alpha) const {
    return FlowView{.previous = previous_, .current = words_, .alpha = std::clamp(alpha, 0.f, 1.f)};
}

std::uint64_t hashFlowWords(const FlowWords& words) {
    std::uint64_t h = 0xcbf29ce484222325ull;
    const auto byte = [&h](std::uint8_t v) {
        h ^= v;
        h *= 0x100000001b3ull;
    };
    const auto word = [&byte](std::uint32_t v) {
        for (int i = 0; i < 4; ++i) byte(static_cast<std::uint8_t>(v >> (8 * i)));
    };
    byte(static_cast<std::uint8_t>(words.state));
    byte(static_cast<std::uint8_t>(words.page));
    byte(words.cursor);
    byte(static_cast<std::uint8_t>(words.session));
    byte(static_cast<std::uint8_t>(words.overlay));
    byte(words.gameMode);
    byte(words.scene3dActive);
    word(words.scene3dHandler);
    word(words.driveFlags);
    byte(words.location);
    word(words.progressScene);
    word(words.idleTicks);
    return h;
}

const char* flowStateName(FlowState state) {
    switch (state) {
    case FlowState::BOOT: return "BOOT";
    case FlowState::TITLE: return "TITLE";
    case FlowState::ATTRACT: return "ATTRACT";
    case FlowState::MENU: return "MENU";
    case FlowState::PLAYING: return "PLAYING";
    }
    return "UNKNOWN";
}

}  // namespace rt::native::gameplay
