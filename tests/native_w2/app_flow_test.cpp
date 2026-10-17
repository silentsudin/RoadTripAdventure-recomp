// W2-02 application flow acceptance. Pure logic; the ELF is only used for a
// dispatch cross-check (the handler selector at 0x21CE08 and its race call).
// Usage: w2_app_flow_test [--elf PATH]   (or RT_ELF=PATH)
// Exit: 0 all thresholds met, 1 threshold miss, 2 NO_ELF.

#include "gameplay/app_flow.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace {

using rt::native::gameplay::AppFlow;
using rt::native::gameplay::DirectLaunch;
using rt::native::gameplay::FlowEvent;
using rt::native::gameplay::FlowGameMode;
using rt::native::gameplay::FlowMenuPage;
using rt::native::gameplay::FlowOverlay;
using rt::native::gameplay::FlowSession;
using rt::native::gameplay::FlowState;
using rt::native::gameplay::FlowWords;

constexpr std::uint32_t kElfBase = 0x1FF000u;
constexpr std::uint32_t kSelectScene3dHandler = 0x0021CE08u;
constexpr std::uint32_t kStartRaceModeTask = 0x0020F950u;
constexpr std::uint32_t kStartRaceModeEnd = 0x00210154u;
constexpr std::uint32_t kStartTownModeTask = 0x00210430u;
constexpr std::uint32_t kStartTownModeEnd = 0x002105B8u;
constexpr std::uint32_t kDispatchWords[7] = {
    0x10800003u, 0x3C020022u, 0x10000003u, 0x2442CE48u,
    0x3C020022u, 0x2442CF80u, 0x03E00008u,
};

// Test-side view of the legal-relevant context. Independent of AppFlow's
// internals: built from the public words and normalised the same way the spec
// below defines it.
struct Ctx {
    FlowState st = FlowState::BOOT;
    FlowMenuPage page = FlowMenuPage::MODE_SELECT;
    std::uint8_t cur = 0;
    FlowSession sess = FlowSession::EXPLORE;
    FlowOverlay ov = FlowOverlay::NONE;
    std::uint8_t mode = 0;
    auto operator<=>(const Ctx&) const = default;
};

Ctx ctxOf(const FlowWords& w) {
    Ctx c;
    c.st = w.state;
    c.page = w.state == FlowState::MENU ? w.page : FlowMenuPage::MODE_SELECT;
    c.cur = w.cursor;
    c.sess = w.state == FlowState::PLAYING ? w.session : FlowSession::EXPLORE;
    c.ov = w.state == FlowState::PLAYING ? w.overlay : FlowOverlay::NONE;
    c.mode = w.gameMode;
    return c;
}

Ctx mk(FlowState st, FlowMenuPage page, std::uint8_t cur, FlowSession sess, FlowOverlay ov, FlowGameMode mode) {
    return Ctx{st, page, cur, sess, ov, static_cast<std::uint8_t>(mode)};
}

