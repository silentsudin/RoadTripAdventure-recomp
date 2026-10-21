// Minimal SDL3 + Vulkan host for the authored GameLoop.  The scene itself is
// rendered by previewRenderScene(), so this presentation layer deliberately
// contains no duplicate road/car/shader code.
#include "assets/car_models.h"
#include "assets/field_converter.h"
#include "assets/field_tile_index.h"
#include "assets/glyph_atlas.h"
#include "assets/native_model.h"
#include "assets/setup_wizard.h"
#include "assets/title_screen.h"
#include "assets/hud_art.h"
#include "assets/nav_routes.h"
#include "audio/bgm_player.h"
#include "audio/engine.h"
#include "audio/vag_loader.h"
#include "gameplay/collectibles.h"
#include "gameplay/dialogue.h"
#include "gameplay/event_bridge.h"
#include "gameplay/input.h"
#include "gameplay/interp.h"
#include "gameplay/glyphs.h"
#include "gameplay/menu.h"
#include "gameplay/nav_drive.h"
#include "gameplay/perfstats.h"
#include "gameplay/radio.h"
#include "gameplay/rumble.h"
#include "gameplay/save_slots.h"
#include "app/enhancements_app.h"
#include "app/output_modes.h"
#include "gameplay/session.h"
#include "gameplay/settings.h"
#include "gameplay/shop.h"
#include "gameplay/tile_streamer.h"
#include "gameplay/world_layout.h"
#include "app/Autopilot.h"
#include "guest/guest_kernel.h"
#include "guest/guest_vif1.h"
#include "guest/guest_gif.h"
#include "guest/guest_gs.h"
#include "guest/guest_shim.h"

namespace guest { extern uint64_t g_diagCdReads, g_diagCdSectors; extern uint16_t g_bootButtons; void gsEnvCensusDump(); }
#include "preview/car_mesh.h"
#include "preview/preview.h"
#include "preview/present_gl.h"
#include "preview/preview_backend.h"
#include "preview/renderer_select.h"
#include "preview/race_hud.h"
#include "preview/rot_quad.h"
#include "preview/title_ui.h"
#include "preview/ui_menu.h"
#include "runtime/guest_facade.h"
#include "runtime/testsocket.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#ifdef __ANDROID__
#include <SDL3/SDL_main.h> // SDLActivity resolves this translation unit's SDL_main.
#include <SDL3/SDL_system.h>
#include <android/log.h>
#include <jni.h>
#endif
#include <vulkan/vulkan.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string_view>
#include <string>
#include <sys/resource.h>
#include <thread>
#include <unordered_map>
#include <vector>

