// tests/native_assets/integration/test_packs_integration.cpp
//
// Disc-backed integration test for the pack-extraction side of the native
// asset pipeline, exercised against the user's own extracted disc at run
// time: enumerate the DMA texture-unit packs (SYS/*.GSL, ITEM/*.GSL,
// SHOP/*.BIN, CAR*/Q*.BIN self-textured chains), parse every upload unit
// through tools/pack_catalog.hpp (the same parser rt-pack-extract runs), and
// pin the full-corpus invariants documented in
// docs/native-reference/sys-gsl-packs.md and car-data-status.md. No game
// data ships with this test; the disc root comes from the environment
// (RT_DISC, default "~/Library/Application Support/RoadTripRecomp/disc"). A
// missing disc or pack is a loud setup failure (nonzero exit), not a silent
// skip: this suite exists to run the real parser over real bytes.
//
// Corpus invariants asserted on the real disc:
//   1. Family file counts: SYS 37, ITEM 12, SHOP 22 (21 T-packs + GARAGE),
//      CAR 151 (Q00..Q150); every pack resolves in .lbn_map.tsv with the
//      byte-size column equal to the real file size.
//   2. Unit counts per family: SYS 968, ITEM 174 (SYS+ITEM 1142, the docs'
//      headline total), SHOP 46, CAR 302; METER0..10 carry 52 units and
//      ITEM/PARTS.GSL 122; every CAR file exactly 2 units.
//   3. Every unit (1490 total) holds the grammar: qwc == 6 + NLOOP, unit
//      span (qwc+1)*16, image span NLOOP*16 right after the 112-byte unit
//      header, TRXPOS DSAX/DSAY zero, a proven PSM set {CT32, CT24, CT16,
//      PSMT8, PSMT4}, and the logical byte count ceil(w*h*bpp/8) within 16
//      bytes (quadword padding) of the image span.
//   4. PSM census: SYS 440 PSMT4 + 43 PSMT8 + 2 PS24 textures and 483
//      palette-shaped units, every palette adjacent to its texture (1:1,
//      zero surplus palettes; the 2 surplus uploads are SYS/S00.GSL's two
//      PSMCT24 32x32 textures); ITEM 87+87; SHOP 23+23; CAR 151+151 with
//      every car PSMT8 128x128 + CT32 16x16.
//   5. Chain placement: SYS/ITEM/CAR chains end exactly at EOF; SHOP
//      T-packs end their chain at 0x3c510 with trailing model payload;
//      SHOP/GARAGE.BIN's directory chain runs 0x19990..0x52380; FONTE's
//      atlas pins (unit 0 PSMT4 512x384 image at file 0x70, DBP 0; CLUT
//      CT16 16x16 at 0x180e0, DBP 0x180); CAR header word 7 == file size.
//   6. Determinism: a second full parse + serialization pass produces
//      byte-identical per-pack catalogs and summary JSON.
//
// Failure accounting is plain counters and if-statements (no assert()), so
// behaviour is NDEBUG-independent; any failed check exits nonzero.

#include "../../../src/native/tools/pack_catalog.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

namespace {

using rt::native::tools::PackChain;
using rt::native::tools::PackFamilySummary;
using rt::native::tools::PackFile;
using rt::native::tools::PackLbnEntry;
using rt::native::tools::PackUnit;

// ---------------------------------------------------------------------------
// Failure accounting (NDEBUG-independent; no assert() anywhere in this file).
// ---------------------------------------------------------------------------
int g_checks = 0;
int g_failures = 0;

void check(bool cond, const std::string& what) {
    ++g_checks;
    if (!cond) {
        ++g_failures;
        std::cerr << "FAIL: " << what << '\n';
    }
}

// Prints the summary line and returns the process exit code.
int finish(const std::string& corpusNote) {
    if (g_failures > 0) {
        std::cerr << "test_packs_integration: " << g_failures << " of " << g_checks
                  << " checks FAILED\n";
        return EXIT_FAILURE;
    }
    std::cout << "test_packs_integration: " << g_checks << " checks passed"
              << (corpusNote.empty() ? "" : " (" + corpusNote + ")") << '\n';
    return EXIT_SUCCESS;
}

using Bytes = std::vector<std::uint8_t>;

std::uint32_t readU32LE(const Bytes& b, std::size_t o) {
    return static_cast<std::uint32_t>(b[o]) | (static_cast<std::uint32_t>(b[o + 1]) << 8) |
           (static_cast<std::uint32_t>(b[o + 2]) << 16) |
           (static_cast<std::uint32_t>(b[o + 3]) << 24);
}

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

bool readFileBytes(const std::string& path, Bytes& out) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return false;
    }
    out.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return !file.bad();
}

