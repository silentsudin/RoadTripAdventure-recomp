#include "media/dashcam_source.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <system_error>

#if defined(_WIN32)
#define RT_DASHCAM_POPEN _popen
#define RT_DASHCAM_PCLOSE _pclose
#else
#include <unistd.h>
#define RT_DASHCAM_POPEN popen
#define RT_DASHCAM_PCLOSE pclose
#endif

namespace rt::native::media {
namespace fs = std::filesystem;

namespace {

std::string lower(std::string_view s) {
    std::string out(s);
    for (char& c : out) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return out;
}

std::optional<std::string> env(const char* name) {
    if (const char* v = std::getenv(name); v && *v) return std::string(v);
    return std::nullopt;
}

int extensionRank(std::string_view extLower) {
    for (std::size_t i = 0; i < kClipExtensions.size(); ++i)
        if (kClipExtensions[i] == extLower) return static_cast<int>(i);
    return -1;
}

bool isExecutableFile(const fs::path& p) {
    std::error_code ec;
    if (!fs::is_regular_file(p, ec)) return false;
#if defined(_WIN32)
    return true;
#else
    return ::access(p.c_str(), X_OK) == 0;
#endif
}

fs::path searchPath(const std::optional<std::string>& pathVar, std::string_view tool) {
    if (!pathVar) return {};
#if defined(_WIN32)
    const char sep = ';';
    const std::array<std::string, 2> names{std::string(tool) + ".exe", std::string(tool)};
#else
    const char sep = ':';
    const std::array<std::string, 1> names{std::string(tool)};
#endif
    std::size_t start = 0;
    const std::string& path = *pathVar;
    while (start <= path.size()) {
        std::size_t end = path.find(sep, start);
        if (end == std::string::npos) end = path.size();
        const std::string dir = path.substr(start, end - start);
        if (!dir.empty()) {
            for (const auto& name : names) {
                const fs::path candidate = fs::path(dir) / name;
                if (isExecutableFile(candidate)) return candidate;
            }
        }
        start = end + 1;
    }
    return {};
}

// Subprocess stderr is discarded so decode failures do not spam the app log;
// failures surface through the exit status and the error string instead.
#if defined(_WIN32)
constexpr const char* kQuietStderr = " 2>NUL";
#else
constexpr const char* kQuietStderr = " 2>/dev/null";
#endif

std::string fmtDouble(double v) {
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.6f", v);
    return buf;
}

// Runs a command and returns stdout bytes; nullopt when the process fails.
std::optional<std::string> runCapture(const std::string& command, std::size_t expectBytes, std::string* error) {
    const std::string quiet = command + kQuietStderr;
#if defined(_WIN32)
    FILE* pipe = RT_DASHCAM_POPEN(quiet.c_str(), "rb");
#else
    FILE* pipe = RT_DASHCAM_POPEN(quiet.c_str(), "r");
#endif
    if (!pipe) {
        if (error) *error = "popen failed";
        return std::nullopt;
    }
    std::string out;
    if (expectBytes) out.reserve(expectBytes);
    char buf[1 << 16];
    std::size_t n;
    while ((n = std::fread(buf, 1, sizeof(buf), pipe)) > 0) out.append(buf, n);
    const int status = RT_DASHCAM_PCLOSE(pipe);
    if (status != 0) {
        if (error) *error = "subprocess exit status " + std::to_string(status);
        return std::nullopt;
    }
    return out;
}

} // namespace

// ---------------------------------------------------------------- fit
SrcRect dashcamSourceCrop(float panelW, float panelH) {
    float x0 = dashcamSrcX(0.f), y0 = dashcamSrcY(0.f);
    float x1 = dashcamSrcX(panelW), y1 = dashcamSrcY(panelH);
    x0 = std::clamp(x0, 0.f, float(kDashcamSrcW));
    x1 = std::clamp(x1, 0.f, float(kDashcamSrcW));
    y0 = std::clamp(y0, 0.f, float(kDashcamSrcH));
    y1 = std::clamp(y1, 0.f, float(kDashcamSrcH));
    return {x0, y0, std::max(0.f, x1 - x0), std::max(0.f, y1 - y0)};
}

// ---------------------------------------------------------------- routes
bool isCourseRoute(RouteId id) { return id < kCourseRouteCount; }

std::string_view packStemFor(RouteId id) {
    return isCourseRoute(id) ? std::string_view(kDashcamRoutes[id].packStem) : std::string_view();
}

std::optional<RouteId> routeForStem(std::string_view stem) {
    const std::string want = lower(stem);
    for (const auto& r : kDashcamRoutes)
        if (lower(r.packStem) == want) return r.id;
    return std::nullopt;
}

std::optional<RouteId> routeForAdventureRace(int raceIndex) {
    if (raceIndex < 0 || raceIndex >= int(kAdventureRaceRoute.size())) return std::nullopt;
    return kAdventureRaceRoute[std::size_t(raceIndex)];
}

std::optional<RouteId> routeForQuickRace(int raceIndex) {
    if (raceIndex < 0 || raceIndex >= int(kQuickRaceRoute.size())) return std::nullopt;
    return kQuickRaceRoute[std::size_t(raceIndex)];
}

std::optional<ClipName> parseClipName(std::string_view fileName) {
    const std::size_t dot = fileName.rfind('.');
    if (dot == std::string_view::npos || dot == 0) return std::nullopt;
    if (extensionRank(lower(fileName.substr(dot))) < 0) return std::nullopt;
    std::string_view base = fileName.substr(0, dot);
    int take = 0;
    if (const std::size_t us = base.find('_'); us != std::string_view::npos) {
        const std::string_view digits = base.substr(us + 1);
        if (digits.empty() || digits.size() > 2) return std::nullopt;
        for (char c : digits)
            if (!std::isdigit(static_cast<unsigned char>(c))) return std::nullopt;
        take = std::atoi(std::string(digits).c_str());
        if (take < 1 || take > kMaxTakeNumber) return std::nullopt;
        base = base.substr(0, us);
    }
    const auto route = routeForStem(base);
    if (!route) return std::nullopt;
    return ClipName{*route, take};
}

std::string canonicalClipName(RouteId id, int take, std::string_view ext) {
    std::string name(packStemFor(id));
    if (name.empty()) return {};
    if (take > 0) name += "_" + std::to_string(take);
    name += lower(ext);
    return name;
}

// ---------------------------------------------------------------- directory
DashcamEnv DashcamEnv::fromProcess() {
    DashcamEnv e;
#if defined(__ANDROID__)
    e.platform = HostPlatform::Android;
#endif
    e.dashcamDir = env("RT_DASHCAM_DIR");
    e.saveDir = env("RT_SAVE_DIR");
    e.dataDir = env("RT_DATA_DIR");
    e.home = env("HOME");
    e.xdgDataHome = env("XDG_DATA_HOME");
    e.appData = env("APPDATA");
    return e;
}

bool pathUsesReleasePackage(const fs::path& p) {
    for (const auto& part : p)
        if (part.string() == kAndroidReleasePackage) return true;
    return false;
}

fs::path resolveDashcamDir(const DashcamEnv& e) {
    if (e.dashcamDir && !pathUsesReleasePackage(*e.dashcamDir)) return fs::path(*e.dashcamDir);
    if (e.platform == HostPlatform::Android) {
        if (e.androidFilesDir && !pathUsesReleasePackage(*e.androidFilesDir))
            return fs::path(*e.androidFilesDir) / kDashcamSubdir;
        return fs::path(kAndroidDevFilesDir) / kDashcamSubdir;
    }
    if (e.saveDir) return fs::path(*e.saveDir) / kDashcamSubdir;
    if (e.dataDir) return fs::path(*e.dataDir) / kDashcamSubdir;
#if defined(__APPLE__)
    if (e.home) return fs::path(*e.home) / "Library/Application Support/RoadTripRecomp" / kDashcamSubdir;
#elif defined(_WIN32)
    if (e.appData) return fs::path(*e.appData) / "RoadTripRecomp" / kDashcamSubdir;
#else
    if (e.xdgDataHome) return fs::path(*e.xdgDataHome) / "RoadTripRecomp" / kDashcamSubdir;
    if (e.home) return fs::path(*e.home) / ".local/share/RoadTripRecomp" / kDashcamSubdir;
#endif
    return fs::path(kDashcamSubdir);
}

// ---------------------------------------------------------------- library
DashcamLibrary::DashcamLibrary(fs::path directory) : directory_(std::move(directory)) { rescan(); }

void DashcamLibrary::rescan() {
    clips_.clear();
    std::error_code ec;
    if (!fs::is_directory(directory_, ec)) return;
    for (fs::directory_iterator it(directory_, ec), end; !ec && it != end; it.increment(ec)) {
        std::error_code fec;
        if (!it->is_regular_file(fec)) continue;
        const std::string name = it->path().filename().string();
        if (const auto parsed = parseClipName(name)) clips_.push_back({parsed->route, parsed->take, it->path()});
    }
    std::sort(clips_.begin(), clips_.end(), [](const ClipEntry& a, const ClipEntry& b) {
        if (a.route != b.route) return a.route < b.route;
        if (a.take != b.take) return a.take < b.take;
        const int ra = extensionRank(lower(a.path.extension().string()));
        const int rb = extensionRank(lower(b.path.extension().string()));
        if (ra != rb) return ra < rb;
        return a.path.filename().string() < b.path.filename().string();
    });
}

std::vector<ClipEntry> DashcamLibrary::clipsFor(RouteId id) const {
    std::vector<ClipEntry> out;
    for (const auto& c : clips_)
        if (c.route == id) out.push_back(c);
    return out;
}

Availability DashcamLibrary::availability(RouteId id) const {
    if (!isCourseRoute(id)) return Availability::NotACourse;
    for (const auto& c : clips_)
        if (c.route == id) return Availability::Present;
    return Availability::Missing;
}

fs::path DashcamLibrary::pathFor(RouteId id) const {
    for (const auto& c : clips_)
        if (c.route == id) return c.path;
    return {};
}

std::optional<fs::path> DashcamLibrary::importClip(RouteId id, const fs::path& source, int take, bool overwrite,
                                                   std::string* error) {
    const auto fail = [error](std::string msg) -> std::optional<fs::path> {
        if (error) *error = std::move(msg);
        return std::nullopt;
    };
    if (!isCourseRoute(id)) return fail("route is not a course pack (0..17)");
    if (take < 0 || take > kMaxTakeNumber) return fail("take out of range");
    if (pathUsesReleasePackage(directory_)) return fail("refusing release-package directory");
    const std::string ext = lower(source.extension().string());
    if (extensionRank(ext) < 0) return fail("unsupported clip extension '" + ext + "'");
    std::error_code ec;
    if (!fs::is_regular_file(source, ec)) return fail("source clip not found");
    fs::create_directories(directory_, ec);
    if (ec) return fail("cannot create " + directory_.string() + ": " + ec.message());
    const fs::path dest = directory_ / canonicalClipName(id, take, ext);
    if (fs::exists(dest, ec) && !overwrite) return fail("destination exists: " + dest.string());
    fs::copy_file(source, dest,
                  overwrite ? fs::copy_options::overwrite_existing : fs::copy_options::none, ec);
    if (ec) return fail("copy failed: " + ec.message());
    rescan();
    return dest;
}

// ---------------------------------------------------------------- tools
ToolPaths locateTools(const std::optional<std::string>& pathVar, const std::optional<std::string>& ffmpegOverride,
                      const std::optional<std::string>& ffprobeOverride) {
    ToolPaths t;
    if (ffmpegOverride && isExecutableFile(*ffmpegOverride)) t.ffmpeg = *ffmpegOverride;
    else t.ffmpeg = searchPath(pathVar, "ffmpeg");
    if (ffprobeOverride && isExecutableFile(*ffprobeOverride)) t.ffprobe = *ffprobeOverride;
    else t.ffprobe = searchPath(pathVar, "ffprobe");
    return t;
}

ToolPaths locateTools() { return locateTools(env("PATH"), env("RT_FFMPEG"), env("RT_FFPROBE")); }

std::string shellQuote(std::string_view arg) {
#if defined(_WIN32)
    std::string out = "\"";
    for (char c : arg) {
        if (c == '"') out += "\\\"";
        else out += c;
    }
    return out + "\"";
#else
    std::string out = "'";
    for (char c : arg) {
        if (c == '\'') out += "'\\''";
        else out += c;
    }
    return out + "'";
#endif
}

std::optional<ClipInfo> parseProbeOutput(std::string_view text) {
    ClipInfo info;
    std::istringstream in{std::string(text)};
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        const auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        const std::string key = line.substr(0, eq), value = line.substr(eq + 1);
        if (value.empty() || value == "N/A") continue;
        if (key == "width") info.width = std::atoi(value.c_str());
        else if (key == "height") info.height = std::atoi(value.c_str());
        else if (key == "nb_frames") info.frameCount = std::atoll(value.c_str());
        else if (key == "duration") info.durationSec = std::atof(value.c_str());
        else if (key == "r_frame_rate" || key == "avg_frame_rate") {
            const auto slash = value.find('/');
            double fps = 0;
            if (slash == std::string::npos) fps = std::atof(value.c_str());
            else {
                const double num = std::atof(value.substr(0, slash).c_str());
                const double den = std::atof(value.substr(slash + 1).c_str());
                fps = den > 0 ? num / den : 0;
            }
            if (key == "r_frame_rate" || info.fps <= 0) info.fps = fps;
        }
    }
    if (info.width <= 0 || info.height <= 0) return std::nullopt;
    return info;
}

