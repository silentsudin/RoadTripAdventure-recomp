// W14: second-screen page model + builder (src/native/preview/second_screen.*).
// `w14_second_screen_test --write-save <dir>` writes a rich Adventure slot 0 for the
// headless shots (RT_SAVE_DIR=<dir> RT_APP_LOAD_SLOT=0 RT_APP_MENU_SCREEN=second-*).
#include "gameplay/save_slots.h"
#include "preview/second_screen.h"

#include <cassert>
#include <cstdio>
#include <cstring>

using namespace rt::native;
using namespace rt::native::preview;
using gameplay::SecondScreenPage;

static bool anyTextWithin(const UiScreen& s, int w, int h) {
    if (s.text.empty() || s.quads.empty()) return false;
    for (const auto& q : s.text) if (q.x < 0 || q.y < 0 || q.x + q.width > w + 1 || q.y + q.height > h + 1) return false;
    return true;
}

int main(int argc, char** argv) {
    gameplay::SaveData save = gameplay::SlotStore::fresh();
    save.playerName = "PLAYER 1";
    save.currencyName = "Coins";
    save.money = 123456;
    save.mileageM = 48280;  // 30 miles
    save.licence = 2;
    save.stampsEarned = {0x7ull, 0};
    save.coinsMissing.fill(0);
    for (int i = 0; i < 11; ++i) save.coinsMissing[static_cast<std::size_t>(i)] = 0xFF;
    save.coinsMissing[11] = 0x0F;  // 92 missing: 8 found
    save.photosTaken[0] = 0x0F;
    save.townsUnvisited = 0x3FC;  // two visited
    save.location = 1;
    save.unvisitedDoors[1] = 0x35;
    save.radioStation = 1;
    if (argc == 3 && std::strcmp(argv[1], "--write-save") == 0) {
        std::string err;
        const bool ok = gameplay::SlotStore(argv[2]).save(0, save, &err);
        std::printf("write-save: %s %s\n", ok ? "ok" : "FAILED", err.c_str());
        return ok ? 0 : 1;
    }

    // Race formatting (SecondScreen.cpp:187-198 semantics).
    bool nb = true;
    assert(secondScreenRaceTime(0x7FFFFFFFu) == "--'--\"--");
    assert(secondScreenRaceTime(3600 + 60 * 5 + 30) == "1'05\"50");
    assert(secondScreenLapNote(100, 160, &nb) == "New best -1\"00" && nb);
    assert(secondScreenLapNote(190, 160, &nb) == "+0\"50" && !nb);
    assert(secondScreenLapNote(100, 0, &nb).empty());

    // Lap tracker: lap time = elapsed delta, best-before-last = the best until then, flash 2.5 s.
    SecondScreenLapTracker laps;
    laps.note(0, 50, 0, 0);
    laps.note(1, 1000, 1000, 1);
    assert(laps.last() == 1000 && laps.bestBeforeLast() == 0 && !laps.newBestFlash(1.0 + 3.0));
    laps.note(2, 1900, 900, 2);
    assert(laps.last() == 900 && laps.bestBeforeLast() == 1000 && laps.newBestFlash(3.0) && !laps.newBestFlash(5.0));

    // Stamp watch: one new bit announces; a bulk change (a loaded save) does not.
    SecondScreenStampWatch watch;
    std::bitset<100> s;
    watch.observe(s, 0);
    s.set(6);
    watch.observe(s, 1);
    assert(watch.recent(2) == 7 && watch.recent(5) == 0);
    s.set(1), s.set(2);
    watch.observe(s, 6);
    assert(watch.recent(7) == 7 || watch.recent(7) == 0);  // unchanged: the old one has aged out

    // 30 Hz throttle: a touch draws at once.
    SecondScreenThrottle t;
    assert(t.due(1.0, false) && !t.due(1.01, false) && t.due(1.01, true) && t.due(1.05, false));

    // Pages: every page builds text inside the window at two sizes; the notebook needs an Adventure.
    SecondScreenData data;
    SecondScreenData rich;
    fillSecondScreenFromSave(rich, save);
    assert(rich.coins == 8 && rich.photos == 4 && rich.townsVisited == 2 && rich.housesLeft == 4 && rich.stamps.count() == 3);
    for (auto [w, h] : {std::pair{1920, 1080}, std::pair{800, 600}})
        for (auto page : {SecondScreenPage::Map, SecondScreenPage::Notebook, SecondScreenPage::Radio}) {
            assert(anyTextWithin(buildSecondScreen(w, h, page, data), w, h));
            assert(anyTextWithin(buildSecondScreen(w, h, page, rich), w, h));
        }
    // Menu pill is 168*S wide on the 1080p design height.
    const UiScreen shot = buildSecondScreen(1920, 1080, SecondScreenPage::Map, data);
    bool pill = false;
    for (const auto& q : shot.quads) pill |= std::abs(q.width - 168.f) < 0.5f && std::abs(q.x + q.width - (1920 - 24 - 24)) < 0.5f;
    assert(pill);
    // Race column renders its stats from the data.
    SecondScreenData race = rich;
    race.racing = true, race.lap = 2, race.laps = 3, race.lastLapFrames = 3000, race.bestBeforeLast = 3100, race.bestLapFrames = 3000;
    assert(buildSecondScreen(1920, 1080, SecondScreenPage::Map, race).text.size() > shot.text.size());
    // Page selection: Map when racing, tap override sticks until the context changes.
    gameplay::SecondScreenPages pages;
    pages.tick({true, false}, std::nullopt);
    assert(pages.page() == SecondScreenPage::Map);
    pages.tick({true, false}, SecondScreenPage::Radio);
    assert(pages.page() == SecondScreenPage::Radio);
    pages.tick({false, false}, std::nullopt);
    pages.tick({true, false}, std::nullopt);
    assert(pages.page() == SecondScreenPage::Map);
    std::printf("w14_second_screen_test: all checks passed\n");
    return 0;
}