// Enumerates, reads and parses every family pack, resolving LBAs through the
// LBA map. Returns false on any setup failure (error names it).
bool parseAllPacks(const std::string& root, const std::map<std::string, PackLbnEntry>& lbn,
                   std::vector<PackFile>& out, std::string& error) {
    out.clear();
    for (const char* family : rt::native::tools::kPackFamilies) {
        std::vector<std::string> names;
        if (!rt::native::tools::enumeratePackFiles(root, family, names, error)) {
            return false;
        }
        for (const std::string& name : names) {
            PackFile pack;
            pack.family = family;
            pack.name = name;
            Bytes bytes;
            if (!readFileBytes((std::filesystem::path(root) / name).string(), bytes)) {
                error = "cannot read '" + name + "'";
                return false;
            }
            rt::native::tools::parsePackFile(pack, bytes);
            const auto it = lbn.find(name);
            if (it != lbn.end()) {
                pack.lba = it->second.lba;
                pack.mapBytes = it->second.bytes;
                pack.lbaKnown = true;
            }
            out.push_back(std::move(pack));
        }
    }
    return true;
}

const PackFile* findPack(const std::vector<PackFile>& packs, const std::string& name) {
    for (const PackFile& pack : packs) {
        if (pack.name == name) {
            return &pack;
        }
    }
    return nullptr;
}

std::vector<const PackUnit*> allUnits(const std::vector<PackFile>& packs) {
    std::vector<const PackUnit*> units;
    for (const PackFile& pack : packs) {
        for (const PackChain& chain : pack.chains) {
            for (const PackUnit& unit : chain.units) {
                units.push_back(&unit);
            }
        }
    }
    return units;
}

} // namespace

