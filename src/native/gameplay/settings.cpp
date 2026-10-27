// Persisted settings + options-menu logic for the native Road Trip app.
// See gameplay/settings.h for the contract. Pure C++: no SDL, no Vulkan, no
// guest/disc data, no clocks. The catalog maps every user option to the
// RT_APP_* env flag(s) it stands in for (enumerated with file:line in
// docs/native-reference/options-menu.md). AUTHORED-for-native (CLAUDE.md:87).
#include "gameplay/settings.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <system_error>

namespace rt::native::gameplay {
namespace {

std::string lower(std::string_view s) {
    std::string out(s);
    for (char& c : out) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return out;
}

std::string_view trim(std::string_view s) {
    std::size_t b = 0, e = s.size();
    while (b < e && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}

bool tryParseLong(std::string_view text, long long& out) {
    const std::string s(trim(text));
    if (s.empty()) return false;
    char* end = nullptr;
    errno = 0;
    const long long v = std::strtoll(s.c_str(), &end, 10);
    if (end == s.c_str() || *end != '\0') return false;
    out = v;
    return true;
}

bool tryParseDouble(std::string_view text, double& out) {
    const std::string s(trim(text));
    if (s.empty()) return false;
    char* end = nullptr;
    errno = 0;
    const double v = std::strtod(s.c_str(), &end);
    if (end == s.c_str() || *end != '\0') return false;
    out = v;
    return true;
}

}  // namespace

// ===========================================================================
// SettingsStore
// ===========================================================================

SettingsStore::SettingsStore(std::filesystem::path path) : path_(std::move(path)) {}

bool SettingsStore::parseBool(std::string_view text, bool& out) {
    const std::string v = lower(trim(text));
    if (v == "true" || v == "1" || v == "yes" || v == "on") { out = true; return true; }
    if (v == "false" || v == "0" || v == "no" || v == "off") { out = false; return true; }
    return false;
}

std::string SettingsStore::formatFloat(float value) {
    char buffer[32];
    std::snprintf(buffer, sizeof buffer, "%g", static_cast<double>(value));
    return buffer;
}

void SettingsStore::clear() {
    lines_.clear();
    skippedLines_ = 0;
}

bool SettingsStore::load() {
    clear();
    if (path_.empty()) return false;
    std::error_code ec;
    if (!std::filesystem::exists(path_, ec) || ec) return false;
    std::ifstream input(path_);
    if (!input) return false;
    std::string raw;
    while (std::getline(input, raw)) {
        if (!raw.empty() && raw.back() == '\r') raw.pop_back();
        const std::string_view t = trim(raw);
        Line line;
        if (t.empty()) {
            line.kind = LineKind::Blank;
            line.raw = raw;
        } else if (t.front() == '#') {
            line.kind = LineKind::Comment;
            line.raw = raw;
        } else {
            const std::size_t eq = raw.find('=');
            if (eq == std::string::npos) {
                line.kind = LineKind::Invalid;
                line.raw = raw;
                ++skippedLines_;
            } else {
                const std::string_view key = trim(std::string_view(raw).substr(0, eq));
                if (key.empty()) {
                    line.kind = LineKind::Invalid;
                    line.raw = raw;
                    ++skippedLines_;
                } else {
                    line.kind = LineKind::Entry;
                    line.key = std::string(key);
                    line.value = std::string(trim(std::string_view(raw).substr(eq + 1)));
                }
            }
        }
        lines_.push_back(std::move(line));
    }
    return true;
}

bool SettingsStore::save() const {
    if (path_.empty()) return false;
    std::error_code ec;
    if (!path_.parent_path().empty()) std::filesystem::create_directories(path_.parent_path(), ec);
    const std::filesystem::path temp = path_.string() + ".tmp";
    {
        std::ofstream out(temp, std::ios::trunc);
        if (!out) return false;
        for (const Line& line : lines_) {
            switch (line.kind) {
            case LineKind::Entry:
                out << line.key << " = " << line.value << '\n';
                break;
            case LineKind::Blank:
            case LineKind::Comment:
            case LineKind::Invalid:
                out << line.raw << '\n';
                break;
            }
        }
        if (!out) return false;
    }
    std::filesystem::rename(temp, path_, ec);
    if (ec) {
        std::filesystem::remove(temp, ec);
        return false;
    }
    return true;
}

int SettingsStore::findFirst(std::string_view key) const {
    for (std::size_t i = 0; i < lines_.size(); ++i)
        if (lines_[i].kind == LineKind::Entry && lines_[i].key == key) return static_cast<int>(i);
    return -1;
}

bool SettingsStore::contains(std::string_view key) const { return findFirst(key) >= 0; }

std::size_t SettingsStore::entryCount() const {
    std::size_t n = 0;
    for (const Line& line : lines_)
        if (line.kind == LineKind::Entry) ++n;
    return n;
}

std::vector<SettingsStore::Entry> SettingsStore::entries() const {
    std::vector<Entry> out;
    out.reserve(entryCount());
    for (const Line& line : lines_)
        if (line.kind == LineKind::Entry) out.push_back({line.key, line.value});
    return out;
}

std::string SettingsStore::getString(std::string_view key, std::string fallback) const {
    const int index = findFirst(key);
    return index < 0 ? fallback : lines_[static_cast<std::size_t>(index)].value;
}

bool SettingsStore::getBool(std::string_view key, bool fallback) const {
    const int index = findFirst(key);
    if (index < 0) return fallback;
    bool value = false;
    if (!parseBool(lines_[static_cast<std::size_t>(index)].value, value)) return fallback;
    return value;
}

int SettingsStore::getInt(std::string_view key, int fallback, int lo, int hi) const {
    if (lo > hi) std::swap(lo, hi);
    const int clampedFallback = std::clamp(fallback, lo, hi);
    const int index = findFirst(key);
    if (index < 0) return clampedFallback;
    long long value = 0;
    if (!tryParseLong(lines_[static_cast<std::size_t>(index)].value, value)) return clampedFallback;
    if (value < lo) return lo;
    if (value > hi) return hi;
    return static_cast<int>(value);
}

float SettingsStore::getFloat(std::string_view key, float fallback, float lo, float hi) const {
    if (lo > hi) std::swap(lo, hi);
    const float clampedFallback = std::clamp(fallback, lo, hi);
    const int index = findFirst(key);
    if (index < 0) return clampedFallback;
    double value = 0.0;
    if (!tryParseDouble(lines_[static_cast<std::size_t>(index)].value, value)) return clampedFallback;
    if (value < static_cast<double>(lo)) return lo;
    if (value > static_cast<double>(hi)) return hi;
    return static_cast<float>(value);
}

std::string SettingsStore::getEnum(std::string_view key, std::string fallback,
                                   const std::vector<std::string>& allowed) const {
    const int index = findFirst(key);
    if (index < 0) return fallback;
    const std::string seen = lower(lines_[static_cast<std::size_t>(index)].value);
    for (const std::string& candidate : allowed)
        if (lower(candidate) == seen) return candidate;
    return fallback;
}

void SettingsStore::setRaw(std::string_view key, std::string_view value) {
    if (key.empty()) return;
    const int index = findFirst(key);
    if (index >= 0) {
        lines_[static_cast<std::size_t>(index)].value = std::string(value);
        return;
    }
    Line line;
    line.kind = LineKind::Entry;
    line.key = std::string(key);
    line.value = std::string(value);
    lines_.push_back(std::move(line));
}

void SettingsStore::setBool(std::string_view key, bool value) {
    setRaw(key, value ? "on" : "off");
}

void SettingsStore::setInt(std::string_view key, int value) {
    setRaw(key, std::to_string(value));
}

void SettingsStore::setFloat(std::string_view key, float value) {
    setRaw(key, formatFloat(value));
}

void SettingsStore::setString(std::string_view key, std::string_view value) {
    setRaw(key, value);
}

bool SettingsStore::erase(std::string_view key) {
    const auto first = std::remove_if(lines_.begin(), lines_.end(), [key](const Line& line) {
        return line.kind == LineKind::Entry && line.key == key;
    });
    if (first == lines_.end()) return false;
    lines_.erase(first, lines_.end());
    return true;
}

std::filesystem::path defaultSettingsPath(const std::filesystem::path& dataDirectory) {
    return dataDirectory / "settings.toml";
}

// ===========================================================================
// OptionsModel
// ===========================================================================

namespace {

// Small builders keep the catalog table readable below.
OptionSpec toggle(std::string id, std::string label, OptionPage page, bool on,
                  OptionAvailability avail, OptionStatus status,
                  std::vector<std::string> flags, std::string note) {
    OptionSpec s;
    s.id = std::move(id); s.label = std::move(label); s.page = page;
    s.type = OptionType::Toggle; s.availability = avail; s.status = status;
    s.defaultRaw = on ? "on" : "off"; s.envFlags = std::move(flags); s.note = std::move(note);
    return s;
}

OptionSpec enumOpt(std::string id, std::string label, OptionPage page,
                   std::vector<std::string> domain, OptionAvailability avail,
                   OptionStatus status, std::vector<std::string> flags, std::string note) {
    OptionSpec s;
    s.id = std::move(id); s.label = std::move(label); s.page = page;
    s.type = OptionType::Enum; s.availability = avail; s.status = status;
    s.domain = std::move(domain);
    s.defaultRaw = s.domain.empty() ? "" : s.domain.front();
    s.envFlags = std::move(flags); s.note = std::move(note);
    return s;
}

OptionSpec integer(std::string id, std::string label, OptionPage page, int lo, int hi,
                   double step, int def, OptionAvailability avail, OptionStatus status,
                   std::vector<std::string> flags, std::string note) {
    OptionSpec s;
    s.id = std::move(id); s.label = std::move(label); s.page = page;
    s.type = OptionType::Integer; s.availability = avail; s.status = status;
    s.min = lo; s.max = hi; s.step = step; s.defaultRaw = std::to_string(def);
    s.envFlags = std::move(flags); s.note = std::move(note);
    return s;
}

OptionSpec real(std::string id, std::string label, OptionPage page, double lo, double hi,
                double step, double def, OptionAvailability avail, OptionStatus status,
                std::vector<std::string> flags, std::string note) {
    OptionSpec s;
    s.id = std::move(id); s.label = std::move(label); s.page = page;
    s.type = OptionType::Float; s.availability = avail; s.status = status;
    s.min = lo; s.max = hi; s.step = step; s.defaultRaw = SettingsStore::formatFloat(static_cast<float>(def));
    s.envFlags = std::move(flags); s.note = std::move(note);
    return s;
}

OptionSpec stringOpt(std::string id, std::string label, OptionPage page,
                     OptionAvailability avail, OptionStatus status,
                     std::vector<std::string> flags, std::string note) {
    OptionSpec s;
    s.id = std::move(id); s.label = std::move(label); s.page = page;
    s.type = OptionType::String; s.availability = avail; s.status = status;
    s.envFlags = std::move(flags); s.note = std::move(note);
    return s;
}

OptionSpec action(std::string id, std::string label, OptionPage page,
                  OptionAvailability avail, OptionStatus status, std::string note) {
    OptionSpec s;
    s.id = std::move(id); s.label = std::move(label); s.page = page;
    s.type = OptionType::Action; s.availability = avail; s.status = status;
    s.note = std::move(note);
    return s;
}

constexpr const char* kPreviewNote =
    "GPU wave - currently preview: the value persists and is shown, but no native "
    "consumer reads it yet (sibling recomp precedent, CLAUDE.md:87-88)";

const char* const kPages[static_cast<int>(OptionPage::Count)] = {
    "Display", "Graphics", "Controllers", "Sound", "Save"};

}  // namespace

bool optionAvailable(const OptionSpec& spec, const MenuContext& context) {
    // Retail: only recomp-catalog rows, never previews of GPU features not built yet.
    if (!context.diagnostics && (!spec.retail || spec.status == OptionStatus::GpuWavePending)) return false;
    switch (spec.availability) {
    case OptionAvailability::Always:
        return true;
    case OptionAvailability::RequiresGamepad:
        return context.gamepadPresent;
    case OptionAvailability::RequiresSecondScreen:
        return context.secondScreenSupported;
    case OptionAvailability::RequiresTerrain:
        return context.terrainEnabled;
    case OptionAvailability::RequiresStream:
        return context.streamEnabled;
    case OptionAvailability::RequiresNav:
        return context.navEnabled;
    case OptionAvailability::RequiresRoute:
        return context.routeEnabled;
    case OptionAvailability::RequiresInterp:
        return context.interpEnabled;
    case OptionAvailability::RequiresAudio:
        return context.audioAvailable;
    case OptionAvailability::RequiresAdventure:
        return context.adventure;
    case OptionAvailability::RequiresDesktop:
        return !context.android;
    case OptionAvailability::RequiresDesktopWindow:
        return !context.android && context.windowed;
    case OptionAvailability::RequiresMapRow:
        return context.hardwareGs && !(context.secondScreenSupported && context.secondScreenOn);
    case OptionAvailability::RequiresFrameGenWarp:
        return context.frameGenWarp;
    case OptionAvailability::RequiresSaveState:
        // The sibling rule (CLAUDE.md:90): Save state / Load state rows only in
        // an Adventure or a race of the player's, no movie, and a GS that can
        // save. Ported, not copied.
        return context.gsCanSaveStates && !context.moviePlaying &&
               (context.adventure || (context.race && context.playerRace));
    }
    return false;
}

const char* optionPageName(OptionPage page) {
    const int index = static_cast<int>(page);
    if (index < 0 || index >= static_cast<int>(OptionPage::Count)) return "Unknown";
    return kPages[index];
}

const char* optionTypeName(OptionType type) {
    switch (type) {
    case OptionType::Toggle: return "toggle";
    case OptionType::Enum: return "enum";
    case OptionType::Integer: return "integer";
    case OptionType::Float: return "float";
    case OptionType::String: return "string";
    case OptionType::Action: return "action";
    }
    return "unknown";
}

const char* optionStatusName(OptionStatus status) {
    switch (status) {
    case OptionStatus::WiredNow: return "wired-now";
    case OptionStatus::GpuWavePending: return "gpu-wave-pending";
    case OptionStatus::HostGluePending: return "host-glue-pending";
    }
    return "unknown";
}

OptionsModel::OptionsModel() {
    using P = OptionPage;
    using A = OptionAvailability;
    using S = OptionStatus;

    // ---- Display ---------------------------------------------------------
    options_.push_back(enumOpt("display.start_state", "Start state", P::Display,
        {"title", "menu", "playing"}, A::Always, S::WiredNow, {"RT_APP_STATE"},
        "launch/harness flag (main.cpp:761); the menu replaces it once input glue lands"));
    options_.push_back(toggle("display.menu", "Options menu (Esc/Start)", P::Display, false,
        A::Always, S::WiredNow, {"RT_MENU"},
        "RT_MENU=1 opens the pause menu for now; real Esc/Start wiring is the GPU-wave glue"));
    options_.push_back(toggle("display.interp", "Frame interpolation", P::Display, false,
        A::Always, S::WiredNow, {"RT_APP_INTERP"},
        "60 Hz logic kept (frame-timing verdict a); only render poses are interpolated"));
    options_.push_back(integer("display.interp_cap", "Interp catch-up cap", P::Display, 1, 8, 1, 2,
        A::RequiresInterp, S::WiredNow, {"RT_APP_INTERP_CAP"}, "max logic steps drained per host frame"));
    options_.push_back(integer("display.interp_rate", "Interp render rate (fps)", P::Display, 0, 240, 1, 0,
        A::RequiresInterp, S::WiredNow, {"RT_APP_INTERP_RATE"}, "0 = pace render on the swapchain (vsync)"));
    options_.push_back(toggle("display.render", "Render to window", P::Display, true,
        A::Always, S::WiredNow, {"RT_APP_NORENDER"}, "off sets RT_APP_NORENDER (headless)"));
    options_.push_back(stringOpt("display.snapshot_path", "Snapshot path (PPM)", P::Display,
        A::Always, S::WiredNow, {"RT_APP_SNAPSHOT"}, "writes one PPM per frame; empty = off"));
    options_.push_back(integer("display.frame_budget", "Frame budget", P::Display, 0, 1000000, 1, 0,
        A::Always, S::WiredNow, {"RT_APP_FRAMES"}, "0 = unlimited; harness/deterministic runs"));
    options_.push_back(stringOpt("display.disc_root", "Disc extract root", P::Display,
        A::Always, S::WiredNow, {"RT_DISC"},
        "default ~/Library/Application Support/RoadTripRecomp/disc"));
    options_.push_back(toggle("display.debug_overlay", "Debug overlay", P::Display, false,
        A::Always, S::WiredNow, {"RT_APP_DEBUG_OVERLAY"}, "frustum stats overlay (preview.cpp:1027)"));
    options_.push_back(enumOpt("display.aa", "Anti-aliasing", P::Display,
        {"off", "fxaa", "smaa", "taa"}, A::Always, S::GpuWavePending, {},
        kPreviewNote));
    options_.push_back(enumOpt("display.upscaler", "Upscaler", P::Display,
        {"none", "fsr1", "metalfx", "gsr1", "gsr2"}, A::Always, S::GpuWavePending, {},
        kPreviewNote));
    options_.push_back(enumOpt("display.map_mode", "Map mode", P::Display,
        {"game", "enhanced"}, A::Always, S::GpuWavePending, {},
        kPreviewNote));

    // ---- Graphics --------------------------------------------------------
    options_.push_back(toggle("graphics.terrain", "Disc terrain", P::Graphics, false,
        A::Always, S::WiredNow, {"RT_APP_TERRAIN"}, "real FLD-tile terrain instead of the grass quad"));
    options_.push_back(toggle("graphics.textures", "Field textures", P::Graphics, false,
        A::RequiresTerrain, S::WiredNow, {"RT_APP_TEXTURES"}, "decode per-tile GS uploads to RGBA8"));
    options_.push_back(stringOpt("graphics.terrain_file", "Single tile file", P::Graphics,
        A::RequiresTerrain, S::WiredNow, {"RT_APP_TERRAIN_FILE"}, "one explicit FLD tile instead of the grid"));
    options_.push_back(toggle("graphics.stream", "Tile streaming (4-slot)", P::Graphics, false,
        A::RequiresTerrain, S::WiredNow, {"RT_APP_STREAM"}, "hardware-style 4-slot window (slot = id & 3)"));
    options_.push_back(integer("graphics.stream_step_frames", "Stream sweep step", P::Graphics, 1, 600, 1, 10,
        A::RequiresStream, S::WiredNow, {"RT_APP_STREAM_STEP_FRAMES"}, "scripted serpentine sweep period"));
    options_.push_back(integer("graphics.stream_row", "Stream start row", P::Graphics, 0, 7, 1, 1,
        A::RequiresStream, S::WiredNow, {"RT_APP_STREAM_ROW"}, "id-grid row the sweep starts on"));
    options_.push_back(toggle("graphics.stream_pos", "Position-driven demand", P::Graphics, false,
        A::RequiresStream, S::WiredNow, {"RT_APP_STREAM_POS"}, "follow the car via TileStreamer::advanceTo"));
    options_.push_back(toggle("graphics.nav", "Nav overlay", P::Graphics, false,
        A::Always, S::WiredNow, {"RT_APP_NAV"}, "draw a ROM nav-graph centerline as ribbons"));
    options_.push_back(toggle("graphics.nav_town", "Town street graph", P::Graphics, false,
        A::RequiresNav, S::WiredNow, {"RT_APP_NAV_TOWN"}, "Peach Town street graph (scene T=1) instead"));
    options_.push_back(toggle("graphics.route", "Route follower", P::Graphics, false,
        A::Always, S::WiredNow, {"RT_APP_ROUTE"}, "drive the render-visible car along graph 0"));
    options_.push_back(toggle("graphics.route_town", "Route on town graph", P::Graphics, false,
        A::RequiresRoute, S::WiredNow, {"RT_APP_ROUTE_TOWN"}, "follow the town street graph instead"));
    options_.push_back(integer("graphics.carmodel", "Car model (Q index)", P::Graphics, 0, 255, 1, 0,
        A::Always, S::WiredNow, {"RT_APP_CARMODEL"}, "load Q{N}.BIN through the CAR pipeline; demo box on failure"));
    options_.push_back(stringOpt("graphics.carmodel_dump", "Car livery dump path", P::Graphics,
        A::Always, S::WiredNow, {"RT_APP_CARMODEL_DUMP"}, "write the decoded livery as PPM"));
    options_.push_back(toggle("graphics.hudtext", "HUD text (FONTE)", P::Graphics, false,
        A::Always, S::WiredNow, {"RT_APP_HUDTEXT"}, "decode SYS/FONTE.GSL and draw race/town text"));
    options_.push_back(enumOpt("graphics.scorehud", "Score/Meter page", P::Graphics,
        {"off", "score", "meter"}, A::Always, S::WiredNow, {"RT_APP_SCOREHUD"}, "livery-test style page overlay"));

    // ---- Controllers -----------------------------------------------------
    options_.push_back(toggle("controllers.triggers", "Analog trigger drive", P::Controllers, false,
        A::RequiresGamepad, S::GpuWavePending, {"RT_APP_TRIGGERS"},
        "mapping module ready (input.{h,cpp} mapTriggersToDrive); app glue lands at the integration wave"));
    options_.push_back(toggle("controllers.rumble", "Rumble", P::Controllers, true,
        A::RequiresGamepad, S::GpuWavePending, {},
        "GPU wave - currently preview: no native haptics yet (sibling Controllers->Vibration)"));
    options_.push_back(real("controllers.rumble_strength", "Rumble strength", P::Controllers, 0.0, 1.0, 0.05, 1.0,
        A::RequiresGamepad, S::GpuWavePending, {},
        "GPU wave - currently preview; used once haptics are wired"));
    options_.push_back(real("controllers.deadzone", "Stick deadzone", P::Controllers, 0.0, 0.9, 0.05, 0.2,
        A::RequiresGamepad, S::GpuWavePending, {},
        "mirrors input.h kStickDeadzone (0.2); store->input glue is GPU wave"));

    // ---- Sound -----------------------------------------------------------
    options_.push_back(toggle("sound.audio", "Audio output", P::Sound, true,
        A::Always, S::WiredNow, {"RT_APP_NOAUDIO"}, "off sets RT_APP_NOAUDIO"));
    options_.push_back(stringOpt("sound.bgm", "BGM track / file", P::Sound,
        A::RequiresAudio, S::WiredNow, {"RT_APP_BGM"}, "TSQ track name or a VAG file path"));
    options_.push_back(toggle("sound.bgm_tsq", "Use TSQ BGM (else VAG)", P::Sound, true,
        A::RequiresAudio, S::WiredNow, {"RT_APP_BGM_TSQ"}, "off forces the VAG path"));
    options_.push_back(enumOpt("sound.radio_station", "Radio station", P::Sound,
        {"off", "peach_fm", "e_radio"}, A::RequiresAdventure, S::HostGluePending, {},
        "AUTHORED: save +0x12DD (0/1/2, PROVEN); the host BGM path does not consume the station yet"));

    // ---- Save ------------------------------------------------------------
    options_.push_back(integer("save.slot", "Save slot", P::Save, 0, 1, 1, 0,
        A::RequiresSaveState, S::WiredNow, {}, "native host slot 0/1 (MemoryCard)"));
    options_.push_back(action("save.save_state", "Save state", P::Save,
        A::RequiresSaveState, S::WiredNow, "GameSession::save(card, slot) -> slot<N>.sav"));
    options_.push_back(action("save.load_state", "Load state", P::Save,
        A::RequiresSaveState, S::WiredNow, "GameSession::load(card, slot) <- slot<N>.sav"));

    // ---- Recomp catalog (retail) -------------------------------------------
    // Order and strings: docs/native-reference/ui-parity.md "Pause Options"; source
    // src/ui/PauseMenu.cpp:397-542, 813-872. The first row of each page is its first retail row.
    const std::string kGlue = "recomp catalog row; persisted, host consumer lands with the window/audio/input glue";
    options_.push_back(enumOpt("display.window_mode", "Display mode", P::Display,
        {"Window", "Borderless fullscreen", "Fullscreen"}, A::RequiresDesktop, S::HostGluePending, {}, kGlue));
    options_.push_back(enumOpt("display.window_size", "Window size", P::Display,
        {"Medium (960x672)", "Small (640x448)", "Large (1280x896)"}, A::RequiresDesktopWindow, S::HostGluePending, {}, kGlue));
    options_.push_back(enumOpt("display.aspect", "Aspect ratio", P::Display,
        {"4:3 (original)", "16:10", "16:9", "21:9", "32:9"}, A::Always, S::HostGluePending, {}, kGlue));
    options_.push_back(enumOpt("display.hud_position", "HUD position", P::Display,
        {"4:3 centred", "Screen edges"}, A::Always, S::HostGluePending, {}, kGlue));
    options_.push_back(enumOpt("display.map", "Map", P::Display,
        {"Enhanced", "Original"}, A::RequiresMapRow, S::HostGluePending, {}, kGlue));
    options_.push_back(enumOpt("display.frame_rate", "Frame rate", P::Display,
        {"60 fps (original)", "120 fps (generated)", "240 fps (generated)"}, A::Always, S::HostGluePending, {}, kGlue));
    options_.push_back(enumOpt("display.frame_generation", "Frame generation", P::Display,
        {"Re-rendered (no added latency)", "Warped, interpolated (+1 refresh latency)", "Warped, extrapolated (edge artefacts)"},
        A::RequiresFrameGenWarp, S::HostGluePending, {}, kGlue));
    options_.push_back(enumOpt("display.frame_skip", "Frame skip", P::Display,
        {"Auto", "Off"}, A::Always, S::HostGluePending, {}, kGlue));
    options_.push_back(enumOpt("display.perf_overlay", "Performance overlay", P::Display,
        {"Off", "Frame rate", "Detailed"}, A::Always, S::HostGluePending, {}, kGlue));
    options_.push_back(enumOpt("display.second_screen", "Second screen", P::Display,
        {"Off", "On"}, A::RequiresSecondScreen, S::HostGluePending, {}, kGlue));
    options_.push_back(enumOpt("sound.volume", "Volume", P::Sound,
        {"100%", "Off", "10%", "20%", "30%", "40%", "50%", "60%", "70%", "80%", "90%"}, A::Always, S::HostGluePending, {}, kGlue));
    options_.push_back(enumOpt("sound.speaker", "Speaker", P::Sound,
        {"Stereo", "Mono"}, A::Always, S::HostGluePending, {}, kGlue));
    options_.push_back(enumOpt("controllers.controls", "Controls", P::Controllers,
        {"Modern", "Classic", "Custom"}, A::RequiresGamepad, S::HostGluePending, {}, kGlue));
    options_.push_back(enumOpt("controllers.vibration", "Vibration", P::Controllers,
        {"Dynamic", "Classic", "Off"}, A::RequiresGamepad, S::HostGluePending, {}, kGlue));
    options_.push_back(enumOpt("controllers.strength", "Strength", P::Controllers,
        {"100%", "75%", "50%", "25%", "Off"}, A::RequiresGamepad, S::HostGluePending, {}, kGlue));

    // Retail rows: the recomp catalog above, the Adventure-only radio, and the state actions.
    for (OptionSpec& spec : options_)
        spec.retail = spec.status == S::HostGluePending || spec.id == "save.save_state" || spec.id == "save.load_state";
}

const OptionSpec* OptionsModel::find(std::string_view id) const {
    for (const OptionSpec& spec : options_)
        if (spec.id == id) return &spec;
    return nullptr;
}

std::vector<const OptionSpec*> OptionsModel::pageRows(OptionPage page,
                                                      const MenuContext& context) const {
    std::vector<const OptionSpec*> rows;
    for (const OptionSpec& spec : options_)
        if (spec.page == page && optionAvailable(spec, context)) rows.push_back(&spec);
    return rows;
}

bool OptionsModel::pageAvailable(OptionPage page, const MenuContext& context) const {
    return !pageRows(page, context).empty();
}

std::vector<OptionPage> OptionsModel::availablePages(const MenuContext& context) const {
    std::vector<OptionPage> pages;
    for (int i = 0; i < static_cast<int>(OptionPage::Count); ++i) {
        const OptionPage page = static_cast<OptionPage>(i);
        if (pageAvailable(page, context)) pages.push_back(page);
    }
    return pages;
}

std::string OptionsModel::valueOf(const OptionSpec& spec, const SettingsStore& store) const {
    switch (spec.type) {
    case OptionType::Toggle: {
        bool def = false;
        (void)SettingsStore::parseBool(spec.defaultRaw, def);
        return store.getBool(spec.id, def) ? "on" : "off";
    }
    case OptionType::Enum:
        return store.getEnum(spec.id, spec.defaultRaw, spec.domain);
    case OptionType::Integer: {
        long long def = 0;
        tryParseLong(spec.defaultRaw, def);
        return std::to_string(store.getInt(spec.id, static_cast<int>(def),
                                           static_cast<int>(spec.min), static_cast<int>(spec.max)));
    }
    case OptionType::Float: {
        double def = 0.0;
        tryParseDouble(spec.defaultRaw, def);
        return SettingsStore::formatFloat(store.getFloat(spec.id, static_cast<float>(def),
                                                         static_cast<float>(spec.min),
                                                         static_cast<float>(spec.max)));
    }
    case OptionType::String:
        return store.getString(spec.id, spec.defaultRaw);
    case OptionType::Action:
        return {};
    }
    return {};
}

bool OptionsModel::setValue(const OptionSpec& spec, SettingsStore& store, std::string_view raw) const {
    switch (spec.type) {
    case OptionType::Toggle: {
        bool value = false;
        if (!SettingsStore::parseBool(raw, value)) return false;
        store.setBool(spec.id, value);
        return true;
    }
    case OptionType::Enum: {
        const std::string seen = lower(trim(raw));
        for (const std::string& candidate : spec.domain)
            if (lower(candidate) == seen) {
                store.setRaw(spec.id, candidate);
                return true;
            }
        return false;  // invalid token: keep the current value
    }
    case OptionType::Integer: {
        long long value = 0;
        if (!tryParseLong(raw, value)) return false;
        const int lo = static_cast<int>(spec.min), hi = static_cast<int>(spec.max);
        const int clamped = static_cast<int>(std::clamp(value, static_cast<long long>(lo), static_cast<long long>(hi)));
        store.setInt(spec.id, clamped);
        return true;
    }
    case OptionType::Float: {
        double value = 0.0;
        if (!tryParseDouble(raw, value)) return false;
        const double clamped = std::clamp(value, spec.min, spec.max);
        store.setFloat(spec.id, static_cast<float>(clamped));
        return true;
    }
    case OptionType::String:
        store.setString(spec.id, raw);
        return true;
    case OptionType::Action:
        return false;
    }
    return false;
}

bool OptionsModel::adjustValue(const OptionSpec& spec, SettingsStore& store, int direction) const {
    if (direction == 0) return false;
    switch (spec.type) {
    case OptionType::Toggle: {
        bool def = false;
        (void)SettingsStore::parseBool(spec.defaultRaw, def);
        const bool current = store.getBool(spec.id, def);
        store.setBool(spec.id, !current);
        return true;
    }
    case OptionType::Enum: {
        const std::size_t count = spec.domain.size();
        if (count == 0) return false;
        const std::string current = store.getEnum(spec.id, spec.defaultRaw, spec.domain);
        std::size_t index = 0;
        for (std::size_t i = 0; i < count; ++i)
            if (spec.domain[i] == current) { index = i; break; }
        const std::size_t next = static_cast<std::size_t>(
            (static_cast<long long>(index) + direction) % static_cast<long long>(count) + count) % count;
        store.setRaw(spec.id, spec.domain[next]);
        return true;
    }
    case OptionType::Integer: {
        long long def = 0;
        tryParseLong(spec.defaultRaw, def);
        const int lo = static_cast<int>(spec.min), hi = static_cast<int>(spec.max);
        const int current = store.getInt(spec.id, static_cast<int>(def), lo, hi);
        const int next = std::clamp(current + static_cast<int>(spec.step) * direction, lo, hi);
        if (next == current) return false;
        store.setInt(spec.id, next);
        return true;
    }
    case OptionType::Float: {
        double def = 0.0;
        tryParseDouble(spec.defaultRaw, def);
        const float current = store.getFloat(spec.id, static_cast<float>(def),
                                             static_cast<float>(spec.min), static_cast<float>(spec.max));
        const double raw = static_cast<double>(current) + spec.step * direction;
        const float next = static_cast<float>(std::clamp(raw, spec.min, spec.max));
        if (next == current) return false;
        store.setFloat(spec.id, next);
        return true;
    }
    case OptionType::String:
    case OptionType::Action:
        return false;
    }
    return false;
}

// ===========================================================================
// MenuNavigator
// ===========================================================================

MenuNavigator::MenuNavigator(const OptionsModel& model, SettingsStore& store, MenuContext context)
    : model_(model), store_(store), context_(context),
      focusMemory_(static_cast<std::size_t>(OptionPage::Count), 0) {}

MenuContext MenuNavigator::effectiveContext() const {
    MenuContext c = context_;
    c.terrainEnabled = store_.getBool("graphics.terrain", false);
    c.streamEnabled = c.terrainEnabled && store_.getBool("graphics.stream", false);
    c.navEnabled = store_.getBool("graphics.nav", false);
    c.routeEnabled = store_.getBool("graphics.route", false);
    c.interpEnabled = store_.getBool("display.interp", false);
    c.audioAvailable = store_.getBool("sound.audio", true);
    c.windowed = store_.getEnum("display.window_mode", "Window", {"Window", "Borderless fullscreen", "Fullscreen"}) == "Window";
    c.secondScreenOn = store_.getEnum("display.second_screen", "Off", {"On", "Off"}) == "On";
    return c;
}

std::vector<OptionPage> MenuNavigator::pages() const {
    return model_.availablePages(effectiveContext());
}

int MenuNavigator::rowCount(OptionPage page) const {
    return static_cast<int>(model_.pageRows(page, effectiveContext()).size());
}

int MenuNavigator::firstAvailableRowIndex(OptionPage page, int from) const {
    const int n = rowCount(page);
    if (n <= 0) return 0;
    return ((from % n) + n) % n;
}

bool MenuNavigator::open() {
    open_ = true;
    pageList_ = true;
    lastAction_.clear();
    pageCursor_ = 0;
    focusIndex_ = 0;
    std::fill(focusMemory_.begin(), focusMemory_.end(), 0u);
    return true;
}

bool MenuNavigator::close() {
    if (!open_) return false;
    open_ = false;
    return true;
}

bool MenuNavigator::moveFocus(int direction) {
    if (!open_ || direction == 0) return false;
    if (pageList_) {
        const auto p = pages();
        if (p.empty()) return false;
        const long long n = static_cast<long long>(p.size());
        pageCursor_ = static_cast<std::size_t>(((static_cast<long long>(pageCursor_) + direction) % n + n) % n);
        return true;
    }
    const int n = rowCount(page_);
    if (n <= 0) return false;
    focusIndex_ = static_cast<std::size_t>(((static_cast<long long>(focusIndex_) + direction) % n + n) % n);
    return true;
}

bool MenuNavigator::adjust(int direction) {
    if (!open_ || pageList_ || direction == 0) return false;
    const auto rows = visibleRows();
    if (rows.empty()) return false;
    clampPageCursor(focusIndex_);
    return model_.adjustValue(*rows[focusIndex_], store_, direction);
}

bool MenuNavigator::confirm() {
    if (!open_) return false;
    if (pageList_) {
        const auto p = pages();
        if (p.empty() || pageCursor_ >= p.size()) return false;
        return enterPage(p[pageCursor_]);
    }
    const auto rows = visibleRows();
    if (rows.empty()) return false;
    clampPageCursor(focusIndex_);
    const OptionSpec& spec = *rows[focusIndex_];
    switch (spec.type) {
    case OptionType::Toggle:
    case OptionType::Enum:
    case OptionType::Integer:
    case OptionType::Float:
        return model_.adjustValue(spec, store_, 1);
    case OptionType::Action:
        lastAction_ = spec.id;
        return true;
    case OptionType::String:
        return false;
    }
    return false;
}

bool MenuNavigator::back() {
    if (!open_) return false;
    if (!pageList_) {
        focusMemory_[static_cast<std::size_t>(page_)] = focusIndex_;
        pageList_ = true;
        return true;
    }
    return close();
}

bool MenuNavigator::enterPage(OptionPage page) {
    const auto p = pages();
    int index = -1;
    for (std::size_t i = 0; i < p.size(); ++i)
        if (p[i] == page) { index = static_cast<int>(i); break; }
    if (index < 0) return false;
    const int n = rowCount(page);
    if (n <= 0) return false;
    page_ = page;
    pageList_ = false;
    pageCursor_ = static_cast<std::size_t>(index);
    std::size_t remembered = focusMemory_[static_cast<std::size_t>(page)];
    if (remembered >= static_cast<std::size_t>(n)) remembered = 0;
    focusIndex_ = remembered;
    return true;
}

std::vector<const OptionSpec*> MenuNavigator::visibleRows() const {
    if (pageList_) return {};
    return model_.pageRows(page_, effectiveContext());
}

void MenuNavigator::clampPageCursor(std::size_t cursor) {
    focusIndex_ = static_cast<std::size_t>(firstAvailableRowIndex(page_, static_cast<int>(cursor)));
}

MenuNavigator::FocusRow MenuNavigator::focusedRow() const {
    FocusRow row;
    if (!open_) return row;
    if (pageList_) {
        const auto p = pages();
        if (p.empty() || pageCursor_ >= p.size()) return row;
        row.isPageRow = true;
        row.page = p[pageCursor_];
        row.label = optionPageName(row.page);
        row.available = true;
        return row;
    }
    const auto rows = model_.pageRows(page_, effectiveContext());
    if (rows.empty()) return row;
    const std::size_t index = focusIndex_ < rows.size() ? focusIndex_ : 0;
    const OptionSpec& spec = *rows[index];
    row.isPageRow = false;
    row.page = spec.page;
    row.id = spec.id;
    row.label = spec.label;
    row.value = model_.valueOf(spec, store_);
    row.type = spec.type;
    row.status = spec.status;
    row.available = true;
    row.domain = spec.domain;
    row.note = spec.note;
    return row;
}

}  // namespace rt::native::gameplay
