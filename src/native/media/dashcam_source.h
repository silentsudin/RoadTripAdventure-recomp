#pragma once

// Dashcam footage source pipeline (W4-05).
//
// GROUNDING SUMMARY (full evidence + transcripts in HANDOFF-W4-05.md):
//
//  * The PS2 game ships NO video footage. PROVEN negative, three probes over the
//    user's SLUS_203.98 and disc listing:
//      - ELF string table (both PT_LOAD segments, 16,399 strings): the only disc
//        file names are the eight `cdrom0:\*.IRX;1` / `IOPRP234.IMG;1` strings at
//        0x2E9A58..0x2E9B00; no .PSS/.IPU/.M2V/.AVI/.STR/.MPG name, no "sceMpeg".
//        Pack files are reached through the ROM LBA descriptor table (0x29BB50,
//        field_tile_index), never by name.
//      - .text scan for IPU access: zero `lui`-based loads/stores to the IPU
//        register block 0x10002000..0x1000203F or its DMA channels 3/4
//        (0x1000B000, 0x1000B400). The same scanner finds 23 VIF1-DMA and 27
//        GIF-DMA accesses (positive control), so the zero is meaningful.
//      - Disc listing (.lbn_map.tsv, 376 entries): BIN/GSL/TSQ/IRX/VAG/TVB/E3D/
//        ICO/IMG/CNF only; no movie container.
//  * The in-game "replay" is NOT footage. It is a deterministic re-simulation:
//      - SaveControlStateForReplay 0x21CBC8 snapshots the 0x3A80-byte car-control
//        table gs_controls_car_object 0x177AC50..cameraBlocks 0x177E6D0 into EE
//        scratch RAM 0x100000 and rewinds three stream cursors (task +0x44/+0x48/
//        +0x4C -> 0x103A84/0x103A88/0x103A8C).
//      - AcquireVehicleInput 0x21A990 records (game flag +0x28 bit 0x20) / plays
//        back (bit 0x40) a run-length input stream per player: 12-byte stride,
//        {u16 runLength, u16 input}, streams interleaved at +0/+4/+8 so they
//        never collide; capacity ends at 0x1FFFF8 (recording stops, bit 0x20
//        cleared). RestoreControlStateFromReplay 0x21CCB8 prints
//        "Replay Size[0..2] %d" (0x2EAE90..0x2EAEC0).
//      - UpdateRaceGameTask 0x20F3B8 starts playback after a race with
//        gs_music_play_bgm(0x0A180201) = SOUND/REPLAY.TSQ and flags 0x10044.
//      - It lives in RAM only: never written to the memory card (save block is
//        13,384 bytes; the replay arena is ~1 MiB).
//  * Therefore "dashcam footage" in this project is developer-recorded host
//    video of the oracle (e.g. a 1920x1080 H.264 screen capture of the
//    PS2Recomp app), copied by the user/dev into the app's files dir. Nothing is
//    extracted from the ROM because the ROM contains none.
//
// ROUTE KEY (PROVEN): RouteId = the course pack-type byte that
// StartRaceModeTask 0x20F950 copies from byte 0 of the selected 16-byte race
// record into game-state +0x22 (docs/native-reference/nav-index-binding.md).
// Values 0..17 are the contiguous COURSE slots C00..C14, C18, C19, C20 (no C15,
// C17 on disc; C16 is an orphan). Every adventure (0x2BDDD0, 35 records) and
// quick-race (0x2BE158, 8 records) entry carries a type in 0..17
// (kAdventureRaceRoute / kQuickRaceRoute below, verified against the ELF by
// tests/native_w5/dashcam_source_test.cpp when the ROM is present).
//
// FILE NAMING (AUTHORED, UNPROVEN-BY-ROM - there is no ROM mapping to prove):
//   <dashcamDir>/<STEM>[_<n>].<ext>, STEM = disc pack stem ("C00".."C20"),
//   n = 1..99 optional take number, ext in kClipExtensions, stem match is
//   case-insensitive. Example: dashcam/C00.mp4, dashcam/C00_2.mov.
//
// Dependency-free C++17. ffmpeg/ffprobe are used only as subprocesses (popen),
// never linked.

