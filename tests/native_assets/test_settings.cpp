// Contract tests for the native persisted-settings + options-menu logic
// (src/native/gameplay/settings.{h,cpp}). Deterministic: a temp-dir file for the
// store, an injected MenuContext for availability, and scripted button
// sequences for the navigator. No SDL, no GPU, no game data.
//
// REFERENCE PRECEDENT (CLAUDE.md:87): the sibling recomp app's menu hides rows a
// device can't use; these pins cover the same rules adapted to the native
// catalog (settings.h documents the mapping).
#include "../../src/native/gameplay/settings.h"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <unistd.h>

using namespace rt::native::gameplay;

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

namespace {

std::filesystem::path tempDir() {
    static int counter = 0;
    std::filesystem::path dir = std::filesystem::temp_directory_path() /
        ("rt_settings_test_" + std::to_string(::getpid()) + "_" + std::to_string(counter++));
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);
    return dir;
}

std::string readAll(const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary);
    return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

}  // namespace

// --------------------------------------------------------------- store

static void testStoreRoundTripPreservesUnknownKeys() {
    const std::filesystem::path dir = tempDir();
    const std::filesystem::path path = defaultSettingsPath(dir);
    CHECK(path == dir / "settings.toml");

    {
        std::ofstream out(path, std::ios::trunc);
        out << "# a hand-written comment\n"
            << "\n"
            << "graphics.terrain = on\n"
            << "future.unknown_option = 42\n"
            << "controllers.rumble = off\n";
    }

    SettingsStore store(path);
    CHECK(store.load());
    CHECK(store.skippedLines() == 0);
    CHECK(store.entryCount() == 3u);
    CHECK(store.getBool("graphics.terrain", false));
    CHECK(store.getInt("future.unknown_option", 0, 0, 1000) == 42);

    // Mutate a known key and add one; the unknown key plus the comment/blank
    // line survive the save byte-for-byte.
    store.setBool("controllers.rumble", true);
    store.setInt("graphics.stream_row", 3);
    CHECK(store.save());

    SettingsStore reloaded(path);
    CHECK(reloaded.load());
    CHECK(reloaded.getBool("graphics.terrain", false));
    CHECK(reloaded.getBool("controllers.rumble", false));
    CHECK(reloaded.getInt("graphics.stream_row", 0, 0, 7) == 3);
    CHECK(reloaded.getInt("future.unknown_option", 0, 0, 1000) == 42);

    const std::string text = readAll(path);
    CHECK(text.find("# a hand-written comment") != std::string::npos);
    CHECK(text.find("future.unknown_option = 42") != std::string::npos);

    std::filesystem::remove_all(dir);
}

static void testStoreSkipsCorruptLinesAndKeepsValidKeys() {
    const std::filesystem::path dir = tempDir();
    const std::filesystem::path path = dir / "settings.toml";
    {
        std::ofstream out(path, std::ios::trunc);
        out << "good.key = 1\n"
            << "this line has no equals sign\n"
            << "  = 5\n"                       // empty key -> corrupt
            << "# comment = not an entry\n"    // comment, not corrupt
            << "another.good = hello\n";
    }
    SettingsStore store(path);
    CHECK(store.load());
    CHECK(store.skippedLines() == 2);
    CHECK(store.getInt("good.key", 0, 0, 9) == 1);
    CHECK(store.getString("another.good", "") == "hello");
    CHECK(!store.contains("this line has no equals sign"));

    // Corrupt lines are preserved on save (a re-save never destroys a file).
    store.setInt("good.key", 7);
    CHECK(store.save());
    const std::string text = readAll(path);
    CHECK(text.find("this line has no equals sign") != std::string::npos);
    CHECK(text.find("good.key = 7") != std::string::npos);

    std::filesystem::remove_all(dir);
}

