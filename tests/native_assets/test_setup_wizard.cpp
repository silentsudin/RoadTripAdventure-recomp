#include "../../src/native/assets/setup_wizard.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace {
int failures = 0;
void check(bool value, const char* message) {
    if (!value) { ++failures; std::cerr << "FAIL: " << message << '\n'; }
}
void write(const std::filesystem::path& path, const std::vector<unsigned char>& bytes) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::binary);
    out.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
}
}

int main() {
    const auto root = std::filesystem::temp_directory_path() / "rt_setup_wizard_test";
    std::error_code ec;
    std::filesystem::remove_all(root, ec);
    std::filesystem::create_directories(root);

    const auto manifest = root / "assets/manifest.json";
    const std::vector<unsigned char> payload{1, 2, 3, 4};
    write(root / "assets/fonts/FONTE.GSL", payload);
    const auto crc = rt::native::assets::crc32(payload);
    rt::native::assets::SetupManifest ready;
    ready.files.push_back({"fonts/FONTE.GSL", payload.size(), crc});
    check(rt::native::assets::writeSetupManifest(manifest, ready), "writes manifest");
    check(rt::native::assets::isSetupManifestValid(root / "assets"), "valid manifest skips extraction");

    write(root / "assets/fonts/FONTE.GSL", {9, 9, 9, 9});
    check(!rt::native::assets::isSetupManifestValid(root / "assets"), "changed file invalidates manifest");

    const auto elf = root / "SLUS_203.98";
    write(elf, {0, 1, 2, 3});
    check(!rt::native::assets::hasExpectedBootElfCrc(elf), "corrupt boot ELF is rejected");
    std::filesystem::remove_all(root, ec);
    return failures == 0 ? 0 : 1;
}
