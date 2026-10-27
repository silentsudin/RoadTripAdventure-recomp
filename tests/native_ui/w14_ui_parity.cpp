// W14.0 structural UI-parity tripwire.  It is manual-only until all surfaces pass.
#include "assets/glyph_atlas.h"
#include "gameplay/settings.h"
#include "preview/race_hud.h"
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;
using namespace rt::native::gameplay;

static int failures, groups, green;
static void check(std::string_view name, bool ok, std::string_view why = {}) {
    ++groups;
    if (ok) { ++green; std::printf("PASS %.*s\n", int(name.size()), name.data()); }
    else std::fprintf(stderr, "FAIL %.*s: %.*s\n", int(name.size()), name.data(), int(why.size()), why.data()), ++failures;
}
static std::string read(const fs::path& path) { std::ifstream in(path, std::ios::binary); return {std::istreambuf_iterator<char>(in), {}}; }

// Blank comments but retain line positions; quoted text is deliberately untouched.
static std::string stripComments(std::string s) {
    for (size_t i = 0; i + 1 < s.size();) {
        if (s[i] == '"' || s[i] == '\'') {
            const char quote = s[i++];
            while (i < s.size()) { if (s[i++] == '\\' && i < s.size()) ++i; else if (s[i - 1] == quote) break; }
        } else if (s[i] == '/' && s[i + 1] == '/') {
            while (i < s.size() && s[i] != '\n') s[i++] = ' ';
        } else if (s[i] == '/' && s[i + 1] == '*') {
            s[i++] = s[i++] = ' ';
            while (i + 1 < s.size() && !(s[i] == '*' && s[i + 1] == '/')) { if (s[i] != '\n') s[i] = ' '; ++i; }
            if (i + 1 < s.size()) s[i++] = s[i++] = ' ';
        } else ++i;
    }
    return s;
}
static void utf8(std::string& out, unsigned cp) {
    if (cp < 0x80) out += char(cp);
    else if (cp < 0x800) { out += char(0xc0 | cp >> 6); out += char(0x80 | (cp & 0x3f)); }
    else if (cp < 0x10000) { out += char(0xe0 | cp >> 12); out += char(0x80 | ((cp >> 6) & 0x3f)); out += char(0x80 | (cp & 0x3f)); }
    else { out += char(0xf0 | cp >> 18); out += char(0x80 | ((cp >> 12) & 0x3f)); out += char(0x80 | ((cp >> 6) & 0x3f)); out += char(0x80 | (cp & 0x3f)); }
}
static int digit(char c) { if (c >= '0' && c <= '9') return c - '0'; if (c >= 'a' && c <= 'f') return c - 'a' + 10; if (c >= 'A' && c <= 'F') return c - 'A' + 10; return -1; }
static std::string decode(std::string_view raw) {
    std::string out;
    for (size_t i = 0; i < raw.size(); ++i) {
        if (raw[i] != '\\') { out += raw[i]; continue; }
        if (++i == raw.size()) break;
        const char e = raw[i];
        if (e == 'x') { unsigned v = 0; int d; bool any = false; while (i + 1 < raw.size() && (d = digit(raw[i + 1])) >= 0) { v = (v << 4) | unsigned(d); ++i; any = true; } if (any) out += char(v & 0xff); }
        else if (e == 'u' || e == 'U') { const int n = e == 'u' ? 4 : 8; unsigned v = 0; bool ok = i + size_t(n) < raw.size(); for (int j = 0; ok && j < n; ++j) { const int d = digit(raw[i + 1 + j]); if (d < 0) ok = false; else v = (v << 4) | unsigned(d); } if (ok) { utf8(out, v); i += n; } }
        else if (e >= '0' && e <= '7') { unsigned v = unsigned(e - '0'); for (int n = 0; n < 2 && i + 1 < raw.size() && raw[i + 1] >= '0' && raw[i + 1] <= '7'; ++n) v = (v << 3) | unsigned(raw[++i] - '0'); out += char(v); }
        else { switch (e) { case 'n': out += '\n'; break; case 't': out += '\t'; break; case 'r': out += '\r'; break; case '"': out += '"'; break; case '\\': out += '\\'; break; default: out += e; } }
    }
    return out;
}
struct Literal { int line; size_t end; std::string value; };
static bool inRanges(int line, const std::vector<std::pair<int, int>>& ranges) { for (auto [a, b] : ranges) if (a <= line && line <= b) return true; return false; }
static std::vector<std::string> literals(const fs::path& path, const std::vector<std::pair<int, int>>& ranges) {
    const std::string s = stripComments(read(path)); std::vector<std::string> out; int line = 1;
    for (size_t i = 0; i < s.size();) {
        if (s[i] == '\n') { ++line; ++i; continue; }
        if (s[i] != '"') { ++i; continue; }
        const int first = line; std::string joined;
        do {
            ++i; std::string raw;
            while (i < s.size() && s[i] != '"') { if (s[i] == '\\' && i + 1 < s.size()) raw += s[i++]; if (s[i] == '\n') ++line; raw += s[i++]; }
            if (i < s.size()) ++i;
            joined += decode(raw);
            size_t j = i; while (j < s.size() && std::isspace(static_cast<unsigned char>(s[j]))) ++j;
            if (j >= s.size() || s[j] != '"') break;
            i = j;
        } while (true);
        if (inRanges(first, ranges) && !joined.empty()) out.push_back(joined);
    }
    return out;
}
static std::string normalize(std::string s) {
    for (char& c : s) if (c == '\n' || c == '\t') c = ' ';
    for (size_t p; (p = s.find("\xE2\x80\xA6")) != std::string::npos;) s.replace(p, 3, "...");
    for (size_t p; (p = s.find("\xE2\x80\x94")) != std::string::npos;) s.replace(p, 3, " - ");
    for (size_t p; (p = s.find("\xC2\xB0" "C")) != std::string::npos;) s.replace(p, 3, " deg C");
    return s;
}
static std::set<std::string> uniqueNormalized(const std::vector<std::string>& source) { std::set<std::string> out; for (const auto& s : source) if (!s.empty()) out.insert(normalize(s)); return out; }
static bool has(const std::vector<const OptionSpec*>& rows, std::string_view label) { return std::any_of(rows.begin(), rows.end(), [&](const OptionSpec* row) { return row->label == label; }); }
static std::string displayToken(std::string_view id, std::string_view token) {
    if (id == "sound.radio_station") { if (token == "off") return "Off"; if (token == "peach_fm") return "PEACH FM"; if (token == "e_radio") return "E-RADIO"; }
    return std::string(token);
}
static std::set<std::string> retailStrings(const OptionsModel& model) {
    std::set<std::string> out;
    for (const auto& row : model.options()) if (row.retail) { out.insert(row.label); for (const auto& token : row.domain) out.insert(displayToken(row.id, token)); }
    return out;
}
static std::set<std::string> nativeLiterals(const fs::path& root, std::initializer_list<fs::path> owners) {
    std::set<std::string> out;
    const auto collect = [&](const fs::path& file) {
        if (file.extension() != ".cpp" && file.extension() != ".h") return;
        const auto parsed = literals(file, {{1, 1000000}});
        const auto normalized = uniqueNormalized(parsed);
        out.insert(normalized.begin(), normalized.end());
    };
    for (const auto& owner : owners) {
        const fs::path path = root / owner;
        if (fs::is_regular_file(path)) { collect(path); continue; }
        if (!fs::is_directory(path)) continue;
        for (const auto& entry : fs::recursive_directory_iterator(path)) if (entry.is_regular_file()) collect(entry.path());
    }
    return out;
}
static void compareSurface(std::string_view group, const std::set<std::string>& expected, const std::set<std::string>& actual, std::string_view reason) {
    std::vector<std::string> missing;
    for (const auto& s : expected) if (!actual.contains(s)) missing.push_back(s);
    if (!missing.empty()) {
        std::fprintf(stderr, "MISSING %.*s (%zu):", int(group.size()), group.data(), missing.size());
        for (const auto& s : missing) std::fprintf(stderr, "\n  %s", s.c_str());
        std::fputc('\n', stderr);
    }
    check(group, missing.empty(), reason);
}