namespace {
constexpr VkMemoryPropertyFlags kHostMemory = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

// The resolved family is app-global so future prompt renderers in this host
// can read appGlyphFamily() at draw time rather than cache a stale pad family.
rt::native::gameplay::GlyphFamily g_appGlyphFamily = rt::native::gameplay::GlyphFamily::Unknown;
rt::native::gameplay::GlyphFamily appGlyphFamily() { return g_appGlyphFamily; }

const char* glyphFamilyName(rt::native::gameplay::GlyphFamily family) {
    using rt::native::gameplay::GlyphFamily;
    switch (family) {
    case GlyphFamily::Playstation: return "ps";
    case GlyphFamily::XboxStyle: return "xbox";
    case GlyphFamily::Nintendo: return "nintendo";
    case GlyphFamily::Unknown: return "unknown";
    case GlyphFamily::Count: break;
    }
    return "unknown";
}

std::optional<rt::native::gameplay::GlyphFamily> forcedGlyphFamily() {
    const char* value = std::getenv("RT_APP_GLYPH_FAMILY");
    if (!value) return std::nullopt;
    const std::string family(value);
    if (family == "ps") return rt::native::gameplay::GlyphFamily::Playstation;
    if (family == "xbox") return rt::native::gameplay::GlyphFamily::XboxStyle;
    if (family == "nintendo") return rt::native::gameplay::GlyphFamily::Nintendo;
    return std::nullopt;
}

std::filesystem::path appDataDirectory() {
    if (const char* value = std::getenv("RT_DATA_DIR")) return value;
    if (const char* home = std::getenv("HOME"))
        return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp";
    return {};
}

void setDefaultEnv(const char* name, const std::string& value);

#ifdef __ANDROID__
void androidLog(int priority, const std::string& message) {
    __android_log_write(priority, "RoadTrip", message.c_str());
}

std::optional<std::filesystem::path> androidExternalFilesDirectory() {
    auto* activity = static_cast<jobject>(SDL_GetAndroidActivity());
    if (!activity) return std::nullopt;
    auto* env = static_cast<JNIEnv*>(SDL_GetAndroidJNIEnv());
    if (!env) return std::nullopt;
    const auto clearException = [env] { if (env->ExceptionCheck()) { env->ExceptionClear(); return true; } return false; };
    jclass activityClass = env->GetObjectClass(activity);
    jmethodID getExternalFilesDir = activityClass ? env->GetMethodID(activityClass, "getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;") : nullptr;
    jobject file = getExternalFilesDir ? env->CallObjectMethod(activity, getExternalFilesDir, nullptr) : nullptr;
    jclass fileClass = file ? env->GetObjectClass(file) : nullptr;
    jmethodID getAbsolutePath = fileClass ? env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;") : nullptr;
    jstring path = getAbsolutePath ? static_cast<jstring>(env->CallObjectMethod(file, getAbsolutePath)) : nullptr;
    std::optional<std::filesystem::path> result;
    if (!clearException() && path) {
        const char* chars = env->GetStringUTFChars(path, nullptr);
        if (chars) { result = std::filesystem::path(chars); env->ReleaseStringUTFChars(path, chars); }
    }
    if (path) env->DeleteLocalRef(path);
    if (fileClass) env->DeleteLocalRef(fileClass);
    if (file) env->DeleteLocalRef(file);
    if (activityClass) env->DeleteLocalRef(activityClass);
    return result;
}

void androidBootstrapEnvironment() {
    const auto dataDirectory = androidExternalFilesDirectory();
    if (!dataDirectory) {
        androidLog(ANDROID_LOG_WARN, "native env: external files directory unavailable");
        return;
    }
    setDefaultEnv("RT_DATA_DIR", dataDirectory->string());
    const char* internalStorage = SDL_GetAndroidInternalStoragePath();
    const std::optional<std::filesystem::path> internalDirectory =
        internalStorage ? std::optional<std::filesystem::path>(internalStorage) : std::nullopt;
    if (internalDirectory) setDefaultEnv("RT_ASSET_DIR", (*internalDirectory / "assets").string());
    if (!std::getenv("RT_DISC")) {
        const auto disc = *dataDirectory / "disc";
        if (std::filesystem::is_directory(disc)) {
            setDefaultEnv("RT_DISC", disc.string());
            androidLog(ANDROID_LOG_INFO, "disc: " + disc.string());
        } else if (internalDirectory && std::filesystem::is_directory(*internalDirectory / "disc")) {
            // Existing dev installs kept their extracted disc in Context.files.
            // Keep RT_DATA_DIR on external files, but reuse this same-package
            // data rather than asking the user to extract again.
            setDefaultEnv("RT_DISC", (*internalDirectory / "disc").string());
            androidLog(ANDROID_LOG_INFO, "disc: internal files/disc");
        } else {
            androidLog(ANDROID_LOG_INFO, "disc: absent");
        }
    }

    std::ifstream envFile(*dataDirectory / "env.txt");
    if (!envFile && internalDirectory) envFile.open(*internalDirectory / "env.txt");
    std::size_t loaded = 0;
    std::string line;
    while (std::getline(envFile, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line.front() == '#') continue;
        const auto split = line.find('=');
        if (split == std::string::npos || split == 0) continue;
        const std::string key = line.substr(0, split);
        if (key.find_first_of(" \t") != std::string::npos) continue;
        (void)setenv(key.c_str(), line.substr(split + 1).c_str(), 1);
        ++loaded;
    }
    androidLog(ANDROID_LOG_INFO, "native env: " + std::to_string(loaded) + " entries from files/env.txt");
}
#endif

void setDefaultEnv(const char* name, const std::string& value) {
    // Environment has precedence over settings.toml; only fill an absent key.
    if (!std::getenv(name)) (void)setenv(name, value.c_str(), 0);
}

void bootstrapSettings() {
    using namespace rt::native::gameplay;
    SettingsStore store(defaultSettingsPath(appDataDirectory()));
    if (!store.load()) {
        std::cout << "rt-app settings: defaults\n";
        return;
    }
    OptionsModel model;
    std::size_t found = 0;
    for (const OptionSpec& option : model.options()) found += store.contains(option.id) ? 1u : 0u;
    // Instantiate the navigator so all availability derives through its
    // effectiveContext() bridge, rather than duplicating terrain/stream/etc.
    MenuNavigator navigator(model, store, {});
    const MenuContext effective = navigator.effectiveContext();
    (void)effective;

    const auto boolFlag = [&store](const char* key, const char* env) {
        // Explicitly stored values write both ways; unset keys leave the env
        // alone so the compiled-in defaults (GAME-LOOK v1: terrain/textures/
        // hudtext on when assets exist) keep governing.
        if (!store.contains(key)) return;
        setDefaultEnv(env, store.getBool(key, false) ? "1" : "0");
    };
    boolFlag("graphics.terrain", "RT_APP_TERRAIN");
    boolFlag("graphics.textures", "RT_APP_TEXTURES");
    boolFlag("graphics.stream", "RT_APP_STREAM");
    boolFlag("graphics.stream_pos", "RT_APP_STREAM_POS");
    boolFlag("graphics.nav", "RT_APP_NAV");
    boolFlag("graphics.nav_town", "RT_APP_NAV_TOWN");
    boolFlag("graphics.route", "RT_APP_ROUTE");
    boolFlag("graphics.route_town", "RT_APP_ROUTE_TOWN");
    boolFlag("graphics.hudtext", "RT_APP_HUDTEXT");
    boolFlag("controllers.triggers", "RT_APP_TRIGGERS");
    if (store.contains("display.interp")) setDefaultEnv("RT_APP_INTERP", store.getBool("display.interp", false) ? "1" : "0");
    if (store.contains("display.interp_cap")) setDefaultEnv("RT_APP_INTERP_CAP", std::to_string(store.getInt("display.interp_cap", 2, 1, 99)));
    if (store.contains("display.interp_rate")) setDefaultEnv("RT_APP_INTERP_RATE", std::to_string(store.getInt("display.interp_rate", 0, 0, 999)));
    if (store.contains("graphics.stream_step_frames")) setDefaultEnv("RT_APP_STREAM_STEP_FRAMES", std::to_string(store.getInt("graphics.stream_step_frames", 10, 0, 999)));
    if (store.contains("graphics.stream_row")) setDefaultEnv("RT_APP_STREAM_ROW", std::to_string(store.getInt("graphics.stream_row", 1, 0, 7)));
    if (store.contains("sound.audio") && !store.getBool("sound.audio", true)) setDefaultEnv("RT_APP_NOAUDIO", "1");
    if (store.contains("sound.bgm")) setDefaultEnv("RT_APP_BGM", store.getString("sound.bgm", ""));
    if (store.contains("sound.bgm_tsq") && !store.getBool("sound.bgm_tsq", true)) setDefaultEnv("RT_APP_BGM_TSQ", "0");
    if (store.contains("sound.radio_station")) {
        // sound.radio_station {off,peach_fm,e_radio} -> the RT_APP_RADIO byte
        // (radio-stations.md §5: settings wiring belongs to the options/settings
        // work). RT_APP_* precedence: an explicit environment value still wins.
        const std::string station = store.getEnum("sound.radio_station", "off", {"off", "peach_fm", "e_radio"});
        const int stationByte = station == "peach_fm" ? 1 : station == "e_radio" ? 2 : 0;
        setDefaultEnv("RT_APP_RADIO", std::to_string(stationByte));
    }
    if (store.contains("display.start_state")) {
        const std::string state = store.getEnum("display.start_state", "title", {"title", "menu", "playing"});
        std::string upper = state;
        std::transform(upper.begin(), upper.end(), upper.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
        setDefaultEnv("RT_APP_STATE", upper);
    }
    std::cout << "rt-app settings: loaded " << found << " of " << model.options().size() << "\n";
}

double processCpuSeconds() {
    rusage usage{};
    if (getrusage(RUSAGE_SELF, &usage) != 0) return 0.0;
    const auto seconds = [](const timeval& value) {
        return static_cast<double>(value.tv_sec) + static_cast<double>(value.tv_usec) / 1'000'000.0;
    };
    return seconds(usage.ru_utime) + seconds(usage.ru_stime);
}

struct Presenter {
    VkInstance instance = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkPhysicalDevice physical = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue queue = VK_NULL_HANDLE;
    std::uint32_t queueFamily = 0;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkExtent2D extent{};
    std::vector<VkImage> images;
    VkCommandPool pool = VK_NULL_HANDLE;
    VkCommandBuffer command = VK_NULL_HANDLE;
    VkBuffer staging = VK_NULL_HANDLE;
    VkDeviceMemory stagingMemory = VK_NULL_HANDLE;
    VkDeviceSize stagingSize = 0;
    VkSemaphore imageAvailable = VK_NULL_HANDLE;
    VkSemaphore renderDone = VK_NULL_HANDLE;
    VkFence frameDone = VK_NULL_HANDLE;
    VkPhysicalDeviceMemoryProperties memory{};
};

constexpr int kAudioSampleRate = 48000;
constexpr int kAudioChannels = 2;

struct AudioHost {
    rt::native::audio::SoundEngine engine;
    std::vector<std::int16_t> bgm;
    rt::native::audio::PcmBuffer radioPcm;   // radio L half (hard-panned left), host-owned
    rt::native::audio::PcmBuffer radioPcmRight;  // radio R half (hard-panned right), host-owned
    rt::native::audio::BgmPlayer tsqBgm;
    bool usingTsqBgm = false;
    SDL_AudioStream* stream = nullptr;
    std::atomic<bool> loggedFirstPeak = false;
    std::atomic<int> peakSinceLog = 0;
    std::atomic<int> framesSinceLog = 0;
};

void SDLCALL fillAudio(void* userdata, SDL_AudioStream* stream, int additionalBytes, int) {
    auto& audio = *static_cast<AudioHost*>(userdata);
    const int frames = additionalBytes / (kAudioChannels * static_cast<int>(sizeof(std::int16_t)));
    if (frames <= 0) return;

    rt::native::audio::OutputBuffer output;
    output.channels = kAudioChannels;
    output.samples.resize(static_cast<std::size_t>(frames) * output.channels);
    if (audio.usingTsqBgm) {
        audio.tsqBgm.pump(output.samples.data(), static_cast<std::size_t>(frames));
        // Keep the TSQ source at the same conservative combined BGM/master
        // gain used by the VAG SoundEngine path below.
        for (auto& sample : output.samples)
            sample = static_cast<std::int16_t>(static_cast<float>(sample) * (.25f * .7f));
    } else {
        audio.engine.tick(frames, output);
    }

    int peak = 0;
    for (const std::int16_t sample : output.samples) {
        const int magnitude = sample < 0 ? -static_cast<int>(sample) : static_cast<int>(sample);
        peak = std::max(peak, magnitude);
    }
    int observed = audio.peakSinceLog.load(std::memory_order_relaxed);
    while (observed < peak && !audio.peakSinceLog.compare_exchange_weak(
               observed, peak, std::memory_order_relaxed)) {}

    const int totalFrames = audio.framesSinceLog.fetch_add(frames, std::memory_order_relaxed) + frames;
    bool expected = false;
    if (audio.loggedFirstPeak.compare_exchange_strong(expected, true, std::memory_order_relaxed)) {
        std::fprintf(stderr, "rt-app audio callback frames=%d peak=%d\n", frames, peak);
    }
    if (totalFrames >= kAudioSampleRate) {
        const int intervalPeak = audio.peakSinceLog.exchange(0, std::memory_order_relaxed);
        audio.framesSinceLog.fetch_sub(kAudioSampleRate, std::memory_order_relaxed);
        std::fprintf(stderr, "rt-app audio peak=%d\n", intervalPeak);
    }
    (void)SDL_PutAudioStreamData(stream, output.samples.data(),
                                 static_cast<int>(output.samples.size() * sizeof(std::int16_t)));
}

std::filesystem::path discRootPath() {
    if (const char* value = std::getenv("RT_DISC")) return value;
    if (const char* home = std::getenv("HOME"))
        return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc";
    return {};
}

// W12 native-boot diagnostic (RT_APP_NATIVE_BOOT=1, default off — every
// shipped/determinism path stays on the authored layer until boot reaches
// title; docs/native-reference/w12-kernel-design.md). Stages the boot ELF
// into guest RAM, runs the compiled guest corpus under the host libkernel
// (green threads, vblank pump); rendering stays the preview layer — this
// measures how far decompiled init walks, not a scene yet.
struct NativeBoot {
    bool ok = false;
    int64_t vblanks = 0;
};

NativeBoot nativeBootFromEnv() {
    NativeBoot nb;
    if (!(std::getenv("RT_APP_NATIVE_BOOT") && *std::getenv("RT_APP_NATIVE_BOOT"))) return nb;
    const std::filesystem::path elf = std::getenv("RT_ELF") && *std::getenv("RT_ELF")
        ? std::filesystem::path(std::getenv("RT_ELF"))
        : discRootPath() / "SLUS_203.98";
    std::ifstream in(elf, std::ios::binary);
    if (!in) {
        std::fprintf(stderr, "[native-boot] no boot ELF at %s\n", elf.string().c_str());
        return nb;
    }
    std::vector<char> img((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    if (!rtguest::loadImage(reinterpret_cast<const std::uint8_t*>(img.data()), img.size())) {
        std::fprintf(stderr, "[native-boot] ELF section staging failed: %s\n", elf.string().c_str());
        return nb;
    }
    std::uint32_t entry = 0;
    std::memcpy(&entry, img.data() + 0x18, 4);  // ELF32 e_off 0x18 = e_entry
    rtguest::kBootMainThread(entry);
    std::cout << "rt-app native-boot: entry=0x" << std::hex << entry << std::dec
              << " image=" << elf.string() << "\n";
    nb.ok = true;
    return nb;
}

// setup_wizard.cpp layout under the assets root: cars/CAR0/Q00.BIN,
// fonts/FONTE.GSL, audio/SOUND/*.VAG. RT_ASSET_DIR wins, then
// RT_DATA_DIR/assets, then the implicit Application Support cache — but only
// when RT_DISC is unset. An explicit RT_DISC that lacks the file must not
// bind that side cache, or the no-assets determinism gate
// (RT_DISC=/nonexistent, default env) stops being byte-identical.
std::filesystem::path packagedAssetsRoot() {
    if (const char* asset = std::getenv("RT_ASSET_DIR"); asset && *asset)
        return std::filesystem::path(asset);
    if (const char* data = std::getenv("RT_DATA_DIR"); data && *data)
        return std::filesystem::path(data) / "assets";
    if (!std::getenv("RT_DISC")) return appDataDirectory() / "assets";
    return {};
}

std::filesystem::path resolveDiscOrPackaged(const std::filesystem::path& discRelative,
                                            const std::filesystem::path& packagedRelative) {
    std::error_code ec;
    const auto primary = discRootPath() / discRelative;
    if (std::filesystem::is_regular_file(primary, ec)) return primary;
    const auto root = packagedAssetsRoot();
    if (!root.empty()) {
        const auto packed = root / packagedRelative;
        if (std::filesystem::is_regular_file(packed, ec)) return packed;
    }
    return {};
}

bool radioStationFileAvailable(rt::native::gameplay::RadioStation station) {
    const auto pair = rt::native::gameplay::stationToVagPair(station);
    if (!pair) return false;
    return !resolveDiscOrPackaged(pair->left, std::filesystem::path("audio") / pair->left).empty();
}

// Explicit RT_APP_RADIO=0 disables. Any other explicit byte keeps the
// station-follow contract. Unset defaults to Peach FM when that VAG is
// reachable; the caller still requires a successful audio init.
std::optional<std::uint8_t> radioStationRequest() {
    if (const char* env = std::getenv("RT_APP_RADIO"); env && *env) {
        const auto byte = static_cast<std::uint8_t>(std::atoi(env));
        if (rt::native::gameplay::stationFromSaveByte(byte) == rt::native::gameplay::RadioStation::Off)
            return std::nullopt;
        return byte;
    }
    // Precedence (regression: bgm/bgm_tsq matrix rows failed after the
    // radio-default wave, 2026-10-17): an EXPLICIT fixed-BGM request
    // (RT_APP_BGM path or RT_APP_BGM_TSQ) always wins over the IMPLICIT
    // default station — scripts pin those to exercise the VAG/TSQ path.
    // An explicit RT_APP_RADIO above still wins over everything.
    if (std::getenv("RT_APP_BGM") || std::getenv("RT_APP_BGM_TSQ"))
        return std::nullopt;
    if (radioStationFileAvailable(rt::native::gameplay::RadioStation::PeachFm))
        return static_cast<std::uint8_t>(rt::native::gameplay::RadioStation::PeachFm);
    return std::nullopt;
}

bool initAudio(AudioHost& audio) {
    // Radio owns the BGM path when a station is selected (radio-stations.md §5):
    // skip the fixed BGM_01 TSQ / RT_APP_BGM VAG so the engine's BGM slots are
    // free for the radio pair and the engine (not the TSQ pump) drives
    // fillAudio. Explicit "0", or unset with no station VAG, keeps the existing
    // BGM behavior. Unset with assets defaults to Peach FM.
    const bool radioOwnsBgm = radioStationRequest().has_value();
    const char* bgmPath = std::getenv("RT_APP_BGM");
    const char* tsqSetting = std::getenv("RT_APP_BGM_TSQ");
    const bool tsqDisabled = tsqSetting && std::strcmp(tsqSetting, "0") == 0;
    const std::filesystem::path discRoot = discRootPath();
    const std::string selectedTrack = bgmPath && *bgmPath ? bgmPath : "BGM_01";
    if (radioOwnsBgm) {
        std::cout << "rt-app radio: station-follow owns the BGM path (BGM load skipped)\n";
    } else if (!tsqDisabled && audio.tsqBgm.load(discRoot, selectedTrack)) {
        audio.usingTsqBgm = true;
        std::cout << "rt-app BGM TSQ loaded: " << selectedTrack
                  << " from " << (discRoot / "SOUND") << "\n";
    } else {
        rt::native::audio::PcmBuffer loadedBgm;
        if (bgmPath && *bgmPath && rt::native::audio::loadVagFile(bgmPath, loadedBgm)) {
            audio.bgm = std::move(loadedBgm.samples);
            std::cout << "rt-app BGM loaded: " << bgmPath << " (" << audio.bgm.size() << " frames)\n";
        } else {
            if (bgmPath && *bgmPath)
                std::cerr << "rt-app BGM load failed, using synthetic tone: " << bgmPath << "\n";
            // A one-second synthetic saw wave keeps the output path usable when no
            // BGM file is selected. Its lifetime is the host's.
            audio.bgm.resize(kAudioSampleRate);
            for (int i = 0; i < kAudioSampleRate; ++i) {
                audio.bgm[static_cast<std::size_t>(i)] =
                    static_cast<std::int16_t>((i % 240) * 100 - 12000);
            }
        }
        audio.engine.setMasterVolume(.25f);
        audio.engine.playBgm(0, 0x0a1c0201u,
                             {audio.bgm.data(), audio.bgm.size(), true}, .7f);
    }

    const SDL_AudioSpec spec{SDL_AUDIO_S16, kAudioChannels, kAudioSampleRate};
    audio.stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                             &spec, fillAudio, &audio);
    if (!audio.stream) {
        std::cerr << "SDL audio disabled: " << SDL_GetError() << "\n";
        return false;
    }
    if (!SDL_ResumeAudioStreamDevice(audio.stream)) {
        std::cerr << "SDL audio disabled: " << SDL_GetError() << "\n";
        SDL_DestroyAudioStream(audio.stream);
        audio.stream = nullptr;
        return false;
    }
    std::cout << "rt-app audio device opened: 48000 Hz, S16, stereo\n";
    return true;
}

void destroyAudio(AudioHost& audio) {
    if (audio.stream) SDL_DestroyAudioStream(audio.stream);
    audio.stream = nullptr;
    audio.engine.stopAll();
}

bool vkOk(VkResult result, const char* what, std::string& error) {
    if (result == VK_SUCCESS) return true;
    error = std::string(what) + " (VkResult " + std::to_string(result) + ")";
    return false;
}

std::uint32_t memoryType(const Presenter& p, std::uint32_t bits, VkMemoryPropertyFlags flags) {
    for (std::uint32_t i = 0; i < p.memory.memoryTypeCount; ++i)
        if ((bits & (1u << i)) && (p.memory.memoryTypes[i].propertyFlags & flags) == flags) return i;
    return UINT32_MAX;
}

void destroySwapchain(Presenter& p) {
    if (p.staging) vkDestroyBuffer(p.device, p.staging, nullptr);
    if (p.stagingMemory) vkFreeMemory(p.device, p.stagingMemory, nullptr);
    p.staging = VK_NULL_HANDLE; p.stagingMemory = VK_NULL_HANDLE; p.stagingSize = 0;
    if (p.swapchain) vkDestroySwapchainKHR(p.device, p.swapchain, nullptr);
    p.swapchain = VK_NULL_HANDLE; p.images.clear(); p.extent = {};
}

void destroy(Presenter& p) {
    if (p.device) vkDeviceWaitIdle(p.device);
    destroySwapchain(p);
    if (p.frameDone) vkDestroyFence(p.device, p.frameDone, nullptr);
    if (p.renderDone) vkDestroySemaphore(p.device, p.renderDone, nullptr);
    if (p.imageAvailable) vkDestroySemaphore(p.device, p.imageAvailable, nullptr);
    if (p.pool) vkDestroyCommandPool(p.device, p.pool, nullptr);
    if (p.device) vkDestroyDevice(p.device, nullptr);
    if (p.surface) vkDestroySurfaceKHR(p.instance, p.surface, nullptr);
    if (p.instance) vkDestroyInstance(p.instance, nullptr);
    p = {};
}

bool makeSwapchain(Presenter& p, SDL_Window* window, std::string& error) {
    int pixelW = 0, pixelH = 0;
    SDL_GetWindowSizeInPixels(window, &pixelW, &pixelH);
    if (pixelW <= 0 || pixelH <= 0) return true; // Minimized; retry on the next size event.
    vkDeviceWaitIdle(p.device);
    destroySwapchain(p);
    VkSurfaceCapabilitiesKHR caps{};
    if (!vkOk(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p.physical, p.surface, &caps), "surface capabilities", error)) return false;
    std::uint32_t count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(p.physical, p.surface, &count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(p.physical, p.surface, &count, formats.data());
    if (formats.empty()) { error = "surface has no formats"; return false; }
    const auto preferred = std::find_if(formats.begin(), formats.end(), [](const auto& f) {
        return f.format == VK_FORMAT_B8G8R8A8_UNORM || f.format == VK_FORMAT_R8G8B8A8_UNORM;
    });
    p.format = (preferred == formats.end() ? formats.front() : *preferred).format;
    p.extent = caps.currentExtent.width != UINT32_MAX ? caps.currentExtent : VkExtent2D{
        std::clamp(static_cast<std::uint32_t>(pixelW), caps.minImageExtent.width, caps.maxImageExtent.width),
        std::clamp(static_cast<std::uint32_t>(pixelH), caps.minImageExtent.height, caps.maxImageExtent.height)};
    std::uint32_t modes = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(p.physical, p.surface, &modes, nullptr);
    std::vector<VkPresentModeKHR> presentModes(modes);
    vkGetPhysicalDeviceSurfacePresentModesKHR(p.physical, p.surface, &modes, presentModes.data());
    const bool mailbox = std::find(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != presentModes.end();
    VkSwapchainCreateInfoKHR ci{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    ci.surface = p.surface; ci.minImageCount = std::max(2u, caps.minImageCount);
    if (caps.maxImageCount && ci.minImageCount > caps.maxImageCount) ci.minImageCount = caps.maxImageCount;
    ci.imageFormat = p.format; ci.imageColorSpace = (preferred == formats.end() ? formats.front() : *preferred).colorSpace;
    ci.imageExtent = p.extent; ci.imageArrayLayers = 1; ci.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; ci.preTransform = caps.currentTransform;
    ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; ci.presentMode = mailbox ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_FIFO_KHR;
    ci.clipped = VK_TRUE;
    if (!vkOk(vkCreateSwapchainKHR(p.device, &ci, nullptr, &p.swapchain), "vkCreateSwapchainKHR", error)) return false;
    vkGetSwapchainImagesKHR(p.device, p.swapchain, &count, nullptr); p.images.resize(count);
    vkGetSwapchainImagesKHR(p.device, p.swapchain, &count, p.images.data());
    const VkDeviceSize bytes = static_cast<VkDeviceSize>(p.extent.width) * p.extent.height * 4;
    VkBufferCreateInfo buffer{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO}; buffer.size = bytes; buffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT; buffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (!vkOk(vkCreateBuffer(p.device, &buffer, nullptr, &p.staging), "vkCreateBuffer(staging)", error)) return false;
    VkMemoryRequirements requirements{}; vkGetBufferMemoryRequirements(p.device, p.staging, &requirements);
    const std::uint32_t type = memoryType(p, requirements.memoryTypeBits, kHostMemory);
    if (type == UINT32_MAX) { error = "no host-visible staging memory"; return false; }
    VkMemoryAllocateInfo alloc{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO}; alloc.allocationSize = requirements.size; alloc.memoryTypeIndex = type;
    if (!vkOk(vkAllocateMemory(p.device, &alloc, nullptr, &p.stagingMemory), "vkAllocateMemory(staging)", error) ||
        !vkOk(vkBindBufferMemory(p.device, p.staging, p.stagingMemory, 0), "vkBindBufferMemory(staging)", error)) return false;
    p.stagingSize = bytes;
    return true;
}

bool init(Presenter& p, SDL_Window* window, std::string& error) {
    if (const char* f = std::getenv("RT_APP_TEST_VKFAIL"); f && *f == '1') { error = "RT_APP_TEST_VKFAIL: forced Vulkan presenter init failure"; return false; }  // W16.7 test hook
    std::uint32_t extensionCount = 0;
    const char* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
    if (!sdlExtensions) { error = SDL_GetError(); return false; }
    std::vector<const char*> extensions(sdlExtensions, sdlExtensions + extensionCount);
#ifdef __APPLE__
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#endif
    VkApplicationInfo app{VK_STRUCTURE_TYPE_APPLICATION_INFO}; app.pApplicationName = "rt-app"; app.apiVersion = VK_API_VERSION_1_1;
    VkInstanceCreateInfo instance{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO}; instance.pApplicationInfo = &app;
    instance.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size()); instance.ppEnabledExtensionNames = extensions.data();
#ifdef __APPLE__
    instance.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
    if (!vkOk(vkCreateInstance(&instance, nullptr, &p.instance), "vkCreateInstance", error) ||
        !SDL_Vulkan_CreateSurface(window, p.instance, nullptr, &p.surface)) { if (error.empty()) error = SDL_GetError(); return false; }
    std::uint32_t deviceCount = 0; vkEnumeratePhysicalDevices(p.instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount); vkEnumeratePhysicalDevices(p.instance, &deviceCount, devices.data());
    for (const auto physical : devices) {
        std::uint32_t queueCount = 0; vkGetPhysicalDeviceQueueFamilyProperties(physical, &queueCount, nullptr);
        std::vector<VkQueueFamilyProperties> queues(queueCount); vkGetPhysicalDeviceQueueFamilyProperties(physical, &queueCount, queues.data());
        for (std::uint32_t i = 0; i < queueCount; ++i) { VkBool32 present = VK_FALSE; vkGetPhysicalDeviceSurfaceSupportKHR(physical, i, p.surface, &present);
            if ((queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && present) { p.physical = physical; p.queueFamily = i; break; } }
        if (p.physical) break;
    }
    if (!p.physical) { error = "no graphics/present queue"; return false; }
    vkGetPhysicalDeviceMemoryProperties(p.physical, &p.memory);
    float priority = 1.f; VkDeviceQueueCreateInfo queue{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO}; queue.queueFamilyIndex = p.queueFamily; queue.queueCount = 1; queue.pQueuePriorities = &priority;
    std::vector<const char*> deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#ifdef __APPLE__
    deviceExtensions.push_back("VK_KHR_portability_subset");
#endif
    VkDeviceCreateInfo device{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO}; device.queueCreateInfoCount = 1; device.pQueueCreateInfos = &queue;
    device.enabledExtensionCount = static_cast<std::uint32_t>(deviceExtensions.size()); device.ppEnabledExtensionNames = deviceExtensions.data();
    if (!vkOk(vkCreateDevice(p.physical, &device, nullptr, &p.device), "vkCreateDevice", error)) return false;
    vkGetDeviceQueue(p.device, p.queueFamily, 0, &p.queue);
    VkCommandPoolCreateInfo pool{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO}; pool.queueFamilyIndex = p.queueFamily; pool.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VkCommandBufferAllocateInfo command{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    if (!vkOk(vkCreateCommandPool(p.device, &pool, nullptr, &p.pool), "vkCreateCommandPool", error)) return false;
    command.commandPool = p.pool; command.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; command.commandBufferCount = 1;
    if (!vkOk(vkAllocateCommandBuffers(p.device, &command, &p.command), "vkAllocateCommandBuffers", error)) return false;
    VkSemaphoreCreateInfo semaphore{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO}; VkFenceCreateInfo fence{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO}; fence.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    if (!vkOk(vkCreateSemaphore(p.device, &semaphore, nullptr, &p.imageAvailable), "vkCreateSemaphore", error) ||
        !vkOk(vkCreateSemaphore(p.device, &semaphore, nullptr, &p.renderDone), "vkCreateSemaphore", error) ||
        !vkOk(vkCreateFence(p.device, &fence, nullptr, &p.frameDone), "vkCreateFence", error)) return false;
    return makeSwapchain(p, window, error);
}

bool present(Presenter& p, const std::vector<std::uint8_t>& rgba, bool& outOfDate, std::string& error) {
    outOfDate = false;
    if (!p.swapchain || rgba.size() != p.stagingSize) return true;
    vkWaitForFences(p.device, 1, &p.frameDone, VK_TRUE, UINT64_MAX);
    std::uint32_t image = 0; const VkResult acquired = vkAcquireNextImageKHR(p.device, p.swapchain, UINT64_MAX, p.imageAvailable, VK_NULL_HANDLE, &image);
    if (acquired == VK_ERROR_OUT_OF_DATE_KHR) { outOfDate = true; return true; }
    if (acquired != VK_SUCCESS && acquired != VK_SUBOPTIMAL_KHR && !vkOk(acquired, "vkAcquireNextImageKHR", error)) return false;
    void* mapped = nullptr; if (!vkOk(vkMapMemory(p.device, p.stagingMemory, 0, p.stagingSize, 0, &mapped), "vkMapMemory(staging)", error)) return false;
    auto* out = static_cast<std::uint8_t*>(mapped);
    if (p.format == VK_FORMAT_B8G8R8A8_UNORM || p.format == VK_FORMAT_B8G8R8A8_SRGB) for (std::size_t i = 0; i < rgba.size(); i += 4) { out[i] = rgba[i + 2]; out[i + 1] = rgba[i + 1]; out[i + 2] = rgba[i]; out[i + 3] = rgba[i + 3]; }
    else std::memcpy(out, rgba.data(), rgba.size());
    vkUnmapMemory(p.device, p.stagingMemory); vkResetFences(p.device, 1, &p.frameDone); vkResetCommandBuffer(p.command, 0);
    VkCommandBufferBeginInfo begin{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO}; vkBeginCommandBuffer(p.command, &begin);
    VkImageMemoryBarrier toTransfer{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER}; toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED; toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; toTransfer.image = p.images[image]; toTransfer.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    vkCmdPipelineBarrier(p.command, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &toTransfer);
    VkBufferImageCopy copy{}; copy.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1}; copy.imageExtent = {p.extent.width, p.extent.height, 1};
    vkCmdCopyBufferToImage(p.command, p.staging, p.images[image], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
    VkImageMemoryBarrier toPresent = toTransfer; toPresent.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; toPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    toPresent.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; toPresent.dstAccessMask = 0;
    vkCmdPipelineBarrier(p.command, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &toPresent);
    vkEndCommandBuffer(p.command);
    const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_TRANSFER_BIT; VkSubmitInfo submit{VK_STRUCTURE_TYPE_SUBMIT_INFO}; submit.waitSemaphoreCount = 1; submit.pWaitSemaphores = &p.imageAvailable; submit.pWaitDstStageMask = &waitStage; submit.commandBufferCount = 1; submit.pCommandBuffers = &p.command; submit.signalSemaphoreCount = 1; submit.pSignalSemaphores = &p.renderDone;
    if (!vkOk(vkQueueSubmit(p.queue, 1, &submit, p.frameDone), "vkQueueSubmit", error)) return false;
    VkPresentInfoKHR info{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR}; info.waitSemaphoreCount = 1; info.pWaitSemaphores = &p.renderDone; info.swapchainCount = 1; info.pSwapchains = &p.swapchain; info.pImageIndices = &image;
    const VkResult result = vkQueuePresentKHR(p.queue, &info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) { outOfDate = true; return true; }
    return result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR || (error = "vkQueuePresentKHR (VkResult " + std::to_string(result) + ")", false);
}

rt::native::assets::NativeModel demoCar() {
    using rt::native::assets::NativeModel; using rt::native::assets::Vertex;
    NativeModel car;
    const std::array<std::array<float, 3>, 8> points{{{-1.f, 0.f, -2.f}, {1.f, 0.f, -2.f}, {1.f, 0.f, 2.f}, {-1.f, 0.f, 2.f}, {-1.f, 1.f, -2.f}, {1.f, 1.f, -2.f}, {1.f, 1.f, 2.f}, {-1.f, 1.f, 2.f}}};
    for (const auto& p : points) car.vertices.push_back(Vertex{p[0], p[1], p[2]});
    car.indices = {0,2,1,0,3,2, 4,5,6,4,6,7, 0,1,5,0,5,4, 1,2,6,1,6,5, 2,3,7,2,7,6, 3,0,4,3,4,7};
    return car;
}
int envFrames() { const char* value = std::getenv("RT_APP_FRAMES"); return value ? std::max(0, std::atoi(value)) : 0; }

// Parses "vb:path,vb:path" (RT_RAM_DUMP, RT_NATIVE_GS_DUMP). Keys must be bare
// numbers: strtoull("vb1372") returns 0 silently, which dumped at VB0 (cycle-16).
static std::vector<std::pair<uint64_t, std::string>> parseVbPaths(const char* e) {
    std::vector<std::pair<uint64_t, std::string>> v;
    for (const char* p = e; p && *p;) {
        const char* key = p;
        const uint64_t vb = std::strtoull(p, nullptr, 10);
        p = strchr(p, ':');
        if (!p) break;
        ++p;
        const char* q = strchr(p, ',');
        if (!(key[0] >= '0' && key[0] <= '9'))
            std::fprintf(stderr, "[vbpaths] ignored '%.*s:' (vblank keys are bare numbers)\n",
                         (int)(p - key - 1), key);
        else
            v.emplace_back(vb, q ? std::string(p, q) : std::string(p));
        p = q ? q + 1 : nullptr;  // past the comma: strtoull(",123") silently returned 0
    }
    return v;
}

// RT_APP_INTERP=1 enables split pacing (gameplay/interp): logic keeps the
// fixed 60-Hz quantum PROVEN in docs/native-reference/frame-timing.md
// (verdict a) while renders run at host refresh with poses interpolated
// between completed logic steps. Default (unset or RT_APP_INTERP=0) keeps
// the one-logic-step-per-frame host loop byte-identical.
bool envInterpEnabled() {
    const char* value = std::getenv("RT_APP_INTERP");
    return value != nullptr && std::strcmp(value, "0") != 0;
}

// RT_APP_INTERP_CAP=<n>: maximum logic steps drained per host frame when
// catching up after a stall (default 2).
int envInterpCap() {
    const char* value = std::getenv("RT_APP_INTERP_CAP");
    return value && *value ? std::max(1, std::atoi(value))
                           : rt::native::gameplay::FixedStepClock::kDefaultCatchUpCap;
}

// RT_APP_INTERP_RATE=<n>: optional explicit render fps cap; 0/absent means
// the swapchain presentation cadence (vsync) paces rendering.
int envInterpRate() {
    const char* value = std::getenv("RT_APP_INTERP_RATE");
    if (value && *value) return std::max(0, std::atoi(value));
    // W3-06: RT_ENHANCE_TARGET_HZ is the registry spelling of the same render
    // fps cap (0 = vsync pacing); explicit RT_APP_INTERP_RATE still wins.
    return rt::native::app::Enhancements::instance().options().target_hz;
}

// RT_APP_TERRAIN=1 replaces the flat grass quad with real terrain extracted
// from shipped FLD tiles in the disc extract.  Default renders the full
// 64-tile world grid; RT_APP_TERRAIN_FILE=PATH overrides with one explicit
// tile.  RT_APP_TEXTURES=1 additionally decodes each tile's GS uploads to
// RGBA8 (decodeFieldTextures) and streams per-vertex page-fraction UVs so
// the preview draws with the authentic textures; pages are deduplicated
// across tiles by content.  Absent/malformed input falls back to the
// authored quad (or the banded mesh), never a failure.

// Authentic tile placement (gameplay/world_layout; PROVEN formulas in
// docs/native-reference/world-geography.md): id -> (halfColumn 0..15,
// row 0..7) on the staggered lattice.  Render space is 0.1 game units,
// anchored so Peach Town has its origin at (0,0).  Peach Town is streaming
// id 43 (halfColumn 11, row 5, FLD/223.BIN) per the scripted scene table
// @0x2be438 (docs/native-reference/scene-tile-map.md) — the earlier id-11
// binding was My Garage / My City (same half-column, row 1: the arithmetic
// coincidence that seeded the mis-binding).  The graph-0 overlay is the
// COURSE/C00 race route, not Peach streets; the town street graph (scene
// T=1, outer dir @0x2d98a8) lands with the nav_routes townGraph API.
namespace {
constexpr std::int32_t kPeachHalfColumn = 11;
constexpr std::int32_t kPeachRow = 5;
constexpr int kPeachId = 43;

struct TileRenderOrigin { float x; float z; };
TileRenderOrigin tileRenderOrigin(int id) {
    const auto p = rt::native::gameplay::tilePlacement(id);
    return {static_cast<float>(p.halfColumn - kPeachHalfColumn) * 80.0f,
            static_cast<float>(p.row - kPeachRow) * 160.0f};
}

// Session car position (game units inside the anchored Peach tile, matching
// tileRenderOrigin) -> authentic fixed units on the world torus for the
// position-driven streamer demand.  Odd-row half-tile X shear applied once
// local Z passes half a tile (FUN_0021ba70 stored-position semantics, so
// gridCellId's FUN_0021c8d0 (-b) correction maps back to the same id).
rt::native::gameplay::FixedPos carFixedUnits(const rt::native::gameplay::Vec2& carGame) {
    const auto peach = rt::native::gameplay::tilePlacement(kPeachId);
    const double step = rt::native::gameplay::kFixedUnitsPerGameUnit;
    std::int32_t x = peach.x0 + static_cast<std::int32_t>(carGame.x * step);
    std::int32_t z = peach.z0 + static_cast<std::int32_t>(carGame.z * step);
    if ((peach.row & 1) != 0 &&
        carGame.z * step >= static_cast<double>(rt::native::gameplay::kHalfTileFixedUnits))
        x += rt::native::gameplay::kHalfTileFixedUnits;
    return {x, z};
}
} // namespace

namespace {
std::uint64_t textureContentHash(const rt::native::assets::FieldTexturePage& page) {
    std::uint64_t h = 1469598103934665603ull;
    const auto mix = [&h](const std::uint8_t* b, std::size_t n) {
        for (std::size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 1099511628211ull; }
    };
    const std::uint32_t dims[2] = {page.width, page.height};
    mix(reinterpret_cast<const std::uint8_t*>(dims), sizeof(dims));
    mix(page.rgba.data(), page.rgba.size());
    return h;
}

// Appends one tile's decoded pages to mesh.textures (deduplicated) and
// rewrites the patch's triangle page ids to the global indices.
void appendTexturedTile(rt::native::preview::TerrainMesh& mesh,
                       const rt::native::assets::FieldTexturedTerrain& tile,
                       float xOffset, float zOffset,
                       std::unordered_map<std::uint64_t, std::uint32_t>& pagesByHash) {
    const std::uint32_t vertexBase = static_cast<std::uint32_t>(mesh.positions.size());
    for (std::size_t i = 0; i < tile.patch.positions.size(); ++i) {
        auto p = tile.patch.positions[i];
        p[0] += xOffset;
        p[2] += zOffset;
        mesh.positions.push_back(p);
        mesh.colours.push_back(tile.patch.colours[i]);
        mesh.uvs.push_back(tile.patch.uvs[i]);
    }
    for (std::uint32_t i : tile.patch.indices) mesh.indices.push_back(vertexBase + i);
    mesh.triangleMaterials.reserve(mesh.triangleMaterials.size() + tile.patch.indices.size() / 3);
    mesh.triangleTexture.reserve(mesh.triangleTexture.size() + tile.patch.triangleTexture.size());
    for (std::size_t t = 0; t < tile.patch.triangleTexture.size(); ++t) {
        const std::int32_t local = tile.patch.triangleTexture[t];
        std::int32_t global = -1;
        if (local >= 0 && static_cast<std::size_t>(local) < tile.patch.pages.size()) {
            const auto& page = tile.patch.pages[static_cast<std::size_t>(local)];
            const auto hash = textureContentHash(page);
            auto it = pagesByHash.find(hash);
            if (it == pagesByHash.end()) {
                global = static_cast<std::int32_t>(mesh.textures.size());
                pagesByHash.emplace(hash, static_cast<std::uint32_t>(global));
                mesh.textures.push_back({page.width, page.height, page.rgba});
            } else {
                global = static_cast<std::int32_t>(it->second);
            }
        }
        mesh.triangleTexture.push_back(global);
        mesh.triangleMaterials.push_back(local < 0 ? 0u : static_cast<std::uint32_t>(local));
    }
}

std::filesystem::path discRoot() { return discRootPath(); }

// GAME-LOOK v1 (2026-10-17): terrain, textures and FONTE HUD text default to
// ON whenever disc assets exist, so a default launch consumes real game data
// instead of the placeholder scene (the visible-parity mandate: shipping the
// converters but keeping them behind env flags left the out-of-box app
// looking nothing like Road Trip).  "=0" opts out explicitly; other
// RT_APP_* graphics switches keep their opt-in presence semantics.
bool discAssetsPresent() {
    std::error_code ec;
    return std::filesystem::is_directory(discRoot() / "FLD", ec);
}
bool featureDefaultOn(const char* env) {
    if (const char* v = std::getenv(env)) return std::strcmp(v, "0") != 0;
    return discAssetsPresent();
}

// FONTE decode shared by the eager title/menu path and the PLAYING HUD.
// Idempotent: a successful decode is not repeated, and a miss is not re-logged.
bool fonteDecodeAttempted = false;
bool loadFonteAtlas(rt::native::assets::GlyphAtlas& atlas) {
    if (atlas.width > 0) return true;
    if (fonteDecodeAttempted) return false;
    fonteDecodeAttempted = true;
    const auto path = resolveDiscOrPackaged("SYS/FONTE.GSL", "fonts/FONTE.GSL");
    if (path.empty()) {
        std::cout << "rt-app hudtext: SKIP (FONTE.GSL missing)\n";
        return false;
    }
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        std::cout << "rt-app hudtext: SKIP (FONTE.GSL missing)\n";
        return false;
    }
    std::vector<std::uint8_t> gsl((std::istreambuf_iterator<char>(input)),
                                 std::istreambuf_iterator<char>());
    auto decoded = rt::native::assets::decodeGlyphAtlas(gsl);
    if (decoded.value.width == 0) {
        std::cout << "rt-app hudtext: SKIP (decode err=" << static_cast<int>(decoded.err)
                  << " offset=" << decoded.errorOffset << ")\n";
        return false;
    }
    atlas = std::move(decoded.value);
    unsigned glyphCount = 0;
    for (bool occupied : atlas.slotNonEmpty) glyphCount += occupied ? 1u : 0u;
    std::cout << "rt-app hudtext: FONTE decoded " << atlas.width << "x"
              << atlas.height << " non-empty slots=" << glyphCount << "\n";
    return true;
}

// RT_APP_STREAM=1 (with RT_APP_TERRAIN=1) streams tiles through four
// hardware-style resident slots instead of the eager 64-tile load: slot
// index = id & 3 (the four 0x3A8000-byte fieldTileData buffers), resident
// set = a 4-column window of one id-grid row, one slot swapped per column
// step (docs/native-reference/field-topology-status.md).  The window follows
// a scripted serpentine sweep because the authentic camera-to-tile selection
// (nav tables, docs/native-reference/nav-tables.md) is not yet consumed.
// Rebuilds the terrain mesh only on swap frames; loads are synchronous (the
// host has no async I/O path yet).
struct StreamedTerrain {
    StreamedTerrain() : streamer(10, 1, 0) {}

    // Advances one host frame; returns true when a slot reloaded and the
    // caller should rebuild the mesh.
    bool refresh() {
        if (!dirty && !streamer.tick()) return false;
        dirty = false;
        bool reloaded = false;
        for (int slot = 0; slot < rt::native::gameplay::kFieldTileSlots; ++slot) {
            const int id = streamer.idInSlot(slot);
            if (slotId[static_cast<std::size_t>(slot)] == id) continue;
            if (loadSlot(slot, id)) reloaded = true;
            std::cout << "rt-app stream: slot " << slot << " <- id " << id << ' '
                      << index[static_cast<std::size_t>(id)].fileName
                      << " (window row " << streamer.row() << " col " << streamer.colStart() << ")\n";
        }
        return reloaded;
    }

    // Position-driven demand (RT_APP_STREAM_POS=1): follows the car with the
    // authentic 4-slot window via TileStreamer::advanceTo (Evict*->Keep*->
    // Load*, slot = id&3, LRU evictions, nearest-first loads). Returns true
    // when any slot changed and the mesh needs a rebuild.
    bool refreshAt(std::int32_t globalX, std::int32_t globalZ) {
        dirty = false;
        bool changed = false;
        for (const auto& cmd : streamer.advanceTo(globalX, globalZ)) {
            switch (cmd.action) {
                case rt::native::gameplay::TileStreamCommand::Action::Evict:
                    slotId[static_cast<std::size_t>(cmd.slot)] = -1;
                    slotBytes[static_cast<std::size_t>(cmd.slot)] = {};
                    slotTexturedValid[static_cast<std::size_t>(cmd.slot)] = false;
                    changed = true;
                    break;
                case rt::native::gameplay::TileStreamCommand::Action::Load:
                    if (loadSlot(cmd.slot, cmd.id)) {
                        changed = true;
                        std::cout << "rt-app stream: demand slot " << cmd.slot << " <- id "
                                  << cmd.id << ' ' << index[static_cast<std::size_t>(cmd.id)].fileName
                                  << " (demanded id " << streamer.lastDemandedId() << ")\n";
                    }
                    break;
                case rt::native::gameplay::TileStreamCommand::Action::Keep:
                    break;
            }
        }
        return changed;
    }

  private:
    bool loadSlot(int slot, int id) {
        const std::string& name = index[static_cast<std::size_t>(id)].fileName;
        const std::size_t slotIndex = static_cast<std::size_t>(slot);
        if (name.empty()) return false;
        std::ifstream input(root / "FLD" / name, std::ios::binary);
        if (!input) return false;
        slotBytes[slotIndex] =
            std::vector<std::uint8_t>((std::istreambuf_iterator<char>(input)),
                                     std::istreambuf_iterator<char>());
        slotId[slotIndex] = id;
        slotTexturedValid[slotIndex] = false;
        if (wantTextures) {
            auto textured = rt::native::assets::buildFieldTexturedTerrain(slotBytes[slotIndex], 0.1f);
            if (textured) {
                slotTextured[slotIndex] = std::move(*textured);
                slotTexturedValid[slotIndex] = true;
            } else {
                std::cout << "rt-app stream: textured build failed for " << name << "\n";
            }
        }
        ++loads;
        return true;
    }

  public:

    // Rebuilds the render mesh from the four resident slots with the same
    // staggered-grid placement as the eager loader (x step 80, z step 160).
    void buildMesh(rt::native::preview::TerrainMesh& mesh) const {
        mesh = rt::native::preview::TerrainMesh{};
        std::unordered_map<std::uint64_t, std::uint32_t> pagesByHash;
        for (int slot = 0; slot < rt::native::gameplay::kFieldTileSlots; ++slot) {
            const int id = slotId[static_cast<std::size_t>(slot)];
            if (id < 0) continue;
            const auto origin = tileRenderOrigin(id);
            const float xOffset = origin.x;
            const float zOffset = origin.z;
            const std::size_t slotIndex = static_cast<std::size_t>(slot);
            if (wantTextures) {
                if (slotTexturedValid[slotIndex])
                    appendTexturedTile(mesh, slotTextured[slotIndex], xOffset, zOffset, pagesByHash);
                continue;
            }
            const auto catalog = rt::native::assets::catalogFieldTile(slotBytes[slotIndex]);
            if (!catalog) continue;
            auto terrain = rt::native::assets::buildFieldRenderTerrain(slotBytes[slotIndex], *catalog, 0.1f);
            const std::uint32_t vertexBase = static_cast<std::uint32_t>(mesh.positions.size());
            for (auto& p : terrain.positions) {
                p[0] += xOffset;
                p[2] += zOffset;
                mesh.positions.push_back(p);
            }
            for (std::uint32_t i : terrain.indices) mesh.indices.push_back(vertexBase + i);
            mesh.triangleMaterials.insert(mesh.triangleMaterials.end(),
                                         terrain.triangleMaterials.begin(),
                                         terrain.triangleMaterials.end());
        }
    }

    rt::native::gameplay::TileStreamer streamer;
    rt::native::assets::FieldTileIndex index{};
    std::filesystem::path root;
    bool wantTextures = false;
    bool dirty = true;
    std::size_t loads = 0;
    std::array<int, 4> slotId{{-1, -1, -1, -1}};
    std::array<std::vector<std::uint8_t>, 4> slotBytes;
    std::array<rt::native::assets::FieldTexturedTerrain, 4> slotTextured;
    std::array<bool, 4> slotTexturedValid{{false, false, false, false}};
};
} // namespace

rt::native::preview::TerrainMesh loadTerrainMesh() {
    rt::native::preview::TerrainMesh mesh;
    if (!featureDefaultOn("RT_APP_TERRAIN")) return mesh;
    const bool wantTextures = featureDefaultOn("RT_APP_TEXTURES");
    // Default coverage is the neighbourhood around the anchored spawn (Peach
    // Town id 43 at origin 0,0): +/- R lattice steps (x step 80 render units
    // = half-tile stagger, z step 160), with shear adjustment for odd-row b
    // per FUN_0021ba70 (docs).  R=1 yields the 5-tile set.
    const int radius = [] {
        const char* v = std::getenv("RT_APP_TILES_RADIUS");
        if (!v) return 1;
        if (std::strcmp(v, "all") == 0) return 1 << 20;
        const int r = std::atoi(v);
        return r > 0 ? r : 1;
    }();
    std::size_t tilesSkippedFar = 0;
    std::unordered_map<std::uint64_t, std::uint32_t> pagesByHash;
    const std::filesystem::path root = discRoot();
    const char* named = std::getenv("RT_APP_TERRAIN_FILE");
    if (named != nullptr) {
        const std::filesystem::path path{named};
        std::ifstream input(path, std::ios::binary);
        if (!input) { std::cout << "rt-app terrain: SKIP (no tile at " << path << ")\n"; return mesh; }
        const std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(input)),
                                             std::istreambuf_iterator<char>());
        if (wantTextures) {
            auto textured = rt::native::assets::buildFieldTexturedTerrain(bytes, 0.1f);
            if (!textured) {
                std::cout << "rt-app terrain: textured build failed for " << path.filename().string()
                        << " (err=" << static_cast<int>(textured.error()) << ")\n";
                return mesh;
            }
            appendTexturedTile(mesh, *textured, 0.f, 0.f, pagesByHash);
            std::cout << "rt-app terrain: loaded " << path.filename().string()
                      << " triangles=" << mesh.indices.size() / 3
                      << " pages=" << mesh.textures.size()
                      << " clut hits/alias/miss=" << textured->clutHits << '/'
                      << textured->clutAliasHits << '/' << textured->clutMisses
                      << " unresolved=" << textured->unresolvedTriangles << "\n";
            return mesh;
        }
        const auto catalog = rt::native::assets::catalogFieldTile(bytes);
        if (!catalog) { std::cout << "rt-app terrain: catalog failed for " << path << "\n"; return mesh; }
        auto terrain = rt::native::assets::buildFieldRenderTerrain(bytes, *catalog, 0.1f);
        mesh.positions = std::move(terrain.positions);
        mesh.indices = std::move(terrain.indices);
        mesh.triangleMaterials = std::move(terrain.triangleMaterials);
        std::cout << "rt-app terrain: loaded " << path.filename().string()
                  << " triangles=" << mesh.indices.size() / 3 << "\n";
        return mesh;
    }

    // Multi-tile path: resolve streaming ids via the ROM LBA descriptor table
    // + size matching (field_tile_index), place each tile at its authentic
    // lattice origin (halfColumn/row from world_layout, x step 80 render units
    // = half-tile stagger, z step 160), anchored on Peach Town.
#ifdef __ANDROID__
    androidLog(ANDROID_LOG_INFO, "scene: terrain build start (index)");
#endif
    const auto index = rt::native::assets::buildFieldTileIndex(root);
    if (!index) {
        std::cout << "rt-app terrain: SKIP (no tile index; err="
                  << static_cast<int>(index.error()) << ")\n";
#ifdef __ANDROID__
        androidLog(ANDROID_LOG_WARN, "scene: terrain index FAILED");
#endif
        return mesh;
    }
    std::size_t tilesLoaded = 0;
    unsigned long long clutHits = 0, clutAlias = 0, clutMiss = 0, unresolved = 0;
    // Per-tile diagnostics (extended 2026-10-17 per task): name + placed
    // origin + vertex count for each loaded tile in the radius; plus later
    // coverage probe over 5x5 grid to show which lattice cells actually
    // contribute vertices near the sample points.
    struct LoadedTileInfo {
        std::string name;
        float ox;
        float oz;
        std::uint32_t vstart;
        std::uint32_t vcount;
    };
    std::vector<LoadedTileInfo> loadedTiles;
#ifdef __ANDROID__
    androidLog(ANDROID_LOG_INFO, "scene: tile index built, loading neighbourhood");
#endif
    for (std::size_t id = 0; id < rt::native::assets::kFieldTileCount; ++id) {
        const std::string& name = (*index)[id].fileName;
        if (name.empty()) continue;
        // Neighbourhood curation before I/O (RT_APP_TILES_RADIUS above).
        {
            const auto p = rt::native::gameplay::tilePlacement(static_cast<int>(id));
            const int b = p.row & 1;  // odd-row shear bit
            const float ox = static_cast<float>(p.halfColumn - kPeachHalfColumn + (b - 1)) * 80.0f;
            const float oz = static_cast<float>(p.row - kPeachRow) * 160.0f;
            if (std::fabs(ox) > radius * 160.f + 1.f ||
                std::fabs(oz) > radius * 160.f + 1.f) {
                ++tilesSkippedFar;
                continue;
            }
        }
#ifdef __ANDROID__
        androidLog(ANDROID_LOG_INFO, "scene: tile " + name + (wantTextures ? " (textured)" : ""));
#endif
        std::ifstream input(root / "FLD" / name, std::ios::binary);
        if (!input) continue;
        const std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(input)),
                                             std::istreambuf_iterator<char>());
        // Authentic lattice origin, anchored so Peach Town (id 43) starts at
        // (0,0) and the spawn car sits on Peach terrain.
        // Adjusted for shear: x includes +(b-1) so even rows align contiguously
        // without staircase offsets (halfCol parity folded).
        const auto p = rt::native::gameplay::tilePlacement(static_cast<int>(id));
        const int b = p.row & 1;
        const float xOffset = static_cast<float>(p.halfColumn - kPeachHalfColumn + (b - 1)) * 80.0f;
        const float zOffset = static_cast<float>(p.row - kPeachRow) * 160.0f;
        if (wantTextures) {
            auto textured = rt::native::assets::buildFieldTexturedTerrain(bytes, 0.1f);
            if (!textured) continue;
            const std::uint32_t vstartTex = static_cast<std::uint32_t>(mesh.positions.size());
            appendTexturedTile(mesh, *textured, xOffset, zOffset, pagesByHash);
            const std::uint32_t vcountTex = static_cast<std::uint32_t>(mesh.positions.size()) - vstartTex;
            clutHits += textured->clutHits;
            clutAlias += textured->clutAliasHits;
            clutMiss += textured->clutMisses;
            unresolved += textured->unresolvedTriangles;
            loadedTiles.push_back({name, xOffset, zOffset, vstartTex, vcountTex});
            std::cout << "rt-app terrain: tile " << name << " origin=(" << xOffset << "," << zOffset << ") verts=" << vcountTex << "\n";
            ++tilesLoaded;
            continue;
        }
        const auto catalog = rt::native::assets::catalogFieldTile(bytes);
        if (!catalog) continue;
        auto terrain = rt::native::assets::buildFieldRenderTerrain(bytes, *catalog, 0.1f);
        const std::uint32_t vertexBase = static_cast<std::uint32_t>(mesh.positions.size());
        mesh.positions.reserve(mesh.positions.size() + terrain.positions.size());
        for (auto& p : terrain.positions) {
            p[0] += xOffset;
            p[2] += zOffset;
            mesh.positions.push_back(p);
        }
        mesh.indices.reserve(mesh.indices.size() + terrain.indices.size());
        for (std::uint32_t i : terrain.indices) mesh.indices.push_back(vertexBase + i);
        mesh.triangleMaterials.insert(mesh.triangleMaterials.end(),
                                     terrain.triangleMaterials.begin(),
                                     terrain.triangleMaterials.end());
        const std::uint32_t vcountNon = static_cast<std::uint32_t>(terrain.positions.size());
        loadedTiles.push_back({name, xOffset, zOffset, vertexBase, vcountNon});
        std::cout << "rt-app terrain: tile " << name << " origin=(" << xOffset << "," << zOffset << ") verts=" << vcountNon << "\n";
        ++tilesLoaded;
    }
    std::cout << "rt-app terrain: tiles=" << tilesLoaded
              << " triangles=" << mesh.indices.size() / 3
              << " radius=" << (radius > 99 ? -1 : radius)
              << " far-skipped=" << tilesSkippedFar;
    if (wantTextures) {
        std::cout << " pages=" << mesh.textures.size()
                  << " clut hits/alias/miss=" << clutHits << '/' << clutAlias << '/' << clutMiss
                  << " unresolved=" << unresolved;
    }
    std::cout << "\n";
    // Full-tile ground-cover fill (AUTHORED, 2026-10-17): FLDT meshes are
    // sparse road/feature patches — the coverage probe proved holes read as
    // the dark underlay, while oracle town frames show continuous ground
    // everywhere. Lay a coarse 16-RU grid per loaded tile; height and colour
    // sample that tile's own nearest vertex; drop 0.08 RU so real detail
    // geometry renders on top. Textured fills ride triangleTexture=-1 ->
    // the white-page bucket (vertex colour x2 via the GS modulation).
    // TEMP diagnostic switch: RT_APP_GROUNDFILL=0 skips the authored fill.
    const bool noGroundFill = [] {
        const char* v = std::getenv("RT_APP_GROUNDFILL");
        return v && std::string_view(v) == "0";
    }();
    if (tilesLoaded > 0 && !noGroundFill) {
        constexpr float kFillStep = 16.f;
        constexpr float kFillDrop = 0.25f;
        constexpr std::size_t kGridN = 10;  // 160 RU tile / 16
        std::size_t fillQuads = 0;
        for (const auto& lt : loadedTiles) {
            if (lt.vcount == 0) continue;
            const std::uint32_t base = static_cast<std::uint32_t>(mesh.positions.size());
            for (std::size_t iz = 0; iz <= kGridN; ++iz) {
                for (std::size_t ix = 0; ix <= kGridN; ++ix) {
                    const float gx = lt.ox + static_cast<float>(ix) * kFillStep;
                    const float gz = lt.oz + static_cast<float>(iz) * kFillStep;
                    // Smooth BASE-terrain height: mean of same-tile vertices
                    // within 24 RU horizontally whose y lies in the road/field
                    // band [−2, 4.5] RU (excludes building tops, tree crowns and
                    // pits whose nearest-vertex sampling produced sky-piercing
                    // jagged walls — red-fill experiment 2026-10-17).
                    float sumY = 0.f;
                    std::array<float, 4> sumC{0.f, 0.f, 0.f, 0.f};
                    int cnt = 0;
                    for (std::uint32_t v = lt.vstart; v < lt.vstart + lt.vcount; ++v) {
                        const auto& p = mesh.positions[v];
                        const float dx = p[0] - gx, dz = p[2] - gz;
                        if (dx * dx + dz * dz > 576.f) continue;      // 24 RU
                        if (p[1] < -2.f || p[1] > 3.5f) continue;      // base band, tightened
                        sumY += p[1];
                        if (v < mesh.colours.size()) {
                            sumC[0] += mesh.colours[v][0]; sumC[1] += mesh.colours[v][1];
                            sumC[2] += mesh.colours[v][2];
                        }
                        ++cnt;
                    }
                    // Fallback when no band vertices nearby: oracle town-ground
                    // mean (102,111,122)/255 measured from town_driving.png.
                    const float y = cnt > 0 ? sumY / static_cast<float>(cnt) : 3.f;
                    // Pre-darkened for the GS modulate-x2 pass: fallback is
                    // the oracle town-ground mean (102,111,122)/255 halved so
                    // clamp(c*t*2) reproduces it.  The blue cast is real: the
                    // oracle plaza sits in blue ambient (row 350 samples
                    // (71,85,143) in town_driving.png) — neutral asphalt gray
                    // scored worse (town-v13 0.464 < town-v12 0.491).
                    std::array<float, 4> c{.20f, .216f, .24f, 1.f};
                    if (cnt > 0) {
                        // Anchor 65% at the oracle ground gray (halved for the
                        // x2 pass); keep 35% of local vertex-colour variation
                        // for grass/road patches (race ground measured
                        // (144,144,166) vs oracle (95,95,95) with pure
                        // sampling 2026-10-17).
                        const float inv = 1.f / static_cast<float>(cnt);
                        c[0] = (.35f * sumC[0] * inv + .65f * .40f) * .5f;
                        c[1] = (.35f * sumC[1] * inv + .65f * .435f) * .5f;
                        c[2] = (.35f * sumC[2] * inv + .65f * .48f) * .5f;
                    }
                    mesh.positions.push_back({gx, y - kFillDrop, gz});
                    if (wantTextures) {
                        mesh.colours.push_back(c);
                        mesh.uvs.push_back({0.f, 0.f});
                    }
                }
            }
            for (std::size_t iz = 0; iz < kGridN; ++iz) {
                for (std::size_t ix = 0; ix < kGridN; ++ix) {
                    const std::uint32_t a =
                        base + static_cast<std::uint32_t>(iz * (kGridN + 1) + ix);
                    const std::uint32_t b = a + 1;
                    const std::uint32_t cc = a + static_cast<std::uint32_t>(kGridN + 1);
                    const std::uint32_t d = cc + 1;
                    // Winding matches the tile convention (see preview underlay).
                    mesh.indices.push_back(a); mesh.indices.push_back(cc); mesh.indices.push_back(b);
                    mesh.indices.push_back(b); mesh.indices.push_back(cc); mesh.indices.push_back(d);
                    mesh.triangleMaterials.push_back(0);
                    mesh.triangleMaterials.push_back(0);
                    if (wantTextures) {
                        mesh.triangleTexture.push_back(-1);
                        mesh.triangleTexture.push_back(-1);
                    }
                    ++fillQuads;
                }
            }
        }
        std::cout << "rt-app terrain: ground-fill quads=" << fillQuads
                  << " drop=" << kFillDrop << " step=" << kFillStep
                  << " positions=" << mesh.positions.size()
                  << " colours=" << mesh.colours.size()
                  << " triTex=" << mesh.triangleTexture.size()
                  << " indices=" << mesh.indices.size() << "\n";
    }
    // Town-orientation scan: centroid of building-height vertices (y 4.5..9
    // RU = house roofs; oracle town_driving.png shows houses framing the
    // plaza). Points the spawn heading at the town core — heading 0 faced an
    // empty field 2026-10-17.
    {
        double bx = 0, bz = 0;
        std::size_t bn = 0;
        for (const auto& p : mesh.positions) {
            if (p[1] > 4.5f && p[1] < 9.f) { bx += p[0]; bz += p[2]; ++bn; }
        }
        if (bn > 0) {
            bx /= static_cast<double>(bn);
            bz /= static_cast<double>(bn);
            const double dx = bx - rt::native::gameplay::kTownSpawnX * .1;
            const double dz = bz - rt::native::gameplay::kTownSpawnZ * .1;
            std::cout << "rt-app terrain: buildings n=" << bn << " centroid=("
                      << bx << ',' << bz << ") dFromSpawn=(" << dx << ',' << dz
                      << ") heading=" << std::atan2(dx, dz) << "rad\n";
        }
    }
    {
        // Height diagnostic (diagnosed the under-town camera, 2026-10-17):
        // overall y range plus the plaza area around the town anchor
        // (506.283, 442.230 game units -> 50.6, 44.2 render units).
        float yMin = 1e30f, yMax = -1e30f;
        float pMin = 1e30f, pMax = -1e30f;
        for (const auto& p : mesh.positions) {
            yMin = std::min(yMin, p[1]);
            yMax = std::max(yMax, p[1]);
            if (std::fabs(p[0] - 50.6f) < 8.f && std::fabs(p[2] - 44.2f) < 8.f) {
                pMin = std::min(pMin, p[1]);
                pMax = std::max(pMax, p[1]);
            }
        }
        std::cout << "rt-app terrain: y-range=" << yMin << ".." << yMax
                  << " plaza(50,44)y=" << (pMax > pMin ? pMin : -999.f) << ".."
                  << (pMax > pMin ? pMax : -999.f) << "\n";
        // Per-loaded tile actual placed ranges (to see if local verts + offset
        // produce contiguous coverage or leave lattice gaps).
        for (const auto& lt : loadedTiles) {
            float xmin=1e30f, xmax=-1e30f, zmin=1e30f, zmax=-1e30f;
            for (std::uint32_t vi=0; vi<lt.vcount; ++vi) {
                const auto& p = mesh.positions[static_cast<std::size_t>(lt.vstart)+vi];
                xmin = std::min(xmin, p[0]); xmax=std::max(xmax,p[0]);
                zmin = std::min(zmin, p[2]); zmax=std::max(zmax,p[2]);
            }
            std::cout << "rt-app terrain: tile " << lt.name << " placed x=" << xmin << ".." << xmax
                      << " z=" << zmin << ".." << zmax << " (origin " << lt.ox << "," << lt.oz << ")\n";
            if (lt.vstart + lt.vcount <= mesh.colours.size()) {
                double cr = 0, cg = 0, cb = 0;
                for (std::uint32_t vi = 0; vi < lt.vcount; ++vi) {
                    const auto& c = mesh.colours[static_cast<std::size_t>(lt.vstart) + vi];
                    cr += c[0]; cg += c[1]; cb += c[2];
                }
                std::cout << "  tile " << lt.name << " vtx colour mean="
                          << cr / lt.vcount << ',' << cg / lt.vcount << ',' << cb / lt.vcount << "\n";
            }
        }
        // Colour diagnostics: vertex RGBA means + first textured page's pixel
        // mean (dark-crush hunt 2026-10-17: mid-frame terrain measured
        // ~(7,19,8) with blue crushed; decide decode vs shader).
        if (!mesh.colours.empty()) {
            double r = 0, g = 0, b = 0, a = 0;
            for (const auto& c : mesh.colours) { r += c[0]; g += c[1]; b += c[2]; a += c[3]; }
            const double n = mesh.colours.size();
            std::cout << "rt-app terrain: vtx RGBA mean=" << r / n << ',' << g / n << ','
                      << b / n << ',' << a / n << "\n";
        }
        if (!mesh.textures.empty()) {
            const auto& t = mesh.textures[0];
            double r = 0, g = 0, bl = 0;
            std::size_t px = t.rgba.size() / 4;
            for (std::size_t i = 0; i < t.rgba.size(); i += 4) { r += t.rgba[i]; g += t.rgba[i + 1]; bl += t.rgba[i + 2]; }
            std::cout << "rt-app terrain: page0 " << t.width << 'x' << t.height << " mean="
                      << r / px << ',' << g / px << ',' << bl / px << "\n";
        }
        // Coverage probe (extended per task): 5x5 grid samples around camera
        // path (x 30..70 step 10, z 20..120 step 20). For each, report the
        // tile-origin cell that has at least one terrain vertex within 8 RU
        // (pinpoints missing lattice cells causing the charcoal staircase).
        std::cout << "rt-app terrain: coverage probe (x,z -> tile@origin with vert<8RU):\n";
        for (float px = 30.f; px <= 70.5f; px += 10.f) {
            for (float pz = 20.f; pz <= 120.5f; pz += 20.f) {
                std::string cell = "none";
                float bestD2 = 64.1f;  // just over 8^2
                for (const auto& lt : loadedTiles) {
                    for (std::uint32_t vi = 0; vi < lt.vcount; ++vi) {
                        const auto& p = mesh.positions[static_cast<std::size_t>(lt.vstart) + vi];
                        const float dx = p[0] - px;
                        const float dz = p[2] - pz;
                        const float d2 = dx * dx + dz * dz;
                        if (d2 < bestD2) {
                            bestD2 = d2;
                            if (d2 < 64.f) {
                                cell = lt.name + "@(" + std::to_string(lt.ox) + "," + std::to_string(lt.oz) + ")";
                            }
                        }
                    }
                }
                std::cout << "  coverage(" << px << "," << pz << ")=" << cell << (bestD2 < 64.f ? "" : " (far)") << "\n";
            }
        }
    }
    return mesh;
}

enum class AppState { TITLE, MENU, LOAD, PLAYING };

const char* appStateName(AppState state) {
    switch (state) {
    case AppState::TITLE: return "TITLE";
    case AppState::MENU: return "MENU";
    case AppState::LOAD: return "LOAD";
    case AppState::PLAYING: return "PLAYING";
    }
    return "UNKNOWN";
}

const char* sessionStateName(rt::native::gameplay::SessionState state) {
    using rt::native::gameplay::SessionState;
    switch (state) {
    case SessionState::EXPLORE: return "EXPLORE";
    case SessionState::RACE: return "RACE";
    case SessionState::RESULTS: return "RESULTS";
    }
    return "UNKNOWN";
}

// The render-visible pose of a finished logic step, i.e. exactly what
// previewRenderScene consumes for placement: the snapshot's car and camera
// fields, plus the optional RT_APP_ROUTE follower override the render block
// applies to the car placement. gameplay::RenderPose carries precisely the
// consumed set, so pipeline-external state (HUD, clock, lighting) stays on
// the fixed step.
rt::native::gameplay::RenderPose renderPoseOfSnapshot(
    const rt::native::gameplay::FrameSnapshot& snapshot,
    const std::optional<rt::native::gameplay::NavDrive>& navDrive) {
    rt::native::gameplay::RenderPose pose = rt::native::gameplay::renderPoseOf(snapshot);
    if (navDrive) {
        const rt::native::gameplay::NavPose nav = navDrive->pose();
        pose.carPosition = {nav.position.x, nav.position.z};
        pose.heading = nav.heading;
    }
    return pose;
}

AppState initialAppState() {
    const char* value = std::getenv("RT_APP_STATE");
    if (value && std::strcmp(value, "MENU") == 0) return AppState::MENU;
    if (value && std::strcmp(value, "LOAD") == 0) return AppState::LOAD;
    if (value && std::strcmp(value, "PLAYING") == 0) return AppState::PLAYING;
    return AppState::TITLE;
}

std::vector<rt::native::preview::ScreenQuad> menuQuads(
    AppState state, const rt::native::gameplay::MenuFlow& flow, int width, int height) {
    using Quad = rt::native::preview::ScreenQuad;
    const float scale = std::min(static_cast<float>(width) / 1024.f, static_cast<float>(height) / 720.f);
    const float offsetX = (width - 1024.f * scale) * .5f;
    const float offsetY = (height - 720.f * scale) * .5f;
    const auto rect = [=](float x, float y, float w, float h, std::array<float, 4> color) {
        return Quad{offsetX + x * scale, offsetY + y * scale, w * scale, h * scale, color};
    };
    const std::array<float, 4> panelColor{.08f, .12f, .22f, 1.f};
    const std::array<float, 4> rowColor{.22f, .32f, .52f, 1.f};
    const std::array<float, 4> selectedRowColor{.36f, .56f, .86f, 1.f};
    const std::array<float, 4> cursorColor{1.f, .72f, .16f, 1.f};
    std::vector<Quad> quads{rect(112, 72, 800, 576, panelColor)};
    if (state == AppState::LOAD) return {};
    if (state == AppState::TITLE) {
        quads.push_back(rect(202, 216, 620, 80, {.55f, .78f, 1.f, 1.f}));
        quads.push_back(rect(286, 386, 452, 30, {.88f, .93f, 1.f, 1.f}));
        return quads;
    }

    quads.push_back(rect(202, 120, 620, 34, {.55f, .78f, 1.f, 1.f}));
    constexpr float rowX = 234, rowY = 204, rowWidth = 556, rowHeight = 58, rowGap = 27;
    for (std::size_t row = 0; row < 4; ++row) {
        const float y = rowY + static_cast<float>(row) * (rowHeight + rowGap);
        if (row == flow.topLevelCursor) quads.push_back(rect(198, y - 8, 628, rowHeight + 16, cursorColor));
        quads.push_back(rect(rowX, y, rowWidth, rowHeight, row == flow.topLevelCursor ? selectedRowColor : rowColor));
        quads.push_back(rect(rowX + 28, y + 19, rowWidth - 56, 20, {.88f, .93f, 1.f, 1.f}));
    }
    return quads;
}

// Real title / main-menu / options screens (preview/ui_menu.h). MenuFlow has
// no options cursor, so that screen stays at focus 0. topLevelCursor is the
// mode-select index (0..3); anything outside the main-menu row count falls
// back to 0. PRESS START blinks on a 32-tick phase of the host frame counter.
rt::native::preview::UiScreen buildLiveMenuScreen(
    AppState state, const rt::native::gameplay::MenuFlow& flow,
    const rt::native::preview::TitleUiState& titleUi, int frames, int width, int height) {
    if (state == AppState::LOAD) {
        auto draw = titleUi.emit(width, height);
        return {std::move(draw.quads), std::move(draw.text)};
    }
    if (state == AppState::TITLE) {
        const bool pressVisible = (frames / 32) % 2 == 0;
        return rt::native::preview::buildTitleScreen(width, height, pressVisible);
    }
    if (flow.screen == rt::native::gameplay::MenuScreen::OPTIONS)
        return rt::native::preview::buildOptionsScreen(width, height, 0);
    int focus = 0;
    if (flow.topLevelCursor < static_cast<std::size_t>(rt::native::preview::kMainMenuRowCount))
        focus = static_cast<int>(flow.topLevelCursor);
    return rt::native::preview::buildMainMenu(width, height, focus);
}

void compositeOver(std::vector<std::uint8_t>& scene, const std::vector<std::uint8_t>& overlay) {
    if (scene.size() != overlay.size()) return;
    for (std::size_t pixel = 0; pixel < scene.size(); pixel += 4) {
        const float alpha = static_cast<float>(overlay[pixel + 3]) / 255.f;
        if (alpha <= 0.f) continue;
        for (std::size_t channel = 0; channel < 3; ++channel) {
            const float source = static_cast<float>(overlay[pixel + channel]);
            const float destination = static_cast<float>(scene[pixel + channel]);
            scene[pixel + channel] = static_cast<std::uint8_t>(source * alpha + destination * (1.f - alpha) + .5f);
        }
    }
}
} // namespace