static void testStoreTypedClampsAndFallbacks() {
    const std::filesystem::path dir = tempDir();
    const std::filesystem::path path = dir / "settings.toml";
    {
        std::ofstream out(path, std::ios::trunc);
        out << "i = 999\n"          // clamps to hi
            << "j = -5\n"           // clamps to lo
            << "f = 3.5\n"          // clamps to hi
            << "b = yes\n"
            << "bad_int = twelve\n" // falls back
            << "bad_bool = maybe\n" // falls back
            << "e = PEACH_FM\n";    // case-insensitive enum match
    }
    SettingsStore store(path);
    CHECK(store.load());

    CHECK(store.getInt("i", 0, 0, 100) == 100);
    CHECK(store.getInt("j", 0, 0, 100) == 0);
    CHECK(store.getFloat("f", 0.f, 0.f, 1.f) == 1.f);
    CHECK(store.getBool("b", false));
    CHECK(store.getInt("bad_int", 42, 0, 100) == 42);
    CHECK(store.getBool("bad_bool", true));
    CHECK(store.getEnum("e", "off", {"off", "peach_fm", "e_radio"}) == "peach_fm");
    CHECK(store.getEnum("missing", "off", {"off"}) == "off");
    CHECK(store.getEnum("e", "off", {"off", "peach_fm"}) == "peach_fm");

    // formatFloat is canonical text a getter reads back.
    store.setFloat("f", 0.25f);
    CHECK(store.getFloat("f", 0.f, 0.f, 1.f) == 0.25f);

    std::filesystem::remove_all(dir);
}

static void testStoreOneWriterAndEmptyPath() {
    SettingsStore empty;
    CHECK(!empty.load());
    CHECK(!empty.save());
    // Unknown-key erase and duplicate collapse.
    SettingsStore store{std::filesystem::path{}};
    store.setInt("k", 1);
    store.setInt("k", 2);
    CHECK(store.entryCount() == 1u);
    CHECK(store.getInt("k", 0, 0, 9) == 2);
    CHECK(store.erase("k"));
    CHECK(store.entryCount() == 0u);
    CHECK(!store.erase("k"));
}

// --------------------------------------------------------------- catalog

static void testCatalogMapsEveryEnumeratedEnvFlag() {
    OptionsModel model;
    const std::vector<std::string> mustCover = {
        "RT_APP_BGM", "RT_APP_BGM_TSQ", "RT_APP_CARMODEL", "RT_APP_CARMODEL_DUMP",
        "RT_APP_DEBUG_OVERLAY", "RT_APP_FRAMES", "RT_APP_HUDTEXT", "RT_APP_INTERP",
        "RT_APP_INTERP_CAP", "RT_APP_INTERP_RATE", "RT_APP_NAV", "RT_APP_NAV_TOWN",
        "RT_APP_NOAUDIO", "RT_APP_NORENDER", "RT_APP_ROUTE", "RT_APP_ROUTE_TOWN",
        "RT_APP_SCOREHUD", "RT_APP_SNAPSHOT", "RT_APP_STATE", "RT_APP_STREAM",
        "RT_APP_STREAM_POS", "RT_APP_STREAM_ROW", "RT_APP_STREAM_STEP_FRAMES",
        "RT_APP_TERRAIN", "RT_APP_TERRAIN_FILE", "RT_APP_TEXTURES", "RT_DISC",
        "RT_TEXTURE_PACK", "RT_APP_TILES_RADIUS",
    };
    for (const std::string& flag : mustCover) {
        bool found = false;
        for (const OptionSpec& spec : model.options())
            for (const std::string& f : spec.envFlags)
                if (f == flag) { found = true; break; }
        CHECK(found);
    }
    // Every option id is unique.
    for (std::size_t i = 0; i < model.options().size(); ++i)
        for (std::size_t j = i + 1; j < model.options().size(); ++j)
            CHECK(model.options()[i].id != model.options()[j].id);
}