static void manifestAndFonte(const fs::path& root) {
    const std::array<std::pair<const char*, std::vector<std::pair<int,int>>>, 7> files{{
        {"src/ui/PauseMenu.cpp", {{293,303},{324,365},{397,768},{813,1048},{1796,2089}}}, {"src/game/GameOptions.cpp", {{59,67}}}, {"src/ui/SetupScreen.cpp", {{264,405}}}, {"src/ui/SecondScreen.cpp", {{174,245},{323,623},{626,824}}}, {"src/ui/MapView.cpp", {{40,191}}}, {"src/ui/PerfOverlay.cpp", {{19,96}}}, {"src/platform/Controllers.h", {{3,78}}}}};
    std::vector<std::string> all; for (const auto& [path, ranges] : files) { auto v = literals(root / path, ranges); all.insert(all.end(), v.begin(), v.end()); }
    const auto manifest = uniqueNormalized(all);
    for (const auto& [path, ranges] : files) std::printf("manifest-class %s: %zu unique\n", path, uniqueNormalized(literals(root / path, ranges)).size());
    constexpr size_t kReconciledTemplateCount = 301; // Set after the first corrected parser run; see PARITY_TESTS.md amendment.
    std::printf("manifest: %zu entries, %zu unique templates (ui-parity.md declares 184)\n", all.size(), manifest.size());
    check("manifest/declared-reconciled-count", manifest.size() == kReconciledTemplateCount, "corrected template count changed; update reconciliation, not exclusions");

    // These are row/value templates, not the prose hints in the same source range.
    const std::set<std::string> optionRowsAndValues{
        "Display mode", "Window", "Borderless fullscreen", "Fullscreen", "Window size", "Aspect ratio", "4:3 (original)", "HUD position", "4:3 centred", "Screen edges", "Map", "Enhanced", "Original", "Frame rate", "60 fps (original)", "120 fps (generated)", "240 fps (generated)", "Frame generation", "Re-rendered (no added latency)", "Warped, interpolated (+1 refresh latency)", "Warped, extrapolated (edge artefacts)", "Frame skip", "Auto", "Off", "Performance overlay", "Frame rate", "Detailed", "Second screen", "On", "Volume", "Speaker", "Mono", "Stereo", "Radio station", "PEACH FM", "E-RADIO"};
    const auto retail = retailStrings(OptionsModel{});
    // Dynamic dimensions/rates/percentages and capability-only values are substitutions per ui-parity.md:90.
    std::set<std::string> expected; for (const auto& s : optionRowsAndValues) expected.insert(s);
    std::set<std::string> actual; for (const auto& s : retail) if (expected.contains(s)) actual.insert(s);
    compareSurface("manifest/options-sound-retail", expected, actual, "retail OptionsModel rows/values differ from PauseMenu.cpp:397-542; radio tokens must map to display spelling");

    const auto setup = uniqueNormalized(literals(root / "src/ui/SetupScreen.cpp", {{264,405}}));
    const auto second = uniqueNormalized(literals(root / "src/ui/SecondScreen.cpp", {{174,245},{323,623},{626,824}}));
    const auto perf = uniqueNormalized(literals(root / "src/ui/PerfOverlay.cpp", {{19,96}}));
    // Controllers.h:3-78 contains declarations/comments, not render literals; the recomp controller surface is PauseMenu.cpp:813-872.
    const std::set<std::string> controllers{"Devices", "Driving", "Controls", "Vibration", "Strength", "Connect a controller and it appears here."};
    const auto nativeSetup = nativeLiterals(root, {"src/native/app", "src/native/assets/setup_wizard.cpp", "src/native/assets/setup_wizard.h", "src/native/preview/title_ui.cpp", "src/native/preview/title_ui.h"});
    const auto nativeSecond = nativeLiterals(root, {"src/native/preview", "src/native/gameplay/map_view.cpp", "src/native/gameplay/map_view.h"});
    const auto nativePerf = nativeLiterals(root, {"src/native/preview", "src/native/gameplay/perfstats.cpp", "src/native/gameplay/perfstats.h", "src/native/app"});
    const auto nativeControllers = nativeLiterals(root, {"src/native/preview/ui_menu.cpp", "src/native/preview/ui_menu.h", "src/native/gameplay/settings.cpp", "src/native/gameplay/settings.h"});
    compareSurface("manifest/setup-backlog", setup, nativeSetup, "native setup surface has not yet exposed recomp strings");
    compareSurface("manifest/second-screen-backlog", second, nativeSecond, "native second-screen surface has not yet exposed recomp strings");
    compareSurface("manifest/perf-overlay-backlog", perf, nativePerf, "native overlay surface has not yet exposed recomp strings");
    compareSurface("manifest/controllers-language-backlog", controllers, nativeControllers, "native controller-language surface has not yet exposed recomp strings");

    bool ascii = true, slots = true;
    for (const auto& v : manifest) for (unsigned char c : v) { ascii &= c == ' ' || (c >= 0x21 && c <= 0x7e); slots &= c == ' ' || rt::native::assets::fonteSlotForChar(char(c)) >= 0; }
    check("fonte/manifest-ascii", ascii, "not ASCII after only the four approved substitutions");
    check("fonte/slot-or-space-advance", slots, "byte has no FONTE slot or space advance");
}