// Spec table, written from the documented design (app-flow.md), not from
// AppFlow's code. Returns the single legal successor, or nothing when the
// event is illegal in that context.
std::optional<Ctx> expectedNext(const Ctx& c, FlowEvent ev) {
    using E = FlowEvent;
    using M = FlowGameMode;
    const Ctx title = mk(FlowState::TITLE, FlowMenuPage::MODE_SELECT, 0, FlowSession::EXPLORE, FlowOverlay::NONE, M::ADVENTURE);
    const Ctx menu0 = mk(FlowState::MENU, FlowMenuPage::MODE_SELECT, 0, FlowSession::EXPLORE, FlowOverlay::NONE, M::ADVENTURE);
    switch (c.st) {
    case FlowState::BOOT:
        if (ev == E::BootComplete) return title;
        return std::nullopt;
    case FlowState::TITLE:
        if (ev == E::Start) return menu0;
        if (ev == E::IdleTimeout) return mk(FlowState::ATTRACT, FlowMenuPage::MODE_SELECT, 0, FlowSession::EXPLORE, FlowOverlay::NONE, M::ATTRACT_DEMO);
        return std::nullopt;
    case FlowState::ATTRACT:
        if (ev == E::AnyKey) return title;
        return std::nullopt;
    case FlowState::MENU:
        if (c.page == FlowMenuPage::MODE_SELECT) {
            if (ev == E::NavUp) return c.cur > 0 ? std::optional(mk(FlowState::MENU, FlowMenuPage::MODE_SELECT, c.cur - 1, FlowSession::EXPLORE, FlowOverlay::NONE, M::ADVENTURE)) : std::nullopt;
            if (ev == E::NavDown) return c.cur < 4 ? std::optional(mk(FlowState::MENU, FlowMenuPage::MODE_SELECT, c.cur + 1, FlowSession::EXPLORE, FlowOverlay::NONE, M::ADVENTURE)) : std::nullopt;
            if (ev == E::Cancel) return title;
            if (ev == E::Confirm) {
                switch (c.cur) {
                case 0: return mk(FlowState::MENU, FlowMenuPage::ADVENTURE_SAVE, 0, FlowSession::EXPLORE, FlowOverlay::NONE, M::ADVENTURE);
                case 1: return mk(FlowState::PLAYING, FlowMenuPage::MODE_SELECT, 0, FlowSession::RACE, FlowOverlay::NONE, M::QUICK_RACE);
                case 2: return mk(FlowState::PLAYING, FlowMenuPage::MODE_SELECT, 0, FlowSession::RACE, FlowOverlay::NONE, M::TWO_PLAYER);
                case 3: return mk(FlowState::MENU, FlowMenuPage::OPTIONS, 3, FlowSession::EXPLORE, FlowOverlay::NONE, M::ADVENTURE);
                case 4: return mk(FlowState::MENU, FlowMenuPage::RESULTS_VIEW, 4, FlowSession::EXPLORE, FlowOverlay::NONE, M::ADVENTURE);
                default: return std::nullopt;
                }
            }
            return std::nullopt;
        }
        if (c.page == FlowMenuPage::ADVENTURE_SAVE) {
            if (ev == E::Confirm) return mk(FlowState::PLAYING, FlowMenuPage::MODE_SELECT, 0, FlowSession::EXPLORE, FlowOverlay::FACTORY_MENU, M::ADVENTURE);
            if (ev == E::Cancel) return menu0;
            return std::nullopt;
        }
        // OPTIONS and RESULTS_VIEW return to the list with the cursor kept.
        if (ev == E::Cancel) return mk(FlowState::MENU, FlowMenuPage::MODE_SELECT, c.cur, FlowSession::EXPLORE, FlowOverlay::NONE, M::ADVENTURE);
        return std::nullopt;
    case FlowState::PLAYING: {
        if (ev == E::Escape) return menu0;
        if (c.sess == FlowSession::RACE) {
            if (ev == E::RaceFinished) return mk(FlowState::PLAYING, FlowMenuPage::MODE_SELECT, 0, FlowSession::RESULTS, FlowOverlay::NONE, static_cast<M>(c.mode));
            return std::nullopt;
        }
        if (c.sess == FlowSession::RESULTS) {
            if (ev == E::ResultsConfirm) return mk(FlowState::PLAYING, FlowMenuPage::MODE_SELECT, 0, FlowSession::EXPLORE, FlowOverlay::NONE, M::ADVENTURE);
            return std::nullopt;
        }
        const auto explore = [&](FlowOverlay ov, std::uint8_t cur) {
            return mk(FlowState::PLAYING, FlowMenuPage::MODE_SELECT, cur, FlowSession::EXPLORE, ov, M::ADVENTURE);
        };
        switch (c.ov) {
        case FlowOverlay::NONE:
            if (ev == E::StartRace) return mk(FlowState::PLAYING, FlowMenuPage::MODE_SELECT, 0, FlowSession::RACE, FlowOverlay::NONE, M::ADVENTURE);
            if (ev == E::OpenDialogue) return explore(FlowOverlay::DIALOGUE, 0);
            if (ev == E::OpenShop) return explore(FlowOverlay::SHOP, 0);
            return std::nullopt;
        case FlowOverlay::DIALOGUE:
            if (ev == E::DialogueDone) return explore(FlowOverlay::NONE, 0);
            return std::nullopt;
        case FlowOverlay::SHOP:
            if (ev == E::CloseShop) return explore(FlowOverlay::NONE, 0);
            return std::nullopt;
        case FlowOverlay::FACTORY_MENU:
            if (ev == E::NavUp) return c.cur > 0 ? std::optional(explore(FlowOverlay::FACTORY_MENU, c.cur - 1)) : std::nullopt;
            if (ev == E::NavDown) return c.cur < 4 ? std::optional(explore(FlowOverlay::FACTORY_MENU, c.cur + 1)) : std::nullopt;
            if (ev == E::Confirm && c.cur == 4) return explore(FlowOverlay::FACTORY_EXIT_DIALOGUE, 0);
            return std::nullopt;
        case FlowOverlay::FACTORY_EXIT_DIALOGUE:
            if (ev == E::DialogueDone) return explore(FlowOverlay::NONE, 0);
            return std::nullopt;
        }
        return std::nullopt;
    }
    }
    return std::nullopt;
}