#ifdef __ANDROID__
extern "C" int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
#else
int main() {
#endif
    constexpr int kInitialWidth = 800, kInitialHeight = 600;
#ifdef __ANDROID__
    // SDL has installed its Android activity before entering SDL_main/main.
    // This precedes every RT_* getenv use, including settings and disc lookup.
    androidBootstrapEnvironment();
    const char* envState = std::getenv("RT_APP_STATE");
    const char* envTerrain = std::getenv("RT_APP_TERRAIN");
    const char* envFrameLimit = std::getenv("RT_APP_FRAMES");
    androidLog(ANDROID_LOG_INFO, std::string("startup: state=") + (envState ? envState : "TITLE") +
        " terrain=" + (envTerrain ? envTerrain : "0") + " frames=" + (envFrameLimit ? envFrameLimit : "unlimited"));
#endif
    // RT_APP_HEADLESS=1: SDL dummy video driver + null presenter.  The game
    // then runs logic + offscreen Vulkan scene/HUD + RT_APP_SNAPSHOT capture
    // with NO WindowServer session (present() no-ops without a swapchain,
    // pacing is time-based) — unblocks pixel verification in sandboxes/CI
    // that lost GUI access.
    const bool headlessApp = std::getenv("RT_APP_HEADLESS") != nullptr;
    // W16.5: headless + RT_APP_RENDERER=gl keeps a real (hidden) video driver: the dummy driver cannot make a GL
    // context.  The scene renders into the renderer's FBO and is read back as in windowed GL; nothing is presented.
    const bool headlessGl = headlessApp && rt::native::preview::parseRendererMode(std::getenv("RT_APP_RENDERER")) ==
                                               rt::native::preview::RendererMode::Gl;
    if (headlessApp && !headlessGl) SDL_SetHint("SDL_VIDEO_DRIVER", "dummy");  // SDL3: no macro
    bootstrapSettings();
    (void)rt::native::assets::RunFirstRunSetupIfRequired();
    // W3-06 enhancement registry (RT_ENHANCE_*): parsed once at startup; every
    // default equals the legacy behaviour, so an unset environment is a no-op.
    // Applied here: target_hz (render cap fallback), fps_display, log_fps.
    // ssaa and physics_hz are registered but have no native application site
    // yet (no supersample path; logic quantum is fixed by
    // FixedStepClock::kStepSeconds) — logged for transparency, wave-board GAP.
    const auto& enhancements = rt::native::app::Enhancements::instance();
    std::cout << "[enhance] " << enhancements.describeAll() << "\n";
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) { std::cerr << "SDL_Init: " << SDL_GetError() << "\n"; return 1; }
    // Output-mode selection (RT_APP_RESOLUTION / RT_APP_FULLSCREEN,
    // output-modes.md).  Unset env stays exactly the legacy 800x600 windowed
    // path: matrix quiet, default snapshots byte-identical.  The title matches
    // the sibling app's chrome ("Road Trip Adventure", src/main.cpp:418) per
    // the UI-parity mandate (no native labels).
    int windowWidth = kInitialWidth, windowHeight = kInitialHeight;
    if (const char* resText = std::getenv("RT_APP_RESOLUTION"); resText && *resText) {
        const rt::native::app::Resolution parsed = rt::native::app::parseResolution(resText);
        if (rt::native::app::valid(parsed)) {
            windowWidth = parsed.width;
            windowHeight = parsed.height;
        } else if (rt::native::app::isNativeDesktop(parsed) && rt::native::app::isPresetName(resText)) {
            const SDL_DisplayMode* desktop = SDL_GetDesktopDisplayMode(SDL_GetPrimaryDisplay());
            if (desktop && desktop->w > 0 && desktop->h > 0) {
                windowWidth = static_cast<int>(desktop->w);
                windowHeight = static_cast<int>(desktop->h);
            }
        } else {
            std::cout << "rt-app resolution: invalid RT_APP_RESOLUTION='" << resText
                      << "' (expected <preset|WxH>), using desktop window defaults\n";
        }
    }
    const bool startFullscreen = [] {
        const char* fs = std::getenv("RT_APP_FULLSCREEN");
        return fs && std::string_view(fs) == "1";
    }();
    std::cout << "rt-app output: " << windowWidth << "x" << windowHeight
              << (startFullscreen ? " fullscreen" : " windowed") << " progressive\n";
    // W16.2: presenter backend (RT_APP_RENDERER=gl -> GL presenter; auto retries GL if Vulkan init fails).
    // Unset/vulkan keeps the exact legacy SDL_WINDOW_VULKAN path.
    const rt::native::preview::RendererMode rendererMode =
        rt::native::preview::parseRendererMode(std::getenv("RT_APP_RENDERER"));
    bool useGlPresenter = (!headlessApp || headlessGl) &&
        rt::native::preview::selectBackend(rendererMode, true, true) == rt::native::preview::Backend::Gl;
    const auto createWindow = [&](bool gl) {
        if (gl) rt::native::preview::glPresenterSetWindowAttributes();
        return SDL_CreateWindow(
            "Road Trip Adventure", windowWidth, windowHeight,
            (headlessGl ? SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN
                : headlessApp ? SDL_WINDOW_RESIZABLE
                         : (gl ? SDL_WINDOW_OPENGL : SDL_WINDOW_VULKAN) | SDL_WINDOW_RESIZABLE)
                | (startFullscreen && !headlessApp ? SDL_WINDOW_FULLSCREEN : 0));
    };
    SDL_Window* window = createWindow(useGlPresenter);
    if (!rt::native::app::progressiveContractSatisfied({})) {
        std::cout << "rt-app output: PROGRESSIVE CONTRACT VIOLATION\n";
    }
    if (!window) { std::cerr << "SDL_CreateWindow: " << SDL_GetError() << "\n"; SDL_Quit(); return 1; }
    Presenter presenter; std::string error; rt::native::preview::GlPresenter* glPresenter = nullptr;
    if (headlessApp) {
        presenter.extent.width = static_cast<std::uint32_t>(windowWidth);
        presenter.extent.height = static_cast<std::uint32_t>(windowHeight);
        std::cout << "rt-app headless: " << (headlessGl ? "hidden GL window" : "dummy video") << ", null presenter "
                  << windowWidth << "x" << windowHeight << "\n";
        if (headlessGl) {
            glPresenter = rt::native::preview::glPresenterCreate(window, error);
            if (!glPresenter) { std::cerr << "headless GL init: " << error << "\n"; SDL_DestroyWindow(window); SDL_Quit(); return 1; }
        }
    } else if (useGlPresenter) {
        glPresenter = rt::native::preview::glPresenterCreate(window, error);
        if (!glPresenter) { std::cerr << "GL init: " << error << "\n"; SDL_DestroyWindow(window); SDL_Quit(); return 1; }
        rt::native::preview::glPresenterResize(glPresenter, presenter.extent.width, presenter.extent.height);
    } else if (!init(presenter, window, error)) {
        destroy(presenter);
        if (rendererMode == rt::native::preview::RendererMode::Auto) {  // auto: GL only when Vulkan init failed
            std::cerr << "Vulkan init: " << error << " -- retrying with GL presenter\n";
            SDL_DestroyWindow(window); window = createWindow(true);
            error.clear();
            (void)setenv("RT_APP_RENDERER", "gl", 1);  // Vulkan is unusable: the scene renderer must be GL too, not a second Vulkan attempt
            glPresenter = window ? rt::native::preview::glPresenterCreate(window, error) : nullptr;
            if (!glPresenter) { std::cerr << "GL init: " << (window ? error : SDL_GetError()) << "\n"; if (window) SDL_DestroyWindow(window); SDL_Quit(); return 1; }
            rt::native::preview::glPresenterResize(glPresenter, presenter.extent.width, presenter.extent.height);
        } else { std::cerr << "Vulkan init: " << error << "\n"; SDL_DestroyWindow(window); SDL_Quit(); return 1; }
    }
    int width = static_cast<int>(presenter.extent.width), height = static_cast<int>(presenter.extent.height);
    // W16.3: the GL scene backend shares the presenter context, so it exists only when that context does.
    if (glPresenter) rt::native::preview::registerGlBackend(rt::native::preview::glBackendOps());
    rt::native::preview::Renderer* renderer = rt::native::preview::previewInit(width, height, error, nullptr);
    if (!renderer) { std::cerr << "scene renderer init: " << error << "\n"; destroy(presenter); rt::native::preview::glPresenterDestroy(glPresenter); SDL_DestroyWindow(window); SDL_Quit(); return 1; }
    const bool noRender = std::getenv("RT_APP_NORENDER") != nullptr; const int maxFrames = envFrames();
    const bool triggersEnabled = [] {
        const char* value = std::getenv("RT_APP_TRIGGERS");
        return value && std::strcmp(value, "1") == 0;
    }();
    if (triggersEnabled) std::cout << "rt-app triggers: enabled\n";
    const bool perfOverlayEnabled = [] {
        const char* value = std::getenv("RT_APP_PERFOVERLAY");
        return (value && std::strcmp(value, "1") == 0) ||
               rt::native::app::Enhancements::instance().options().fps_display;
    }();
    const bool perfLogEnabled = [] {
        const char* value = std::getenv("RT_APP_PERFLOG");
        return (value && std::strcmp(value, "1") == 0) ||
               rt::native::app::Enhancements::instance().options().log_fps;
    }();
    // RT_APP_STREAM=1 (with RT_APP_TERRAIN=1) replaces the eager 64-tile
    // load with hardware-style 4-slot streaming (slot = id & 3).
    // GAME-LOOK v1: all scene asset decoding (terrain, streaming, nav,
    // route driver, FONTE atlas) is built lazily on the first PLAYING frame
    // so TITLE/attract boot never pays the ~11 s terrain+texture+glyph cost
    // it used to pay at startup on every launch state.
    rt::native::preview::TerrainMesh terrainMesh;
    std::optional<StreamedTerrain> streamed;
    std::vector<rt::native::preview::RouteLine> navLines;
    // Minimap road ink: Peach Town street graph (NavRouteTables townGraph(1),
    // tile-local game units — same space as snapshot.carPosition) stamped
    // navy into the bottom-left pill on drive frames.
    std::vector<std::array<float, 4>> mmRoads;
    // Race-map v1 ink (W3-02 law, oracle-golden adjudicated 2026-10-18): the
    // full segment list of staticGraph(10) (the 143-node loop chosen by IoU
    // overlay against build/scratch/oracle-golden/race_hud.png), plus the
    // auto-fit parameters — world-space centre of the node bounding box and
    // the uniform px-per-unit scale that fits height 138 / width 64 design px.
    std::vector<std::array<float, 4>> mmRaceNodes;
    float mmRaceCx = 0.f, mmRaceCz = 0.f, mmRaceFit = 0.f;
    std::optional<rt::native::gameplay::NavDrive> navDrive;
    constexpr float kNavRouteSpeedUnitsPerFrame = 30.f;
    int navRouteLap = 0;
    bool hudTextEnabled = false;
    bool scoreHudEnabled = false;
    bool scoreHudMeter = false;
    bool hudTextWarned = false;
    rt::native::assets::GlyphAtlas fontAtlas;
    auto initSceneAssets = [&]() {
#ifdef __ANDROID__
        androidLog(ANDROID_LOG_INFO, "scene: assets init begin");
#endif
    if (std::getenv("RT_APP_STREAM") != nullptr && featureDefaultOn("RT_APP_TERRAIN")) {
        if (std::getenv("RT_APP_TERRAIN_FILE") != nullptr)
            std::cout << "rt-app stream: RT_APP_TERRAIN_FILE ignored under stream mode\n";
        auto index = rt::native::assets::buildFieldTileIndex(discRoot());
        if (!index) {
            std::cout << "rt-app stream: SKIP (no tile index; err="
                      << static_cast<int>(index.error()) << ")\n";
        } else {
            StreamedTerrain st;
            st.root = discRoot();
            st.index = *index;
            st.wantTextures = featureDefaultOn("RT_APP_TEXTURES");
            const char* stepValue = std::getenv("RT_APP_STREAM_STEP_FRAMES");
            const char* rowValue = std::getenv("RT_APP_STREAM_ROW");
            const int step = stepValue ? std::atoi(stepValue) : 10;
            const int startRow = rowValue ? std::atoi(rowValue) : 1;
            st.streamer = rt::native::gameplay::TileStreamer(step, startRow, 0);
            streamed = std::move(st);
            std::cout << "rt-app stream: 4-slot streaming textured=" << (streamed->wantTextures ? 1 : 0)
                      << " step=" << step << " startRow=" << startRow << "\n";
        }
    } else {
        terrainMesh = loadTerrainMesh();
    }
    // RT_APP_NAV=1 overlays a ROM nav-graph's centerline segments (static
    // graph 0 = the COURSE/C00 race-route pack graph, per
    // docs/native-reference/scene-tile-map.md — not Peach streets) as bright
    // yellow strips.  RT_APP_NAV_TOWN=1 instead draws the AUTHENTIC Peach Town
    // street graph (scene T=1, outer dir @0x2d98a8 -> car0 pair
    // 0x2c5ff0/0x2c67d0, 84 segments) — tile-local [0,1600] game units, so
    // scaled 0.1 both overlays land inside the anchored Peach tile at (0,0).
    if (std::getenv("RT_APP_NAV") != nullptr) {
        auto tables = rt::native::assets::NavRouteTables::open(discRoot() / "SLUS_203.98");
        if (!tables) {
            std::cout << "rt-app nav: SKIP (ELF unavailable, err=" << static_cast<int>(tables.error()) << ")\n";
        } else {
            const bool navTown = std::getenv("RT_APP_NAV_TOWN") != nullptr;
            auto graph = navTown ? tables->townGraph(1) : tables->staticGraph(0);
            if (!graph) {
                std::cout << "rt-app nav: SKIP (" << (navTown ? "town graph 1 absent" : "static graph 0 absent")
                          << ")\n";
            } else {
                for (const auto& seg : graph->segments)
                    navLines.push_back({seg.ax * .1f, seg.az * .1f, seg.bx * .1f, seg.bz * .1f,
                                       1.f, .85f, .15f});
                if (navTown)
                    std::cout << "rt-app nav: town graph 1 segments=" << navLines.size()
                              << " vaddr=0x" << std::hex << graph->segmentVaddr << std::dec << "\n";
                else
                    std::cout << "rt-app nav: static graph 0 segments=" << navLines.size()
                              << " vaddr=0x" << std::hex << graph->segmentVaddr << std::dec << "\n";
            }
        }
    }
    // Minimap roads: authentic Peach Town street graph (outer dir @0x2d98a8
    // -> car0 pair 0x2c5ff0/0x2c67d0, 84 segments; scene-tile-map.md), kept
    // in tile-local game units because the pill projection works in that
    // space (delta from snapshot.carPosition).  Loaded whenever the pill is
    // drawn (RT_APP_MINIMAP=0 kills both).
    if (!(std::getenv("RT_APP_MINIMAP") != nullptr && std::string_view(std::getenv("RT_APP_MINIMAP")) == "0")) {
        auto mmTables = rt::native::assets::NavRouteTables::open(discRoot() / "SLUS_203.98");
        if (mmTables) {
            if (auto graph = mmTables->townGraph(1)) {
                for (const auto& seg : graph->segments)
                    mmRoads.push_back({seg.ax, seg.az, seg.bx, seg.bz});
                std::cout << "rt-app minimap: town street segments=" << mmRoads.size() << "\n";
            }
            if (auto race = mmTables->staticGraph(10)) {
                float minX = 1e30f, maxX = -1e30f, minZ = 1e30f, maxZ = -1e30f;
                for (const auto& seg : race->segments) {
                    mmRaceNodes.push_back({seg.ax, seg.az, seg.bx, seg.bz});
                    minX = std::min(minX, std::min(seg.ax, seg.bx));
                    maxX = std::max(maxX, std::max(seg.ax, seg.bx));
                    minZ = std::min(minZ, std::min(seg.az, seg.bz));
                    maxZ = std::max(maxZ, std::max(seg.az, seg.bz));
                }
                if (!mmRaceNodes.empty() && maxX > minX && maxZ > minZ) {
                    // Uniform fit: height 138 design px, bounded by the 64 px
                    // window width (see the race-map v1 law at the draw site).
                    mmRaceFit = std::min(138.f / (maxZ - minZ), 64.f / (maxX - minX));
                    mmRaceCx = (minX + maxX) * .5f;
                    mmRaceCz = (minZ + maxZ) * .5f;
                }
                std::cout << "rt-app race map: static graph 10 segments=" << mmRaceNodes.size()
                          << " fit=" << mmRaceFit << " centre=("
                          << mmRaceCx << "," << mmRaceCz << ")\n";
            }
        }
    }
    // RT_APP_ROUTE=1 drives the render-visible demo car along the static
    // graph 0 centerline (COURSE/C00 race route per scene-tile-map.md) with
    // a deterministic arc-length follower (gameplay/nav_drive).  Session
    // authoring keeps its own car state; only the snapshot handed to the
    // scene renderer is overridden.
    if (std::getenv("RT_APP_ROUTE") != nullptr) {
        auto tables = rt::native::assets::NavRouteTables::open(discRoot() / "SLUS_203.98");
        if (!tables) {
            std::cout << "rt-app route: SKIP (ELF unavailable, err=" << static_cast<int>(tables.error()) << ")\n";
        } else {
            const bool routeTown = std::getenv("RT_APP_ROUTE_TOWN") != nullptr;
            auto graph = routeTown ? tables->townGraph(1) : tables->staticGraph(0);
            if (!graph) {
                std::cout << "rt-app route: SKIP (" << (routeTown ? "town graph 1 absent" : "static graph 0 absent")
                          << ")\n";
            } else {
                const rt::native::gameplay::NavPath path = rt::native::gameplay::buildNavPath(*graph);
                if (path.centerline.size() < 2) {
                    std::cout << "rt-app route: SKIP (centerline has no segments)\n";
                } else {
                    navDrive.emplace(path);
                    std::cout << "rt-app route: " << (routeTown ? "town" : "nav")
                              << " lap drive segments="
                              << navDrive->segmentCount()
                              << " length=" << navDrive->totalLength() << "\n";
                }
            }
        }
    }
    // RT_APP_HUDTEXT=1 presents HUD text through the textured pass: decode
    // the shipped FONTE glyph atlas once (SYS/FONTE.GSL) and place race
    // lap/time + clock strings at the ROM-proven elements
    // (docs/native-reference/hud-layout.md; glyph grid per
    // docs/native-reference/sys-gsl-packs.md §4 corrected 2026-10-16).
    hudTextEnabled = featureDefaultOn("RT_APP_HUDTEXT");
    // RT_APP_SCOREHUD[=meter]: overlay the SCORE/METER page text builders
    // (livery-test style presentation).  Timer + speed-digit placements are
    // hud-layout.md CONFIRMED race-HUD anchors; the score/money label block
    // is AUTHORED (no ROM evidence for the livery-test page layout), and
    // score value here is a demo aggregate of collected collectibles.
    const char* scoreHudEnv = std::getenv("RT_APP_SCOREHUD");
    scoreHudEnabled = scoreHudEnv != nullptr;
    scoreHudMeter = scoreHudEnv && std::string(scoreHudEnv) == "meter";
    if (hudTextEnabled && !loadFonteAtlas(fontAtlas)) hudTextEnabled = false;
#ifdef __ANDROID__
        androidLog(ANDROID_LOG_INFO, "scene: assets init done");
#endif
    };
    bool sceneAssetsReady = false;
    AudioHost audio;
    if (std::getenv("RT_APP_NOAUDIO") == nullptr) (void)initAudio(audio);
    const auto recreateSwapchain = [&]() {
        if (headlessApp) return true;  // no surface exists to rebuild
        int newW = 0, newH = 0; SDL_GetWindowSizeInPixels(window, &newW, &newH);
        if (newW <= 0 || newH <= 0) return true;
        if (glPresenter) rt::native::preview::glPresenterResize(glPresenter, presenter.extent.width, presenter.extent.height);
        else if (!makeSwapchain(presenter, window, error)) { std::cerr << "swapchain recreate: " << error << "\n"; return false; }
        rt::native::preview::previewDestroy(renderer);
        renderer = rt::native::preview::previewInit(presenter.extent.width, presenter.extent.height, error, nullptr);
        if (!renderer) { std::cerr << "scene resize: " << error << "\n"; return false; }
        return true;
    };
    auto car = demoCar();
    bool carIsBox = true;
    // Default player car: CAR0/Q00.BIN via loadCarMeshFromFile, once. Primary
    // is the disc extract; fallback is the setup-wizard assets cache
    // (cars/CAR0/Q00.BIN). RT_APP_CARMODEL keeps its own livery path below.
    if (!std::getenv("RT_APP_CARMODEL")) {
        const auto carPath = resolveDiscOrPackaged("CAR0/Q00.BIN", "cars/CAR0/Q00.BIN");
        rt::native::preview::CarMeshResult carMesh;
        if (!carPath.empty() &&
            rt::native::preview::loadCarMeshFromFile(carPath.string(), carMesh) &&
            carMesh.model.vertices.size() >= 3 && carMesh.model.indices.size() >= 3) {
            // previewRenderScene heads and scales along Z (4.4 / z-extent).
            // Q00's longest horizontal is often X; yaw onto Z once so the
            // silhouette matches the demo box's length axis.
            if (carMesh.chosenAxis == "x") {
                for (auto& v : carMesh.model.vertices) {
                    const float x = v.px;
                    v.px = -v.pz;
                    v.pz = x;
                    const float nx = v.nx;
                    v.nx = -v.nz;
                    v.nz = nx;
                }
            }
            car = std::move(carMesh.model);
            carIsBox = false;
            std::cout << "app: car model " << carPath.string()
                      << " verts=" << car.vertices.size()
                      << " tris=" << car.indices.size() / 3
                      << " scale=" << carMesh.appliedScale
                      << " axis=" << carMesh.chosenAxis << "\n";
        }
    }
    // RT_APP_CARMODEL[=N]: replace the authored demo box with a real shipped
    // car model (Q{N}.BIN under CAR0..CAR5, else CARS) through the proven
    // CAR pipeline (docs/native-reference/car-data-status.md): flat mode
    // gains the authentic silhouette, textured mode samples the car's own
    // 128x128 PSMT8 livery.  Load failures degrade to the demo box.
    std::optional<rt::native::preview::PreviewTexture> carLivery;
    // Shipped race-HUD art (dial face METER0.GSL + SCORE.GSL atlas) loaded
    // once at runtime; missing disc leaves the authored placeholders.
    static const rt::native::assets::HudArt hudArt = rt::native::assets::loadHudArt(discRoot());
    if (const char* dbg = std::getenv("RT_APP_CARDIAG"); dbg && *dbg == '1')
        std::cout << "rt-app hudart: dial=" << hudArt.speedDial.has_value()
                  << " digits=" << hudArt.speedDigits.has_value()
                  << " banner=" << hudArt.raceBanner.has_value()
                  << " atlas=" << hudArt.raceAtlas.has_value()
                  << " lapDigits=" << hudArt.lapDigits.has_value()
                  << " lapDisc=" << hudArt.lapDisc.has_value()
                  << " gauge=" << hudArt.gaugePage.has_value() << "\n";
    if (const char* cm = std::getenv("RT_APP_CARMODEL")) {
        const int q = *cm ? std::atoi(cm) : 0;
        char leaf[24];
        std::snprintf(leaf, sizeof leaf, "Q%02d.BIN", q);
        std::filesystem::path found;
        for (const char* dir : {"CAR0", "CAR1", "CAR2", "CAR3", "CAR4", "CAR5", "CARS"}) {
            const auto candidate = discRoot() / dir / leaf;
            if (std::filesystem::exists(candidate)) { found = candidate; break; }
        }
        if (found.empty()) {
            std::cout << "rt-app carmodel: SKIP (" << leaf << " not in extract)\n";
        } else {
            std::ifstream in(found, std::ios::binary);
            std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(in)),
                                           std::istreambuf_iterator<char>());
            // Provenance digest: same FNV-1a/xorshift placeholder as the
            // converter tools (hasValidProvenance only rejects all-zero).
            std::array<std::uint8_t, 32> digest{};
            std::uint64_t hv = 0xcbf29ce484222325ull;
            for (std::uint8_t b : bytes) { hv ^= b; hv *= 0x100000001b3ull; }
            std::uint64_t st = hv | 1ull;
            for (auto& d : digest) { st ^= st << 13; st ^= st >> 7; st ^= st << 17; d = static_cast<std::uint8_t>(st); }
            auto loaded = rt::native::assets::loadCarAssets(bytes, found.string(), digest);
            if (loaded.err != rt::native::assets::CarAssetError::Ok) {
                std::cout << "rt-app carmodel: SKIP (" << leaf << " err="
                          << static_cast<int>(loaded.err) << " offset=" << loaded.errorOffset << ")\n";
            } else {
                rt::native::assets::NativeModel merged;
                for (const auto& m : loaded.models) {
                    const auto base = static_cast<std::uint32_t>(merged.vertices.size());
                    merged.vertices.insert(merged.vertices.end(), m.vertices.begin(), m.vertices.end());
                    for (std::uint32_t i : m.indices) merged.indices.push_back(base + i);
                }
                if (merged.vertices.size() < 3 || merged.indices.size() < 3) {
                    std::cout << "rt-app carmodel: SKIP (" << leaf << " empty mesh)\n";
                } else {
                    car = std::move(merged);
                    carIsBox = false;
                    const auto& img = loaded.container.images[0];
                    carLivery = rt::native::preview::PreviewTexture{img.width, img.height, img.pixels};
                    float u0 = 1e9f, u1 = -1e9f, v0 = 1e9f, v1 = -1e9f;
                    for (const auto& v : car.vertices) {
                        u0 = std::min(u0, v.u); u1 = std::max(u1, v.u);
                        v0 = std::min(v0, v.v); v1 = std::max(v1, v.v);
                    }
                    std::cout << "rt-app carmodel: Q" << q << " meshes=" << loaded.models.size()
                              << " verts=" << car.vertices.size() << " tris=" << car.indices.size() / 3
                              << " livery=" << img.width << "x" << img.height
                              << " uv=[" << u0 << ',' << u1 << "]x[" << v0 << ',' << v1 << "]\n";
                    // Optional livery dump for eyeball verification:
                    // RT_APP_CARMODEL_DUMP=PATH writes the RGBA8 record as PPM.
                    if (const char* dump = std::getenv("RT_APP_CARMODEL_DUMP")) {
                        std::ofstream out(dump, std::ios::binary);
                        out << "P6\n" << img.width << ' ' << img.height << "\n255\n";
                        for (std::size_t i = 0; i + 3 < img.pixels.size(); i += 4)
                            out.put(static_cast<char>(img.pixels[i]))
                                .put(static_cast<char>(img.pixels[i + 1]))
                                .put(static_cast<char>(img.pixels[i + 2]));
                    }
                }
            }
        }
    }
    if (carIsBox) std::cout << "app: car model fallback (box)\n";
    // W3-04 slot-store seam (docs/native-reference/save-layout.md): native v2
    // save files (Container-wrapped SaveData, atomic tmp+rename, v1 raw
    // migration).  Direct slot overrides remain developer-opt-in so tests stay
    // deterministic with the defaults OFF:
    //   RT_APP_LOAD_SLOT=<0|1>  restore that slot into session progress at boot
    //   RT_APP_SAVE_SLOT=<0|1>  write session progress to that slot at shutdown
    rt::native::gameplay::SlotStore slotStore;
    rt::native::preview::FakeCardBackend titleCard;
    if (slotStore.list()[0]) {
        rt::native::gameplay::SaveData saved{};
        std::string saveError;
        titleCard.setSlot(0, {.exists = true, .valid = slotStore.load(0, saved, &saveError),
                              .corrupt = !saveError.empty()});
    }
    rt::native::preview::TitleUiState titleUi(titleCard);
    const auto envSlot = [](const char* name) -> std::optional<int> {
        const char* raw = std::getenv(name);
        if (!raw) return std::nullopt;
        if (raw[0] == '0' && raw[1] == '\0') return 0;
        if (raw[0] == '1' && raw[1] == '\0') return 1;
        std::cout << "[save] ignoring " << name << "=\"" << raw << "\" (expect 0 or 1)\n";
        return std::nullopt;
    };
    // One lap lets the headless host exercise the complete authored session
    // loop promptly; GameSession enables race mode for its owned GameLoop.
    rt::native::gameplay::GameSession session(1);
    if (const auto slot = envSlot("RT_APP_LOAD_SLOT")) {
        rt::native::gameplay::SaveData loaded{};
        std::string error;
        if (slotStore.load(*slot, loaded, &error)) {
            auto& progress = const_cast<rt::native::gameplay::SaveData&>(session.progress());
            progress = std::move(loaded);
            std::cout << "[save] loaded slot " << *slot << " (money " << progress.money
                      << ", radio " << static_cast<int>(progress.radioStation) << ")\n";
        } else {
            std::cout << "[save] load slot " << *slot << " failed: " << error << " (continuing fresh)\n";
        }
    }
    // --- Radio glue (docs/native-reference/radio-stations.md §5) ------------
    // Explicit RT_APP_RADIO selects the station byte; "0" disables. Unset
    // defaults ON (Peach FM) when audio init succeeded and the station VAG
    // exists. bootstrapSettings() can still seed the env from
    // sound.radio_station; an explicit environment value wins.
    rt::native::gameplay::RadioState radio(session.progress().radioStation);
    bool radioFollow = false;
    if (audio.stream) {
        if (const auto station = radioStationRequest()) {
            radio.requestSaveByte(*station);  // clamps 0..2 (radio.cpp:57)
            radioFollow = true;
        }
    }
    std::cout << "app: radio=" << (radioFollow ? "on" : "off") << "\n";
    // startRadio decodes the station's authentic stereo pair and hands it to the
    // SoundEngine as TWO hard-panned voices, matching the guest's SNDMOD split
    // (radio-stations.md §5 "Stereo (AUTHORED mapping)"): the L file plays at
    // pan -1 (left speaker only) and the R file at pan +1 (right speaker only).
    // SoundEngine::setVoicePan (engine.h) re-pans the bound BGM voices; the
    // mixer's PanLaw (mixer.h:48-61) yields L=1,R=0 at pan=-1 and L=0,R=1 at
    // pan=+1. If the R half fails to load we fall back to the L half centred
    // (mono, audible both sides) rather than losing a speaker.
    const auto startRadio = [&](const rt::native::gameplay::RadioVagPair& pair) {
        rt::native::audio::PcmBuffer left, right;
        const auto leftPath = resolveDiscOrPackaged(pair.left, std::filesystem::path("audio") / pair.left);
        if (leftPath.empty() || !rt::native::audio::loadVagFile(leftPath, left)) {
            std::cerr << "rt-app radio: VAG load failed: "
                      << (leftPath.empty() ? (discRoot() / pair.left).string() : leftPath.string()) << "\n";
            return;
        }
        bool haveRight = false;
        const auto rightPath = resolveDiscOrPackaged(pair.right, std::filesystem::path("audio") / pair.right);
        if (!rightPath.empty() && rt::native::audio::loadVagFile(rightPath, right))
            haveRight = true;
        SDL_LockAudioStream(audio.stream);   // fillAudio ticks the engine concurrently
        audio.engine.stopBgm();              // mirrors cmd 0x35 (options Off, 0x213A30)
        audio.radioPcm = std::move(left);    // host owns the PCM lifetime
        audio.engine.playBgm(0, 0u,
                             {audio.radioPcm.samples.data(), audio.radioPcm.frames, /*loop=*/true}, .7f);
        audio.engine.setVoicePan(0, haveRight ? -1.f : 0.f);
        if (haveRight) {
            audio.radioPcmRight = std::move(right);
            audio.engine.playBgm(1, 0u,
                                 {audio.radioPcmRight.samples.data(), audio.radioPcmRight.frames, /*loop=*/true},
                                 .7f);
            audio.engine.setVoicePan(1, +1.f);
        } else {
            audio.radioPcmRight = {};
        }
        SDL_UnlockAudioStream(audio.stream);
        std::cout << "rt-app radio: started " << pair.left
                  << (haveRight ? " + R (hard-panned stereo)" : " (mono-left fallback, R half missing)")
                  << " (" << audio.radioPcm.frames << " frames)\n";
    };
    const auto stopRadio = [&] {
        SDL_LockAudioStream(audio.stream);
        audio.engine.stopBgm();
        audio.radioPcm = {};
        audio.radioPcmRight = {};
        SDL_UnlockAudioStream(audio.stream);
        std::cout << "rt-app radio: stopped\n";
    };
    // These host-owned examples deliberately use synthetic names, IDs, and
    // positions; they do not load any guest game data.
    std::vector<rt::native::gameplay::Collectible> collectibles{
        {rt::native::gameplay::CollectibleKind::STAMP, {400.f, 400.f}, 30.f, 0},
        {rt::native::gameplay::CollectibleKind::COIN, {1200.f, 400.f}, 30.f, 0},
        {rt::native::gameplay::CollectibleKind::STAMP, {1200.f, 1200.f}, 30.f, 1},
    };
    rt::native::gameplay::Shop shop{{
        {1, "road map", 100, 1},
        {2, "fuel can", 250, 1},
        {3, "lucky charm", 500, 1},
    }};
    rt::native::gameplay::Dialogue dialogue;
    const std::vector<rt::native::gameplay::DialogueLine> dialogueLines{
        {"Guide", "Welcome to the scenic route."},
        {"Guide", "Collect stamps and coins as you drive."},
        {"Guide", "The shop has supplies when you need them."},
    };
    bool shopOpen = false;
    rt::native::gameplay::ControllerConfig controllerConfig;
    bool playerDriven = false, running = true; int frames = 0;
    std::uint64_t simulationFrame = 0;
    NativeBoot nativeBoot = nativeBootFromEnv();
    AppState state = initialAppState();
    // RT_APP_MODE=race: straight to SessionState::RACE at startup (desktop
    // race-HUD capture for the shipped dial; the menu path needs interaction
    // the headless harness lacks).  Autopilot (!playerDriven) drives.
    if (state == AppState::PLAYING) {
        if (const char* mode = std::getenv("RT_APP_MODE"); mode && std::strcmp(mode, "race") == 0) {
            session.startRace();
            std::cout << "rt-app mode: RACE (forced by RT_APP_MODE=race)\n";
        }
    }
    // RT_APP_STATE=PLAYING is the host's direct-launch shortcut.  Log and
    // expose its initial EXPLORE state for one frame, then start the same
    // first AI race that older direct launches used.
    bool directStartRace = state == AppState::PLAYING;
    // RT_APP_MODE=explore holds SessionState::EXPLORE indefinitely for the
    // drive-mode HUD capture against oracle town_driving.png — the legacy
    // direct-launch auto-race leaves only one explore frame.
    if (const char* mode = std::getenv("RT_APP_MODE"); mode && std::strcmp(mode, "explore") == 0)
        directStartRace = false;
    rt::native::gameplay::MenuFlow menuFlow;
    if (state == AppState::MENU) menuFlow.screen = rt::native::gameplay::MenuScreen::MODE_SELECT;
    rt::native::preview::TitleAction titleAction = rt::native::preview::TitleAction::None;
    bool loggedAppState = false, loggedSessionState = false;
    AppState lastLoggedAppState = state;
    rt::native::gameplay::SessionState lastLoggedSessionState = session.state();
    // Session snapshot of the last completed logic step. Hoisted so render-only
    // iterations under split pacing reuse the last step boundary; in the
    // default pacing each iteration overwrites it exactly where it used to be
    // declared. HUD, streaming demand and collects always read this boundary.
    rt::native::gameplay::FrameSnapshot snapshot;
    SDL_Gamepad* gamepad = nullptr;
    rt::native::gameplay::TriggerDriveState triggerState;
    const auto setPadFamily = [&](SDL_Gamepad* pad, SDL_JoystickID id) {
        g_appGlyphFamily = forcedGlyphFamily().value_or(
            rt::native::gameplay::mapSdlPadType(static_cast<int>(SDL_GetGamepadType(pad))));
        std::cout << "[input] pad " << id << " family " << glyphFamilyName(appGlyphFamily()) << "\n";
    };
    const auto openGamepad = [&](SDL_JoystickID id) {
        if (gamepad) return;
        gamepad = SDL_OpenGamepad(id);
        if (gamepad) setPadFamily(gamepad, id);
    };
    if (const auto forced = forcedGlyphFamily()) {
        g_appGlyphFamily = *forced;
        // Keep the forced-family matrix row deterministic on hosts with no pad.
        std::cout << "[input] pad 0 family " << glyphFamilyName(appGlyphFamily()) << " (forced)\n";
    }
    int connectedPadCount = 0;
    SDL_JoystickID* connectedPads = SDL_GetGamepads(&connectedPadCount);
    if (connectedPads && connectedPadCount > 0) openGamepad(connectedPads[0]);
    SDL_free(connectedPads);
    // --- Rumble glue (docs/native-reference/rumble-haptics.md §3) -----------
    // Opt-in: RT_APP_RUMBLE must be EXACTLY "1". Any other value (or unset)
    // leaves the channel force-disabled and this host makes no SDL rumble call
    // and no Channel::tick -- a byte-for-byte default-launch no-op.
    const bool rumbleEnabled = [] {
        const char* value = std::getenv("RT_APP_RUMBLE");
        return value && std::strcmp(value, "1") == 0;
    }();
    // One Channel per player pad; the host opens a single pad (P1), so one channel.
    rt::native::gameplay::rumble::Channel rumbleChannel;
    std::uint8_t rumbleMaster = rt::native::gameplay::rumble::kMasterOff;
    if (rumbleEnabled) {
        // Best-effort gate bytes from the settings store. The option encodings are
        // {0x00 off, 0x02 mask A, 0x04 mask B} (rumble-haptics.md §1, options
        // writer 0x00208E84..0x00208E94); the CONFIRMED gate is
        // (playerByte & masterByte) != 0 (0x0020ACC8). The save-image per-player
        // byte at 0x0177AC50+0xC/+0xD is still OPEN (rumble-haptics.md §5,
        // save-layout.md:74-80), so the native player byte mirrors the master
        // mask. controllers.rumble_strength (settings.cpp:484) is not consumed:
        // Channel exposes no global gain (rumble.h:161-198).
        rt::native::gameplay::SettingsStore rumbleStore(
            rt::native::gameplay::defaultSettingsPath(appDataDirectory()));
        const bool rumbleOn = rumbleStore.load()
            ? rumbleStore.getBool("controllers.rumble", true) : true;
        rumbleMaster = rumbleOn ? rt::native::gameplay::rumble::kMasterMaskA
                                : rt::native::gameplay::rumble::kMasterOff;
        std::cout << "rt-app rumble: enabled master=" << static_cast<int>(rumbleMaster) << "\n";
    }
    // RT_APP_INTERP=1 split pacing state (gameplay/interp; design and
    // determinism contract in docs/native-reference/frame-interpolation.md).
    const bool interpEnabled = envInterpEnabled();
    rt::native::gameplay::FixedStepClock interpClock(envInterpCap());
    const int interpRenderRate = envInterpRate();
    // Poses of the two most recent completed logic steps (the capture path
    // and the blend source for interpolated rendering).
    rt::native::gameplay::RenderPose interpPrevPose = renderPoseOfSnapshot(snapshot, navDrive);
    rt::native::gameplay::RenderPose interpCurPose = interpPrevPose;
    bool haveInterpPose = false;
    std::uint64_t interpRenders = 0;
    const auto monotonicSeconds = [] {
        return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    };
    rt::native::gameplay::PerfStats perf(monotonicSeconds, processCpuSeconds);
    // Kept here (rather than in the Android/bootstrap blocks) so concurrent
    // extraction work has no overlap: it owns only the simulation boundary.
    rt::native::runtime::GuestFacade guestFacade(session, snapshot, [&](int tile) {
        std::cout << "rt-app facade: tile warp " << tile << "\n";
        // Re-anchor the explore car on warp so the harness sees the town
        // square (43 = Peach Town, oracle-golden-frames.md:54 constants).
        // Other facade-valid tiles have no proven centre -> lattice origin
        // (0,0) at datum; GAP: per-town spawn data unmerged.
        // Thread note: an attached socket forces lockstep pacing, so warp
        // commands are serialised between simulation steps — this sim-thread
        // mutation cannot race the loop.
        if (tile == 43)
            session.setExplorePose(rt::native::gameplay::kTownSpawnX,
                                  rt::native::gameplay::kTownSpawnZ, 0.f);
        else
            session.setExplorePose(0.f, 0.f, 0.f, 0.f);
    });
    rt::native::runtime::TestSocket testSocket;
    testSocket.startIfRequested([&](const std::string& command, const std::string& line) {
        if (command == "stats") {
            const auto totals = perf.totals();
            return std::string("{\"ok\":true,\"vblank\":") + std::to_string(testSocket.vblank())
                + ",\"presents\":" + std::to_string(totals.presents) + "}\n";
        }
        return guestFacade.command(command, line);
    });
    const bool autopilotEnabled = headlessApp && [] {
        const char* value = std::getenv("RT_APP_AUTOPILOT");
        return value && std::strcmp(value, "1") == 0;
    }();
    const int autopilotHoldFrames = [] {
        const char* value = std::getenv("RT_APP_AUTOPILOT_HOLD");
        return value ? std::max(1, std::atoi(value)) : 1;
    }();
    rt::native::app::Autopilot autopilot({.enabled = autopilotEnabled,
                                          .resultsHoldFrames = autopilotHoldFrames});
    if (autopilotEnabled)
        std::cout << "rt-app autopilot: enabled results-hold=" << autopilotHoldFrames << "\n";
    rt::native::gameplay::OverlayModel perfOverlay(true);
    std::string perfOverlayText;
    double nextPerfWindow = monotonicSeconds() + perf.windowSeconds();
    auto lastPresent = std::chrono::steady_clock::now();
    int effectiveInterpRenderRate = interpRenderRate;
    if (interpEnabled) {
        // Effective render cadence: explicit RT_APP_INTERP_RATE wins,
        // otherwise the queried display refresh (swapchain cadence paces
        // rendering when the query fails).
        const SDL_DisplayMode* desktop = SDL_GetDesktopDisplayMode(SDL_GetDisplayForWindow(window));
        if (effectiveInterpRenderRate <= 0 && desktop && desktop->refresh_rate > 0.f)
            effectiveInterpRenderRate = static_cast<int>(desktop->refresh_rate + 0.5f);
        std::cout << "rt-app interp: fixed-step sim 60Hz render-rate="
                  << (effectiveInterpRenderRate > 0 ? std::to_string(effectiveInterpRenderRate)
                                                     : std::string("vsync"))
                  << " catch-up cap=" << interpClock.catchUpCap() << "\n";
    }
    auto previous = std::chrono::steady_clock::now();
    // FONTE is required for TITLE/MENU, not only the PLAYING HUD. Decode once
    // before the first frame so the title blink does not wait for the lazy
    // scene-asset path. A miss keeps menuQuads() byte-identical.
    (void)loadFonteAtlas(fontAtlas);
    // W3-05 event bridge (docs/native-reference/event-bridge.md): the seam
    // where authored native script events (radio, countdown, camera preset,
    // subtitle, flags) are pumped into the app once per presented frame.
    // No producer enqueues yet — ROM opcode mappings (0x85-0x8B are
    // rumble-hold bytes) stay unclaimed — so the default callbacks just log
    // any event that a future authoring pass emits before the session step.
    rt::native::gameplay::events::EventBridge eventBridge;
    const rt::native::gameplay::events::Callbacks eventCallbacks{
        [](const rt::native::gameplay::events::Event& ev) {
            std::cout << "[event] type=" << static_cast<int>(ev.type)
                      << " opcode=0x" << std::hex << static_cast<int>(ev.opcode)
                      << " addr=0x" << std::hex << ev.address << std::dec
                      << " valueKnown=" << (ev.valueKnown ? 1 : 0) << "\n";
        },
    };
    // W16.7: RT_APP_TEST_RESIZE=<W>x<H>@<frame> resizes the window once (windowed only).
    int testResizeW = 0, testResizeH = 0; long testResizeFrame = -1;
    if (const char* r = std::getenv("RT_APP_TEST_RESIZE"); r && std::sscanf(r, "%dx%d@%ld", &testResizeW, &testResizeH, &testResizeFrame) != 3) testResizeFrame = -1;
    while (running && (!maxFrames || frames < maxFrames)) {
        if (testSocket.lockstep() && !testSocket.takeStep()) {
            // A socket-attached run is a fixed 60-Hz logic clock.  Never let
            // render interpolation advance it while waiting for `run`/`step`.
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
        if (interpEnabled && !testSocket.lockstep() && effectiveInterpRenderRate > 0) {
            const auto minimumFrame = std::chrono::duration<double>(1.0 / effectiveInterpRenderRate);
            const auto elapsed = std::chrono::steady_clock::now() - lastPresent;
            if (elapsed < minimumFrame)
                std::this_thread::sleep_for(minimumFrame - elapsed);
            lastPresent = std::chrono::steady_clock::now();
        }
        if (testResizeFrame >= 0 && static_cast<long>(frames) == testResizeFrame && !headlessApp) {  // W16.7 test hook
            testResizeFrame = -1; SDL_SetWindowSize(window, testResizeW, testResizeH);
        }
        SDL_Event event; bool resize = false;
        rt::native::gameplay::MenuInput menuInput;
        bool titleAdvance = false, returnToMenu = false;
        bool startRace = false, openShop = false, openDialogue = false, advanceDialogue = false;
        int shopSelection = -1;
        while (SDL_PollEvent(&event)) { if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) running = false;
            if (event.type == SDL_EVENT_GAMEPAD_ADDED) openGamepad(event.gdevice.which);
            if (event.type == SDL_EVENT_GAMEPAD_REMOVED && gamepad &&
                SDL_GetGamepadID(gamepad) == event.gdevice.which) {
                SDL_CloseGamepad(gamepad);
                gamepad = nullptr;
                triggerState = {};
                g_appGlyphFamily = forcedGlyphFamily().value_or(rt::native::gameplay::GlyphFamily::Unknown);
            }
            if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
                if (state == AppState::TITLE) titleAdvance = true;
                if (state == AppState::MENU || state == AppState::LOAD) {
                    menuInput.up = menuInput.up || event.key.scancode == SDL_SCANCODE_UP;
                    menuInput.down = menuInput.down || event.key.scancode == SDL_SCANCODE_DOWN;
                    menuInput.confirm = menuInput.confirm || event.key.scancode == SDL_SCANCODE_RETURN;
                    menuInput.cancel = menuInput.cancel || event.key.scancode == SDL_SCANCODE_ESCAPE;
                }
                if (state == AppState::PLAYING) {
                    if (event.key.scancode == SDL_SCANCODE_ESCAPE) returnToMenu = true;
                    if (event.key.scancode == SDL_SCANCODE_TAB &&
                        session.state() == rt::native::gameplay::SessionState::RACE)
                        playerDriven = !playerDriven;
                    if (session.state() == rt::native::gameplay::SessionState::EXPLORE) {
                        openShop = openShop || event.key.scancode == SDL_SCANCODE_S;
                        openDialogue = openDialogue || event.key.scancode == SDL_SCANCODE_D;
                        if (shopOpen && event.key.scancode >= SDL_SCANCODE_1 && event.key.scancode <= SDL_SCANCODE_3)
                            shopSelection = static_cast<int>(event.key.scancode - SDL_SCANCODE_1);
                        if (dialogue.isActive() && event.key.scancode == SDL_SCANCODE_RETURN)
                            advanceDialogue = true;
                        else if (!dialogue.isActive() &&
                                 (event.key.scancode == SDL_SCANCODE_RETURN || event.key.scancode == SDL_SCANCODE_R))
                            startRace = true;
                    }
                    if (session.state() == rt::native::gameplay::SessionState::RESULTS &&
                        event.key.scancode == SDL_SCANCODE_RETURN)
                        session.continueFromResults();
                }
            }
            if (event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) resize = true; }
        if (!running) break;
        if (resize && !recreateSwapchain()) break;
        const auto now = std::chrono::steady_clock::now(); const float dt = std::min(std::chrono::duration<float>(now - previous).count(), 0.1f); previous = now;
        if (state == AppState::TITLE && titleAdvance) {
            menuFlow = {};
            (void)rt::native::gameplay::tickMenuFlow(menuFlow, {.confirm = true}, dt);
            state = AppState::MENU;
        } else if (state == AppState::MENU) {
            const auto menuEvent = rt::native::gameplay::tickMenuFlow(menuFlow, menuInput, dt);
            if (menuEvent.type == rt::native::gameplay::MenuEventType::MODE_SELECTED) {
                if (menuEvent.mode == rt::native::gameplay::MenuMode::ADVENTURE) {
                    state = AppState::LOAD;
                } else {
                    state = AppState::PLAYING;
                    // Preserve the old direct-to-race menu launch while leaving
                    // EXPLORE available after results (and via direct PLAYING).
                    session.startRace();
                }
            }
            if (menuEvent.type == rt::native::gameplay::MenuEventType::CANCEL && menuFlow.screen == rt::native::gameplay::MenuScreen::TITLE)
                state = AppState::TITLE;
        } else if (state == AppState::LOAD) {
            if (menuInput.up) titleUi.up();
            if (menuInput.down) titleUi.down();
            if (menuInput.cancel && titleUi.back() == rt::native::preview::TitleAction::Back) {
                state = AppState::MENU;
            } else if (menuInput.confirm) {
                titleAction = titleUi.confirm();
            }
            titleUi.tick();
            if (titleUi.transition() == rt::native::preview::TitleTransition::Complete) {
                if (titleAction == rt::native::preview::TitleAction::LoadContinue) {
                    rt::native::gameplay::SaveData saved{};
                    std::string saveError;
                    if (slotStore.load(0, saved, &saveError))
                        const_cast<rt::native::gameplay::SaveData&>(session.progress()) = std::move(saved);
                } else if (titleAction == rt::native::preview::TitleAction::StartNewGame) {
                    const_cast<rt::native::gameplay::SaveData&>(session.progress()) = slotStore.fresh();
                }
                state = AppState::PLAYING;
                session.startRace();
            }
        } else if (state == AppState::PLAYING && returnToMenu) {
            menuFlow.screen = rt::native::gameplay::MenuScreen::MODE_SELECT;
            state = AppState::MENU;
        }
        const bool* keys = SDL_GetKeyboardState(nullptr);
        const auto held = [keys](SDL_Scancode key) { return keys && keys[key]; };
        rt::native::gameplay::PadState pad;
        rt::native::gameplay::AnalogAxes axes;
        axes.stickX = (held(SDL_SCANCODE_RIGHT) || held(SDL_SCANCODE_D) ? 1.f : 0.f) -
                      (held(SDL_SCANCODE_LEFT) || held(SDL_SCANCODE_A) ? 1.f : 0.f);
        if (held(SDL_SCANCODE_UP) || held(SDL_SCANCODE_W))
            rt::native::gameplay::setPressed(pad, controllerConfig.getButtonConfig(rt::native::gameplay::ControllerAction::ACCELERATE));
        if (held(SDL_SCANCODE_DOWN) || held(SDL_SCANCODE_S))
            rt::native::gameplay::setPressed(pad, controllerConfig.getButtonConfig(rt::native::gameplay::ControllerAction::BRAKE));
        if (held(SDL_SCANCODE_SPACE))
            rt::native::gameplay::setPressed(pad, controllerConfig.getButtonConfig(rt::native::gameplay::ControllerAction::REVERSE));
        // One logic step of the fixed 60-Hz quantum: everything that must keep
        // the original VBlank-paced cadence (session tick, route follower,
        // shop/dialogue/collectibles, state/session logs, stream demand) per
        // docs/native-reference/frame-timing.md. Default pacing runs it exactly
        // once per iteration exactly here, so the default host loop stays
        // identical to the pre-interp one; split pacing (RT_APP_INTERP=1)
        // drains it from the accumulator below.
        const auto stepLogic = [&] {
            // W12 native-boot diagnostic: one virtual vblank (VBlankStart
            // semantics: alarms/vsync flag/intc fire) + green-thread pump per
            // 60 Hz logic step. Flag-gated: the default loop is untouched.
            if (nativeBoot.ok) {
                // The oracle runs guest code in EE float mode (RZ + flush,
                // ps2_ee_fpu_mode.h:36-39); our guest TUs run on this thread,
                // so adopt the same sticky mode (W12 177e/177f 1-ulp class).
                static const struct EEFloatMode {
                    EEFloatMode() {
#if defined(__aarch64__)
                        uint64_t fpcr;
                        __asm__ volatile("mrs %0, fpcr" : "=r"(fpcr));
                        fpcr = (fpcr & ~(3ull << 22)) | (3ull << 22) | (1ull << 24);  // RMode=RZ, FZ
                        __asm__ volatile("msr fpcr, %0" : : "r"(fpcr));
#elif defined(__x86_64__)
                        unsigned csr = _mm_getcsr();
                        csr = (csr & ~0x6000u) | 0x6000u | 0x8040u;  // RC + FTZ + DAZ
                        _mm_setcsr(csr);
#endif
                    }
                } g_eeFloatMode;
                static FILE* hashLog = [] {
                    const char* p = std::getenv("RT_STATE_HASH");
                    return p ? std::fopen(p, "w") : nullptr;
                }();
                static const uint64_t hashInterval = [] {
                    const char* e = std::getenv("RT_STATE_HASH_INTERVAL");
                    return e && *e ? (uint64_t)std::strtoull(e, nullptr, 10) : 60ull;
                }();
                // RT_RAM_DUMP="vb:path,vb:path": raw 32MB kRam slices at those
                // vblanks, for byte diffing against the oracle's EERM chunk.
                static const std::vector<std::pair<uint64_t, std::string>> ramDumps =
                    parseVbPaths(std::getenv("RT_RAM_DUMP"));
                // RT_PAD_SCRIPT="vb:hexmask[:hold],...": active-low button presses,
                // window vb..vb+hold inclusive (default hold 2 = 3 vblanks).
                // RT_BOOT_START=N keeps the legacy Up/Cross/Start ladder.
                struct Press { long at; uint16_t mask; int hold; };
                static const std::vector<Press> padScript = [] {
                    std::vector<Press> v;
                    if (const char* e = std::getenv("RT_PAD_SCRIPT"); e && *e) {
                        for (const char* p = e; p && *p;) {
                            const long vb = std::strtol(p, nullptr, 10);
                            const char* c1 = strchr(p, ':');
                            if (!c1) break;
                            const uint16_t mask = (uint16_t)std::strtoul(c1 + 1, nullptr, 16);
                            int hold = 2;
                            const char* c2 = strchr(c1 + 1, ':');
                            const char* comma = strchr(c1 + 1, ',');
                            if (c2 && (!comma || c2 < comma)) hold = std::atoi(c2 + 1);
                            v.push_back({vb, mask, hold});
                            p = comma ? comma + 1 : nullptr;
                        }
                    } else if (const char* e = std::getenv("RT_BOOT_START"); e && *e) {
                        const long n = std::atol(e);
                        v = {{n, (uint16_t)~0x0010u, 2},
                             {n + 120, (uint16_t)~0x4000u, 2},
                             {n + 240, (uint16_t)~0x0008u, 2}};
                    }
                    return v;
                }();
                uint16_t btn = 0xFFFFu;
                for (const Press& s : padScript)
                    if (nativeBoot.vblanks >= s.at && nativeBoot.vblanks <= s.at + s.hold) {
                        btn = s.mask;
                        break;
                    }
                // An attached lockstep socket drives the guest pad directly (rtharness),
                // overriding the script; it idles at 0xffff so an idle socket changes nothing.
                if (testSocket.lockstep()) btn = testSocket.pad().buttons;
                guest::g_bootButtons = btn;
                rtguest::kVblank();
                rtguest::kPumpToIdle();
                rtguest::gsRenderBatches();  // W3 3c-2: draws land in GS VRAM
                if (hashLog && nativeBoot.vblanks % hashInterval == 0) {
                    // RT_STATE_HASH: mirror of the oracle's ps2_test_harness
                    // hash (FNV-1a 64 over 8-byte words of EE RAM), emitted
                    // at the same vblank%interval point. Only `ee=` is real:
                    // the native port has no scratchpad/VU1/IOP RAM model.
                    uint64_t h = 0xcbf29ce484222325ull;
                    for (size_t i = 0; i + 8 <= RT_GUEST_RAM_SIZE; i += 8) {
                        uint64_t w;
                        std::memcpy(&w, rtguest::kRam + i, 8);
                        h = (h ^ w) * 0x100000001b3ull;
                    }
                    std::fprintf(hashLog, "%llu ee=%016llx\n",
                                 (unsigned long long)nativeBoot.vblanks,
                                 (unsigned long long)h);
                    std::fflush(hashLog);
                }
                for (const auto& [vb, path] : ramDumps) {
                    if (uint64_t(nativeBoot.vblanks) != vb) continue;
                    if (FILE* f = std::fopen(path.c_str(), "wb")) {
                        std::fwrite(rtguest::kRam, 1, RT_GUEST_RAM_SIZE, f);
                        std::fclose(f);
                    }
                }
                // RT_NATIVE_GS_DUMP="vb:path,...": W3 3c scanout readback —
                // 'RGBS' + u32 w,h + RGBA8 rows of the applied display env,
                // graded against oracle test-socket frame PNGs.
                static const std::vector<std::pair<uint64_t, std::string>> gsDumps =
                    parseVbPaths(std::getenv("RT_NATIVE_GS_DUMP"));
                for (const auto& [vb, path] : gsDumps) {
                    if (uint64_t(nativeBoot.vblanks) != vb) continue;
                    if (path.rfind("VRAM:", 0) == 0) {  // raw 4 MiB GS local memory
                        if (FILE* f = std::fopen(path.substr(5).c_str(), "wb")) {
                            std::fwrite(rtguest::gsVram(), 1, rtguest::gsVramSize(), f);
                            std::fclose(f);
                        }
                        std::fprintf(stderr, "[scanout] vb=%llu vram=%s\n",
                                     (unsigned long long)vb, path.c_str());
                        continue;
                    }
                    if (path.rfind("STATE:", 0) == 0) {  // GS text dump seam
                        rtguest::gsDebugDumpState(path.substr(6).c_str());
                        std::fprintf(stderr, "[scanout] vb=%llu state=%s\n",
                                     (unsigned long long)vb, path.c_str());
                        continue;
                    }
                    std::vector<uint8_t> img;
                    uint32_t w = 0, h = 0;
                    if (!rtguest::gsScanoutRgba(img, &w, &h, -1)) continue;
                    if (FILE* f = std::fopen(path.c_str(), "wb")) {
                        std::fwrite("RGBS", 1, 4, f);
                        std::fwrite(&w, 4, 1, f);
                        std::fwrite(&h, 4, 1, f);
                        std::fwrite(img.data(), 1, img.size(), f);
                        std::fclose(f);
                    }
                    std::fprintf(stderr, "[scanout] vb=%llu w=%u h=%u fbp=0x%x psm=0x%x "
                                 "dispfb=%llx display=%llx pmode=%llx smode2=%llx\n",
                                 (unsigned long long)vb, w, h,
                                 unsigned(rtguest::gsDisplayEnv()[2] & 0x1FFull),
                                 unsigned((rtguest::gsDisplayEnv()[2] >> 15) & 0x1F),
                                 (unsigned long long)rtguest::gsDisplayEnv()[2],
                                 (unsigned long long)rtguest::gsDisplayEnv()[3],
                                 (unsigned long long)rtguest::gsDisplayEnv()[0],
                                 (unsigned long long)rtguest::gsDisplayEnv()[1]);
                }
                if (std::getenv("RT_BOOT_TRACE_WIN")) {
                    // Per-vblank load-machine trace in [lo,hi] (comma pair).
                    static const long lo = [] {
                        const char* e = std::getenv("RT_BOOT_TRACE_WIN");
                        return std::atol(e);
                    }();
                    static const long hi = [] {
                        const char* e = std::getenv("RT_BOOT_TRACE_WIN");
                        const char* c = strchr(e, ',');
                        return c ? std::atol(c + 1) : 0L;
                    }();
                    const long v = nativeBoot.vblanks;
                    if (v >= lo && v <= hi) {
                        auto rd = [](uint32_t a) { uint32_t x; std::memcpy(&x, rtguest::kRam + a, 4); return x; };
                        if (v == hi) {
                            // Window end: one-shot table of live task nodes.
                            for (int n = 0; n < 48; ++n) {
                                const uint32_t node = 0x177a0d0u + n * 0x30u;
                                if ((rd(node) & 0xFFu) == 0) continue;
                                std::cout << std::hex << "[tasks] n=" << n << " act=" << rd(node)
                                          << " hnd=" << rd(node + 0xc) << " req=" << rd(node + 0x1c)
                                          << " f24=" << rd(node + 0x24) << " w28=" << rd(node + 0x28)
                                          << " hook=" << rd(node + 0x2c) << "\n";
                            }
                        }
                        std::cout << std::hex << "[tw] vb=" << std::dec << v
                                  << " step=" << rd(0x3356c0) << " loading=" << rd(0x333370)
                                  << " req=" << rd(0x333374) << " ldr1c=" << rd(0x177a11c)
                                  << " hnd=" << rd(0x177a10c)
                                  << " b26=" << (rd(0x177a124) >> 16) % 256
                                  << " b27=" << (rd(0x177a124) >> 24) % 256 << "\n";
                    }
                }
                if (std::getenv("RT_RAM_CANARY")) {
                    // .rodata must never be written after staging. First
                    // mismatch pins the vblank; active task handlers name the
                    // suspect task.
                    static const std::vector<uint8_t> golden(
                        rtguest::kRam + 0x2e9a00, rtguest::kRam + 0x332b00);
                    size_t off = 0;
                    while (off < golden.size() && rtguest::kRam[0x2e9a00 + off] == golden[off]) ++off;
                    if (off != golden.size()) {
                        std::cout << "rt-app RAM-CANARY vblank=" << nativeBoot.vblanks
                                  << " first-off=0x" << std::hex << (0x2e9a00 + off)
                                  << " want=" << int(golden[off])
                                  << " got=" << int(rtguest::kRam[0x2e9a00 + off]) << "\n";
                        for (int n = 0; n < 48; ++n) {
                            const uint8_t* node = rtguest::kRam + 0x177a0d0 + n * 0x30;
                            uint32_t hnd;
                            std::memcpy(&hnd, node + 0xc, 4);
                            if (node[0]) std::cout << "  node" << n << " act=" << int(node[0])
                                                   << " hnd=0x" << hnd << "\n";
                        }
                        std::cout << std::dec;
                        std::exit(3);
                    }
                }
                if (++nativeBoot.vblanks % 30 == 0) {
                    // Scene words mirror config/game_state.toml [game]/boot_diag:
                    // is the load machine busy, and what task handler is installed.
                    auto rd = [](uint32_t a) { uint32_t v; std::memcpy(&v, rtguest::kRam + a, 4); return v; };
                    std::cout << std::hex << "rt-app native-boot vblank=" << std::dec
                              << nativeBoot.vblanks
                              << " threads=" << rtguest::kThreadCount()
                              << " semas=" << rtguest::kSemaCount()
                              << " cdreads=" << guest::g_diagCdReads
                              << " sectors=" << guest::g_diagCdSectors
                              << " scene3d=" << rd(0x3356c4)
                              << " uiCtx=" << rd(0x3356cc)
                              << " mode=" << (rd(0x33590a) & 0xFF)
                              << " step=" << rd(0x3356c0)
                              << " loading=" << rd(0x333370)
                              << " task0=" << rd(0x177a0d0)
                              << " flow=" << (rd(0x177a0d4) & 0xFF) << ":"
                              << ((rd(0x177a0f4) >> ((0x24 % 4) * 8)) & 0xFF) << ":"
                              << ((rd(0x177a0f4) >> ((0x25 % 4) * 8)) & 0xFF)
                              << " taskHandler=" << rd(0x177a0dc)
                              // Loader node (0x177a100): adopted request + cycle
                              // bytes; car-pointer table; attract car block model id.
                              << " ldr1c=" << rd(0x177a11c)
                              << " b26=" << (rd(0x177a124) >> 16) % 256
                              << " b27=" << (rd(0x177a124) >> 24) % 256
                              << " cars=" << rd(0x177ea50) << ":" << rd(0x177ea54)
                              << ":" << rd(0x177ea58) << ":" << rd(0x177ea5c) << ":" << rd(0x177ea60)
                              << " car0id=" << (rd(0x177f764) & 0xFF)
                              // .rodata integrity + game heap header (arena at
                              // 0x2dce74; string at 0x2ea638 = fatal fmt).
                              << " ro638=" << rd(0x2ea638) << " heap0=" << rd(0x2dce70)
                              << "\n";
                }
            }
            // Trigger polling intentionally lives in this fixed-step lambda:
            // split-pacing render iterations must not advance its pulse/reverse
            // state or make controller drive depend on presentation cadence.
            if (testSocket.lockstep()) {
                const auto injected = testSocket.pad();
                pad.pressedButtons = static_cast<std::uint16_t>(~injected.buttons);
                axes.stickX = std::clamp((injected.lx - 128) / 127.f, -1.f, 1.f);
            }
            auto input = rt::native::gameplay::playerCarInput(pad, axes, controllerConfig);
            if (triggersEnabled && gamepad) {
                const auto trigger = [gamepad](SDL_GamepadAxis axis) {
                    return std::clamp(static_cast<float>(SDL_GetGamepadAxis(gamepad, axis)) / 32767.f, 0.f, 1.f);
                };
                const auto drive = rt::native::gameplay::mapTriggersToDrive(
                    {.leftTrigger = trigger(SDL_GAMEPAD_AXIS_LEFT_TRIGGER),
                     .rightTrigger = trigger(SDL_GAMEPAD_AXIS_RIGHT_TRIGGER),
                     .speed = static_cast<int>(snapshot.speed), .standstill = snapshot.speed == 0.f}, triggerState);
                // Analog drive augments, never replaces, keyboard controller bits.
                input.gas = input.gas || drive.input.gas;
                input.brake = input.brake || drive.input.brake;
                input.reverse = input.reverse || drive.input.reverse;
            }
            (void)input;
            if (state == AppState::PLAYING) {
                // W3-05: drain queued script events immediately before the
                // session consumes input so a trigger lands in the same logic
                // step it was emitted in (no cross-step latency).
                eventBridge.drain(eventCallbacks);
                // GameSession owns the race loop.  Its current public update path
                // is AI-driven; Tab remains the race-mode selection control for
                // the host UI and is logged below.
                const auto autopilotStep = testSocket.lockstep()
                    ? rt::native::app::AutopilotStep{}
                    : autopilot.step(session.state());
                if (autopilotStep.continueResults) {
                    session.continueFromResults();
                    std::cout << "rt-app autopilot: continued results\n";
                }
                session.update(menuInput, testSocket.lockstep() ? &input : autopilotStep.driveInput);
                // Widescreen policy (output-modes.md): 3D fills the window; fovY
                // pinned, aspect follows the presented extent.  Applied every
                // frame because startRace() rebuilds loop params.
                session.setProjectionAspect(
                    static_cast<float>(presenter.extent.width) /
                    static_cast<float>(presenter.extent.height));
                snapshot = session.lastSnapshot();
                simulationFrame = snapshot.frame;
                if (navDrive) {
                    // Advance the render-only follower once per authored frame and
                    // announce each completed closed lap.
                    const rt::native::gameplay::NavPose pose = navDrive->step(kNavRouteSpeedUnitsPerFrame);
                    if (pose.lap > navRouteLap) {
                        navRouteLap = pose.lap;
                        std::cout << "rt-app route: lap " << pose.lap << " complete\n";
                    }
                }

                if (session.state() == rt::native::gameplay::SessionState::EXPLORE) {
                    // Radio town-frame apply (radio-stations.md §4/§5): ONE
                    // onTownFrame() per town frame, mirroring
                    // UpdateTownExplorationTask -> ApplySoundVolumes
                    // (0x2106C0/0x2107C8). Deferred: a station request becomes
                    // audible only at this boundary; Off stops the stream. Gated
                    // on an open audio device so unset/OFF stays silent.
                    if (radioFollow && audio.stream) {
                        const rt::native::gameplay::RadioApply radioApply = radio.onTownFrame();
                        if (radioApply.action == rt::native::gameplay::RadioStreamAction::Start && radioApply.pair)
                            startRadio(*radioApply.pair);
                        else if (radioApply.action == rt::native::gameplay::RadioStreamAction::Stop)
                            stopRadio();
                    }
                    if (openShop && !shopOpen) {
                        shopOpen = true;
                        std::cout << "rt-app shop opened: 1=road map, 2=fuel can, 3=lucky charm\n";
                    }
                    if (openDialogue && !dialogue.isActive()) {
                        dialogue.open(dialogueLines);
                        const auto* line = dialogue.currentLine();
                        std::cout << "rt-app dialogue " << line->speaker << ": " << line->text << "\n";
                    }
                    if (shopOpen && shopSelection >= 0) {
                        // GameSession owns a mutable SaveData; its public accessor is
                        // read-only for callers that only display progress.
                        auto& progress = const_cast<rt::native::gameplay::SaveData&>(session.progress());
                        const std::uint32_t moneyBefore = progress.money;
                        const bool bought = shop.buy(progress, static_cast<std::size_t>(shopSelection));
                        const char* itemName = shopSelection < static_cast<int>(shop.items.size())
                                ? shop.items[static_cast<std::size_t>(shopSelection)].name.c_str() : "invalid item";
                        std::cout << "rt-app shop buy " << itemName << " result=" << (bought ? "ok" : "failed")
                                  << " money=" << moneyBefore << "->" << progress.money << "\n";
                    }
                    if (advanceDialogue && dialogue.isActive()) {
                        const bool finished = dialogue.advance({.confirm = true});
                        if (finished)
                            std::cout << "rt-app dialogue closed\n";
                        else {
                            const auto* line = dialogue.currentLine();
                            std::cout << "rt-app dialogue " << line->speaker << ": " << line->text << "\n";
                        }
                    }
                    if (startRace) session.startRace();
                }

                // In EXPLORE, this uses the retained session snapshot.  The AI
                // drives during RACE, too, so checking that snapshot there makes
                // the synthetic pickup path observable in a headless launch.
                if (session.state() == rt::native::gameplay::SessionState::EXPLORE ||
                    (session.state() == rt::native::gameplay::SessionState::RACE && !playerDriven)) {
                    auto& progress = const_cast<rt::native::gameplay::SaveData&>(session.progress());
                    for (auto& collectible : collectibles) {
                        if (!rt::native::gameplay::tryCollect(collectible, snapshot.carPosition, progress)) continue;
                        std::cout << "rt-app collected "
                                  << (collectible.kind == rt::native::gameplay::CollectibleKind::STAMP ? "stamp " : "coin ")
                                  << collectible.index << " total="
                                  << rt::native::gameplay::collectedCount(collectibles) << "\n";
                    }
                }

                // --- Rumble (rumble-haptics.md §3 step 3) -------------------
                // One Channel tick per 60Hz logic step, after the session has
                // advanced the car. Output is emitted only on the frame a burst
                // starts; SDL_RumbleGamepad is the two-motor level+duration command
                // the original primitive emits. SDL_RumbleGamepad on a null pad is
                // guarded: an absent gamepad is legal (headless).
                //
                // Event-wiring GAPS: the authored loop exposes NO collision /
                // landing / scrape / crash resolution to the host yet --
                //   * landing: CarState::fallSpeed[3] (car_state.h:39) is private to
                //     GameLoop; integrateCar (game_loop.cpp:52-59) runs tickCar but
                //     surfaces no landing event through FrameSnapshot.
                //   * collision / scrape: the loop has no collision resolution
                //     (game_loop.cpp tick() only runs tickAi + integrateCar).
                //   * crash: no ApplyCollisionImpulse analogue; impulse codes 0..10
                //     (0x00219C50/60/80) are not produced natively.
                // TODO(rumble events): hook submitLanding (0x0021B360),
                //   submitCollision (0x0021793C/A0), submitCrash (codes 0..10) and
                //   submitScrape (AUTHORED, rumble-haptics.md §2) here once those
                //   sources exist; no fake events are synthesized meanwhile.
                // TODO(rumble test buzz): submitTestBuzz() belongs on the options
                //   rumble toggle flipping ON (0x00208EA0/B4) -- GAP: the native
                //   loop has no settings-change hook (main.cpp only reads the store
                //   at boot).
                if (rumbleEnabled) {
                    rumbleChannel.setSuppressed(false);
                    // Gate bytes re-applied each 60Hz logic step (rumble-haptics.md
                    // §3 step 3): no runtime settings-change hook exists to push a
                    // new option byte, so the boot value is re-asserted here.
                    rumbleChannel.setOptions(rumbleMaster, rumbleMaster);
                    const rt::native::gameplay::rumble::Output rumbleOut = rumbleChannel.tick();
                    if (rumbleOut.active() && gamepad) {
                        SDL_RumbleGamepad(gamepad,
                                          static_cast<Uint16>(rumbleOut.leftIntensity * 65535.0f),
                                          static_cast<Uint16>(rumbleOut.rightIntensity * 65535.0f),
                                          static_cast<Uint32>(rumbleOut.durationSeconds() * 1000.0f));
                    }
                }
            }
            // Leaving gameplay cancels any active burst and clears the device
            // (rumble-haptics.md §3 step 4). One-shot: isActive() is false after
            // stop(), so later non-playing frames are no-ops.
            if (state != AppState::PLAYING && rumbleEnabled && rumbleChannel.isActive()) {
                rumbleChannel.stop();
                // SDL_RumbleGamepad with 0 intensity stops any rumble
                // (SDL_gamepad.h:1517-1518); this SDL 3.2.0 has no
                // SDL_StopGamepadRumble, so zero-intensity is the documented stop.
                if (gamepad) (void)SDL_RumbleGamepad(gamepad, 0, 0, 0);
            }
            if (!loggedAppState || state != lastLoggedAppState) {
                std::cout << "rt-app state=" << appStateName(state) << "\n";
                lastLoggedAppState = state;
                loggedAppState = true;
            }
            if (state == AppState::PLAYING && (!loggedSessionState || session.state() != lastLoggedSessionState)) {
                std::cout << "rt-app session state=" << sessionStateName(session.state())
                          << " lap=" << snapshot.lap << " finished=" << snapshot.finished
                          << " money=" << session.progress().money << "\n";
                lastLoggedSessionState = session.state();
                loggedSessionState = true;
            }
            if (state == AppState::PLAYING && directStartRace) {
                session.startRace();
                directStartRace = false;
            }
            if (streamed) {
                // RT_APP_STREAM_POS=1: authentic position-driven demand (the car's
                // fixed-unit world position drives the 4-slot window); default
                // keeps the scripted serpentine sweep for the deterministic matrix.
                const bool changed = std::getenv("RT_APP_STREAM_POS") != nullptr
                    ? streamed->refreshAt(carFixedUnits(snapshot.carPosition).x,
                                         carFixedUnits(snapshot.carPosition).z)
                    : streamed->refresh();
                if (changed) streamed->buildMesh(terrainMesh);
            }
            // Event-derived one-shot commands belong to a single logic step;
            // clearing them keeps a multi-step catch-up drain from applying
            // one key event to every drained step (default pacing is
            // unaffected: the poll remakes the flags each iteration).
            openShop = openDialogue = advanceDialogue = startRace = false;
            shopSelection = -1;
            perf.noteLogicStep();
            if (testSocket.lockstep()) testSocket.completeStep();
        };
        if (interpEnabled && !testSocket.lockstep()) {
            // Split pacing: bank real time, then drain at most catchUpCap
            // logic steps per host frame. RT_APP_FRAMES still counts logic
            // steps (`frames` increments only inside the drain), so the exit
            // log's steps=N equals RT_APP_FRAMES whatever the render rate —
            // the proof that the simulation rate is unchanged.
            interpClock.addElapsed(dt);
            const int owed = interpClock.stepsReady();
            for (int i = 0; i < owed; ++i) {
                if (maxFrames && frames >= maxFrames) break;
                interpPrevPose = interpCurPose;
                stepLogic();
                interpCurPose = renderPoseOfSnapshot(snapshot, navDrive);
                if (!haveInterpPose) {
                    // There is no older completed step at startup; rendering
                    // the first completed pose avoids blending from zeros.
                    interpPrevPose = interpCurPose;
                    haveInterpPose = true;
                }
                interpClock.takeStep();
                ++frames;
            }
        } else {
            stepLogic();
        }
        if (!noRender && presenter.extent.width && presenter.extent.height) {
            bool retry = false;
            // Render-local copy so the route follower can move the visible car
            // without disturbing session authoring state (lap/collectible logic
            // above still reads the authored snapshot).
            rt::native::gameplay::FrameSnapshot renderSnapshot = snapshot;
            if (interpEnabled && !testSocket.lockstep() && haveInterpPose) {
                // RT_APP_SNAPSHOT is a logic-step-boundary (t=0) capture
                // contract. It deliberately uses the completed snapshot,
                // rather than a host-time blend, preserving P6 determinism.
                const auto pose = std::getenv("RT_APP_SNAPSHOT")
                                      ? interpCurPose
                                      : rt::native::gameplay::lerpRenderPose(
                                            interpPrevPose, interpCurPose, interpClock.alpha());
                renderSnapshot = rt::native::gameplay::snapshotWithRenderPose(snapshot, pose);
            } else if (navDrive) {
                const rt::native::gameplay::NavPose pose = navDrive->pose();
                // carPosition is consumed in game units (preview scales .1).
                renderSnapshot.carPosition = {pose.position.x, pose.position.z};
                renderSnapshot.heading = pose.heading;
            }
            // ---- Terrain-following view lift (race datum fix) -----------
            // Physics rides the course datum y=0 (GameLoop::groundY_) while
            // the FLDT tiles carry absolute elevation (Peach Town plaza sits at
            // ~+3.0 RU, oracle y=30 GU). Un-lifted, the chase camera buries
            // under the town slab once a race starts (ident-underlay
            // experiment 2026-10-17: the ground-fill sheet wrapped over the
            // sky). Band-average the terrain under the car (same base band
            // [−2,4.5] RU, radius 24, that preview's carLift uses) and rigidly
            // translate the view so the eye keeps its design clearance
            // kChaseBack*sin(kChasePitch)+chaseBaseHeight = 7*0.0859+2.0 ~= 2.6
            // RU above the actual ground. s.view stores lookAt transposed
            // column-major (view[c*4+r]=m[r*4+c]): eye.y =
            // -(v[4]*v[12]+v[5]*v[13]+v[6]*v[14]) and a world lift dy is
            // applied via v[12+r] -= dy*v[4+r]. Lift only upward (dy>0) so
            // valleys never drag the camera down.
            if (!terrainMesh.positions.empty()) {
                const float cx = renderSnapshot.carPosition.x * .1f;
                const float cz = renderSnapshot.carPosition.z * .1f;
                float sumY = 0.f;
                int cnt = 0;
                for (const auto& p : terrainMesh.positions) {
                    const float dx = p[0] - cx, dz = p[2] - cz;
                    if (dx * dx + dz * dz > 576.f) continue;   // 24 RU
                    if (p[1] < -2.f || p[1] > 4.5f) continue;  // base band
                    sumY += p[1];
                    ++cnt;
                }
                if (cnt > 0) {
                    const float ground = sumY / static_cast<float>(cnt);
                    float* v = renderSnapshot.view;
                    const float eyeY = -(v[4] * v[12] + v[5] * v[13] + v[6] * v[14]);
                    const float dy = ground + 2.6f - eyeY;
                    if (dy > 0.f)
                        for (int r = 0; r < 3; ++r)
                            renderSnapshot.view[12 + r] -= dy * renderSnapshot.view[4 + r];
                }
            }
            if (state == AppState::PLAYING && !sceneAssetsReady) {
                initSceneAssets();
                sceneAssetsReady = true;
            }
            do {
                std::vector<std::uint8_t> pixels;
                bool rendered = false;
                if (state == AppState::PLAYING) {
                    rendered = rt::native::preview::previewRenderScene(
                        renderer, car, renderSnapshot, session.world(), pixels, error,
                        terrainMesh.indices.empty() ? nullptr : &terrainMesh,
                        navLines.empty() ? nullptr : &navLines,
                        carLivery ? &*carLivery : nullptr);
                    // The bar/lap-square placeholders are a RACE-mode stand-in
                    // only; the oracle town view has nothing top-left (its HUD
                    // is gauge + minimap pills along the bottom) —
                    // town_driving.png check 2026-10-17.  With the shipped
                    // SCORE.GSL banner art available (raceBanner) the authentic
                    // TOTAL TIME block below replaces the placeholder bars even
                    // in RACE.
                    if (rendered && session.state() == rt::native::gameplay::SessionState::RACE &&
                        !hudArt.raceBanner) {
                        std::vector<std::uint8_t> hudPixels;
                        rendered = rt::native::preview::previewRender2D(
                            renderer, rt::native::preview::raceHudQuads(snapshot, session.totalLaps(),
                                                                         static_cast<int>(presenter.extent.width), static_cast<int>(presenter.extent.height)),
                            {0.f, 0.f, 0.f, 0.f}, hudPixels, error);
                        if (rendered) compositeOver(pixels, hudPixels);
                    }
                    // Authentic race top-left block through the W4-06 verified
                    // layout builders (race_hud_layout.h; 463 ROM+golden
                    // checks).  Sprites group by ROM texture page/CLUT bank:
                    // SCORE CSA-0 (raceAtlas; rank digits, hidden today),
                    // SCORE CSA-1 (raceBanner: TOTALTIME banner at (20,20)
                    // uncentred per ROM group 0x2EC1A8, ElementC capsule at
                    // alpha 0.509, timer digits on page v64 row pitch 14),
                    // SCORE page-1 CSA-7 lap digits and CSA-8 lap disc
                    // (W4-12 HudArt pages when decoded, authored fallbacks
                    // otherwise).  The gauge page (Meter0) rides the shared
                    // dial block below.  GAP: rank/ElementD hidden — the
                    // native session has no rival cars (place lives at car
                    // +0x247 per race-state-inputs.md for a future grid).
                    if (rendered && session.state() == rt::native::gameplay::SessionState::RACE &&
                        hudArt.raceBanner) {
                        namespace pv = rt::native::preview;
                        namespace rg = roadtrip::gameplay;
                        const pv::RaceHudViewport vp = pv::raceHudStretchViewport(
                            static_cast<int>(presenter.extent.width),
                            static_cast<int>(presenter.extent.height));
                        // Current-lap frame counter resets when a lap completes
                        // (ROM clears car+0x260 at the boundary 0x0022D4C0);
                        // native completions arrive through snapshot.lap.
                        static int raceLapSeen = -1;
                        static int raceLapStartFrame = 0;
                        if (static_cast<int>(snapshot.lap) != raceLapSeen) {
                            raceLapSeen = static_cast<int>(snapshot.lap);
                            raceLapStartFrame = static_cast<int>(snapshot.elapsedFrames);
                        }
                        pv::RaceHudLayoutState hs = pv::raceHudLayoutStateFor(
                            snapshot,
                            static_cast<std::int32_t>(snapshot.elapsedFrames) - raceLapStartFrame,
                            0, 24, snapshot.gear);  // place0/totalCars provisional (no rival grid GAP); gear = W6-04 driveline
                        hs.showRank = false;  // no rival cars natively (GAP)
                        auto drawTextured = [&](const std::vector<pv::HudQuad>& quads,
                                              const rt::native::preview::PreviewTexture& tex) {
                            if (quads.empty()) return;
                            std::vector<std::uint8_t> px;
                            std::string err;
                            if (pv::previewUploadFontAtlas(renderer, tex, err) &&
                                pv::previewRender2DTextured(
                                    renderer, quads, {0.f, 0.f, 0.f, 0.f}, px, error))
                                compositeOver(pixels, px);
                            else if (const char* dbg = std::getenv("RT_APP_CARDIAG"); dbg && *dbg == '1') {
                                static bool once = false;
                                if (!once) { once = true; std::cout << "rt-app race HUD pass FAIL: " << err << " / " << error << "\n"; }
                            }
                        };
                        std::vector<pv::HudQuad> csa0, csa1, lapDigitQ, lapDiscQ;
                        for (const auto& s : pv::raceHudLayoutSprites(hs)) {
                            if (s.page == rg::RaceHudPage::Meter0 ||
                                s.page == rg::RaceHudPage::Meter0Sub)
                                continue;  // the shared dial block below owns the gauge
                            const pv::HudQuad q = pv::raceHudSpriteQuad(s, vp);
                            if (s.page == rg::RaceHudPage::Score0 && s.csa == 0) csa0.push_back(q);
                            else if (s.page == rg::RaceHudPage::Score0) csa1.push_back(q);
                            else if (s.csa == 8) lapDiscQ.push_back(q);
                            else lapDigitQ.push_back(q);
                        }
                        if (hudArt.raceAtlas) drawTextured(csa0, *hudArt.raceAtlas);
                        drawTextured(csa1, *hudArt.raceBanner);
                        if (hudArt.lapDigits) drawTextured(lapDigitQ, *hudArt.lapDigits);
                        else drawTextured(pv::raceHudLapDigitFallbackQuads(hs, vp), *hudArt.raceBanner);
                        if (hudArt.lapDisc) drawTextured(lapDiscQ, *hudArt.lapDisc);
                        else {
                            std::vector<std::uint8_t> discPixels;
                            if (pv::previewRender2D(renderer, pv::raceHudLapDiscFallback(hs, vp),
                                                   {0.f, 0.f, 0.f, 0.f}, discPixels, error))
                                compositeOver(pixels, discPixels);
                        }
                    }
                        // Shipped speedometer dial through the W4-06 verified
                        // Meter0 pass (ROM quad 0x2A8FF0: dial face 140x128 at
                        // design (484,294); the earlier (492,336) w132 blit sat
                        // on the 2P anchor 0x2A9AF0's region).  Rides the
                        // shared HUD slot; the font upload below now detects
                        // the buffer switch and rebinds FONTE (preview.cpp
                        // hudAtlasPtr), so digits stay crisp over the dial.
                        if (rendered &&
                            (session.state() == rt::native::gameplay::SessionState::RACE ||
                             session.state() == rt::native::gameplay::SessionState::EXPLORE) &&
                            hudArt.speedDial) {
                            const float sx = static_cast<float>(presenter.extent.width) / 640.f;
                            const float sy = static_cast<float>(presenter.extent.height) / 448.f;
                            std::vector<std::uint8_t> dialPixels;
                            std::string dialError;
                            // Pass 1: baked dial ring + gear glyph through the
                            // W4-06 Meter0 pass (face sprite u(0,0)+140x128 at
                            // dest (484,294) per ROM quad 0x2A8FF0; gear digit
                            // 20x14 at dial offset (97,60) = (581,354),
                            // u=(g%5)*20+140 v=(g/5)*16+80).  The prior
                            // full-column blit at (492,336) w132 sat on the
                            // 2P anchor region (0x2A9AF0).
                            rt::native::preview::RaceHudLayoutState dialState;
                            dialState.gear = renderSnapshot.gear;  // W6-04 driveline: 0 = R, 1..5 forward
                            if (rt::native::preview::previewUploadFontAtlas(
                                    renderer, *hudArt.speedDial, dialError) &&
                                rt::native::preview::previewRender2DTextured(
                                    renderer,
                                    rt::native::preview::raceHudLayoutQuads(
                                        dialState, rt::native::preview::RaceHudPass::Meter0,
                                        rt::native::preview::raceHudStretchViewport(
                                            static_cast<int>(presenter.extent.width),
                                            static_cast<int>(presenter.extent.height))),
                                    {0.f, 0.f, 0.f, 0.f}, dialPixels, error))
                                compositeOver(pixels, dialPixels);
                            // Pass 2: SPEED DISPLAY LAW (verified 2026-10-18
                            // against the oracle goldens build/scratch/oracle-
                            // golden/*.png + METER0 page atlas measurement;
                            // supersedes the provisional /100 kph guess and the
                            // 20x26 METER10 cells with placeX {40,59,78}).
                            // The numeral is MPH: V = floor(speed / 55.07)
                            // clamped 0..186, zero-suppressed num_digits(V)
                            // quads right-aligned with the ones digit at UI
                            // (573,388), step -17 px per higher place; each
                            // quad 16x23 sampling METER0 (hudArt.speedDial,
                            // 256x128) at u = 140+16*(d%7), v = 23*(d/7);
                            // uniform white, no tint.  "mph" is baked into the
                            // METER0 page and is never drawn separately here.
                            // mph badge box: x 556..604, y 389..416.
                            // GAUGE RULING (W4-11, ROM-verified): the needle
                            // law is theta = pi*(clamp(rpm,0,10000)/10000-0.7),
                            // 0 up, positive CW, input = engine RPM at car
                            // +0x1D0 — NOT mph (div.s 10000.0f @0x332F50,
                            // add.s -0.7f @0x332F54 via 0x249DC0->0x249520).
                            // Wired 2026-10-18: live rpm = renderSnapshot.rpm
                            // (W6-04 driveline, game_loop.cpp), page = METER0
                            // unit-2 CSA12 128x120 @0x8550 (hudArt.gaugePage).
                            if (hudArt.speedDial) {
                                int mph = static_cast<int>(std::floor(renderSnapshot.speed / 55.07));
                                mph = std::max(0, std::min(186, mph));
                                const int nd = (mph >= 100) ? 3 : (mph >= 10) ? 2 : 1;
                                const int placeDiv[3] = {100, 10, 1};
                                std::vector<rt::native::preview::HudQuad> digitQuads;
                                for (int p = 0; p < 3; ++p) {
                                    if (p < 3 - nd) continue;  // suppress leading zeros
                                    const int d = (mph / placeDiv[p]) % 10;
                                    const float u0 = static_cast<float>(140 + 16 * (d % 7)) / 256.f;
                                    const float v0 = static_cast<float>(23 * (d / 7)) / 128.f;
                                    digitQuads.push_back({(573 - 17 * (2 - p)) * sx, 388.f * sy,
                                                        16.f * sx, 23.f * sy,
                                                        u0, v0, u0 + 16.f / 256.f, v0 + 23.f / 128.f,
                                                        {1.f, 1.f, 1.f, 1.f}});
                                }
                                std::vector<std::uint8_t> digitPixels;
                                if (!digitQuads.empty() &&
                                    rt::native::preview::previewUploadFontAtlas(renderer, *hudArt.speedDial, dialError) &&
                                    rt::native::preview::previewRender2DTextured(
                                        renderer, digitQuads, {0.f, 0.f, 0.f, 0.f}, digitPixels, error))
                                    compositeOver(pixels, digitPixels);
                            }
                            // Tachometer needle (W7-01 law, W8-04 rot-quad
                            // path): cell (3,53,8,59) of METER0 unit-2 CSA12
                            // 128x120 @0x8550 (hudArt.gaugePage).  dialOrigin
                            // is the 1P gauge origin (484,294); needleQuad
                            // adds kPivot (70,68) itself — pass it once.
                            if (hudArt.gaugePage) {
                                if (std::getenv("RT_APP_HUDTEXT"))
                                    std::fprintf(stderr, "[hud] speed_word=%.0f mph=%d rpm=%d\n",
                                                 renderSnapshot.speed,
                                                 static_cast<int>(std::floor(renderSnapshot.speed / 55.07)),
                                                 renderSnapshot.rpm);
                                rt::native::preview::RgbaSurface dialSurface{
                                    static_cast<int>(presenter.extent.width),
                                    static_cast<int>(presenter.extent.height),
                                    std::span<std::uint8_t>(pixels)};
                                rt::native::preview::drawGaugeNeedle(
                                    dialSurface, static_cast<float>(renderSnapshot.rpm),
                                    *hudArt.gaugePage,
                                    {484.f * sx, 294.f * sy, sx, sy});
                            }
                            // Pass 3: time-of-day clock in the gauge's dark
                            // bottom band — oracle town_driving.png shows teal
                            // LCD "09:13" at UI x~545..595 y~442.  Digits are
                            // the green/teal LCD cells baked on the METER0 page
                            // (cell geometry PIL-measured 2026-10-17: 7 cells at
                            // v 0..20 + 3 at v 23..43, u = 140 + d*16, 14x21
                            // glyphs).  No colon cell exists on the page, so the
                            // separator is authored as two teal squares matching
                            // the digit colour (1,212,161)/255 in a flat pass.
                            {
                                const unsigned ticks = snapshot.clock;
                                const unsigned perH = static_cast<unsigned>(rt::native::gameplay::kClockTicksPerHour);
                                const unsigned perM = static_cast<unsigned>(rt::native::gameplay::kClockTicksPerMinute);
                                const int hd = static_cast<int>((ticks / perH) % 24u);
                                const int mn = static_cast<int>((ticks % perH) / perM);
                                const int dg[4] = {hd / 10, hd % 10, mn / 10, mn % 10};
                                std::vector<rt::native::preview::HudQuad> clockQuads;
                                const float kx[4] = {545.f, 557.f, 573.f, 585.f};
                                for (int i = 0; i < 4; ++i) {
                                    const int d = dg[i];
                                    const float u0 = static_cast<float>(140 + (d < 7 ? d * 16 : (d - 7) * 16)) / 256.f;
                                    const float v0 = d < 7 ? 0.f : 23.f / 128.f;
                                    clockQuads.push_back({kx[i] * sx, 442.f * sy, 10.f * sx, 14.f * sy,
                                                        u0, v0, u0 + 14.f / 256.f, v0 + 21.f / 128.f,
                                                        {1.f, 1.f, 1.f, 1.f}});
                                }
                                // Day/night badge left of the clock — oracle sun
                                // badge yellow bbox px (1028..1053, 413..424) ->
                                // UI (514,443.6,12.5,11.8), widened ~1 UI px per
                                // side for the blue diamond rim -> (512,441,17,16);
                                // atlas cell v 48..73 u 164..188 (METER0 unit 0,
                                // sun/moon cluster; key-yellow excluded by the
                                // decoder's alpha, diamond blue (64,186,239) +
                                // sun orange (237,155,2) measured in-page).
                                clockQuads.push_back({512.f * sx, 441.f * sy, 17.f * sx, 16.f * sy,
                                                    164.f / 256.f, 48.f / 128.f, 188.f / 256.f, 73.f / 128.f,
                                                    {1.f, 1.f, 1.f, 1.f}});
                                if (rt::native::preview::previewUploadFontAtlas(renderer, *hudArt.speedDial, dialError) &&
                                    !clockQuads.empty() &&
                                    rt::native::preview::previewRender2DTextured(
                                        renderer, clockQuads, {0.f, 0.f, 0.f, 0.f}, dialPixels, error))
                                    compositeOver(pixels, dialPixels);
                                const std::array<float, 4> kTeal{0.f, .83f, .63f, 1.f};
                                std::vector<rt::native::preview::ScreenQuad> colon{
                                    {569.f * sx, 445.f * sy, 3.f * sx, 3.f * sy, kTeal},
                                    {569.f * sx, 451.f * sy, 3.f * sx, 3.f * sy, kTeal}};
                                std::vector<std::uint8_t> colonPixels;
                                if (rt::native::preview::previewRender2D(
                                        renderer, colon, {0.f, 0.f, 0.f, 0.f}, colonPixels, error))
                                    compositeOver(pixels, colonPixels);
                            }
                            // Oracle town minimap, re-adjudicated 2026-10-18
                            // directly against build/scratch/oracle-golden/*.png
                            // (1280x447 shot px -> design x/2, y*448/447).
                            // Chrome plate rounded rect (35,301,135,145) corner
                            // r~8; the lavender map field (44,309,116,105) sits
                            // in the UPPER plate with a chrome bar below it down
                            // to y~446 (row-scan: field ends ~y414, chrome gray
                            // continues to y446); dark-green grips (0,104,8) at
                            // x 31..40 and 162..171, y 336..386; roads navy
                            // (45,45,116) ~3 design px wide stamped from the
                            // Peach Town street graph (townGraph(1)); red car
                            // dot 3.25x4.5 design px.  Scroll law RESOLVED
                            // 2026-10-18 (coordinator mask fit): roads shift
                            // (+6,-9) png px between town_driving.png and
                            // town_driving_street.png (road-mask residual
                            // 3276 @shift 0 -> 496 @best shift), while the car
                            // dot sits at the field centre (px ~206,365 vs
                            // centre 205,362) — a car-centred north-up world
                            // window that scrolls with the car, NOT a static
                            // window; native's car-centred projection is the
                            // right model.  Zoom per the ELF §1 handler law
                            // (minimap.md: mapViewMatrix 0x21F920, x scale s =
                            // 0.20 working estimate from road-grid spacing, y
                            // scale s * 0.47 squash, DAT_00332CAC = -0.47; the
                            // equal-axis kKy = 0.20 estimate came from the now
                            // disproved static-window reading) (grid
                            // spacing ~7.5-10 px per 50 world units on x).  Green
                            // route dashes are a GAP.  In a RACE the oracle's
                            // map element is a different layout entirely — see
                            // the race-map v1 block below.
                            {
                                const char* mmEnv = std::getenv("RT_APP_MINIMAP");
                                const bool drawMinimap = !(mmEnv && std::string_view(mmEnv) == "0");
                                // The pill drawn here is the TOWN minimap (Peach
                                // Town street graph, lavender field, chrome).  The
                                // oracle race element is the translucent ribbon
                                // drawn by the race-map v1 block below.
                                const bool mmRace =
                                    session.state() == rt::native::gameplay::SessionState::RACE;
                                const std::array<float, 4> kChrome{128.f / 255.f, 128.f / 255.f, 128.f / 255.f, 1.f};
                                const std::array<float, 4> kChromeHi{212.f / 255.f, 212.f / 255.f, 212.f / 255.f, 1.f};
                                const std::array<float, 4> kChromeLo{115.f / 255.f, 115.f / 255.f, 115.f / 255.f, 1.f};
                                const std::array<float, 4> kGrip{0.f, 104.f / 255.f, 8.f / 255.f, 1.f};
                                const std::array<float, 4> kField{116.f / 255.f, 108.f / 255.f, 128.f / 255.f, 1.f};
                                const std::array<float, 4> kRoad{45.f / 255.f, 45.f / 255.f, 116.f / 255.f, 1.f};
                                const std::array<float, 4> kCarDot{1.f, 1.f / 255.f, 1.f / 255.f, 1.f};
                                std::vector<rt::native::preview::ScreenQuad> mapQuads;
                                if (drawMinimap && !mmRace) {
                                    // Rounded pill body rasterised in 1-UI-px rows
                                    // (golden-truth corner radius ~8 design px).
                                    constexpr float kPx = 35.f, kPy = 301.f, kPw = 135.f, kPh = 145.f, kRad = 8.f;
                                    for (float yy = 0.f; yy < kPh; yy += 1.f) {
                                        float inset = 0.f;
                                        if (yy < kRad)
                                            inset = kRad - std::sqrt(std::max(0.f, kRad * kRad - (kRad - yy) * (kRad - yy)));
                                        else if (yy > kPh - kRad)
                                            inset = kRad - std::sqrt(std::max(0.f, kRad * kRad - (yy - (kPh - kRad)) * (yy - (kPh - kRad))));
                                        const float h = std::min(1.f, kPh - yy);
                                        mapQuads.push_back({(kPx + inset) * sx, (kPy + yy) * sy,
                                                          (kPw - 2.f * inset) * sx, h * sy, kChrome});
                                    }
                                    // Frame bevel: bright top edge along the new
                                    // plate top (y ~301, x 42..161) and the
                                    // darker bottom edge row near the plate foot
                                    // (chrome bar below the field, y ~444).
                                    mapQuads.push_back({42.f * sx, 301.f * sy, 119.f * sx, 1.5f * sy, kChromeHi});
                                    mapQuads.push_back({46.f * sx, 444.f * sy, 113.f * sx, 2.f * sy, kChromeLo});
                                    // Dark-green grip blocks, both ends (golden x
                                    // 31..40 and 162..171, y 336..386).
                                    mapQuads.push_back({31.f * sx, 336.f * sy, 9.f * sx, 50.f * sy, kGrip});
                                    mapQuads.push_back({162.f * sx, 336.f * sy, 9.f * sx, 50.f * sy, kGrip});
                                    // Field + roads + car.  Lavender field sits in
                                    // the UPPER plate (golden-truth 44,309,116,105;
                                    // chrome bar continues below to y~446).
                                    constexpr float kFx = 44.f, kFy = 309.f, kFw = 116.f, kFh = 105.f;
                                    mapQuads.push_back({kFx * sx, kFy * sy, kFw * sx, kFh * sy, kField});
                                    const float carUiX = kFx + kFw * .5f, carUiY = kFy + kFh * .5f;
                                    // Working zoom kKx = kKy = 0.20 design px per
                                    // world unit (grid spacing 7.5-8.5 px per 50
                                    // units); no 0.47 squash on the map.
                                    constexpr float kKx = .20f;
                                    constexpr float kKy = .20f;
                                    for (const auto& sgm : mmRoads) {
                                        const float ax = carUiX + (sgm[0] - renderSnapshot.carPosition.x) * kKx;
                                        const float ay = carUiY - (sgm[1] - renderSnapshot.carPosition.z) * kKy;
                                        const float bx = carUiX + (sgm[2] - renderSnapshot.carPosition.x) * kKx;
                                        const float by = carUiY - (sgm[3] - renderSnapshot.carPosition.z) * kKy;
                                        const float dx = bx - ax, dy = by - ay;
                                        const int steps = std::max(1, static_cast<int>(std::hypot(dx, dy)));
                                        for (int i = 0; i <= steps; ++i) {
                                            const float t = static_cast<float>(i) / static_cast<float>(steps);
                                            const float px = ax + dx * t, py = ay + dy * t;
                                            if (px < kFx || px > kFx + kFw || py < kFy || py > kFy + kFh) continue;
                                            mapQuads.push_back({(px - 1.5f) * sx, (py - 1.5f) * sy, 3.f * sx, 3.f * sy, kRoad});
                                        }
                                    }
                                    // Car dot 3.25x4.5 design px (golden px
                                    // 200..213 x 361..370 in the town shots).
                                    mapQuads.push_back({(carUiX - 1.625f) * sx, (carUiY - 2.25f) * sy, 3.25f * sx, 4.5f * sy, kCarDot});
                                }
                                std::vector<std::uint8_t> mapPixels;
                                if (drawMinimap && !mmRace && !mapQuads.empty() &&
                                    rt::native::preview::previewRender2D(
                                        renderer, mapQuads, {0.f, 0.f, 0.f, 0.f}, mapPixels, error))
                                    compositeOver(pixels, mapPixels);
                                if (drawMinimap && mmRace && !mmRaceNodes.empty() && mmRaceFit > 0.f) {
                                    // ---- Race map v1 (staticGraph(10); oracle-
                                    // golden adjudicated 2026-10-18) --------------
                                    // The oracle race element is a translucent
                                    // blue-violet road ribbon, upper-left, with NO
                                    // field quad and NO chrome (build/scratch/
                                    // oracle-golden/race_hud.png: core (61,61,131)
                                    // fully-saturated chroma, stroke ~4-6 design
                                    // px, red player arrow; green route dashes x>
                                    // 60 tied to raceMapPlacements -> GAP).  v1
                                    // law: draw the whole course world-fixed and
                                    // auto-fit — staticGraph(10), the 143-node
                                    // loop chosen by IoU overlay (the oracle
                                    // reveals only ~40% of it per lap, so the
                                    // native full reveal is a documented v1
                                    // divergence) — through the identity
                                    // orientation (screen x = +world x, screen y
                                    // up = +world z, no town 0.47 squash),
                                    // uniform scale to height 138 design px inside
                                    // rect x 48..112, y 134..272 (centre
                                    // (80,203)).  Ink layers: ~8px outer glow at
                                    // .25, 6px halo at .45, 5px core at .55; the
                                    // per-stamp overlap along the path saturates
                                    // the core toward the oracle's pure core
                                    // chroma, while single stamps keep the
                                    // oracle's translucency at sparse edges.
                                    // Player-centred scroll via minimap_track::
                                    // trackProject, the zoom schedule and the 39
                                    // raceMapPlacements @0x2A4050 are refinement
                                    // GAPs (.qoder/wave-board.md).
                                    const float cx = 80.f, cy = 203.f;
                                    const auto proj = [&](float wx, float wz) {
                                        return std::array<float, 2>{
                                            cx + (wx - mmRaceCx) * mmRaceFit,
                                            cy - (wz - mmRaceCz) * mmRaceFit};
                                    };
                                    const std::array<float, 4> kRibbonGlow{
                                        70.f / 255.f, 70.f / 255.f, 140.f / 255.f, .25f};
                                    const std::array<float, 4> kRibbonMid{
                                        70.f / 255.f, 70.f / 255.f, 140.f / 255.f, .45f};
                                    const std::array<float, 4> kRibbonCore{
                                        70.f / 255.f, 70.f / 255.f, 140.f / 255.f, .55f};
                                    // Liang-Barsky clip against the ribbon window
                                    // (1 design px margin so the glow bleeds to
                                    // the measured ribbon extent, not past it).
                                    const auto clip = [&](float& x0, float& y0, float& x1, float& y1) {
                                        float t0 = 0.f, t1 = 1.f;
                                        const float dx = x1 - x0, dy = y1 - y0;
                                        const float p[4] = {-dx, dx, -dy, dy};
                                        const float q[4] = {x0 - 47.f, 113.f - x0, y0 - 133.f, 273.f - y0};
                                        for (int i = 0; i < 4; ++i) {
                                            if (p[i] == 0.f) {
                                                if (q[i] < 0.f) return false;
                                            } else {
                                                const float r = q[i] / p[i];
                                                if (p[i] < 0.f) { if (r > t1) return false; if (r > t0) t0 = r; }
                                                else             { if (r < t0) return false; if (r < t1) t1 = r; }
                                            }
                                        }
                                        x0 += dx * t0; y0 += dy * t0;
                                        x1 += dx * t1; y1 += dy * t1;
                                        return true;
                                    };
                                    const auto stamp = [&](float x0, float y0, float x1, float y1,
                                                        float w, const std::array<float, 4>& col,
                                                        std::vector<rt::native::preview::ScreenQuad>& dst) {
                                        if (!clip(x0, y0, x1, y1)) return;
                                        const int steps = std::max(1, static_cast<int>(
                                            std::hypot(x1 - x0, y1 - y0)));
                                        for (int i = 0; i <= steps; ++i) {
                                            const float t = static_cast<float>(i) / static_cast<float>(steps);
                                            dst.push_back({(x0 + (x1 - x0) * t - w * .5f) * sx,
                                                        (y0 + (y1 - y0) * t - w * .5f) * sy,
                                                        w * sx, w * sy, col});
                                        }
                                    };
                                    std::vector<rt::native::preview::ScreenQuad> halo, core;
                                    for (const auto& sgm : mmRaceNodes) {
                                        const auto a = proj(sgm[0], sgm[1]);
                                        const auto b = proj(sgm[2], sgm[3]);
                                        stamp(a[0], a[1], b[0], b[1], 8.f, kRibbonGlow, halo);
                                        stamp(a[0], a[1], b[0], b[1], 6.f, kRibbonMid, halo);
                                        stamp(a[0], a[1], b[0], b[1], 5.f, kRibbonCore, core);
                                    }
                                    // Red player arrow: short tail along heading
                                    // plus head dot (heading 0 = +Z = screen up in
                                    // the identity orientation, so forward on
                                    // screen is (sin h, -cos h)).
                                    const auto carUi = proj(renderSnapshot.carPosition.x,
                                                        renderSnapshot.carPosition.z);
                                    const float dirX = std::sin(renderSnapshot.heading);
                                    const float dirY = -std::cos(renderSnapshot.heading);
                                    stamp(carUi[0] - dirX * 5.f, carUi[1] - dirY * 5.f,
                                        carUi[0] + dirX * 2.f, carUi[1] + dirY * 2.f,
                                        3.f, kCarDot, core);
                                    core.push_back({(carUi[0] - 2.f) * sx, (carUi[1] - 2.f) * sy,
                                                 4.f * sx, 4.f * sy, kCarDot});
                                    std::vector<std::uint8_t> ribbonPixels;
                                    if (!halo.empty() &&
                                        rt::native::preview::previewRender2D(
                                            renderer, halo, {0.f, 0.f, 0.f, 0.f}, ribbonPixels, error))
                                        compositeOver(pixels, ribbonPixels);
                                    std::vector<std::uint8_t> corePixels;
                                    if (!core.empty() &&
                                        rt::native::preview::previewRender2D(
                                            renderer, core, {0.f, 0.f, 0.f, 0.f}, corePixels, error))
                                        compositeOver(pixels, corePixels);
                                }
                            }
                        }
                        if (hudTextEnabled && rendered) {
                            // Idempotent per renderer: re-uploads only after a
                            // renderer recreate (resize path), no-op otherwise.
                            const rt::native::preview::PreviewTexture atlasTex{
                                fontAtlas.width, fontAtlas.height, fontAtlas.pixels};
                            std::string texError;
                            if (rt::native::preview::previewUploadFontAtlas(renderer, atlasTex, texError)) {
                                const int w = static_cast<int>(presenter.extent.width);
                                const int h = static_cast<int>(presenter.extent.height);
                                 std::vector<rt::native::preview::HudQuad> textQuads =
                                    scoreHudEnabled
                                        ? rt::native::preview::scoreMeterPageTextQuads(
                                              scoreHudMeter
                                                  ? rt::native::preview::ScoreMeterPage::Meter
                                                  : rt::native::preview::ScoreMeterPage::Score,
                                              snapshot,
                                              static_cast<int>(rt::native::gameplay::collectedCount(collectibles)) * 500,
                                              static_cast<int>(session.progress().money), w, h)
                                        : session.state() == rt::native::gameplay::SessionState::RACE
                                               // Authentic SCORE.GSL timer digits replace the
                                               // FONTE stand-in when the banner page is bound.
                                               ? (hudArt.raceBanner
                                                      ? std::vector<rt::native::preview::HudQuad>{}
                                                      : rt::native::preview::raceHudTextQuads(
                                                            snapshot, session.totalLaps(), w, h))
                                               // Oracle town view keeps the clock inside the
                                               // gauge band (pass 3), not top-right; only fall
                                               // back to the FONTE corner clock without art.
                                               : (hudArt.speedDial
                                                      ? std::vector<rt::native::preview::HudQuad>{}
                                                      : rt::native::preview::townClockTextQuads(snapshot.clock, w, h));
                                if (perfOverlayEnabled && !perfOverlayText.empty()) {
                                    // The HUD's ground-level overlay precedent uses UI y=6;
                                    // keep this opt-in row in that slot and create no quads
                                    // unless both FONTE HUD text and the perf overlay are live.
                                    rt::native::preview::appendFonteText(textQuads, perfOverlayText.c_str(), 6.f, 6.f,
                                                                         {1.f, 1.f, 1.f, .95f}, w, h);
                                }
                                if (scoreHudEnabled && !hudTextWarned) {
                                    hudTextWarned = true;
                                    std::cout << "rt-app scorehud: page=" << (scoreHudMeter ? "METER" : "SCORE")
                                              << " (AUTHORED label block; digit anchors per hud-layout.md)\n";
                                }
                                std::vector<std::uint8_t> textPixels;
                                if (!textQuads.empty() &&
                                    rt::native::preview::previewRender2DTextured(
                                        renderer, textQuads, {0.f, 0.f, 0.f, 0.f}, textPixels, error))
                                    compositeOver(pixels, textPixels);
                            } else if (!hudTextWarned) {
                                hudTextWarned = true;
                                std::cout << "rt-app hudtext: upload failed: " << texError << "\n";
                            }
                        }
                } else {
                    const int menuW = static_cast<int>(presenter.extent.width);
                    const int menuH = static_cast<int>(presenter.extent.height);
                    const std::array<float, 4> menuClear{.025f, .035f, .075f, 1.f};
                    if (fontAtlas.width > 0) {
                        // SYS/TITLE.GSL unit 0 is the shipped 640x448 plate,
                        // cars, road and sky.  Its runtime-only decoder is
                        // deliberately optional: a missing disc takes the
                        // old menuQuads path below unchanged.
                        static const std::optional<rt::native::preview::PreviewTexture> titleBackdrop =
                            rt::native::assets::loadTitleBackdrop(discRoot());
                        if (state == AppState::TITLE && titleBackdrop) {
                            std::string titleError;
                            const rt::native::preview::HudQuad fullscreen{
                                0.f, 0.f, static_cast<float>(menuW), static_cast<float>(menuH),
                                0.f, 0.f, 1.f, 1.f, {1.f, 1.f, 1.f, 1.f}};
                            if (rt::native::preview::previewUploadFontAtlas(renderer, *titleBackdrop, titleError) &&
                                rt::native::preview::previewRender2DTextured(
                                    renderer, {fullscreen}, {0.f, 0.f, 0.f, 1.f}, pixels, error)) {
                                // Rebind FONTE after the backdrop: the same
                                // 2D textured pass then keeps the blink and
                                // publisher/copyright text over the art.
                                const auto ui = buildLiveMenuScreen(state, menuFlow, titleUi, frames, menuW, menuH);
                                const rt::native::preview::PreviewTexture atlasTex{
                                    fontAtlas.width, fontAtlas.height, fontAtlas.pixels};
                                if (!ui.text.empty() &&
                                    rt::native::preview::previewUploadFontAtlas(renderer, atlasTex, titleError)) {
                                    std::vector<std::uint8_t> textPixels;
                                    if (rt::native::preview::previewRender2DTextured(
                                            renderer, ui.text, {0.f, 0.f, 0.f, 0.f}, textPixels, error))
                                        compositeOver(pixels, textPixels);
                                }
                                rendered = true;
                            } else {
                                rendered = false;
                                if (!titleError.empty()) error = titleError;
                            }
                        } else {
                        // Oracle main_menu.png shows the shipped title scene
                        // behind the menu panel.  Reuse the TITLE.GSL backdrop
                        // (same runtime-only texture as the title screen): draw
                        // it opaque, then drop the flat dark panel base and
                        // composite the menu rows over it with a transparent
                        // clear.  No art (no disc / decode miss) keeps the
                        // legacy opaque menuClear path byte-identical.
                        const bool artBackdrop = titleBackdrop && state == AppState::MENU &&
                            (menuFlow.screen == rt::native::gameplay::MenuScreen::MODE_SELECT ||
                             menuFlow.screen == rt::native::gameplay::MenuScreen::OPTIONS);
                        bool backdropDrawn = false;
                        if (artBackdrop) {
                            std::string bdError;
                            const rt::native::preview::HudQuad fullscreen{
                                0.f, 0.f, static_cast<float>(menuW), static_cast<float>(menuH),
                                0.f, 0.f, 1.f, 1.f, {1.f, 1.f, 1.f, 1.f}};
                            backdropDrawn =
                                rt::native::preview::previewUploadFontAtlas(renderer, *titleBackdrop, bdError) &&
                                rt::native::preview::previewRender2DTextured(
                                    renderer, {fullscreen}, {0.f, 0.f, 0.f, 1.f}, pixels, error);
                            if (!backdropDrawn && !bdError.empty()) std::cout << "rt-app menu-backdrop: " << bdError << "\n";
                        }
                        // Keep menuQuads' color backdrop; drop the placeholder
                        // bars (TITLE's two colored rects, and the MENU rows)
                        // and composite the real UiScreen over that panel.
                        auto backdrop = menuQuads(state, menuFlow, menuW, menuH);
                        std::vector<rt::native::preview::ScreenQuad> quads;
                        if (!backdrop.empty() && !backdropDrawn) quads.push_back(backdrop.front());
                        const auto ui = buildLiveMenuScreen(state, menuFlow, titleUi, frames, menuW, menuH);
                        quads.insert(quads.end(), ui.quads.begin(), ui.quads.end());
                        if (backdropDrawn) {
                            std::vector<std::uint8_t> panelPixels;
                            rendered = rt::native::preview::previewRender2D(
                                renderer, quads, {0.f, 0.f, 0.f, 0.f}, panelPixels, error);
                            if (rendered) compositeOver(pixels, panelPixels);
                        } else {
                            rendered = rt::native::preview::previewRender2D(
                                renderer, quads, menuClear, pixels, error);
                        }
                        if (rendered && !ui.text.empty()) {
                            const rt::native::preview::PreviewTexture atlasTex{
                                fontAtlas.width, fontAtlas.height, fontAtlas.pixels};
                            std::string texError;
                            if (rt::native::preview::previewUploadFontAtlas(renderer, atlasTex, texError)) {
                                std::vector<std::uint8_t> textPixels;
                                if (rt::native::preview::previewRender2DTextured(
                                        renderer, ui.text, {0.f, 0.f, 0.f, 0.f}, textPixels, error))
                                    compositeOver(pixels, textPixels);
                            } else if (!hudTextWarned) {
                                hudTextWarned = true;
                                std::cout << "rt-app hudtext: upload failed: " << texError << "\n";
                            }
                        }
                        }
                    } else {
                        auto fallback = menuQuads(state, menuFlow, menuW, menuH);
                        if (state == AppState::LOAD) fallback = titleUi.emit(menuW, menuH).quads;
                        rendered = rt::native::preview::previewRender2D(
                            renderer, fallback, menuClear, pixels, error);
                    }
                }
                if (!rendered || !((glPresenter && !headlessApp) ? rt::native::preview::glPresent(glPresenter, pixels, error) : present(presenter, pixels, retry, error))) {
                    std::cerr << "render: " << error << "\n"; running = false; break;
                }
                perf.notePresent();
                if (interpEnabled) ++interpRenders;
                if (const char* snap = std::getenv("RT_APP_SNAPSHOT")) {
                    // Optional one-shot: RT_APP_SNAPSHOT_AT=<simframe> dumps
                    // that logic frame only (default: every frame, last frame
                    // wins).  Lets adjudication grab a mid-race frame instead
                    // of the RESULTS screen the final frame shows.
                    const char* at = std::getenv("RT_APP_SNAPSHOT_AT");
                    const long want = at ? std::atol(at) : -1;
                    if (want < 0 || static_cast<long>(renderSnapshot.frame) == want) {
                    std::ofstream snapStream(snap, std::ios::binary);
                    snapStream << "P6\n" << presenter.extent.width << ' ' << presenter.extent.height
                             << "\n255\n";
                    for (std::size_t i = 0; i + 3 < pixels.size(); i += 4) {
                        snapStream.put(static_cast<char>(pixels[i]));
                        snapStream.put(static_cast<char>(pixels[i + 1]));
                        snapStream.put(static_cast<char>(pixels[i + 2]));
                    }
                    std::cout << "rt-app snapshot: " << snap << " (" << presenter.extent.width
                              << 'x' << presenter.extent.height << ")\n";
                    }
                }
                if (retry && !recreateSwapchain()) { running = false; break; }
            } while (retry && presenter.extent.width && presenter.extent.height);
        }
        const double perfNow = perf.now();
        if (perfNow >= nextPerfWindow) {
            while (perfNow >= nextPerfWindow) nextPerfWindow += perf.windowSeconds();
            if (const auto window = perf.lastWindow(); window && !window->warmup) {
                perfOverlayText = perfOverlay.text(*window);
                if (perfLogEnabled) {
                    const auto integer = [](double value) { return std::to_string(static_cast<long>(std::floor(std::max(0.0, value) + .5))); };
                    const auto decimal = [](double value) {
                        std::ostringstream out;
                        out << std::fixed << std::setprecision(1)
                            << std::floor(std::max(0.0, value) * 10.0 + .5) / 10.0;
                        return out.str();
                    };
                    std::cout << "[perf] fps=" << integer(window->renderFps)
                              << " game=" << integer(window->logicRate)
                              << " gen=" << decimal(window->genFactor)
                              << " worst=" << decimal(window->worstFrameMs)
                              << " cpu=" << integer(window->cpuPercent) << "\n";
                }
            }
        }
        if (!interpEnabled || testSocket.lockstep()) ++frames;
    }
    std::cout << "rt-app frames=" << frames << " simulation_frame=" << simulationFrame << "\n";
    if (nativeBoot.ok) {  // where decompiled boot stopped: kernel-object census + quarantined faults
        std::size_t nf = 0;
        const rtguest::Fault* fs = rtguest::faults(&nf);
        std::cout << "rt-app native-boot end: vblanks=" << nativeBoot.vblanks
                  << " threads=" << rtguest::kThreadCount() << " semas=" << rtguest::kSemaCount()
                  << " faults=" << nf << "\n";
        for (std::size_t i = 0; i < nf && i < 24; ++i)
            std::cout << "  fault 0x" << std::hex << fs[i].addr << std::dec
                      << " x" << fs[i].count << "\n";
        rtguest::vif1MaybeDumpCode();  // RT_VIF1_CODE_DUMP (W3 step 2 aid)
        if (rtguest::vif1HistOn()) rtguest::vif1HistDump();  // census even for short boots
        if (rtguest::gifCensusOn()) rtguest::gifCensusDump();
        if (rtguest::gsCensusOn()) rtguest::gsCensusDump();
        if (rtguest::gifCensusOn()) guest::gsEnvCensusDump();
    }
    if (interpEnabled)
        std::cout << "rt-app interp: steps=" << interpClock.stepsTaken()
                  << " renders=" << interpRenders << "\n";
    if (streamed) {
        std::cout << "rt-app stream: loads=" << streamed->loads
                  << " final window row=" << streamed->streamer.row()
                  << " col=" << streamed->streamer.colStart() << "\n";
    }
    if (rumbleEnabled && rumbleChannel.isActive()) {
        rumbleChannel.stop();
        // Zero-intensity SDL_RumbleGamepad is the stop (SDL_gamepad.h:1517-1518).
        if (gamepad) (void)SDL_RumbleGamepad(gamepad, 0, 0, 0);
    }
    if (gamepad) SDL_CloseGamepad(gamepad);
    destroyAudio(audio);
    // W3-04: opt-in shutdown persistence (see slot-store seam at session boot).
    if (const auto slot = envSlot("RT_APP_SAVE_SLOT")) {
        std::string error;
        if (slotStore.save(*slot, session.progress(), &error))
            std::cout << "[save] wrote slot " << *slot << " (" << slotStore.path(*slot).string() << ")\n";
        else
            std::cout << "[save] save slot " << *slot << " failed: " << error << "\n";
    }
    rt::native::preview::previewDestroy(renderer); destroy(presenter); rt::native::preview::glPresenterDestroy(glPresenter); SDL_DestroyWindow(window); SDL_Quit();
    return 0;
}