#include <array>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace rt::native::media {

// ---------------------------------------------------------------- fit constants
// Dashcam panel fit in the native HUD. The footage is 1920x1080; a panel-local
// point p (HUD design px) samples source pixel
//     src = p * kDashcamScale + (kDashcamXOff, kDashcamYOff)
// Verified against the oracle at 73.3% IoU (kDashcamFitIoU); refinement pending.
// The direction (panel -> source) is the one under which both legacy forms are
// self-consistent (448 * 2.4107 = 1080, (1920 - 640 * 2.4107) / 2 = 188.6).
//
// LEGACY MAPPINGS ARE WRONG AND MUST NOT BE USED:
//   - scale 2.25 + 54 px  (never subtracted the ~54 shot-px letterbox bar;
//     pushed the town pill ~24 design px low, see main.cpp minimap comment and
//     docs/native-reference/minimap.md §0.4)
//   - scale 2.4107 + xoff 188
inline constexpr int kDashcamSrcW = 1920;
inline constexpr int kDashcamSrcH = 1080;
inline constexpr float kDashcamScale = 1.5f;
inline constexpr float kDashcamXOff = 0.0f;
inline constexpr float kDashcamYOff = 317.0f;   // "~317": refinement pending
inline constexpr float kDashcamFitIoU = 0.733f; // vs oracle

struct SrcRect {
    float x = 0, y = 0, w = 0, h = 0;
};

constexpr float dashcamSrcX(float panelX) { return panelX * kDashcamScale + kDashcamXOff; }
constexpr float dashcamSrcY(float panelY) { return panelY * kDashcamScale + kDashcamYOff; }
constexpr float dashcamPanelX(float srcX) { return (srcX - kDashcamXOff) / kDashcamScale; }
constexpr float dashcamPanelY(float srcY) { return (srcY - kDashcamYOff) / kDashcamScale; }
// Source crop a panelW x panelH panel samples, clipped to the 1920x1080 frame.
SrcRect dashcamSourceCrop(float panelW, float panelH);

// ---------------------------------------------------------------- route table
using RouteId = std::uint8_t;

inline constexpr RouteId kCourseRouteCount = 18; // COURSE slots 0..17

struct DashcamRoute {
    RouteId id;           // pack-type byte (game state +0x22)
    const char* packStem; // disc pack COURSE/<stem>.BIN and footage stem
};

// nav-index-binding.md "Static directory: type/index -> pack" rows 0..17.
inline constexpr std::array<DashcamRoute, kCourseRouteCount> kDashcamRoutes{{
    {0, "C00"},  {1, "C01"},  {2, "C02"},  {3, "C03"},  {4, "C04"},  {5, "C05"},
    {6, "C06"},  {7, "C07"},  {8, "C08"},  {9, "C09"},  {10, "C10"}, {11, "C11"},
    {12, "C12"}, {13, "C13"}, {14, "C14"}, {15, "C18"}, {16, "C19"}, {17, "C20"},
}};

// Byte 0 of each 16-byte race record (gs_game_race_tables_adventure 0x2BDDD0,
// index < 0x23 per StartRaceModeTask 0x20F950; gs_game_race_tables_quick_race
// 0x2BE158). Read from SLUS_203.98 (file offset = vaddr - 0x1FF000).
inline constexpr std::array<RouteId, 35> kAdventureRaceRoute{{
    0, 0, 1, 3, 2, 2, 4, 4, 5, 5, 7, 7, 12, 12, 10, 8, 9, 8,
    6, 11, 13, 13, 14, 14, 17, 15, 17, 16, 1, 2, 5, 11, 12, 14, 8,
}};
inline constexpr std::uint32_t kAdventureRaceTableVaddr = 0x2BDDD0;
inline constexpr std::array<RouteId, 8> kQuickRaceRoute{{0, 2, 5, 6, 9, 14, 16, 17}};
inline constexpr std::uint32_t kQuickRaceTableVaddr = 0x2BE158;
inline constexpr std::uint32_t kRaceRecordStride = 0x10;