// The Texture pack row lists what is installed: "Off" first, then every
// folder under <data>/textures/packs in sorted order, from the data root the
// app resolves (RT_DATA_DIR). A pack dropped in shows up on the next build.
static void testTexturePackRowListsFolders() {
    const std::filesystem::path dir = tempDir();
    const char* saved = std::getenv("RT_DATA_DIR");
    const std::string keep = saved ? saved : "";
    std::filesystem::create_directories(dir / "textures" / "packs" / "zeta");
    std::filesystem::create_directories(dir / "textures" / "packs" / "alpha");
    std::filesystem::create_directories(dir / "textures" / "packs" / "Off");  // reserved value
    std::filesystem::create_directories(dir / "textures" / "dumps");  // not a pack
    ::setenv("RT_DATA_DIR", dir.string().c_str(), 1);
    OptionsModel model;
    const OptionSpec* pack = model.find("graphics.texture_pack");
    CHECK(pack != nullptr);
    if (pack) {
        CHECK(pack->type == OptionType::Enum);
        CHECK(pack->domain.size() == 3);
        CHECK(!pack->domain.empty() && pack->domain[0] == "Off");
        CHECK(pack->domain.size() > 2 && pack->domain[1] == "alpha" && pack->domain[2] == "zeta");
        CHECK(pack->defaultRaw == "Off");
        // A folder literally named "Off" is not offered a second time.
        CHECK(std::count(pack->domain.begin(), pack->domain.end(), std::string("Off")) == 1);
    }
    if (saved) ::setenv("RT_DATA_DIR", keep.c_str(), 1); else ::unsetenv("RT_DATA_DIR");
    std::filesystem::remove_all(dir);

    // With no data root at all (neither RT_DATA_DIR nor HOME) the row still
    // lists only "Off": nothing is scanned relative to the working directory.
    const char* savedHome = std::getenv("HOME");
    const std::string keepHome = savedHome ? savedHome : "";
    ::unsetenv("RT_DATA_DIR");
    ::unsetenv("HOME");
    OptionsModel bare;
    const OptionSpec* barePack = bare.find("graphics.texture_pack");
    CHECK(barePack != nullptr);
    if (barePack) CHECK(barePack->domain.size() == 1 && barePack->domain[0] == "Off");
    if (savedHome) ::setenv("HOME", keepHome.c_str(), 1);
    if (saved) ::setenv("RT_DATA_DIR", keep.c_str(), 1);
}

static void testAvailabilityGamepadHiddenTriggers() {
    OptionsModel model;
    const OptionSpec* triggers = model.find("controllers.triggers");
    const OptionSpec* rumble = model.find("controllers.rumble");
    const OptionSpec* saveState = model.find("save.save_state");
    const OptionSpec* second = model.find("display.second_screen");
    const OptionSpec* radio = model.find("sound.radio_station");
    const OptionSpec* navTown = model.find("graphics.nav_town");
    CHECK(triggers && rumble && saveState && second && radio && navTown);

    // No gamepad: the trigger and rumble rows are hidden (not shown dead).
    MenuContext noPad;
    noPad.diagnostics = true;  // pin predates the retail catalog (W14.1a): navigate the diagnostic rows
    noPad.gamepadPresent = false;
    CHECK(!optionAvailable(*triggers, noPad));
    CHECK(!optionAvailable(*rumble, noPad));
    CHECK(model.pageRows(OptionPage::Controllers, noPad).empty());

    // A connected pad reveals them.
    MenuContext pad = noPad;
    pad.gamepadPresent = true;
    CHECK(optionAvailable(*triggers, pad));
    CHECK(optionAvailable(*rumble, pad));
    CHECK(!model.pageRows(OptionPage::Controllers, pad).empty());

    // Second screen hides without a presentation display.
    CHECK(!optionAvailable(*second, pad));
    MenuContext twoScreens = pad;
    twoScreens.secondScreenSupported = true;
    CHECK(optionAvailable(*second, twoScreens));

    // Radio: Adventure only (the sibling rule), never in a plain race.
    MenuContext race = pad;
    race.race = true;
    CHECK(!optionAvailable(*radio, race));
    MenuContext adventure = pad;
    adventure.adventure = true;
    CHECK(optionAvailable(*radio, adventure));

    // Save-state rows: Adventure or the player's race, no movie, GS can save
    // (ported from CLAUDE.md:90).
    MenuContext playable = pad;
    playable.adventure = true;
    CHECK(optionAvailable(*saveState, playable));
    MenuContext playerRace = pad;
    playerRace.race = true;
    playerRace.playerRace = true;
    CHECK(optionAvailable(*saveState, playerRace));
    MenuContext movie = playable;
    movie.moviePlaying = true;
    CHECK(!optionAvailable(*saveState, movie));
    MenuContext aiRace = pad;
    aiRace.race = true;
    aiRace.playerRace = false;
    CHECK(!optionAvailable(*saveState, aiRace));
    MenuContext noGs = playable;
    noGs.gsCanSaveStates = false;
    CHECK(!optionAvailable(*saveState, noGs));

    // Terrain-gated rows.
    CHECK(!optionAvailable(*navTown, pad));  // nav off
    MenuContext nav = pad;
    nav.navEnabled = true;
    CHECK(optionAvailable(*navTown, nav));
}

// --------------------------------------------------------------- navigator

// A context with a pad, a second screen and terrain on, exercising the widest
// catalog the navigator can present deterministically.
static MenuContext richContext(SettingsStore& store) {
    store.setBool("graphics.terrain", true);
    MenuContext c;
    c.diagnostics = true;  // pins predate the W14.1a retail catalog: navigate the diagnostic rows
    c.gamepadPresent = true;
    c.secondScreenSupported = true;
    c.adventure = true;
    return c;
}