struct Suite {
    int goldenPass = 0;
    int goldenTotal = 9;
    int edgesOk = 0;
    int edgesTotal = 0;
    int reachable = 0;
    int specReachable = 0;
    int illegalAccepted = 0;
    int fuzzMismatch = 0;
    std::uint64_t fuzzHash = 0;
    std::uint64_t fuzzHashReplay = 0;
    std::uint64_t goldenHash = 0;
    std::uint64_t edgeHash = 0;
    int elfChecks = 0;
    int elfTotal = 0;
    int wordEffects = 0;
    int wordEffectsTotal = 0;
};

struct GStep {
    std::optional<FlowEvent> ev;
    std::uint32_t ticks;
};

void play(AppFlow& flow, const std::vector<GStep>& steps) {
    for (const auto& s : steps) {
        flow.step(s.ev);
        for (std::uint32_t i = 0; i < s.ticks; ++i) flow.step();
    }
}

using Check = bool (*)(const FlowWords&);

bool isTitleMenuGame0(const FlowWords& w) { return w.state == FlowState::TITLE && w.gameMode == 0; }
bool isMainMenu(const FlowWords& w) { return w.state == FlowState::MENU && w.page == FlowMenuPage::MODE_SELECT && w.gameMode == 0; }
bool isOptions(const FlowWords& w) { return w.state == FlowState::MENU && w.page == FlowMenuPage::OPTIONS && w.gameMode == 0; }
bool isAttract(const FlowWords& w) {
    return w.state == FlowState::ATTRACT && w.gameMode == 8 && w.scene3dActive == 2 &&
           w.scene3dHandler == 0x0021CE48u && w.driveFlags == 0x10104u && w.progressScene == 0;
}
bool isQuickRaceHud(const FlowWords& w) {
    return w.state == FlowState::PLAYING && w.session == FlowSession::RACE && w.gameMode == 1 &&
           w.scene3dActive == 2 && w.scene3dHandler == 0x0021CE48u && w.driveFlags == 0x124u && w.progressScene == 0;
}
bool isQuickRaceDriving(const FlowWords& w) {
    return w.state == FlowState::PLAYING && w.session == FlowSession::RACE && w.gameMode == 1 &&
           w.scene3dHandler == 0x0021CE48u && w.progressScene == 0;
}
bool isFactory(const FlowWords& w) {
    return w.state == FlowState::PLAYING && w.overlay == FlowOverlay::FACTORY_MENU && w.gameMode == 0 &&
           w.progressScene == 6 && w.location == 1;
}
bool isTownDriving(const FlowWords& w) {
    return w.state == FlowState::PLAYING && w.session == FlowSession::EXPLORE && w.overlay == FlowOverlay::NONE &&
           w.gameMode == 0 && w.scene3dHandler == 0x0021CF80u && w.driveFlags == 0x4u && w.progressScene == 2 &&
           w.location == 1;
}