bool isCourseRoute(RouteId id);
// "C00".."C20" for course routes, empty for anything else.
std::string_view packStemFor(RouteId id);
std::optional<RouteId> routeForStem(std::string_view stem); // case-insensitive
std::optional<RouteId> routeForAdventureRace(int raceIndex);
std::optional<RouteId> routeForQuickRace(int raceIndex);

// Accepted footage containers, priority order for pathFor().
inline constexpr std::array<std::string_view, 5> kClipExtensions{{".mp4", ".mov", ".mkv", ".webm", ".m4v"}};
inline constexpr int kMaxTakeNumber = 99;

// Parses "<STEM>[_<n>].<ext>"; returns route + take (0 = untagged primary).
struct ClipName {
    RouteId route;
    int take;
};
std::optional<ClipName> parseClipName(std::string_view fileName);
// Canonical file name for a take: "C00.mp4", "C00_2.mp4".
std::string canonicalClipName(RouteId id, int take = 0, std::string_view ext = ".mp4");

// ---------------------------------------------------------------- directory
// Android first-run target: the DEV build's app files dir. Never the release id.
inline constexpr std::string_view kAndroidDevPackage = "io.github.silentsudin.roadtrip.dev";
inline constexpr std::string_view kAndroidReleasePackage = "io.github.silentsudin.roadtrip";
inline constexpr std::string_view kAndroidDevFilesDir = "/data/data/io.github.silentsudin.roadtrip.dev/files";
inline constexpr std::string_view kDashcamSubdir = "dashcam";

enum class HostPlatform { Desktop, Android };

// Injectable view of the environment so resolution is testable. fromProcess()
// fills it from getenv (RT_DASHCAM_DIR, RT_SAVE_DIR, RT_DATA_DIR, HOME,
// XDG_DATA_HOME, APPDATA) and the compile target.
struct DashcamEnv {
    HostPlatform platform = HostPlatform::Desktop;
    std::optional<std::string> dashcamDir;      // RT_DASHCAM_DIR: explicit override
    std::optional<std::string> androidFilesDir; // Context.getFilesDir() (SDL internal storage)
    std::optional<std::string> saveDir;         // RT_SAVE_DIR
    std::optional<std::string> dataDir;         // RT_DATA_DIR
    std::optional<std::string> home;            // HOME
    std::optional<std::string> xdgDataHome;     // XDG_DATA_HOME
    std::optional<std::string> appData;         // APPDATA
    static DashcamEnv fromProcess();
};

// True when any path component is exactly the release application id.
bool pathUsesReleasePackage(const std::filesystem::path& p);

// Resolution order:
//  1. RT_DASHCAM_DIR (verbatim).
//  2. Android: <androidFilesDir>/dashcam if supplied and not the release
//     package, else kAndroidDevFilesDir/dashcam.
//  3. Desktop: $RT_SAVE_DIR/dashcam.
//  4. Desktop app-local: <app data dir>/dashcam, app data dir = RT_DATA_DIR,
//     else macOS ~/Library/Application Support/RoadTripRecomp, Windows
//     %APPDATA%/RoadTripRecomp, other $XDG_DATA_HOME|~/.local/share/RoadTripRecomp.
//  5. ./dashcam (relative to the working directory).
std::filesystem::path resolveDashcamDir(const DashcamEnv& env);

// ---------------------------------------------------------------- clip index
enum class Availability { NotACourse, Missing, Present };

struct ClipEntry {
    RouteId route;
    int take;
    std::filesystem::path path;
};

class DashcamLibrary {
public:
    explicit DashcamLibrary(std::filesystem::path directory);
    const std::filesystem::path& directory() const { return directory_; }

