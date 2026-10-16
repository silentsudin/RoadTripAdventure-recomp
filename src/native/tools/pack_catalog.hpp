#pragma once

// src/native/tools/pack_catalog.hpp
//
// Shared pack parser + deterministic JSON writer for rt-pack-extract
// (tools/pack_extract_main.cpp) and its disc-backed integration test
// (tests/native_assets/integration/test_packs_integration.cpp). Header-only
// so both targets compile it directly from their registered sources.
//
// What a pack is (docs/native-reference/sys-gsl-packs.md section 1, the
// grammar byte-verified on all 1142 SYS+ITEM units; car-data-status.md
// texture chain, 302 units): a serialized PS2 DMA packet stream of
// texture-upload units
//
//   [DMA CNT tag 16B][tool junk 16B][GIF A+D quads 0x50..0x53 64B]
//   [GIF IMAGE tag 16B][NLOOP*16 bytes of pixel data]
//
// with the invariant qwc == 6 + NLOOP per unit, the whole chain terminated
// by a DMA END tag (byte 3 = 0x70) whose own (qwc+1)*16 trailing bytes close
// the span. Family layouts on top of that grammar:
//   SYS/*.GSL, ITEM/*.GSL  chain covers [0, EOF); END lands exactly at EOF.
//   CARn/Q*.BIN            12-word model header; the chain starts at header
//                          word 6 (u32 @ 0x18) and covers [word6, EOF).
//   SHOP/T*.BIN            chain starts at 0; non-chain payload follows the
//                          END tag inside the file.
//   SHOP/GARAGE.BIN        16-byte region directory (word 0 = directory byte
//                          size, then ascending region starts); the texture
//                          chain runs from the first to the last boundary and
//                          model payload follows it.
//
// Register field extraction mirrors the byte-level packing proven in
// docs/native-reference/field-textures.md ("the game packs BITBLTBUF as
// dbw<<48 | dbp<<32 | psm<<56 and TRXREG as h<<32 | w") and implemented by
// src/native/assets/image_converter.cpp (reused read-only as the reference
// for these conventions; its convertImage() itself cannot parse pack units,
// because a pack unit has the 16 junk bytes where that decoder requires a
// GIF A+D packet header). CLUT shaping uses image_converter's exact
// isClutShaped() predicate.
//
// No game data is embedded or committed: every function takes the user's own
// extracted disc bytes at run time, and the JSON output is derived purely
// from those bytes (no timestamps, no environment), so two runs over the
// same disc produce byte-identical files.

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
#include <span>
#include <string>
#include <vector>

namespace rt::native::tools {

// ---------------------------------------------------------------------------
// Constants (sys-gsl-packs.md section 1; image_converter.cpp conventions)
// ---------------------------------------------------------------------------

// GS pixel storage modes carried by the pack corpus (field-textures.md keeps
// psm raw; the proven set across SYS+ITEM+SHOP+CAR is exactly these five).
inline constexpr std::uint32_t kPsmCt32 = 0x00;
inline constexpr std::uint32_t kPsmCt24 = 0x01;
inline constexpr std::uint32_t kPsmCt16 = 0x02;
inline constexpr std::uint32_t kPsmCt16S = 0x0A;
inline constexpr std::uint32_t kPsmT8 = 0x13;
inline constexpr std::uint32_t kPsmT4 = 0x14;
inline constexpr std::uint32_t kPsmT8H = 0x1B;
inline constexpr std::uint32_t kPsmT4Hl = 0x24;
inline constexpr std::uint32_t kPsmT4Hh = 0x2C;

// DMA tag identity byte (byte 3 of the tag qword): 0x10 CNT, 0x70 END; the
// game's own loader fatals on anything else ("GSL DMA Tag Error!").
inline constexpr std::uint8_t kDmaTagCnt = 0x10;
inline constexpr std::uint8_t kDmaTagEnd = 0x70;

// Unit layout: tag(16) + junk(16) + four A+D quads(64) + IMAGE tag(16) = 112
// bytes before the pixel data, i.e. qwc = 6 + NLOOP quadwords per unit.
inline constexpr std::uint64_t kPackUnitHeaderBytes = 112;

// Walk cap, mirroring image_converter.cpp's kMaxChainTags defence in depth.
inline constexpr int kMaxPackChainTags = 65536;

// Families this tool covers, in output order.
inline constexpr const char* kPackFamilies[] = {"SYS", "ITEM", "SHOP", "CAR"};

// CD-ROM sector size; .lbn_map.tsv's third column is a byte count (verified
// equal to the real file size on every disc file), so sectors are computed.
inline constexpr std::uint64_t kPackSectorBytes = 2048;

// GIF A+D register addresses of the upload packet (Extract.cpp conventions).
inline constexpr std::uint8_t kRegBlitBuf = 0x50;     // BITBLTBUF
inline constexpr std::uint8_t kRegTransferPos = 0x51; // TRXPOS
inline constexpr std::uint8_t kRegTransferReg = 0x52; // TRXREG
inline constexpr std::uint8_t kRegTransferDir = 0x53; // TRXDIR

// ---------------------------------------------------------------------------
// Little-endian readers and bitfields
// ---------------------------------------------------------------------------

inline std::uint64_t packReadU64Le(std::span<const std::uint8_t> b, std::uint64_t o) {
    std::uint64_t v = 0;
    for (int i = 7; i >= 0; --i) {
        v = (v << 8) | static_cast<std::uint64_t>(b[static_cast<std::size_t>(o) + static_cast<std::size_t>(i)]);
    }
    return v;
}

inline std::uint32_t packReadU32Le(std::span<const std::uint8_t> b, std::uint64_t o) {
    return static_cast<std::uint32_t>(b[static_cast<std::size_t>(o)]) |
           (static_cast<std::uint32_t>(b[static_cast<std::size_t>(o) + 1]) << 8) |
           (static_cast<std::uint32_t>(b[static_cast<std::size_t>(o) + 2]) << 16) |
           (static_cast<std::uint32_t>(b[static_cast<std::size_t>(o) + 3]) << 24);
}

inline std::uint16_t packReadU16Le(std::span<const std::uint8_t> b, std::uint64_t o) {
    return static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(b[static_cast<std::size_t>(o)]) |
        (static_cast<std::uint16_t>(b[static_cast<std::size_t>(o) + 1]) << 8));
}

