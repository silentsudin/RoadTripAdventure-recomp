#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace rt::native::assets {

struct SetupManifestFile {
    std::string path;
    std::uintmax_t size = 0;
    std::uint32_t crc32 = 0;
};

struct SetupManifest { std::vector<SetupManifestFile> files; };

std::uint32_t crc32(const std::vector<unsigned char>& bytes);
bool hasExpectedBootElfCrc(const std::filesystem::path& elf);
bool writeSetupManifest(const std::filesystem::path& path, const SetupManifest& manifest);
bool isSetupManifestValid(const std::filesystem::path& assetsRoot);

// Runs before the renderer's normal title/state selection. It never throws:
// an absent or invalid user-supplied disc leaves the app running in setup.
bool RunFirstRunSetupIfRequired();

} // namespace rt::native::assets