struct Golden {
    const char* name;
    std::vector<GStep> steps;
    Check check;
};

std::vector<Golden> goldens() {
    const auto toFactory = std::vector<GStep>{
        {FlowEvent::BootComplete, 720},
        {FlowEvent::Start, 180},
        {FlowEvent::Confirm, 0},
        {FlowEvent::Confirm, 0},
    };
    auto toTown = toFactory;
    for (int i = 0; i < 4; ++i) toTown.push_back({FlowEvent::NavDown, 0});
    toTown.push_back({FlowEvent::Confirm, 0});
    toTown.push_back({FlowEvent::DialogueDone, 720});
    auto townStreet = toTown;
    townStreet.push_back({std::nullopt, 300});
    auto raceStart = std::vector<GStep>{
        {FlowEvent::BootComplete, 720},
        {FlowEvent::Start, 180},
        {FlowEvent::NavDown, 0},
        {FlowEvent::Confirm, 2447},
    };
    auto raceDriving = raceStart;
    raceDriving.push_back({std::nullopt, 731});
    return {
        {"title", {{FlowEvent::BootComplete, 720}}, isTitleMenuGame0},
        {"main_menu", {{FlowEvent::BootComplete, 720}, {FlowEvent::Start, 180}}, isMainMenu},
        {"options",
         {{FlowEvent::BootComplete, 720}, {FlowEvent::Start, 180}, {FlowEvent::NavDown, 0}, {FlowEvent::NavDown, 0},
          {FlowEvent::NavDown, 0}, {FlowEvent::Confirm, 120}},
         isOptions},
        {"attract", {{FlowEvent::BootComplete, 0}, {std::nullopt, 2700}}, isAttract},
        {"race_hud", raceStart, isQuickRaceHud},
        {"race_hud_driving", raceDriving, isQuickRaceDriving},
        {"factory_menu", toFactory, isFactory},
        {"town_driving", toTown, isTownDriving},
        {"town_driving_street", townStreet, isTownDriving},
    };
}

// Each check asserts only the words the oracle documents for that frame
// (oracle-golden-frames.md, Frames table). Title/menu frames record game_mode
// only, so they assert nothing else.
int runGoldens(std::uint64_t& hash) {
    int pass = 0;
    int idx = 0;
    for (const auto& g : goldens()) {
        AppFlow flow;
        play(flow, g.steps);
        const bool ok = g.check(flow.current());
        if (ok) ++pass;
        hash ^= rt::native::gameplay::hashFlowWords(flow.current()) + static_cast<std::uint64_t>(idx) * 0x9E3779B97F4A7C15ull;
        ++idx;
    }
    return pass;
}

void runEdges(Suite& s) {
    std::map<Ctx, AppFlow> seen;
    std::set<Ctx> specSeen;
    std::vector<Ctx> queue;
    AppFlow start;
    seen.emplace(ctxOf(start.current()), start);
    queue.push_back(ctxOf(start.current()));
    std::vector<Ctx> specQueue{ctxOf(start.current())};
    specSeen.insert(specQueue.front());
    while (!specQueue.empty()) {
        const Ctx c = specQueue.back();
        specQueue.pop_back();
        for (std::uint8_t e = 0; e < rt::native::gameplay::kFlowEventCount; ++e) {
            if (auto n = expectedNext(c, static_cast<FlowEvent>(e)); n && specSeen.insert(*n).second) specQueue.push_back(*n);
        }
    }
    s.specReachable = static_cast<int>(specSeen.size());
    for (std::size_t qi = 0; qi < queue.size(); ++qi) {
        const Ctx c = queue[qi];
        const AppFlow rep = seen.at(c);
        for (std::uint8_t e = 0; e < rt::native::gameplay::kFlowEventCount; ++e) {
            const auto ev = static_cast<FlowEvent>(e);
            const auto want = expectedNext(c, ev);
            AppFlow copy = rep;
            const auto got = copy.apply(ev);
            const Ctx after = ctxOf(copy.current());
            if (want) {
                ++s.edgesTotal;
                if (got.accepted && after == *want) ++s.edgesOk;
            } else if (got.accepted) {
                ++s.illegalAccepted;
            }
            if (got.accepted && !seen.contains(after)) {
                seen.emplace(after, copy);
                queue.push_back(after);
            }
            s.edgeHash = (s.edgeHash ^ (static_cast<std::uint64_t>(e) << 8 | (got.accepted ? 1u : 0u))) * 0x100000001b3ull;
        }
    }
    s.reachable = static_cast<int>(seen.size());
}

