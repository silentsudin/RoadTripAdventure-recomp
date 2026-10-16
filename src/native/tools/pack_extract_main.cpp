// src/native/tools/pack_extract_main.cpp
//
// rt-pack-extract: first-run pack-extraction cataloger for the authored
// native pipeline. Enumerates the DMA texture-unit packs on the user's own
// extracted disc (SYS/*.GSL, ITEM/*.GSL, SHOP/*.BIN, CAR*/Q*.BIN
// self-textured chains), parses every upload unit with the grammar of
// docs/native-reference/sys-gsl-packs.md (unit layout
// [CNT tag][junk][A+D 0x50..0x53][IMAGE tag][pixels], invariant
// qwc == 6 + NLOOP), resolves each pack's LBA through the disc's
// .lbn_map.tsv, and writes per-pack JSON catalogs plus a top-level summary.
// No game data ships with this tool and no pixels are written: catalogs
// describe structure only. The parser/writer live in pack_catalog.hpp, which
// the disc-backed integration test compiles too.
//
// Usage: rt-pack-extract [disc-root] [output-dir]
//   disc-root: RT_DISC when set, else
//              "~/Library/Application Support/RoadTripRecomp/disc"
//   output-dir: default "build/extract" (per-pack catalogs under
//               <output-dir>/<family>/..., summary at <output-dir>/summary.json)
//
// Exit codes: 0 every unit validated, 1 unreadable disc/map or a rejected
// pack (details on stderr and in summary.json), 2 usage error.

#include "pack_catalog.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

namespace {

constexpr int kExitUsage = 2;

// Disc root from the environment: RT_DISC when set, otherwise the standard
// extracted-disc location under the user's home directory.
std::string discRoot() {
    if (const char* env = std::getenv("RT_DISC"); env != nullptr && env[0] != '\0') {
        return env;
    }
    const char* home = std::getenv("HOME");
    if (home == nullptr || home[0] == '\0') {
        return {};
    }
    return (std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc")
        .string();
}

// Whole file as bytes; prints its own error and returns false when the file
// cannot be read.
bool readFileBytes(const std::string& path, std::vector<std::uint8_t>& out) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "rt-pack-extract: cannot open '" << path << "'\n";
        return false;
    }
    out.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    if (file.bad()) {
        std::cerr << "rt-pack-extract: error reading '" << path << "'\n";
        return false;
    }
    return true;
}

bool writeFile(const std::filesystem::path& path, const std::string& text) {
    std::error_code ec;
    std::filesystem::create_directories(path.parent_path(), ec);
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file) {
        std::cerr << "rt-pack-extract: cannot write '" << path.string() << "'\n";
        return false;
    }
    file.write(text.data(), static_cast<std::streamsize>(text.size()));
    file.close();
    return !file.fail();
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc > 3) {
        std::cerr << "usage: rt-pack-extract [disc-root] [output-dir]\n";
        return kExitUsage;
    }
    const std::string root = argc >= 2 ? argv[1] : discRoot();
    const std::string outDir = argc >= 3 ? argv[2] : "build/extract";
    if (root.empty()) {
        std::cerr << "rt-pack-extract: cannot resolve the disc root "
                     "(pass it as argument 1 or set RT_DISC; HOME is unset)\n";
        return EXIT_FAILURE;
    }

    // LBA resolution table (name \t lba \t byte-size at the disc root).
    std::map<std::string, rt::native::tools::PackLbnEntry> lbn;
    std::string lbnError;
    if (!rt::native::tools::loadPackLbnMap((std::filesystem::path(root) / ".lbn_map.tsv").string(),
                                           lbn, lbnError)) {
        std::cerr << "rt-pack-extract: " << lbnError << '\n';
        return EXIT_FAILURE;
    }

    // Enumerate + parse + resolve every family, in fixed order.
    std::vector<rt::native::tools::PackFile> packs;
    for (const char* family : rt::native::tools::kPackFamilies) {
        std::vector<std::string> names;
        std::string error;
        if (!rt::native::tools::enumeratePackFiles(root, family, names, error)) {
            std::cerr << "rt-pack-extract: " << error << '\n';
            return EXIT_FAILURE;
        }
        for (const std::string& name : names) {
            rt::native::tools::PackFile pack;
            pack.family = family;
            pack.name = name;

            std::vector<std::uint8_t> bytes;
            if (readFileBytes((std::filesystem::path(root) / name).string(), bytes)) {
                rt::native::tools::parsePackFile(pack, bytes);
            } else {
                pack.error = "unreadable pack file";
            }

            const auto it = lbn.find(name);
            if (it != lbn.end()) {
                pack.lba = it->second.lba;
                pack.mapBytes = it->second.bytes;
                pack.lbaKnown = true;
            } else if (pack.error.empty()) {
                pack.error = "pack is not in .lbn_map.tsv";
            }
            packs.push_back(std::move(pack));
        }
    }

    // Per-pack catalogs under <outDir>/<family>/..., summary at the root.
    std::size_t written = 0;
    for (const rt::native::tools::PackFile& pack : packs) {
        const std::string json = rt::native::tools::packFileToJson(pack);
        std::filesystem::path path = std::filesystem::path(outDir) / pack.family /
                                     pack.name.substr(pack.name.find('/') + 1);
        path += ".json";
        if (!writeFile(path, json)) {
            return EXIT_FAILURE;
        }
        ++written;
    }
    const std::string summary = rt::native::tools::packSummaryToJson(packs, root);
    if (!writeFile(std::filesystem::path(outDir) / "summary.json", summary)) {
        return EXIT_FAILURE;
    }
    ++written;

    // Console report.
    std::map<std::string, rt::native::tools::PackFamilySummary> families;
    rt::native::tools::summarizePackFiles(packs, families);
    std::cout << "rt-pack-extract: disc root: " << root << '\n';
    for (const char* family : rt::native::tools::kPackFamilies) {
        const auto it = families.find(family);
        if (it == families.end()) {
            continue;
        }
        std::cout << "rt-pack-extract: " << family << ": " << it->second.packs << " packs, "
                  << it->second.units << " units (" << it->second.textureUnits
                  << " texture, " << it->second.clutUnits << " clut-shaped)\n";
    }
    std::uint32_t units = 0;
    for (const auto& fam : families) {
        units += fam.second.units;
    }
    const bool allValid = std::all_of(packs.begin(), packs.end(),
                                      [](const rt::native::tools::PackFile& pack) {
                                          return pack.valid();
                                      });
    std::cout << "rt-pack-extract: total: " << packs.size() << " packs, " << units
              << " units; " << (allValid ? "all units valid" : "VALIDATION FAILURES") << '\n';
    std::cout << "rt-pack-extract: wrote " << written << " JSON files to " << outDir << '\n';
    if (!allValid) {
        for (const rt::native::tools::PackFile& pack : packs) {
            if (pack.valid()) {
                continue;
            }
            std::cerr << "rt-pack-extract: FAILED " << pack.name << ": " << pack.error << '\n';
        }
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