// ---------------------------------------------------------------- policy
Presentation decidePresentation(Availability availability, bool runtimeAvailable) {
    if (availability != Availability::Present) return Presentation::Skip;
    return runtimeAvailable ? Presentation::Footage : Presentation::PlaceholderStill;
}

RgbaFrame placeholderFrame(int width, int height) {
    RgbaFrame f;
    if (width <= 0 || height <= 0) return f;
    f.width = width;
    f.height = height;
    f.pixels.resize(std::size_t(width) * height * 4);
    for (std::size_t i = 0; i < f.pixels.size(); i += 4)
        std::copy(kPlaceholderRgba.begin(), kPlaceholderRgba.end(), f.pixels.begin() + std::ptrdiff_t(i));
    return f;
}

// ---------------------------------------------------------------- provider
FfmpegFrameProvider::FfmpegFrameProvider() : tools_(locateTools()) {}
FfmpegFrameProvider::FfmpegFrameProvider(ToolPaths tools) : tools_(std::move(tools)) {}

bool FfmpegFrameProvider::isRuntimeAvailable() const {
    return !tools_.ffmpeg.empty() && !tools_.ffprobe.empty() && isExecutableFile(tools_.ffmpeg) &&
           isExecutableFile(tools_.ffprobe);
}

std::optional<ClipInfo> FfmpegFrameProvider::probe(const fs::path& clip, std::string* error) const {
    if (!isRuntimeAvailable()) {
        if (error) *error = "ffprobe unavailable";
        return std::nullopt;
    }
    std::error_code ec;
    if (!fs::is_regular_file(clip, ec)) {
        if (error) *error = "clip not found";
        return std::nullopt;
    }
    const std::string cmd = shellQuote(tools_.ffprobe.string()) +
                            " -v error -select_streams v:0"
                            " -show_entries stream=width,height,r_frame_rate,avg_frame_rate,nb_frames"
                            " -show_entries format=duration -of default=noprint_wrappers=1 " +
                            shellQuote(clip.string());
    const auto out = runCapture(cmd, 0, error);
    if (!out) return std::nullopt;
    auto info = parseProbeOutput(*out);
    if (!info && error) *error = "ffprobe output had no video stream";
    return info;
}