inline std::uint64_t packField(std::uint64_t v, unsigned lo, unsigned n) {
    return (v >> lo) & ((1ull << n) - 1ull);
}

// "0x1bec0": compact hex for error messages (never parsed back).
inline std::string packHex(std::uint64_t v) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "0x%llx", static_cast<unsigned long long>(v));
    return buf;
}

// ---------------------------------------------------------------------------
// PSM helpers (mirror image_converter.cpp)
// ---------------------------------------------------------------------------

// Canonical short name, or nullptr for a mode outside the decoder's set.
inline const char* packPsmName(std::uint32_t psm) {
    switch (psm) {
        case kPsmCt32: return "PSMCT32";
        case kPsmCt24: return "PSMCT24";
        case kPsmCt16: return "PSMCT16";
        case kPsmCt16S: return "PSMCT16S";
        case kPsmT8: return "PSMT8";
        case kPsmT4: return "PSMT4";
        case kPsmT8H: return "PSMT8H";
        case kPsmT4Hl: return "PSMT4HL";
        case kPsmT4Hh: return "PSMT4HH";
    }
    return nullptr;
}

inline std::uint32_t packPsmBitsPerPixel(std::uint32_t psm) {
    switch (psm) {
        case kPsmCt32: return 32;
        case kPsmCt24: return 24;
        case kPsmCt16:
        case kPsmCt16S: return 16;
        case kPsmT8:
        case kPsmT8H: return 8;
        case kPsmT4:
        case kPsmT4Hl:
        case kPsmT4Hh: return 4;
    }
    return 0;
}

inline bool packPsmIsIndexed(std::uint32_t psm) {
    return psm == kPsmT8 || psm == kPsmT8H || psm == kPsmT4 || psm == kPsmT4Hl ||
           psm == kPsmT4Hh;
}

// A palette-shaped upload, exactly image_converter.cpp's isClutShaped()
// (Extract.cpp:208-211): 32-bit or 16-bit colour at 8x2/16x2/16x16-ish sizes.
inline bool packIsClutShaped(std::uint32_t psm, std::uint32_t w, std::uint32_t h) {
    return (psm == kPsmCt32 || psm == kPsmCt16 || psm == kPsmCt16S) && w <= 16 &&
           (h <= 2 || (w == 16 && h == 16) || (w == 8 && h == 8));
}

// "PSMT4" / "PSM_0x1C": label used in JSON and histogram keys.
inline std::string packPsmLabel(std::uint32_t psm) {
    if (const char* name = packPsmName(psm)) {
        return name;
    }
    char buf[16];
    std::snprintf(buf, sizeof(buf), "PSM_0x%02X", psm);
    return buf;
}

// ---------------------------------------------------------------------------
// Parsed pack model
// ---------------------------------------------------------------------------