// Word-level effects the oracle does not sample directly: progress writes per
// session, flag bits, the exact title timeout and the view snapshot rule.
int runWordEffects(int& total) {
    int pass = 0;
    total = 0;
    const auto expect = [&pass, &total](bool ok) {
        ++total;
        if (ok) ++pass;
    };
    {
        AppFlow f = AppFlow::directLaunch(DirectLaunch::PLAYING_EXPLORE);
        expect(f.current().progressScene == 2 && f.current().scene3dHandler == 0x0021CF80u);
        f.apply(FlowEvent::StartRace);
        expect(f.current().progressScene == 10 && f.current().scene3dHandler == 0x0021CE48u &&
               f.current().driveFlags == 0x124u);
        f.apply(FlowEvent::RaceFinished);
        expect(f.current().progressScene == 3 && f.current().session == FlowSession::RESULTS &&
               (f.current().driveFlags & 0x20u) == 0 && (f.current().driveFlags & 0x08u) != 0);
        f.apply(FlowEvent::ResultsConfirm);
        expect(f.current().progressScene == 2 && f.current().driveFlags == 0x4u &&
               f.current().session == FlowSession::EXPLORE);
    }
    {
        AppFlow f;
        f.apply(FlowEvent::BootComplete);
        f.apply(FlowEvent::Start);
        f.apply(FlowEvent::NavDown);
        f.apply(FlowEvent::Confirm);
        expect(f.current().gameMode == 1 && f.current().progressScene == 0);
        f.apply(FlowEvent::RaceFinished);
        expect(f.current().progressScene == 0 && f.current().session == FlowSession::RESULTS);
        f.apply(FlowEvent::Escape);
        expect(f.current().state == FlowState::MENU && f.current().gameMode == 0 &&
               f.current().scene3dHandler == 0 && f.current().driveFlags == 0);
    }
    {
        AppFlow f;
        f.apply(FlowEvent::BootComplete);
        for (std::uint32_t i = 0; i < 2699; ++i) f.step();
        expect(f.current().state == FlowState::TITLE);
        f.step();
        expect(f.current().state == FlowState::ATTRACT && f.current().gameMode == 8);
    }
    {
        AppFlow f;
        f.apply(FlowEvent::BootComplete);
        for (std::uint32_t i = 0; i < 2000; ++i) f.step();
        f.step(FlowEvent::Start);
        for (std::uint32_t i = 0; i < 2000; ++i) f.step();
        expect(f.current().state == FlowState::MENU);
    }
    {
        AppFlow f = AppFlow::directLaunch(DirectLaunch::TITLE);
        f.step(FlowEvent::Start);
        const auto view = f.view(0.25f);
        expect(view.presented().state == FlowState::TITLE && view.current.state == FlowState::MENU);
        expect(f.view(0.75f).presented().state == FlowState::MENU);
    }
    expect(AppFlow::directLaunchForState("MENU", "") == DirectLaunch::MENU_MODE_SELECT);
    expect(AppFlow::directLaunchForState("PLAYING", "explore") == DirectLaunch::PLAYING_EXPLORE);
    expect(AppFlow::directLaunchForState("PLAYING", "race") == DirectLaunch::PLAYING_RACE);
    expect(AppFlow::directLaunchForState("bogus", "") == DirectLaunch::TITLE);
    return pass;
}

struct Rng {
    std::uint64_t state;
    std::uint64_t next() {
        std::uint64_t z = (state += 0x9E3779B97F4A7C15ull);
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
        return z ^ (z >> 31);
    }
};