RgbaFrame FfmpegFrameProvider::runRawVideo(const fs::path& clip, double timeSec, const std::string& filter, int outW,
                                           int outH, std::string* error) const {
    const std::size_t expect = std::size_t(outW) * outH * 4;
    const std::string cmd = shellQuote(tools_.ffmpeg.string()) + " -v error -nostdin -ss " +
                            fmtDouble(std::max(0.0, timeSec)) + " -i " + shellQuote(clip.string()) +
                            " -frames:v 1 -vf " + shellQuote(filter) + " -f rawvideo -pix_fmt rgba -";
    const auto out = runCapture(cmd, expect, error);
    RgbaFrame f;
    if (!out) return f;
    if (out->size() != expect) {
        if (error) *error = "decoded " + std::to_string(out->size()) + " bytes, expected " + std::to_string(expect);
        return f;
    }
    f.width = outW;
    f.height = outH;
    f.pixels.assign(out->begin(), out->end());
    return f;
}

RgbaFrame FfmpegFrameProvider::frameAt(const fs::path& clip, double timeSec, int outW, int outH,
                                       std::string* error) const {
    if (!isRuntimeAvailable()) {
        if (error) *error = "ffmpeg unavailable";
        return {};
    }
    if (outW <= 0 || outH <= 0) {
        const auto info = probe(clip, error);
        if (!info) return {};
        outW = info->width;
        outH = info->height;
    }
    return runRawVideo(clip, timeSec, "scale=" + std::to_string(outW) + ":" + std::to_string(outH), outW, outH,
                       error);
}