// One validated texture-upload unit (one CNT tag + its inline GIF payload).
struct PackUnit {
    std::uint64_t offset = 0;      // unit start = the CNT tag, file-relative
    std::uint64_t byteSpan = 0;    // (qwc+1)*16: tag, payload and all
    std::uint32_t qwc = 0;         // DMA tag quadword count
    std::uint32_t nloop = 0;       // GIF IMAGE tag NLOOP (pixel quadwords)
    std::uint32_t width = 0;       // TRXREG RRW
    std::uint32_t height = 0;      // TRXREG RRH
    std::uint32_t psm = 0;         // BITBLTBUF DPSM (raw GS value)
    std::uint32_t dbp = 0;         // BITBLTBUF DBP (destination VRAM page)
    std::uint32_t dbw = 0;         // BITBLTBUF DBW
    std::uint32_t dsax = 0;        // TRXPOS DSAX (0 on the whole corpus)
    std::uint32_t dsay = 0;        // TRXPOS DSAY (0 on the whole corpus)
    std::uint64_t imageOffset = 0; // pixel data start (offset + 112)
    std::uint64_t imageBytes = 0;  // NLOOP*16 (quadword-padded span)
    std::uint64_t logicalBytes = 0;// ceil(width*height*bpp/8); 0 when bpp unknown
    bool clutShaped = false;       // palette-shaped upload (isClutShaped)
};

// One CNT..END chain: the unit sequence plus where it ends inside its span.
struct PackChain {
    std::uint64_t chainOffset = 0;   // first CNT tag, file-relative
    std::uint64_t chainBytes = 0;    // chainOffset -> past the END tag trailer
    std::uint64_t trailingBytes = 0; // span end - chain end (non-chain payload)
    std::vector<PackUnit> units;
    std::string error;               // empty when the whole chain validated

    std::uint32_t unitCount() const { return static_cast<std::uint32_t>(units.size()); }
};

// One pack file (a .GSL pack, a SHOP pack, or a self-textured CAR container).
struct PackFile {
    std::string family;      // "SYS" / "ITEM" / "SHOP" / "CAR"
    std::string name;        // disc-relative path, e.g. "SYS/FONTE.GSL"
    std::uint64_t fileBytes = 0;
    std::uint64_t lba = 0;       // from .lbn_map.tsv
    bool lbaKnown = false;
    std::uint64_t mapBytes = 0;  // .lbn_map.tsv byte-size column
    std::vector<PackChain> chains;
    std::string error;           // empty when every unit validated

    std::uint32_t unitCount() const {
        std::uint32_t n = 0;
        for (const PackChain& c : chains) {
            n += c.unitCount();
        }
        return n;
    }
    std::uint32_t clutUnitCount() const {
        std::uint32_t n = 0;
        for (const PackChain& c : chains) {
            for (const PackUnit& u : c.units) {
                if (u.clutShaped) {
                    ++n;
                }
            }
        }
        return n;
    }
    std::uint32_t textureUnitCount() const { return unitCount() - clutUnitCount(); }
    bool valid() const {
        if (!error.empty()) {
            return false;
        }
        for (const PackChain& c : chains) {
            if (!c.error.empty()) {
                return false;
            }
        }
        return true;
    }
};

// ---------------------------------------------------------------------------
// Chain parse (sys-gsl-packs.md section 1, the game's own validation)
// ---------------------------------------------------------------------------