static void adapters() {
    OptionsModel model; MenuContext rich{.gamepadPresent=true,.secondScreenSupported=true,.adventure=true,.hardwareGs=true};
    const auto display = model.pageRows(OptionPage::Display, rich);
    check("adapter/rich-recomp-display-order", display.size() == 9 && display[0]->label == "Display mode", "OptionsModel catalog is not recomp Display order");
    bool pending = true; for (const auto& row : model.options()) if (row.status == OptionStatus::GpuWavePending) for (auto page : {OptionPage::Display,OptionPage::Graphics,OptionPage::Controllers,OptionPage::Sound}) pending &= !has(model.pageRows(page,rich), row.label);
    check("adapter/pending-gpu-hidden", pending, "GpuWavePending row exposed; ui-parity.md:54 requires hidden");
    auto android=rich; android.secondScreenSupported=false; check("adapter/android-no-display", !has(model.pageRows(OptionPage::Display,android),"Second screen"), "second screen visible");
    auto noPad=rich; noPad.gamepadPresent=false; check("adapter/no-pad", !model.pageAvailable(OptionPage::Controllers,noPad), "Controllers page does not model recomp no-pad state");
    auto noAdventure=rich; noAdventure.adventure=false; check("adapter/non-adventure", !has(model.pageRows(OptionPage::Sound,noAdventure),"Radio station"), "radio visible");
    auto movie=rich; movie.moviePlaying=true; check("adapter/movie", model.pageRows(OptionPage::Save,movie).empty(), "save visible during movie");
    SettingsStore store{fs::temp_directory_path()/"w14_ui_parity.toml"}; auto* start=model.find("display.start_state"); MenuNavigator a(model,store,rich),b(model,store,rich); a.open(); b.open();
    const bool deterministic=start&&model.valueOf(*start,store)=="title"&&model.adjustValue(*start,store,-1)&&model.valueOf(*start,store)=="playing"&&a.focusedRow().id==b.focusedRow().id&&a.moveFocus(1)&&b.moveFocus(1)&&a.focusedRow().id==b.focusedRow().id;
    check("adapter/internal-id-wrap-skip-focus", deterministic, "mapping/wrap/focus not deterministic");
}
static void geometry(const fs::path& root) {
    auto scale=[](float h,float b=1){return std::max(.5f,h/1080*b);}; check("geometry/theme-scale-pause",scale(1080)==1&&scale(2160)==2&&520*scale(2160)==1040&&80*scale(2160)==160,"S/Pause dimensions differ from ui-parity.md:8,29,113");
    auto q=rt::native::preview::fonteTextQuads("A",100,50,{1,1,1,1},1920,1080); check("geometry/640x448-mapping",!q.empty()&&q[0].x==465.f&&q[0].y==148.5f,"not the centered uniform mapping");
    const std::array<std::string_view,16> hex{{"485800F2","C05B1E","F86018","F8BF84","0098C8","8ED4F0","0F5E92","F6E68F","ECECEC","2C6CE8","0838B0","F8C838","F2C230","304090F2","91D2E8","000A2899"}}; std::string source; for(const auto&e:fs::recursive_directory_iterator(root/"src/native"))if(e.path().extension()==".h"||e.path().extension()==".cpp")source+=read(e.path()); bool colors=true;for(auto h:hex)colors&=source.find(h)!=std::string::npos;check("theme/native-role-colors",colors,"native source lacks exact recomp Theme constants");
}
int main(){const fs::path root=RT_W14_REPO_ROOT;manifestAndFonte(root);adapters();geometry(root);std::printf("w14_ui_parity: %d/%d groups green\n",green,groups);return failures?1:0;}