// 5000 seeded steps, each an optional event then one 60 Hz tick. Every step is
// checked against the spec table, including the automatic title timeout.
void runFuzz(std::uint64_t& hashOut, int& mismatch, int& illegal) {
    Rng rng{0x5EED0202ull};
    AppFlow flow;
    std::uint64_t h = 0xcbf29ce484222325ull;
    for (int step = 0; step < 5000; ++step) {
        const std::uint64_t r = rng.next();
        std::optional<FlowEvent> ev;
        if ((r & 7u) != 0) ev = static_cast<FlowEvent>((r >> 3) % rt::native::gameplay::kFlowEventCount);
        const Ctx before = ctxOf(flow.current());
        const auto out = flow.step(ev);
        std::optional<Ctx> want = before;
        if (ev) {
            const auto exp = expectedNext(before, *ev);
            if (out.input.accepted && !exp) ++illegal;
            if (out.input.accepted != exp.has_value()) ++mismatch;
            if (exp) want = exp;
        }
        if (out.attractTimeout) {
            const auto exp = expectedNext(*want, FlowEvent::IdleTimeout);
            if (!exp) ++mismatch;
            else want = exp;
        } else if (want->st == FlowState::TITLE && flow.current().idleTicks >= rt::native::gameplay::kFlowTitleIdleTicks) {
            ++mismatch;
        }
        if (ctxOf(flow.current()) != *want) ++mismatch;
        h = (h ^ rt::native::gameplay::hashFlowWords(flow.current())) * 0x100000001b3ull;
        h = (h ^ (ev ? static_cast<std::uint64_t>(*ev) + 1 : 0u)) * 0x100000001b3ull;
    }
    hashOut = h;
}

std::vector<std::uint8_t> readFile(const std::string& path, bool& ok) {
    std::ifstream in(path, std::ios::binary);
    ok = static_cast<bool>(in);
    if (!ok) return {};
    return std::vector<std::uint8_t>(std::istreambuf_iterator<char>(in), {});
}

// Reads one little-endian word at a virtual address. Out of range clears ok.
std::uint32_t wordAt(const std::vector<std::uint8_t>& elf, std::uint32_t va, bool& ok) {
    const std::uint32_t off = va - kElfBase;
    if (va < kElfBase || off + 4 > elf.size()) {
        ok = false;
        return 0;
    }
    return static_cast<std::uint32_t>(elf[off]) | static_cast<std::uint32_t>(elf[off + 1]) << 8 |
           static_cast<std::uint32_t>(elf[off + 2]) << 16 | static_cast<std::uint32_t>(elf[off + 3]) << 24;
}

// True when a jal to target sits in [lo, hi). When requiredDelay is set, the
// delay-slot word must equal it.
bool hasJal(const std::vector<std::uint8_t>& elf, std::uint32_t lo, std::uint32_t hi, std::uint32_t target,
            std::optional<std::uint32_t> requiredDelay) {
    const std::uint32_t jal = 0x0C000000u | ((target >> 2) & 0x03FFFFFFu);
    bool ok = true;
    for (std::uint32_t va = lo; va < hi; va += 4) {
        if (wordAt(elf, va, ok) != jal) continue;
        if (!requiredDelay || wordAt(elf, va + 4, ok) == *requiredDelay) return ok;
    }
    return false;
}

// Three dispatch checks: the 7-word handler selector, the race call that
// passes a0=1, and the town call. Returns the number that pass (of 3).
int runElf(const std::vector<std::uint8_t>& elf, Suite& s) {
    s.elfTotal = 3;
    bool ok = true;
    bool dispatch = true;
    for (int i = 0; i < 7; ++i) {
        dispatch = dispatch && wordAt(elf, kSelectScene3dHandler + 4u * static_cast<std::uint32_t>(i), ok) == kDispatchWords[i];
    }
    int pass = (dispatch && ok) ? 1 : 0;
    // StartRaceModeTask (00210..) calls SelectScene3dHandler(1): addiu a0,zero,1 in the delay slot.
    if (hasJal(elf, kStartRaceModeTask, kStartRaceModeEnd, kSelectScene3dHandler, 0x24040001u)) ++pass;
    // StartTownModeTask calls SelectScene3dHandler(0) (decomp 00210430.c:15).
    if (hasJal(elf, kStartTownModeTask, kStartTownModeEnd, kSelectScene3dHandler, std::nullopt)) ++pass;
    s.elfChecks = pass;
    return pass;
}