// Walks one CNT..END chain inside [start, spanEnd) and validates every unit:
// CNT/END tag bytes, the four A+D register addresses 0x50..0x53 in order,
// TRXDIR == 0, IMAGE flg == 2, and the qwc == 6 + NLOOP invariant. On error
// the units parsed so far are kept and chain.error names the offset.
inline PackChain parsePackChain(std::span<const std::uint8_t> bytes, std::uint64_t start,
                                std::uint64_t spanEnd) {
    PackChain chain;
    chain.chainOffset = start;
    if (start > spanEnd || spanEnd > bytes.size()) {
        chain.error = "chain span outside the file";
        return chain;
    }
    if (start == spanEnd) {
        chain.error = "empty chain span";
        return chain;
    }

    std::uint64_t cur = start;
    for (int tagIndex = 0; tagIndex < kMaxPackChainTags; ++tagIndex) {
        if (cur + 16 > spanEnd) {
            chain.error = "truncated DMA tag at " + packHex(cur);
            return chain;
        }
        const std::uint8_t idByte = bytes[static_cast<std::size_t>(cur) + 3];
        const std::uint32_t qwc = packReadU16Le(bytes, cur);
        if (idByte == kDmaTagEnd) {
            const std::uint64_t tail = cur + (static_cast<std::uint64_t>(qwc) + 1) * 16;
            if (tail > spanEnd) {
                chain.error = "END tag at " + packHex(cur) + " overruns the span";
                return chain;
            }
            chain.chainBytes = tail - start;
            chain.trailingBytes = spanEnd - tail;
            return chain;
        }
        if (idByte != kDmaTagCnt) {
            chain.error = "unknown DMA tag id byte " + packHex(idByte) + " at " + packHex(cur);
            return chain;
        }
        if (qwc < 6) {
            chain.error = "unit at " + packHex(cur) + " has qwc " + std::to_string(qwc) +
                          " below the 6-quadword minimum";
            return chain;
        }
        const std::uint64_t stride = (static_cast<std::uint64_t>(qwc) + 1) * 16;
        if (cur + stride > spanEnd) {
            chain.error = "unit at " + packHex(cur) + " payload runs past the span end";
            return chain;
        }

        // Four GIF A+D quads {u64 value, u64 addr}: regs 0x50..0x53 in order.
        std::uint64_t value[4] = {};
        for (std::uint32_t k = 0; k < 4; ++k) {
            const std::uint64_t quad = cur + 32 + static_cast<std::uint64_t>(k) * 16;
            value[k] = packReadU64Le(bytes, quad);
            const std::uint64_t addr = packReadU64Le(bytes, quad + 8) & 0xFF;
            if (addr != kRegBlitBuf + k) {
                chain.error = "unit at " + packHex(cur) + " A+D quad " + std::to_string(k) +
                              " writes register " + packHex(addr) + ", expected " +
                              packHex(kRegBlitBuf + k);
                return chain;
            }
        }
        if ((value[3] & 3) != 0) {
            chain.error = "unit at " + packHex(cur) + " has TRXDIR != 0";
            return chain;
        }

        // GIF IMAGE-mode tag carrying the pixel quadword count.
        const std::uint64_t imageTag = packReadU64Le(bytes, cur + 96);
        if (packField(imageTag, 58, 2) != 2) {
            chain.error = "unit at " + packHex(cur) + " IMAGE tag flg != 2";
            return chain;
        }
        const std::uint32_t nloop = static_cast<std::uint32_t>(imageTag & 0x7FFF);
        if (qwc != 6 + nloop) {
            chain.error = "unit at " + packHex(cur) + " violates qwc == 6 + NLOOP (qwc " +
                          std::to_string(qwc) + ", NLOOP " + std::to_string(nloop) + ")";
            return chain;
        }

        // Register fields, in the game's byte-level packing (field-textures.md):
        // BITBLTBUF = dbw<<48 | dbp<<32 | psm<<56 (low 32 bits are tool junk),
        // TRXREG = h<<32 | w, TRXPOS DSAX/DSAY at bits 32..42/48..58.
        const std::uint64_t blitBuf = value[0];
        const std::uint64_t transferPos = value[1];
        const std::uint64_t transferReg = value[2];

        PackUnit unit;
        unit.offset = cur;
        unit.byteSpan = stride;
        unit.qwc = qwc;
        unit.nloop = nloop;
        unit.psm = static_cast<std::uint32_t>(packField(blitBuf, 56, 6));
        unit.dbp = static_cast<std::uint32_t>(packField(blitBuf, 32, 14));
        unit.dbw = static_cast<std::uint32_t>(packField(blitBuf, 48, 6));
        unit.dsax = static_cast<std::uint32_t>(packField(transferPos, 32, 11));
        unit.dsay = static_cast<std::uint32_t>(packField(transferPos, 48, 11));
        unit.width = static_cast<std::uint32_t>(packField(transferReg, 0, 12));
        unit.height = static_cast<std::uint32_t>(packField(transferReg, 32, 12));
        unit.imageOffset = cur + kPackUnitHeaderBytes;
        unit.imageBytes = static_cast<std::uint64_t>(nloop) * 16;
        const std::uint32_t bpp = packPsmBitsPerPixel(unit.psm);
        if (bpp != 0 && unit.width != 0 && unit.height != 0) {
            const std::uint64_t texels = static_cast<std::uint64_t>(unit.width) * unit.height;
            unit.logicalBytes = (texels * bpp + 7) / 8;
        }
        unit.clutShaped = packIsClutShaped(unit.psm, unit.width, unit.height);
        chain.units.push_back(unit);
        cur += stride;
    }
    chain.error = "no END tag within the walk cap";
    return chain;
}

// ---------------------------------------------------------------------------
// Pack-file layout per family
// ---------------------------------------------------------------------------