static void testNavigatorInitialStateAndWrap() {
    const std::filesystem::path dir = tempDir();
    SettingsStore store{dir / "settings.toml"};
    OptionsModel model;
    MenuNavigator nav(model, store, richContext(store));

    CHECK(nav.open());
    CHECK(nav.isOpen());
    CHECK(nav.inPageList());
    // Deterministic first focus: the first available page.
    CHECK(nav.focusedRow().isPageRow);
    CHECK(nav.page() == OptionPage::Display);

    const auto pages = model.availablePages(nav.effectiveContext());
    CHECK(!pages.empty());
    // Up wraps to the last page.
    CHECK(nav.moveFocus(-1));
    CHECK(nav.focusedRow().page == pages.back());
    // Down wraps back to the first.
    CHECK(nav.moveFocus(1));
    CHECK(nav.focusedRow().page == pages.front());

    CHECK(nav.close());
    CHECK(!nav.isOpen());

    std::filesystem::remove_all(dir);
}

static void testNavigatorPageEntryAdjustAndBack() {
    const std::filesystem::path dir = tempDir();
    SettingsStore store{dir / "settings.toml"};
    OptionsModel model;
    MenuNavigator nav(model, store, richContext(store));
    nav.open();

    // Enter Controllers (has rows only with the pad present).
    CHECK(nav.enterPage(OptionPage::Controllers));
    CHECK(!nav.inPageList());
    CHECK(nav.page() == OptionPage::Controllers);
    const auto rows = nav.visibleRows();
    CHECK(!rows.empty());
    CHECK(nav.focusedRow().id == rows.front()->id);

    // Toggle the focused row with the confirm button.
    const std::string before = model.valueOf(*rows.front(), store);
    CHECK(nav.confirm());
    const std::string after = model.valueOf(*rows.front(), store);
    CHECK(before != after);
    CHECK(before == "off" && after == "on");  // triggers default off

    // Back returns to the page list; a second back closes.
    CHECK(nav.back());
    CHECK(nav.inPageList());
    CHECK(nav.back());
    CHECK(!nav.isOpen());

    std::filesystem::remove_all(dir);
}

static void testNavigatorSkipsUnavailableAndAdjustDomain() {
    const std::filesystem::path dir = tempDir();
    SettingsStore store{dir / "settings.toml"};
    OptionsModel model;

    // No gamepad: Display page has interp_cap hidden (requires interp off too),
    // and Controllers page is empty. Turning interp on should reveal its
    // dependent rows.
    MenuContext ctx;
    ctx.diagnostics = true;  // pin predates the W14.1a retail catalog
    ctx.gamepadPresent = false;
    MenuNavigator nav(model, store, ctx);
    nav.open();

    // Controllers is unavailable without a pad -> page list skips it.
    const auto pages = model.availablePages(nav.effectiveContext());
    CHECK(std::find(pages.begin(), pages.end(), OptionPage::Controllers) == pages.end());

    // Enter Display; interp on, so interp_cap/rate are hidden until interp=on.
    CHECK(nav.enterPage(OptionPage::Display));
    const auto beforeRows = nav.visibleRows();
    bool hasCapBefore = false;
    for (const OptionSpec* s : beforeRows) if (s->id == "display.interp_cap") hasCapBefore = true;
    CHECK(!hasCapBefore);

    // Focus the interp toggle and confirm it on.
    while (nav.focusedRow().id != "display.interp") CHECK(nav.moveFocus(1));
    CHECK(nav.confirm());
    CHECK(store.getBool("display.interp", false));

    // Now the dependent row appears and the navigator can reach it.
    const auto afterRows = nav.visibleRows();
    bool hasCapAfter = false;
    for (const OptionSpec* s : afterRows) if (s->id == "display.interp_cap") hasCapAfter = true;
    CHECK(hasCapAfter);

    while (nav.focusedRow().id != "display.interp_cap") CHECK(nav.moveFocus(1));
    // Domain adjust: left/right move within [1, 8]; the left edge clamps.
    for (int i = 0; i < 12; ++i) nav.adjust(-1);
    CHECK(store.getInt("display.interp_cap", 0, 1, 8) == 1);
    CHECK(nav.adjust(1));
    CHECK(store.getInt("display.interp_cap", 0, 1, 8) == 2);

    std::filesystem::remove_all(dir);
}