    // Re-scans the directory (non-recursive). Missing dir is not an error.
    void rescan();
    // Clips for a route, sorted by take then extension priority.
    std::vector<ClipEntry> clipsFor(RouteId id) const;
    Availability availability(RouteId id) const;
    // Primary clip (lowest take, best extension) or empty path.
    std::filesystem::path pathFor(RouteId id) const;
    std::size_t clipCount() const { return clips_.size(); }

    // First-run import: copies a developer recording into the canonical name.
    // Creates the directory. Refuses release-package paths and overwrites
    // unless `overwrite`. Returns the destination or nullopt (+error).
    std::optional<std::filesystem::path> importClip(RouteId id, const std::filesystem::path& source,
                                                    int take = 0, bool overwrite = false,
                                                    std::string* error = nullptr);

private:
    std::filesystem::path directory_;
    std::vector<ClipEntry> clips_;
};

// ---------------------------------------------------------------- frame provider
struct ToolPaths {
    std::filesystem::path ffmpeg;
    std::filesystem::path ffprobe;
};

// Searches RT_FFMPEG / RT_FFPROBE overrides then PATH. Empty members if absent.
ToolPaths locateTools();
ToolPaths locateTools(const std::optional<std::string>& pathVar, const std::optional<std::string>& ffmpegOverride,
                      const std::optional<std::string>& ffprobeOverride);

struct ClipInfo {
    int width = 0;
    int height = 0;
    double fps = 0.0;
    double durationSec = 0.0;
    long long frameCount = 0; // 0 when the container does not declare it
};

struct RgbaFrame {
    int width = 0;
    int height = 0;
    std::vector<std::uint8_t> pixels; // width*height*4, row-major, top-down
    bool valid() const { return width > 0 && height > 0 && pixels.size() == std::size_t(width) * height * 4; }
};

// Fallback policy when footage cannot be shown:
//   - route not a course, or no clip on disk          -> Skip (panel not drawn)
//   - clip present but ffmpeg/ffprobe absent or the
//     decode fails                                    -> PlaceholderStill
//     (solid kPlaceholderRgba fill of the panel rect, no text, so HUD layout
//      and frame comparisons stay stable)
//   - clip present and runtime available              -> Footage
enum class Presentation { Footage, PlaceholderStill, Skip };
inline constexpr std::array<std::uint8_t, 4> kPlaceholderRgba{{24, 24, 28, 255}};
Presentation decidePresentation(Availability availability, bool runtimeAvailable);
RgbaFrame placeholderFrame(int width, int height);

class FfmpegFrameProvider {
public:
    FfmpegFrameProvider(); // locateTools()
    explicit FfmpegFrameProvider(ToolPaths tools);

    bool isRuntimeAvailable() const;
    const ToolPaths& tools() const { return tools_; }

    std::optional<ClipInfo> probe(const std::filesystem::path& clip, std::string* error = nullptr) const;
    // Decodes the frame at timeSec, scaled to outW x outH (<=0: native size via
    // probe). Returns an invalid frame on any failure.
    RgbaFrame frameAt(const std::filesystem::path& clip, double timeSec, int outW = 0, int outH = 0,
                      std::string* error = nullptr) const;
    // Decodes the dashcam panel crop (dashcamSourceCrop) scaled to the panel.
    RgbaFrame panelFrameAt(const std::filesystem::path& clip, double timeSec, int panelW, int panelH,
                           std::string* error = nullptr) const;
    // Full policy: footage when possible, placeholder otherwise, empty on Skip.
    RgbaFrame presentRoute(const DashcamLibrary& library, RouteId id, double timeSec, int panelW, int panelH,
                           Presentation* decided = nullptr) const;

private:
    RgbaFrame runRawVideo(const std::filesystem::path& clip, double timeSec, const std::string& filter, int outW,
                          int outH, std::string* error) const;
    ToolPaths tools_;
};

// Shell-quotes one argument for the host shell used by popen.
std::string shellQuote(std::string_view arg);
// Parses ffprobe "key=value" output (default writer, noprint_wrappers=1).
std::optional<ClipInfo> parseProbeOutput(std::string_view text);

} // namespace rt::native::media
