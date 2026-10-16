// Synthetic contract tests plus an optional local-disc cross-check for the
// streaming id -> FLD filename resolver (see docs/native-reference/
// field-topology-status.md "Tile identity & streaming").

#include "../../src/native/assets/field_tile_index.h"

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace rt::native::assets;

namespace {

int g_checks = 0;
int g_failures = 0;

void check(bool condition, const std::string& description) {
    ++g_checks;
    if (!condition) {
        ++g_failures;
        std::cerr << "FAIL: " << description << '\n';
    }
}

std::filesystem::path discRoot() {
    if (const char* root = std::getenv("RT_DISC")) {
        return root;
    }
    const char* home = std::getenv("HOME");
    if (home == nullptr) {
        return {};
    }
    return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc";
}

void appendWord(std::vector<std::uint8_t>& bytes, std::uint32_t value) {
    for (int i = 0; i < 4; ++i) {
        bytes.push_back(static_cast<std::uint8_t>((value >> (8 * i)) & 0xFF));
    }
}

// Minimal ELF32 with one PT_LOAD mapping the descriptor table at file
// offset 0x1000.
std::vector<std::uint8_t> makeSyntheticElf(std::uint32_t tableVaddr) {
    std::vector<std::uint8_t> elf(0x1000, 0);
    elf[0] = 0x7f; elf[1] = 'E'; elf[2] = 'L'; elf[3] = 'F'; elf[4] = 1;  // ELFCLASS32
    const auto put32 = [&](std::size_t at, std::uint32_t value) {
        for (int i = 0; i < 4; ++i) {
            elf[at + static_cast<std::size_t>(i)] =
                static_cast<std::uint8_t>((value >> (8 * i)) & 0xFF);
        }
    };
    const auto put16 = [&](std::size_t at, std::uint16_t value) {
        elf[at] = static_cast<std::uint8_t>(value & 0xFF);
        elf[at + 1] = static_cast<std::uint8_t>((value >> 8) & 0xFF);
    };
    put16(16, 1);            // e_type
    put16(18, 0x36);        // e_machine MIPS
    put32(20, 1);           // e_version
    put32(28, 52);          // e_phoff
    put16(42, 32);          // e_phentsize
    put16(44, 1);           // e_phnum
    put32(52, 1);           // p_type PT_LOAD
    put32(56, 0x1000);      // p_offset
    put32(60, tableVaddr);  // p_vaddr
    put32(68, 64 * 12);     // p_filesz
    return elf;
}

void appendDescriptors(std::vector<std::uint8_t>& elf, std::uint32_t sectorOffsetBase) {
    for (std::uint32_t id = 0; id < 64; ++id) {
        appendWord(elf, kFieldTileDescriptorTag);
        appendWord(elf, sectorOffsetBase + id * 2);
        appendWord(elf, 1);  // one sector each
    }
}

void writeFile(const std::filesystem::path& path, std::size_t size) {
    std::ofstream out(path, std::ios::binary);
    out.write(std::string(size, '\x5a').data(), static_cast<std::streamsize>(size));
}

std::filesystem::path syntheticDisc(bool allNames) {
    const std::filesystem::path root =
        std::filesystem::temp_directory_path() / "rt_field_tile_index_synth";
    std::error_code ec;
    std::filesystem::remove_all(root, ec);
    std::filesystem::create_directories(root / "FLD");
    std::ofstream out(root / "SLUS_203.98", std::ios::binary);
    std::vector<std::uint8_t> elf = makeSyntheticElf(kFieldTileTableVaddr);
    appendDescriptors(elf, 0x100);
    out.write(reinterpret_cast<const char*>(elf.data()), static_cast<std::streamsize>(elf.size()));
    out.close();
    if (allNames) {
        for (std::uint32_t a = 0; a < 4; ++a) {
            for (std::uint32_t b = 0; b < 4; ++b) {
                for (std::uint32_t c = 0; c < 4; ++c) {
                    const std::string name =
                        std::to_string(a) + std::to_string(b) + std::to_string(c) + ".BIN";
                    writeFile(root / "FLD" / name, 2048);
                }
            }
        }
    } else {
        writeFile(root / "FLD" / "000.BIN", 2048);
    }
    return root;
}

void testFormula() {
    check(fieldTileIdFromName("000.BIN") == 0, "formula: 000 -> 0");
    check(fieldTileIdFromName("001.BIN") == 2, "formula: 001 -> 2 (swap2)");
    check(fieldTileIdFromName("002.BIN") == 1, "formula: 002 -> 1 (swap2)");
    check(fieldTileIdFromName("003.BIN") == 3, "formula: 003 -> 3");
    check(fieldTileIdFromName("132.BIN") == ((1 << 4) | (3 << 2) | 1), "formula: 132 -> 29");
    check(fieldTileIdFromName("333.BIN") == 63, "formula: 333 -> 63");
    check(fieldTileIdFromName("400.BIN") == -1, "formula: out-of-range digit rejected");
    check(fieldTileIdFromName("12.BIN") == -1, "formula: short stem rejected");
    check(fieldTileIdFromName("12A.BIN") == -1, "formula: non-digit rejected");
    check(fieldTileIdFromName("000") == -1, "formula: missing extension rejected");
}

void testSyntheticDisc() {
    const std::filesystem::path root = syntheticDisc(true);
    const auto result = buildFieldTileIndex(root);
    check(result.has_value(), "synthetic: full name set resolves");
    if (!result) {
        return;
    }
    std::size_t mismatches = 0;
    std::size_t fallbacks = 0;
    for (std::size_t id = 0; id < 64; ++id) {
        if ((*result)[id].fileName.empty()) {
            ++mismatches;
        }
        if ((*result)[id].sizeOnlyFallback) {
            ++fallbacks;
        }
    }
    check(mismatches == 0, "synthetic: all 64 ids named");
    check(fallbacks == 0, "synthetic: every id matched by formula");
    check((*result)[1].fileName == "002.BIN", "synthetic: id 1 is 002.BIN");
    check((*result)[29].fileName == "132.BIN", "synthetic: id 29 is 132.BIN");
    std::error_code ec;
    std::filesystem::remove_all(root, ec);
}

void testErrorPaths() {
    // Missing ELF -> Truncated.
    const std::filesystem::path empty =
        std::filesystem::temp_directory_path() / "rt_field_tile_index_empty";
    std::error_code ec;
    std::filesystem::remove_all(empty, ec);
    std::filesystem::create_directories(empty / "FLD");
    check(buildFieldTileIndex(empty).error() == FieldTileError::Truncated,
          "errors: missing ELF is Truncated");
    std::filesystem::remove_all(empty, ec);

    // Bad descriptor tag -> BadHeader.
    const std::filesystem::path badTagRoot = syntheticDisc(true);
    {
        std::fstream out(badTagRoot / "SLUS_203.98",
                        std::ios::in | std::ios::out | std::ios::binary);
        const std::streampos at = 0x1000 + 4 * 12;  // descriptor 4
        const std::uint32_t junk = 0xDEADBEEF;
        out.seekp(at);
        out.write(reinterpret_cast<const char*>(&junk), 4);
    }
    check(buildFieldTileIndex(badTagRoot).error() == FieldTileError::BadHeader,
          "errors: bad descriptor tag is BadHeader");
    std::filesystem::remove_all(badTagRoot, ec);

    // Missing names -> MissingProvenance.
    const std::filesystem::path partial = syntheticDisc(false);
    check(buildFieldTileIndex(partial).error() == FieldTileError::MissingProvenance,
          "errors: unmatched ids are MissingProvenance");
    std::filesystem::remove_all(partial, ec);

    // Overlapping LBA ranges -> BadHeader.
    const std::filesystem::path overlapRoot = syntheticDisc(true);
    {
        std::fstream out(overlapRoot / "SLUS_203.98",
                        std::ios::in | std::ios::out | std::ios::binary);
        // id 7: same sector offset as id 0.
        const std::streampos at = 0x1000 + 7 * 12 + 4;
        const std::uint32_t off = 0x100;
        out.seekp(at);
        out.write(reinterpret_cast<const char*>(&off), 4);
    }
    check(buildFieldTileIndex(overlapRoot).error() == FieldTileError::BadHeader,
          "errors: overlapping LBA ranges are BadHeader");
    std::filesystem::remove_all(overlapRoot, ec);
}

void testRealDiscWhenAvailable() {
    const std::filesystem::path root = discRoot();
    if (root.empty() || !std::filesystem::exists(root / "FLD") ||
        !std::filesystem::exists(root / "SLUS_203.98")) {
        std::cout << "test_field_tile_index: SKIP disc corpus (not found)\n";
        return;
    }
    const auto result = buildFieldTileIndex(root);
    check(result.has_value(), "disc: index resolves for shipped extract");
    if (!result) {
        return;
    }
    std::size_t formulaAgreement = 0;
    std::size_t fallbacks = 0;
    for (std::size_t id = 0; id < 64; ++id) {
        if (fieldTileIdFromName((*result)[id].fileName) == static_cast<int>(id)) {
            ++formulaAgreement;
        }
        if ((*result)[id].sizeOnlyFallback) {
            ++fallbacks;
        }
    }
    check(formulaAgreement == 64, "disc: all ids agree with the digit formula");
    check(fallbacks == 0, "disc: no size-only fallbacks needed");
    std::cout << "test_field_tile_index: disc index 64/64, formula agreement 64/64"
              << " (e.g. id0=" << (*result)[0].fileName << " id29=" << (*result)[29].fileName
              << " id41=" << (*result)[41].fileName << ")\n";
}

}  // namespace

int main() {
    testFormula();
    testSyntheticDisc();
    testErrorPaths();
    testRealDiscWhenAvailable();

    std::cout << "test_field_tile_index: " << (g_checks - g_failures) << "/" << g_checks
              << " checks passed\n";
    return g_failures == 0 ? 0 : 1;
}