static void testNavigatorEnumRejectsInvalidKeepsCurrent() {
    const std::filesystem::path dir = tempDir();
    SettingsStore store{dir / "settings.toml"};
    OptionsModel model;
    const OptionSpec* startState = model.find("display.start_state");
    CHECK(startState);

    // Default (domain[0]) when the store is empty.
    CHECK(model.valueOf(*startState, store) == "title");

    // An invalid token is rejected and the current value is kept.
    CHECK(!model.setValue(*startState, store, "nonsense"));
    CHECK(model.valueOf(*startState, store) == "title");

    // A valid token (any case) stores the canonical spelling.
    CHECK(model.setValue(*startState, store, "PLAYING"));
    CHECK(model.valueOf(*startState, store) == "playing");

    // Loop adjust wraps through the domain.
    CHECK(model.adjustValue(*startState, store, 1));
    CHECK(model.valueOf(*startState, store) == "title");  // playing -> title (wrapped)
    CHECK(model.adjustValue(*startState, store, -1));
    CHECK(model.valueOf(*startState, store) == "playing");  // back the other way

    std::filesystem::remove_all(dir);
}

static void testNavigatorScriptedSequenceDeterministic() {
    const std::filesystem::path dir = tempDir();
    SettingsStore store{dir / "settings.toml"};
    OptionsModel model;
    MenuNavigator nav(model, store, richContext(store));
    nav.open();

    // Scripted gamepad-only sequence: down, down, confirm (enter a page),
    // down, adjust +1, confirm (toggle), back, back.
    CHECK(nav.moveFocus(1));
    CHECK(nav.moveFocus(1));
    CHECK(nav.confirm());
    CHECK(!nav.inPageList());
    const OptionPage entered = nav.page();
    CHECK(nav.moveFocus(1));
    const std::string focusedId = nav.focusedRow().id;
    const std::string before = nav.focusedRow().value;
    CHECK(nav.confirm());
    const std::string after = nav.focusedRow().value;
    CHECK(before != after);
    CHECK(!nav.focusedRow().id.empty());
    CHECK(nav.back());
    CHECK(nav.inPageList());

    // Running the identical sequence on a fresh navigator + store yields the
    // identical focus path (determinism).
    const std::filesystem::path dir2 = tempDir();
    SettingsStore store2{dir2 / "settings.toml"};
    MenuNavigator nav2(model, store2, richContext(store2));
    nav2.open();
    CHECK(nav2.moveFocus(1));
    CHECK(nav2.moveFocus(1));
    CHECK(nav2.confirm());
    CHECK(nav2.page() == entered);
    CHECK(nav2.moveFocus(1));
    CHECK(nav2.focusedRow().id == focusedId);

    std::filesystem::remove_all(dir);
    std::filesystem::remove_all(dir2);
}

static void testNavigatorActionRows() {
    const std::filesystem::path dir = tempDir();
    SettingsStore store{dir / "settings.toml"};
    OptionsModel model;
    MenuContext ctx;
    ctx.diagnostics = true;  // pin predates the W14.1a retail catalog
    ctx.adventure = true;  // save rows available
    MenuNavigator nav(model, store, ctx);
    nav.open();
    CHECK(nav.enterPage(OptionPage::Save));
    // Walk to the Save state action and confirm it.
    while (nav.focusedRow().type != OptionType::Action) CHECK(nav.moveFocus(1));
    const std::string id = nav.focusedRow().id;
    CHECK(nav.confirm());
    CHECK(nav.lastAction() == id);

    std::filesystem::remove_all(dir);
}

int main() {
    testStoreRoundTripPreservesUnknownKeys();
    testStoreSkipsCorruptLinesAndKeepsValidKeys();
    testStoreTypedClampsAndFallbacks();
    testStoreOneWriterAndEmptyPath();
    testCatalogMapsEveryEnumeratedEnvFlag();
    testTexturePackRowListsFolders();
    testAvailabilityGamepadHiddenTriggers();
    testNavigatorInitialStateAndWrap();
    testNavigatorPageEntryAdjustAndBack();
    testNavigatorSkipsUnavailableAndAdjustDomain();
    testNavigatorEnumRejectsInvalidKeepsCurrent();
    testNavigatorScriptedSequenceDeterministic();
    testNavigatorActionRows();
    if (g_fail) { std::fprintf(stderr, "%d failure(s)\n", g_fail); return 1; }
    std::puts("settings: all checks passed");
    return 0;
}