Suite runSuite(const std::vector<std::uint8_t>& elf) {
    Suite s;
    s.goldenPass = runGoldens(s.goldenHash);
    runEdges(s);
    int mismatch = 0, illegal = 0;
    runFuzz(s.fuzzHash, mismatch, illegal);
    s.fuzzMismatch = mismatch;
    s.illegalAccepted += illegal;
    std::uint64_t replay = 0;
    int m2 = 0, i2 = 0;
    runFuzz(replay, m2, i2);
    s.fuzzHashReplay = replay;
    s.fuzzMismatch += m2;
    s.illegalAccepted += i2;
    s.wordEffects = runWordEffects(s.wordEffectsTotal);
    runElf(elf, s);
    return s;
}

std::uint64_t digest(const Suite& s) {
    std::uint64_t h = 0xcbf29ce484222325ull;
    const std::uint64_t vals[] = {static_cast<std::uint64_t>(s.goldenPass), static_cast<std::uint64_t>(s.edgesOk),
                                  static_cast<std::uint64_t>(s.edgesTotal), static_cast<std::uint64_t>(s.reachable),
                                  static_cast<std::uint64_t>(s.illegalAccepted), static_cast<std::uint64_t>(s.fuzzMismatch),
                                  s.fuzzHash, s.fuzzHashReplay, s.goldenHash, s.edgeHash,
                                  static_cast<std::uint64_t>(s.elfChecks), static_cast<std::uint64_t>(s.wordEffects)};
    for (std::uint64_t v : vals) {
        for (int i = 0; i < 8; ++i) {
            h ^= static_cast<std::uint8_t>(v >> (8 * i));
            h *= 0x100000001b3ull;
        }
    }
    return h;
}

}  // namespace

int main(int argc, char** argv) {
    std::string elfPath;
    if (const char* env = std::getenv("RT_ELF")) elfPath = env;
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::string(argv[i]) == "--elf") elfPath = argv[i + 1];
    }
    if (elfPath.empty()) {
        if (const char* home = std::getenv("HOME")) {
            elfPath = std::string(home) + "/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98";
        }
    }
    bool elfOk = false;
    const auto elf = readFile(elfPath, elfOk);
    if (!elfOk) {
        std::printf("W2-02 NO_ELF path=%s\n", elfPath.c_str());
        return 2;
    }

    const Suite first = runSuite(elf);
    const Suite second = runSuite(elf);
    const bool deterministic = digest(first) == digest(second);

    const bool goldenOk = first.goldenPass == first.goldenTotal;
    const bool edgesOk = first.edgesOk == first.edgesTotal && first.edgesTotal > 0 &&
                         first.reachable == first.specReachable;
    const bool illegalOk = first.illegalAccepted == 0;
    const bool replayOk = first.fuzzHash == first.fuzzHashReplay && first.fuzzMismatch == 0;
    const bool elfOkAll = first.elfChecks == first.elfTotal;
    const bool effectsOk = first.wordEffects == first.wordEffectsTotal;
    const bool pass = goldenOk && edgesOk && illegalOk && replayOk && elfOkAll && effectsOk && deterministic;

    std::printf("W2-02 SCORE golden_words=%d/%d edges=%d/%d illegal_accepted=%d replay_hash_equal=%d "
                "deterministic=%d reachable=%d word_effects=%d/%d elf_dispatch=%d/%d %s\n",
                first.goldenPass, first.goldenTotal, first.edgesOk, first.edgesTotal, first.illegalAccepted,
                replayOk ? 1 : 0, deterministic ? 1 : 0, first.reachable, first.wordEffects, first.wordEffectsTotal,
                first.elfChecks, first.elfTotal, pass ? "PASS" : "FAIL");
    return pass ? 0 : 1;
}