RgbaFrame FfmpegFrameProvider::panelFrameAt(const fs::path& clip, double timeSec, int panelW, int panelH,
                                            std::string* error) const {
    if (!isRuntimeAvailable()) {
        if (error) *error = "ffmpeg unavailable";
        return {};
    }
    if (panelW <= 0 || panelH <= 0) {
        if (error) *error = "empty panel";
        return {};
    }
    const SrcRect r = dashcamSourceCrop(float(panelW), float(panelH));
    if (r.w <= 0 || r.h <= 0) {
        if (error) *error = "panel crop lies outside the source frame";
        return {};
    }
    // Crop expressed relative to iw/ih so clips that are not 1920x1080 still
    // take the same normalised window.
    char filter[256];
    std::snprintf(filter, sizeof(filter),
                  "crop=w=iw*%.6f:h=ih*%.6f:x=iw*%.6f:y=ih*%.6f,scale=%d:%d", r.w / kDashcamSrcW,
                  r.h / kDashcamSrcH, r.x / kDashcamSrcW, r.y / kDashcamSrcH, panelW, panelH);
    return runRawVideo(clip, timeSec, filter, panelW, panelH, error);
}

RgbaFrame FfmpegFrameProvider::presentRoute(const DashcamLibrary& library, RouteId id, double timeSec, int panelW,
                                            int panelH, Presentation* decided) const {
    Presentation p = decidePresentation(library.availability(id), isRuntimeAvailable());
    RgbaFrame frame;
    if (p == Presentation::Footage) {
        frame = panelFrameAt(library.pathFor(id), timeSec, panelW, panelH);
        if (!frame.valid()) p = Presentation::PlaceholderStill;
    }
    if (p == Presentation::PlaceholderStill) frame = placeholderFrame(panelW, panelH);
    if (decided) *decided = p;
    return frame;
}

} // namespace rt::native::media