// Parses one pack file according to its family layout (see the header
// comment). Chains that failed keep their partial units and error text; the
// first failure is also copied into file.error.
inline void parsePackFile(PackFile& file, const std::vector<std::uint8_t>& bytes) {
    file.fileBytes = bytes.size();
    file.chains.clear();
    file.error.clear();
    const std::span<const std::uint8_t> span(bytes.data(), bytes.size());

    // The chain must end exactly at spanEnd (zero trailing bytes); used where
    // the corpus proves the chain covers its whole span.
    const auto requireNoTrailing = [&file](const PackChain& chain, std::uint64_t spanEnd) {
        if (chain.error.empty() && chain.trailingBytes != 0) {
            file.error = "chain in '" + file.name + "' ends at " +
                         packHex(chain.chainOffset + chain.chainBytes) + " but the span runs to " +
                         packHex(spanEnd);
        }
    };

    if (file.family == "CAR") {
        // 12-word model header; the texture chain starts at header word 6.
        if (bytes.size() < 0x30) {
            file.error = "CAR container shorter than the 12-word header";
            return;
        }
        const std::uint64_t chainStart = packReadU32Le(span, 0x18);
        if (chainStart < 0x30 || chainStart % 16 != 0 || chainStart >= bytes.size()) {
            file.error = "CAR container word 6 (" + packHex(chainStart) +
                         ") is not a valid texture-chain start";
            return;
        }
        PackChain chain = parsePackChain(span, chainStart, bytes.size());
        requireNoTrailing(chain, bytes.size());
        if (file.error.empty() && !chain.error.empty()) {
            file.error = chain.error;
        }
        file.chains.push_back(std::move(chain));
        return;
    }

    if (file.family == "SHOP") {
        // SHOP/GARAGE.BIN prefixes its chain with a region directory: word 0
        // is a byte size, the following nonzero words are ascending region
        // starts, and the texture chain runs from the first to the last one
        // (END must land exactly on the last boundary). Every other SHOP pack
        // starts its chain at 0 and carries trailing model payload after the
        // END tag.
        const std::uint64_t word0 = bytes.size() >= 4 ? packReadU32Le(span, 0) : 0;
        const bool plausibleDirectory =
            word0 % 4 == 0 && word0 >= 8 && word0 <= 4096 && word0 < bytes.size();
        if (plausibleDirectory) {
            const std::size_t words = static_cast<std::size_t>(word0) / 4;
            std::uint64_t first = 0;
            std::uint64_t last = 0;
            bool ascending = true;
            std::uint64_t prev = 0;
            bool any = false;
            for (std::size_t k = 1; k < words; ++k) {
                const std::uint64_t boundary = packReadU32Le(span, k * 4);
                if (boundary == 0) {
                    continue;
                }
                if (any && boundary <= prev) {
                    ascending = false;
                }
                if (!any) {
                    first = boundary;
                }
                last = boundary;
                prev = boundary;
                any = true;
            }
            if (any && ascending && first % 16 == 0 && first + 16 <= last && last <= bytes.size() &&
                first + 16 <= bytes.size() && bytes[static_cast<std::size_t>(first) + 3] == kDmaTagCnt) {
                PackChain chain = parsePackChain(span, first, last);
                // The directory promise: END exactly on the last boundary.
                if (chain.error.empty() && chain.trailingBytes != 0) {
                    chain.error = "directory chain ends at " +
                                  packHex(chain.chainOffset + chain.chainBytes) +
                                  ", not on the last boundary " + packHex(last);
                }
                if (chain.error.empty()) {
                    file.chains.push_back(std::move(chain));
                    return;
                }
                // Fall through to the plain-chain reading on any surprise.
            }
        }
        PackChain chain = parsePackChain(span, 0, bytes.size());
        if (file.error.empty() && !chain.error.empty()) {
            file.error = chain.error;
        }
        file.chains.push_back(std::move(chain));
        return;
    }

    // SYS/*.GSL and ITEM/*.GSL: the chain is the whole file.
    PackChain chain = parsePackChain(span, 0, bytes.size());
    requireNoTrailing(chain, bytes.size());
    if (file.error.empty() && !chain.error.empty()) {
        file.error = chain.error;
    }
    file.chains.push_back(std::move(chain));
}

// ---------------------------------------------------------------------------
// .lbn_map.tsv (name \t lba \t byte-size; third column proven == file size)
// ---------------------------------------------------------------------------

struct PackLbnEntry {
    std::uint64_t lba = 0;
    std::uint64_t bytes = 0;
};