int main() {
    const std::string root = discRoot();
    if (root.empty()) {
        std::cerr << "test_packs_integration: cannot resolve the disc root "
                     "(set RT_DISC; HOME is unset)\n";
        return EXIT_FAILURE;
    }
    {
        std::error_code ec;
        if (!std::filesystem::is_directory(root, ec)) {
            std::cerr << "test_packs_integration: disc root '" << root
                      << "' is not a directory; set RT_DISC to the extracted disc root "
                         "(no game data ships with this test)\n";
            return EXIT_FAILURE;
        }
    }
    std::cout << "test_packs_integration: " << root << '\n';

    // --- setup: LBA map ------------------------------------------------------
    std::map<std::string, PackLbnEntry> lbn;
    std::string error;
    if (!rt::native::tools::loadPackLbnMap(
            (std::filesystem::path(root) / ".lbn_map.tsv").string(), lbn, error)) {
        std::cerr << "test_packs_integration: " << error << '\n';
        return EXIT_FAILURE;
    }
    check(lbn.size() >= 200, ".lbn_map.tsv holds the disc's file table (>= 200 entries)");

    // --- 1. enumeration, parse success, LBA join -----------------------------
    std::vector<PackFile> packs;
    if (!parseAllPacks(root, lbn, packs, error)) {
        std::cerr << "test_packs_integration: " << error << '\n';
        return EXIT_FAILURE;
    }
    check(packs.size() == 222, "222 packs across SYS+ITEM+SHOP+CAR (got " +
                                   std::to_string(packs.size()) + ")");

    std::map<std::string, std::uint32_t> familyFileCount;
    for (const PackFile& pack : packs) {
        ++familyFileCount[pack.family];
        check(pack.lbaKnown, pack.name + " resolves in .lbn_map.tsv");
        check(pack.mapBytes == pack.fileBytes,
              pack.name + " byte-size column matches the real file size");
        check(pack.valid(), pack.name + " parses with every unit valid" +
                                (pack.error.empty() ? "" : " (error: " + pack.error + ")"));
    }
    check(familyFileCount["SYS"] == 37, "SYS holds 37 .GSL packs (got " +
                                            std::to_string(familyFileCount["SYS"]) + ")");
    check(familyFileCount["ITEM"] == 12, "ITEM holds 12 .GSL packs");
    check(familyFileCount["SHOP"] == 22, "SHOP holds 22 .BIN packs (21 T + GARAGE)");
    check(familyFileCount["CAR"] == 151, "CAR holds 151 self-textured Q packs (Q00..Q150)");

    // --- 2. unit counts per family (the docs' totals) ------------------------
    const std::vector<const PackUnit*> units = allUnits(packs);
    std::map<std::string, std::uint32_t> familyUnitCount;
    for (const PackUnit* unit : units) {
        (void)unit;
    }
    {
        std::map<std::string, std::uint32_t> perFamily;
        for (const PackFile& pack : packs) {
            perFamily[pack.family] += pack.unitCount();
        }
        check(perFamily["SYS"] == 968, "SYS carries 968 units (got " +
                                           std::to_string(perFamily["SYS"]) + ")");
        check(perFamily["ITEM"] == 174, "ITEM carries 174 units (got " +
                                            std::to_string(perFamily["ITEM"]) + ")");
        check(perFamily["SYS"] + perFamily["ITEM"] == 1142,
              "SYS+ITEM carry the documented 1142 units");
        check(perFamily["SHOP"] == 46, "SHOP carries 46 units");
        check(perFamily["CAR"] == 302, "CAR carries 302 units (151 x texture+palette)");

        const PackFile* parts = findPack(packs, "ITEM/PARTS.GSL");
        check(parts != nullptr && parts->unitCount() == 122, "ITEM/PARTS.GSL carries 122 units");
        std::uint32_t meterFiles = 0;
        std::uint32_t meterUnits = 0;
        for (const PackFile& pack : packs) {
            if (pack.family == "ITEM" && pack.name.rfind("ITEM/METER", 0) == 0) {
                ++meterFiles;
                meterUnits += pack.unitCount();
            }
        }
        check(meterFiles == 11, "ITEM/METER0..10 are 11 gauge packs");
        check(meterUnits == 52, "METER0..10 carry 52 units (got " + std::to_string(meterUnits) +
                                    ")");
    }

    // --- 3. per-unit grammar on all 1490 units -------------------------------
    check(units.size() == 1490, "the corpus holds 1490 units (got " +
                                    std::to_string(units.size()) + ")");
    std::uint32_t grammarFailures = 0;
    std::map<std::string, std::uint32_t> psmSeen;
    for (const PackFile& pack : packs) {
        for (const PackChain& chain : pack.chains) {
            if (!chain.error.empty()) {
                ++grammarFailures;
                continue;
            }
            for (const PackUnit& unit : chain.units) {
                const std::string tag = pack.name + " unit@" + rt::native::tools::packHex(unit.offset);
                bool ok = true;
                ok = ok && unit.qwc == 6 + unit.nloop;             // the headline invariant
                ok = ok && unit.byteSpan == (static_cast<std::uint64_t>(unit.qwc) + 1) * 16;
                ok = ok && unit.imageBytes == static_cast<std::uint64_t>(unit.nloop) * 16;
                ok = ok && unit.imageOffset == unit.offset + 112;
                ok = ok && unit.width != 0 && unit.height != 0;
                ok = ok && unit.dsax == 0 && unit.dsay == 0;       // whole-image transfers
                const std::uint32_t bpp = rt::native::tools::packPsmBitsPerPixel(unit.psm);
                const bool provenPsm = unit.psm == 0x00 || unit.psm == 0x01 ||
                                       unit.psm == 0x02 || unit.psm == 0x13 || unit.psm == 0x14;
                ok = ok && provenPsm && bpp != 0;
                const std::uint64_t texels = static_cast<std::uint64_t>(unit.width) * unit.height;
                const std::uint64_t logical = (texels * bpp + 7) / 8;
                ok = ok && unit.logicalBytes == logical;
                ok = ok && unit.imageBytes >= logical && unit.imageBytes - logical < 16;
                if (!ok) {
                    ++grammarFailures;
                    std::cerr << "FAIL: " << tag << " violates the unit grammar\n";
                }
                ++psmSeen[rt::native::tools::packPsmLabel(unit.psm)];
            }
        }
    }
    check(grammarFailures == 0, "every unit holds qwc == 6 + NLOOP and the unit layout");
    check(psmSeen.size() == 5, "the corpus PSM set is exactly {CT32, CT24, CT16, T8, T4}");

    // --- 4. PSM census and texture->palette adjacency ------------------------
    {
        std::map<std::string, std::uint32_t> tex, clu, paired, orphanClut, padded;
        for (const PackFile& pack : packs) {
            for (const PackChain& chain : pack.chains) {
                for (std::size_t i = 0; i < chain.units.size(); ++i) {
                    const PackUnit& unit = chain.units[i];
                    const bool isClut = unit.clutShaped;
                    (isClut ? clu : tex)[pack.family]++;
                    if (unit.imageBytes > unit.logicalBytes) {
                        padded[pack.name]++;
                    }
                    if (isClut) {
                        const bool followsTexture =
                            i > 0 && rt::native::tools::packPsmIsIndexed(chain.units[i - 1].psm) &&
                            chain.units[i - 1].clutShaped == false &&
                            chain.units[i - 1].offset + chain.units[i - 1].byteSpan == unit.offset;
                        if (!followsTexture) {
                            orphanClut[pack.family]++;
                        }
                    } else if (i + 1 < chain.units.size() && chain.units[i + 1].clutShaped &&
                               chain.units[i + 1].offset == unit.offset + unit.byteSpan) {
                        paired[pack.family]++;
                    }
                }
            }
        }
        check(tex["SYS"] == 485 && clu["SYS"] == 483,
              "SYS: 485 texture uploads (440 T4 + 43 T8 + 2 CT24) and 483 palettes");
        check(paired["SYS"] == 483 && orphanClut["SYS"] == 0,
              "SYS: every palette sits right after its texture (no surplus palettes)");
        check(tex["ITEM"] == 87 && clu["ITEM"] == 87, "ITEM: 87 textures + 87 palettes, paired");
        check(paired["ITEM"] == 87 && orphanClut["ITEM"] == 0, "ITEM: 1:1 texture/palette pairing");
        check(tex["SHOP"] == 23 && clu["SHOP"] == 23, "SHOP: 23 textures + 23 palettes");
        check(tex["CAR"] == 151 && clu["CAR"] == 151, "CAR: 151 textures + 151 palettes");

        // The 2 surplus SYS uploads are the S00.GSL PSMCT24 icons: textures
        // without a following palette, outside the indexed census.
        std::uint32_t sysUnpairedTextures = tex["SYS"] - paired["SYS"];
        check(sysUnpairedTextures == 2, "SYS: exactly 2 surplus textures without a palette");
        const PackFile* s00 = findPack(packs, "SYS/S00.GSL");
        std::uint32_t s00Ct24 = 0;
        if (s00 != nullptr) {
            for (const PackChain& chain : s00->chains) {
                for (const PackUnit& unit : chain.units) {
                    if (unit.psm == 0x01 && unit.width == 32 && unit.height == 32) {
                        ++s00Ct24;
                    }
                }
            }
        }
        check(s00Ct24 == 2, "SYS/S00.GSL carries the 2 PSMCT24 32x32 surplus textures");

        // Quad-padded uploads: 4 PSMT4 units whose logical size is not a
        // multiple of 16 (SELECT 200x150; METER4 40x150 + 140x36; METER5
        // 28x55) - NLOOP rounds up, everything else is byte-exact.
        std::uint32_t paddedTotal = 0;
        for (const auto& entry : padded) {
            paddedTotal += entry.second;
        }
        check(paddedTotal == 4, "exactly 4 units need quadword padding (got " +
                                    std::to_string(paddedTotal) + ")");
        check(padded.count("SYS/SELECT.GSL") == 1 && padded.count("ITEM/METER4.GSL") == 1 &&
                  padded.count("ITEM/METER5.GSL") == 1,
              "the quad-padded units are SELECT 200x150, METER4 x2, METER5");
    }

    // --- 5. chain placement, FONTE pins, CAR pins ----------------------------
    for (const PackFile& pack : packs) {
        if (pack.family == "SHOP") {
            continue; // T-packs and GARAGE carry trailing payload by design
        }
        for (const PackChain& chain : pack.chains) {
            check(chain.trailingBytes == 0,
                  pack.name + " chain ends exactly at the span end");
            check(chain.chainOffset + chain.chainBytes == pack.fileBytes,
                  pack.name + " chain covers its whole span");
        }
    }
    {
        std::uint32_t tPacks = 0;
        for (const PackFile& pack : packs) {
            if (pack.family != "SHOP" || pack.name == "SHOP/GARAGE.BIN") {
                continue;
            }
            ++tPacks;
            check(pack.chains.size() == 1 && pack.chains[0].unitCount() == 2 &&
                      pack.chains[0].trailingBytes > 0,
                  pack.name + ": one chain of texture+palette plus trailing model payload");
            if (pack.chains.size() == 1 && pack.chains[0].unitCount() == 2) {
                const PackUnit& tex = pack.chains[0].units[0];
                const PackUnit& pal = pack.chains[0].units[1];
                check(tex.psm == 0x13 && tex.width == 640 && tex.height == 384,
                      pack.name + " texture is PSMT8 640x384");
                check(pal.psm == 0x00 && pal.width == 16 && pal.height == 16,
                      pack.name + " palette is PSMCT32 16x16");
            }
        }
        check(tPacks == 21, "21 SHOP T-packs");

        const PackFile* garage = findPack(packs, "SHOP/GARAGE.BIN");
        check(garage != nullptr && garage->valid(), "SHOP/GARAGE.BIN parses via its directory");
        if (garage != nullptr && garage->chains.size() == 1) {
            const PackChain& chain = garage->chains[0];
            check(chain.chainOffset == 0x19990, "GARAGE chain starts at directory boundary 0x19990");
            check(chain.chainOffset + chain.chainBytes == 0x52380,
                  "GARAGE chain ends exactly on the last directory boundary 0x52380");
            check(chain.chainOffset + chain.chainBytes < garage->fileBytes,
                  "GARAGE keeps model payload after the chain");
            check(chain.trailingBytes == 0,
                  "GARAGE chain ends exactly on its directory span end");
            check(chain.unitCount() == 4, "GARAGE carries 4 units (2 textures + 2 palettes)");
            if (chain.unitCount() == 4) {
                check(chain.units[0].psm == 0x13 && chain.units[0].width == 640 &&
                          chain.units[0].height == 256,
                      "GARAGE unit 0 is PSMT8 640x256");
                check(chain.units[1].clutShaped && chain.units[2].psm == 0x13 &&
                          chain.units[2].width == 512 && chain.units[2].height == 128,
                      "GARAGE units 1..2 are its palette + PSMT8 512x128");
            }
        }

        // FONTE, the doc's worked example (sys-gsl-packs.md sections 3-4).
        const PackFile* fonte = findPack(packs, "SYS/FONTE.GSL");
        check(fonte != nullptr && fonte->valid() && fonte->chains.size() == 1 &&
                  fonte->chains[0].unitCount() == 2,
              "SYS/FONTE.GSL is a 2-unit pack (atlas + CLUT)");
        if (fonte != nullptr && fonte->chains.size() == 1 && fonte->chains[0].unitCount() == 2) {
            const PackUnit& atlas = fonte->chains[0].units[0];
            const PackUnit& clut = fonte->chains[0].units[1];
            check(atlas.offset == 0 && atlas.psm == 0x14 && atlas.width == 512 &&
                      atlas.height == 384,
                  "FONTE unit 0 is the PSMT4 512x384 glyph atlas");
            check(atlas.dbp == 0, "FONTE atlas keeps DBP 0 (runtime-rebased category)");
            check(atlas.imageOffset == 0x70 && atlas.imageBytes == 98304,
                  "FONTE atlas image data sits at file 0x70, 98304 bytes");
            check(clut.offset == 0x18070 && clut.imageOffset == 0x180e0,
                  "FONTE CLUT upload sits at 0x18070 with image data at 0x180e0");
            check(clut.psm == 0x02 && clut.width == 16 && clut.height == 16,
                  "FONTE CLUT is PSMCT16 16x16 (256 entries)");
            check(clut.dbp == 0x180, "FONTE CLUT DBP 0x180 = past the texture's 0x180 pages");
        }

        // CAR containers: header word 6 selects the chain, word 7 is the size.
        std::uint32_t carHeaderFailures = 0;
        for (const PackFile& pack : packs) {
            if (pack.family != "CAR") {
                continue;
            }
            Bytes bytes;
            if (!readFileBytes((std::filesystem::path(root) / pack.name).string(), bytes) ||
                bytes.size() < 0x30 || pack.chains.size() != 1) {
                ++carHeaderFailures;
                continue;
            }
            const PackChain& chain = pack.chains[0];
            const std::uint32_t word6 = readU32LE(bytes, 0x18);
            const std::uint32_t word7 = readU32LE(bytes, 0x1c);
            if (chain.chainOffset != word6 || word6 % 16 != 0 || word7 != bytes.size()) {
                ++carHeaderFailures;
            }
            if (chain.unitCount() != 2) {
                ++carHeaderFailures;
                continue;
            }
            const PackUnit& tex = chain.units[0];
            const PackUnit& pal = chain.units[1];
            if (!(tex.psm == 0x13 && tex.width == 128 && tex.height == 128 && tex.dbp == 0 &&
                  pal.psm == 0x00 && pal.width == 16 && pal.height == 16 && pal.dbp == 64)) {
                ++carHeaderFailures;
            }
        }
        check(carHeaderFailures == 0,
              "every CAR container: chain at word 6, word 7 == file size, "
              "PSMT8 128x128 @DBP0 + CT32 16x16 @DBP64");
    }

    // --- 6. determinism: a second parse + serialization is byte-identical ----
    {
        std::vector<PackFile> second;
        if (!parseAllPacks(root, lbn, second, error)) {
            std::cerr << "test_packs_integration: " << error << '\n';
            return finish("setup broke before the determinism pass");
        }
        check(second.size() == packs.size(), "the second pass enumerates the same 222 packs");
        bool identical = second.size() == packs.size();
        for (std::size_t i = 0; identical && i < packs.size(); ++i) {
            identical = rt::native::tools::packFileToJson(second[i]) ==
                        rt::native::tools::packFileToJson(packs[i]);
        }
        check(identical, "two parse+serialize passes produce byte-identical pack catalogs");

        const std::string summaryA = rt::native::tools::packSummaryToJson(packs, root);
        const std::string summaryB = rt::native::tools::packSummaryToJson(second, root);
        check(summaryA == summaryB, "the summary JSON is byte-identical across passes");

        std::map<std::string, PackFamilySummary> famA, famB;
        rt::native::tools::summarizePackFiles(packs, famA);
        rt::native::tools::summarizePackFiles(second, famB);
        check(famA == famB, "the per-family summary aggregates identically across passes");
    }

    std::map<std::string, PackFamilySummary> families;
    rt::native::tools::summarizePackFiles(packs, families);
    std::uint32_t totalUnits = 0;
    for (const auto& entry : families) {
        totalUnits += entry.second.units;
    }
    return finish(std::to_string(totalUnits) + " units across " +
                  std::to_string(packs.size()) + " packs, catalogs deterministic");
}
