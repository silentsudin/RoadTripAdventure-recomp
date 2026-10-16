#include "setup_wizard.h"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

#ifdef __ANDROID__
#include <android/log.h>
#endif

namespace rt::native::assets {
namespace {
constexpr std::uint32_t kBootElfCrc = 0x5A49851Du;

void log(const std::string& message) {
    std::cout << message << '\n';
#ifdef __ANDROID__
    __android_log_write(ANDROID_LOG_INFO, "RoadTrip", message.c_str());
#endif
}

std::uint32_t crcFile(const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary);
    std::uint32_t crc = 0xFFFFFFFFu;
    char buffer[64 * 1024];
    while (in) {
        in.read(buffer, sizeof buffer);
        for (std::streamsize i = 0; i < in.gcount(); ++i) {
            crc ^= static_cast<unsigned char>(buffer[i]);
            for (int bit = 0; bit != 8; ++bit) crc = (crc >> 1) ^ (0xEDB88320u & -(crc & 1u));
        }
    }
    return crc ^ 0xFFFFFFFFu;
}

std::filesystem::path dataRoot() {
    if (const char* value = std::getenv("RT_DATA_DIR")) return value;
    if (const char* home = std::getenv("HOME")) return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp";
    return {};
}

std::filesystem::path findDisc() {
    if (const char* value = std::getenv("RT_DISC"); value && *value && std::filesystem::is_directory(value)) return value;
    const auto data = dataRoot();
    for (const auto& candidate : {data / "rom", data / "disc", std::filesystem::path("/storage/emulated/0/Download/RoadTripDisc")})
        if (std::filesystem::is_directory(candidate)) return candidate;
    std::error_code ec;
    const auto downloads = std::filesystem::path("/storage/emulated/0/Download");
    if (std::filesystem::is_directory(downloads, ec)) for (const auto& entry : std::filesystem::directory_iterator(downloads, ec))
        if (entry.path().extension() == ".iso") log("rom: ISO candidate skipped-not-yet " + entry.path().filename().string());
    return {};
}

bool parseManifest(const std::filesystem::path& manifest, SetupManifest& result) {
    std::ifstream in(manifest);
    std::string line;
    while (std::getline(in, line)) {
        if (line.rfind("file\t", 0) != 0) continue;
        std::istringstream fields(line.substr(5));
        SetupManifestFile file;
        if (!std::getline(fields, file.path, '\t') || !(fields >> file.size) || fields.get() != '\t' || !(fields >> std::hex >> file.crc32)) return false;
        result.files.push_back(std::move(file));
    }
    return !result.files.empty();
}

void collect(const std::filesystem::path& root, const char* relative, const char* bucket,
             std::vector<std::pair<std::filesystem::path, std::filesystem::path>>& out) {
    const auto source = root / relative;
    if (!std::filesystem::exists(source)) return;
    if (std::filesystem::is_regular_file(source)) { out.emplace_back(source, std::filesystem::path(bucket) / source.filename()); return; }
    for (const auto& entry : std::filesystem::recursive_directory_iterator(source))
        if (entry.is_regular_file()) out.emplace_back(entry.path(), std::filesystem::path(bucket) / std::filesystem::relative(entry.path(), source));
}
} // namespace

std::uint32_t crc32(const std::vector<unsigned char>& bytes) {
    std::uint32_t crc = 0xFFFFFFFFu;
    for (unsigned char byte : bytes) { crc ^= byte; for (int bit = 0; bit != 8; ++bit) crc = (crc >> 1) ^ (0xEDB88320u & -(crc & 1u)); }
    return crc ^ 0xFFFFFFFFu;
}

bool hasExpectedBootElfCrc(const std::filesystem::path& elf) { return std::filesystem::is_regular_file(elf) && crcFile(elf) == kBootElfCrc; }

bool writeSetupManifest(const std::filesystem::path& path, const SetupManifest& manifest) {
    std::error_code ec; std::filesystem::create_directories(path.parent_path(), ec);
    std::ofstream out(path);
    if (!out) return false;
    out << "{\n  \"format\": \"roadtrip-native-assets-v1\",\n";
    for (const auto& file : manifest.files) out << "file\t" << file.path << '\t' << file.size << '\t' << std::hex << file.crc32 << std::dec << '\n';
    return static_cast<bool>(out);
}

bool isSetupManifestValid(const std::filesystem::path& assetsRoot) {
    SetupManifest manifest;
    if (!parseManifest(assetsRoot / "manifest.json", manifest)) return false;
    for (const auto& file : manifest.files) {
        const auto path = assetsRoot / file.path;
        std::error_code ec;
        if (!std::filesystem::is_regular_file(path, ec) || std::filesystem::file_size(path, ec) != file.size || crcFile(path) != file.crc32) return false;
    }
    return true;
}

bool RunFirstRunSetupIfRequired() {
    const auto assets = std::getenv("RT_ASSET_DIR") ? std::filesystem::path(std::getenv("RT_ASSET_DIR")) : dataRoot() / "assets";
    if (isSetupManifestValid(assets)) { log("assets: ready"); return true; }
    const auto disc = findDisc();
    if (disc.empty() || !hasExpectedBootElfCrc(disc / "SLUS_203.98")) { log("rom: invalid"); return false; }

    std::vector<std::pair<std::filesystem::path, std::filesystem::path>> files;
    collect(disc, "SYS/FONTE.GSL", "fonts", files);
    collect(disc, "FLD", "textures/FLD", files);
    for (const char* dir : {"CAR0", "CAR1", "CAR2", "CAR3", "CAR4", "CAR5", "CARS"}) collect(disc, dir, (std::string("cars/") + dir).c_str(), files);
    collect(disc, "SOUND", "audio/SOUND", files);
    if (files.empty()) { log("rom: invalid"); return false; }

    SetupManifest manifest;
    for (std::size_t i = 0; i < files.size(); ++i) {
        const auto& [source, relative] = files[i]; const auto destination = assets / relative;
        std::error_code ec; std::filesystem::create_directories(destination.parent_path(), ec);
        const auto expectedSize = std::filesystem::file_size(source, ec); const auto expectedCrc = crcFile(source);
        if (!std::filesystem::is_regular_file(destination, ec) || std::filesystem::file_size(destination, ec) != expectedSize || crcFile(destination) != expectedCrc) {
            std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing, ec);
            if (ec) { log("extract: failed " + relative.string()); return false; }
        }
        manifest.files.push_back({relative.generic_string(), expectedSize, expectedCrc});
        const unsigned percent = static_cast<unsigned>((i + 1) * 100 / files.size());
        const char* kind = relative.string().find("fonts/") == 0 ? "FONTE" : relative.string().find("textures/") == 0 ? "terrain" : relative.string().find("cars/") == 0 ? "cars" : "audio";
        log(std::string("extract: ") + kind + " " + std::to_string(percent) + "%");
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    if (!writeSetupManifest(assets / "manifest.json", manifest) || !isSetupManifestValid(assets)) { log("extract: manifest failed"); return false; }
    log("assets: ready");
    return true;
}
} // namespace rt::native::assets