// Reads the disc's LBA map. Returns false (with error set) when the file
// cannot be opened; malformed lines are skipped.
inline bool loadPackLbnMap(const std::string& tsvPath,
                           std::map<std::string, PackLbnEntry>& out, std::string& error) {
    out.clear();
    std::ifstream file(tsvPath);
    if (!file) {
        error = "cannot open '" + tsvPath + "'";
        return false;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        const std::size_t tab1 = line.find('\t');
        if (tab1 == std::string::npos) {
            continue;
        }
        const std::size_t tab2 = line.find('\t', tab1 + 1);
        if (tab2 == std::string::npos) {
            continue;
        }
        const std::string name = line.substr(0, tab1);
        const std::string lba = line.substr(tab1 + 1, tab2 - tab1 - 1);
        const std::string size = line.substr(tab2 + 1);
        PackLbnEntry entry;
        entry.lba = std::strtoull(lba.c_str(), nullptr, 10);
        entry.bytes = std::strtoull(size.c_str(), nullptr, 10);
        out[name] = entry;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Enumeration (fixed order, byte-wise name sort: deterministic output)
// ---------------------------------------------------------------------------

inline void packCollectFiles(const std::filesystem::path& dir, const char* extension,
                             const char* prefix, std::vector<std::string>& out) {
    std::error_code ec;
    std::filesystem::directory_iterator it(dir, ec);
    if (ec) {
        return;
    }
    const std::size_t extensionLen = std::char_traits<char>::length(extension);
    const std::size_t prefixLen = std::char_traits<char>::length(prefix);
    for (const std::filesystem::directory_entry& entry : it) {
        std::error_code fileEc;
        if (!entry.is_regular_file(fileEc)) {
            continue;
        }
        const std::string name = entry.path().filename().string();
        if (name.size() <= extensionLen + prefixLen ||
            name.compare(name.size() - extensionLen, extensionLen, extension) != 0 ||
            name.compare(0, prefixLen, prefix) != 0) {
            continue;
        }
        out.push_back(name);
    }
}

// Lists one family's pack files as disc-relative names ("SYS/FONT.GSL",
// "CAR0/Q00.BIN"), sorted. Returns false (error set) when a family directory
// is missing or yields no packs: the disc corpus is fully known, so either is
// a loud setup failure, not an empty result.
inline bool enumeratePackFiles(const std::string& discRoot, const std::string& family,
                               std::vector<std::string>& out, std::string& error) {
    out.clear();
    const std::filesystem::path root(discRoot);
    if (family == "CAR") {
        const char* carDirs[] = {"CAR0", "CAR1", "CAR2", "CAR3", "CAR4", "CARS"};
        for (const char* dir : carDirs) {
            const std::filesystem::path path = root / dir;
            std::error_code ec;
            if (!std::filesystem::is_directory(path, ec)) {
                error = "missing CAR directory '" + path.string() + "'";
                return false;
            }
            const std::size_t before = out.size();
            packCollectFiles(path, ".BIN", "Q", out); // self-textured Q*.BIN packs
            if (out.size() == before) {
                error = "no self-textured Q*.BIN packs in '" + path.string() + "'";
                return false;
            }
            // Keep the on-disc subdirectory in the disc-relative name.
            for (std::size_t i = before; i < out.size(); ++i) {
                out[i] = std::string(dir) + "/" + out[i];
            }
        }
    } else {
        const std::filesystem::path path = root / family;
        std::error_code ec;
        if (!std::filesystem::is_directory(path, ec)) {
            error = "missing pack directory '" + path.string() + "'";
            return false;
        }
        if (family == "SHOP") {
            packCollectFiles(path, ".BIN", "", out); // every .BIN pack
        } else {
            packCollectFiles(path, ".GSL", "", out); // .GSL packs
        }
        if (out.empty()) {
            error = "no packs found in '" + path.string() + "'";
            return false;
        }
    }
    std::sort(out.begin(), out.end());
    for (std::string& name : out) {
        if (family != "CAR" && name.find('/') == std::string::npos) {
            name = family + "/" + name;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// Deterministic JSON output (hand-rolled; integers only, keys in fixed or
// sorted order, so two runs over the same disc are byte-identical)
// ---------------------------------------------------------------------------

inline std::string packJsonEscape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    char buf[8];
                    std::snprintf(buf, sizeof(buf), "\\u%04X",
                                  static_cast<unsigned>(static_cast<unsigned char>(c)));
                    out += buf;
                } else {
                    out += c;
                }
        }
    }
    return out;
}

// One unit object. A non-palette unit immediately followed by a palette unit
// carries the paired "clut" descriptor (the palette's image data offset and
// format); a palette unit carries its own entry count.
inline std::string packUnitToJson(const PackUnit& unit, std::uint32_t index, const PackUnit* next) {
    std::string j = "          {\n";
    auto num = [&j](const char* key, std::uint64_t v, bool comma = true) {
        j += "            \"";
        j += key;
        j += "\": ";
        j += std::to_string(v);
        if (comma) {
            j += ',';
        }
        j += '\n';
    };
    num("offset", unit.offset);
    num("byteSpan", unit.byteSpan);
    num("qwc", unit.qwc);
    num("nloop", unit.nloop);
    num("width", unit.width);
    num("height", unit.height);
    j += "            \"psm\": \"" + packJsonEscape(packPsmLabel(unit.psm)) + "\",\n";
    num("psmValue", unit.psm);
    num("dbp", unit.dbp);
    num("dbw", unit.dbw);
    num("dsax", unit.dsax);
    num("dsay", unit.dsay);
    num("imageOffset", unit.imageOffset);
    num("imageBytes", unit.imageBytes);
    num("logicalBytes", unit.logicalBytes);
    j += "            \"clutShaped\": ";
    j += unit.clutShaped ? "true" : "false";
    j += ",\n";
    if (unit.clutShaped) {
        num("entries", static_cast<std::uint64_t>(unit.width) * unit.height, false);
    } else if (next != nullptr && next->clutShaped && next->offset == unit.offset + unit.byteSpan) {
        j += "            \"clut\": {\n";
        j += "              \"unit\": " + std::to_string(index + 1) + ",\n";
        j += "              \"offset\": " + std::to_string(next->imageOffset) + ",\n";
        j += "              \"psm\": \"" + packJsonEscape(packPsmLabel(next->psm)) + "\",\n";
        j += "              \"entries\": " +
             std::to_string(static_cast<std::uint64_t>(next->width) * next->height) + "\n";
        j += "            }\n";
    } else {
        // No palette follows this upload (the corpus's only such textures are
        // SYS/S00.GSL's two PSMCT24 icons); drop the trailing comma.
        std::string trimmed = j.substr(0, j.size() - 2);
        trimmed += "\n";
        j = std::move(trimmed);
        j += "          }";
        return j;
    }
    j += "          }";
    return j;
}

inline std::string packChainToJson(const PackChain& chain, std::uint32_t index) {
    std::string j = "        {\n";
    j += "          \"index\": " + std::to_string(index) + ",\n";
    j += "          \"chainOffset\": " + std::to_string(chain.chainOffset) + ",\n";
    j += "          \"chainBytes\": " + std::to_string(chain.chainBytes) + ",\n";
    j += "          \"trailingBytes\": " + std::to_string(chain.trailingBytes) + ",\n";
    j += "          \"units\": " + std::to_string(chain.unitCount());
    if (!chain.error.empty()) {
        j += ",\n          \"error\": \"" + packJsonEscape(chain.error) + "\"";
    }
    j += chain.units.empty() ? "\n" : ",\n";
    if (!chain.units.empty()) {
        j += "          \"unitList\": [\n";
        for (std::size_t i = 0; i < chain.units.size(); ++i) {
            const PackUnit* next = i + 1 < chain.units.size() ? &chain.units[i + 1] : nullptr;
            j += packUnitToJson(chain.units[i], static_cast<std::uint32_t>(i), next);
            j += (i + 1 < chain.units.size()) ? ",\n" : "\n";
        }
        j += "          ]\n";
    }
    j += "        }";
    return j;
}

inline std::string packFileToJson(const PackFile& file) {
    std::string j = "{\n";
    j += "  \"family\": \"" + packJsonEscape(file.family) + "\",\n";
    j += "  \"pack\": \"" +
         packJsonEscape(file.name.substr(file.name.find('/') + 1)) + "\",\n";
    j += "  \"file\": \"" + packJsonEscape(file.name) + "\",\n";
    j += "  \"lba\": " + (file.lbaKnown ? std::to_string(file.lba) : std::string("null")) + ",\n";
    j += "  \"mapBytes\": " + std::to_string(file.mapBytes) + ",\n";
    j += "  \"sectors\": " +
         std::to_string((file.mapBytes + kPackSectorBytes - 1) / kPackSectorBytes) + ",\n";
    j += "  \"fileBytes\": " + std::to_string(file.fileBytes) + ",\n";
    j += "  \"unitCount\": " + std::to_string(file.unitCount()) + ",\n";
    j += "  \"textureUnits\": " + std::to_string(file.textureUnitCount()) + ",\n";
    j += "  \"clutUnits\": " + std::to_string(file.clutUnitCount()) + ",\n";
    j += "  \"valid\": " + std::string(file.valid() ? "true" : "false");
    if (!file.error.empty()) {
        j += ",\n  \"error\": \"" + packJsonEscape(file.error) + "\"";
    }
    j += file.chains.empty() ? "\n" : ",\n";
    if (!file.chains.empty()) {
        j += "  \"chains\": [\n";
        for (std::size_t i = 0; i < file.chains.size(); ++i) {
            j += packChainToJson(file.chains[i], static_cast<std::uint32_t>(i));
            j += (i + 1 < file.chains.size()) ? ",\n" : "\n";
        }
        j += "  ]\n";
    }
    j += "}\n";
    return j;
}

// Per-family aggregation used by the summary JSON and the integration test.
struct PackFamilySummary {
    std::uint32_t packs = 0;
    std::uint32_t units = 0;
    std::uint32_t textureUnits = 0;
    std::uint32_t clutUnits = 0;
    std::map<std::string, std::uint64_t> dimensions; // "PSMT4 64x64" -> count

    bool operator==(const PackFamilySummary&) const = default;
};

inline std::string packDimensionsToJson(const std::map<std::string, std::uint64_t>& dims,
                                        int indent) {
    const std::string pad(static_cast<std::size_t>(indent), ' ');
    std::string j = "{";
    bool first = true;
    for (const auto& entry : dims) {
        if (!first) {
            j += ',';
        }
        first = false;
        j += '\n';
        j += pad;
        j += "\"" + packJsonEscape(entry.first) + "\": " + std::to_string(entry.second);
    }
    if (!dims.empty()) {
        j += '\n';
        j += pad;
    }
    j += "}";
    return j;
}

inline void summarizePackFiles(const std::vector<PackFile>& files,
                               std::map<std::string, PackFamilySummary>& families) {
    families.clear();
    for (const char* fam : kPackFamilies) {
        families[fam]; // keep the fixed family order in the summary
    }
    for (const PackFile& file : files) {
        PackFamilySummary& summary = families[file.family];
        ++summary.packs;
        for (const PackChain& chain : file.chains) {
            for (const PackUnit& unit : chain.units) {
                ++summary.units;
                if (unit.clutShaped) {
                    ++summary.clutUnits;
                } else {
                    ++summary.textureUnits;
                }
                ++summary.dimensions[packPsmLabel(unit.psm) + " " + std::to_string(unit.width) +
                                     "x" + std::to_string(unit.height)];
            }
        }
    }
}

inline std::string packSummaryToJson(const std::vector<PackFile>& files,
                                     const std::string& discRoot) {
    std::map<std::string, PackFamilySummary> families;
    summarizePackFiles(files, families);

    std::uint32_t packs = 0;
    std::uint32_t units = 0;
    std::uint32_t textures = 0;
    std::uint32_t cluts = 0;
    bool allValid = true;
    std::map<std::string, std::uint64_t> allDimensions;
    std::vector<const PackFile*> errors;
    for (const PackFile& file : files) {
        ++packs;
        units += file.unitCount();
        textures += file.textureUnitCount();
        cluts += file.clutUnitCount();
        if (!file.valid()) {
            allValid = false;
            errors.push_back(&file);
        }
    }
    for (const auto& fam : families) {
        for (const auto& dim : fam.second.dimensions) {
            allDimensions[dim.first] += dim.second;
        }
    }

    std::string j = "{\n";
    j += "  \"tool\": \"rt-pack-extract\",\n";
    j += "  \"discRoot\": \"" + packJsonEscape(discRoot) + "\",\n";
    j += "  \"packs\": " + std::to_string(packs) + ",\n";
    j += "  \"units\": " + std::to_string(units) + ",\n";
    j += "  \"textureUnits\": " + std::to_string(textures) + ",\n";
    j += "  \"clutUnits\": " + std::to_string(cluts) + ",\n";
    j += "  \"allUnitsValid\": " + std::string(allValid ? "true" : "false") + ",\n";
    j += "  \"families\": {\n";
    bool firstFam = true;
    for (const char* famName : kPackFamilies) {
        const auto famIt = families.find(famName);
        if (famIt == families.end()) {
            continue;
        }
        if (!firstFam) {
            j += ",\n";
        }
        firstFam = false;
        j += "    \"" + packJsonEscape(famIt->first) + "\": {\n";
        j += "      \"packs\": " + std::to_string(famIt->second.packs) + ",\n";
        j += "      \"units\": " + std::to_string(famIt->second.units) + ",\n";
        j += "      \"textureUnits\": " + std::to_string(famIt->second.textureUnits) + ",\n";
        j += "      \"clutUnits\": " + std::to_string(famIt->second.clutUnits) + ",\n";
        j += "      \"dimensions\": " + packDimensionsToJson(famIt->second.dimensions, 8) + "\n";
        j += "    }";
    }
    j += "\n  },\n";
    j += "  \"dimensions\": " + packDimensionsToJson(allDimensions, 4) + ",\n";
    j += "  \"errors\": [";
    for (std::size_t i = 0; i < errors.size(); ++i) {
        j += (i == 0 ? "\n" : ",\n");
        j += "    {\"file\": \"" + packJsonEscape(errors[i]->name) + "\", \"error\": \"" +
             packJsonEscape(errors[i]->error) + "\"}";
    }
    j += errors.empty() ? "]\n" : "\n  ]\n";
    j += "}\n";
    return j;
}

} // namespace rt::native::tools
