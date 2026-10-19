// W12 host services, wave 4 (libcdvd). Reviewed against the oracle by the
// coordinator (return-value table spot-checked at CD.cpp:375-467,996; LBA
// claims table-matched against the extraction sidecar).

// Mirrors the oracle's ps2_stubs CD family (Kernel/Stubs/CD.cpp + its
// Helpers/Support.h disc reader) for the 13 CD entries in
// config/native_host_stubs.txt. Semantics are the oracle's exactly; the
// disc SOURCE differs (native extracted tree + optional raw image, see the
// substrate helper below). Nothing here is compiled from ps2xRuntime
// (decomp hard rule).
//
// The oracle never posts a CDVD INTC nor signals the game's semaphores: it
// makes sceCdRead copy sectors synchronously (return 1) and makes
// sceCdSync/sceCdSyncS return 0 (not busy). The game's own poll loop lives
// in the game code the oracle REPLACES (sceCdSync 0x278740 / sceCdSyncS
// 0x2787e0 bodies spin on DAT_002e6f30 + sceSifCheckStatRpc). See
// REPORT.md "Completion mechanism".
#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <cstdlib>
#include "guest_shim.h"
#include "guest_kernel.h"
#include "guest_decls.inc"

namespace guest {

// Unique-per-file registration (RT_GUEST_FN's struct name would collide).
#define RT_STUB_FN(addr, name)                                       \
    namespace { struct RtReg_##addr { RtReg_##addr() {               \
        rtguest::registerGuest(addr,                                 \
            reinterpret_cast<void*>(&name)); } } rt_reg_##addr##_; }

namespace {

constexpr uint32_t kCdSectorSize = 2048;
constexpr uint32_t kCdPseudoLbnStart = 0x00100000u;  // oracle Support.h:7

// ---- RAM access (oracle getMemPtr: null outside physical RAM) ----
void* mem(uint32_t a) {
    const uint32_t p = a & 0x01FFFFFFu;
    return p < RT_GUEST_RAM_SIZE ? rtguest::kRam + p : nullptr;
}
uint32_t rdW(uint32_t a) {
    uint32_t v = 0;
    std::memcpy(&v, rtguest::kRam + (a & 0x01FFFFFFu), 4);
    return v;
}
void wrW(uint32_t a, uint32_t v) {
    std::memcpy(rtguest::kRam + (a & 0x01FFFFFFu), &v, 4);
}
std::string cstrGuest(uint32_t a, uint32_t cap) {
    if (!mem(a)) return {};
    const uint32_t p = a & 0x01FFFFFFu;
    std::string s;
    for (uint32_t i = 0; i < cap && rtguest::kRam[p + i]; ++i) s += (char)rtguest::kRam[p + i];
    return s;
}

// ---- CD state (oracle Support.h globals) ----
bool g_cdInitialized = false;
int32_t g_lastCdError = 0;
uint32_t g_cdMode = 0;
uint32_t g_cdStreamingLbn = 0;          // read position, for GetReadPos if ever needed
uint32_t g_cdStreamingEndLbn = 0xFFFFFFFFu;
uint32_t g_nextPseudoLbn = kCdPseudoLbnStart;

// A registered CD file: path -> host file + pseudo-LBN (oracle CdFileEntry).
struct CdFileEntry {
    std::string hostPath;
    uint32_t sizeBytes = 0;
    uint32_t baseLbn = 0;
    uint32_t sectors = 0;
};
std::map<std::string, CdFileEntry> g_cdFilesByKey;  // normalized path -> entry

// ---- native disc substrate (oracle Support.h getCdRootPath / readCdSectors) ----
// Oracle: readCdSectors (Support.h:435) serves (1) pseudo-LBN files mapped by
// sceCdSearchFile, then (2) the raw image at lbn*2048 (getCdImagePath). Native
// equivalent:
//   * RT_DISC is the extracted tree (src/native/app/main.cpp discRootPath());
//   * "<disc>/.lbn_map.tsv" (first-run extraction, src/native/assets/
//     iso_extract.h) records every extracted file's ISO9660 LBA ("path\tlba\t
//     bytes"), which IS the game's CD LSN (oracle sceCdIntToPos adds +150 for
//     MSF display; LSN == ISO LBA, no lead-in offset).
//
// TODO(deviation) [substrate]: raw-LBN reads for files NOT in the extraction
// manifest (SYSTEM.CNF, boot2, and any hard-coded SYS-table LSN that falls
// outside ACTION/CAR*/COURSE/FLD/ITEM/SHOP/SOUND/SYS) need the RAW image.
// Native keeps the image only transiently inside FirstRunExtraction and never
// records its path at runtime. Coordinator must (a) retain the image path (an
// RT_IMAGE env or a "<disc>/.image_path" sidecar) and (b) expose a sector
// reader here (IsoImage::readSectors, iso_extract.h, or a plain 2048-stride
// fread for cooked .iso). Until then, unmanifested raw-LBN reads fail.

std::string g_discRoot;                     // RT_DISC, lazy
std::map<uint32_t, CdFileEntry> g_lbnIndex; // ISO LBA -> extracted host file (lazy)
bool g_lbnIndexBuilt = false;

std::string discRoot() {
    if (g_discRoot.empty()) {
        if (const char* d = std::getenv("RT_DISC")) g_discRoot = d;
        else if (const char* h = std::getenv("HOME"))
            g_discRoot = std::string(h) + "/Library/Application Support/RoadTripRecomp/disc";
    }
    return g_discRoot;
}

// Build LBA -> file from the extraction sidecar once (oracle registerCdFileAtLbn
// counterpart; the sidecar supplies the LBA the sibling extractor recorded).
void buildLbnIndex() {
    if (g_lbnIndexBuilt) return;
    g_lbnIndexBuilt = true;
    std::ifstream in(discRoot() + "/.lbn_map.tsv");
    if (!in) return;
    std::string line;
    while (std::getline(in, line)) {
        const size_t t1 = line.find('\t');
        if (t1 == std::string::npos) continue;
        const size_t t2 = line.find('\t', t1 + 1);
        if (t2 == std::string::npos) continue;
        const std::string path = line.substr(0, t1);
        const uint32_t lba = (uint32_t)std::strtoul(line.substr(t1 + 1, t2 - t1 - 1).c_str(), nullptr, 10);
        const uint64_t bytes = std::strtoull(line.substr(t2 + 1).c_str(), nullptr, 10);
        CdFileEntry e;
        e.hostPath = discRoot() + "/" + path;
        e.sizeBytes = (uint32_t)(bytes & 0xFFFFFFFFu);
        e.baseLbn = lba;
        e.sectors = (uint32_t)((bytes + (kCdSectorSize - 1)) / kCdSectorSize);
        if (e.sectors == 0) e.sectors = 1;  // oracle sectorsForBytes floor
        g_lbnIndex.emplace(lba, e);
    }
}

// Read one host range (oracle Support.h readHostRange:402).
bool readHostRange(const std::string& path, uint64_t offset, uint8_t* dst, size_t bytes) {
    if (!dst) { g_lastCdError = -1; return false; }
    if (bytes == 0) { g_lastCdError = 0; return true; }
    std::memset(dst, 0, bytes);
    std::ifstream f(path, std::ios::binary);
    if (!f.is_open()) { g_lastCdError = -1; return false; }
    f.seekg((std::streamoff)offset, std::ios::beg);
    if (!f.good()) { g_lastCdError = -1; return false; }
    f.read((char*)dst, (std::streamsize)bytes);
    g_lastCdError = 0;
    return true;
}

// Read raw CD sectors (oracle Support.h readCdSectors:435). dst is a HOST
// pointer (already RAM-mapped by the caller). Returns false + sets
// g_lastCdError on an unresolved LBN.
bool readCdSectors(uint32_t lbn, uint32_t sectors, uint8_t* dst, size_t byteCount) {
    // 1) pseudo-LBN files registered by sceCdSearchFile.
    for (const auto& [key, e] : g_cdFilesByKey) {
        if (lbn < e.baseLbn || lbn >= e.baseLbn + e.sectors) continue;
        return readHostRange(e.hostPath, (uint64_t)(lbn - e.baseLbn) * kCdSectorSize, dst, byteCount);
    }
    // 2) extracted-tree files at their original ISO LBA.
    buildLbnIndex();
    for (const auto& [lba, e] : g_lbnIndex) {
        if (lbn < e.baseLbn || lbn >= e.baseLbn + e.sectors) continue;
        return readHostRange(e.hostPath, (uint64_t)(lbn - e.baseLbn) * kCdSectorSize, dst, byteCount);
    }
    // 3) raw image fallback (RESEARCH: no path retained by native yet).
    //    Oracle Support.h:450-467 reads getCdImagePath() at lbn*2048. Wire the
    //    image path (RT_IMAGE / sidecar) here to serve unmanifested LBNs.
    if (const char* img = std::getenv("RT_IMAGE")) {
        if (*img) return readHostRange(img, (uint64_t)lbn * kCdSectorSize, dst, byteCount);
    }
    static int shown = 0;
    if (shown < 32)
        std::fprintf(stderr, "[hostlib] sceCdRead unresolved LBN 0x%x sectors=%u (no mapped file, no RT_IMAGE)\n",
                     lbn, sectors);
    if (shown < 1000) ++shown;
    g_lastCdError = -1;
    return false;
}

// Lowercase + strip "cdrom0:"/"cdrom:"/"host:" prefix + leading '/' + ';N'
// (oracle Support.h normalizeCdPathNoPrefix:79).
std::string normCdPath(std::string path) {
    for (auto& c : path) if (c == '\\') c = '/';
    for (auto& c : path) if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
    if (path.rfind("cdrom0:", 0) == 0) path.erase(0, 7);
    else if (path.rfind("cdrom:", 0) == 0) path.erase(0, 6);
    while (!path.empty() && (path.front() == '/' || path.front() == '\\')) path.erase(path.begin());
    const size_t semi = path.find(';');
    if (semi != std::string::npos) {
        bool numeric = true;
        for (size_t i = semi + 1; i < path.size(); ++i) if (path[i] < '0' || path[i] > '9') { numeric = false; break; }
        if (numeric) path.erase(semi);
    }
    return path;
}

// Register a path -> host file at a fresh pseudo-LBN (oracle registerCdFile:325).
bool registerCdFile(const std::string& ps2Path, CdFileEntry& out) {
    const std::string key = normCdPath(ps2Path);
    if (key.empty()) { g_lastCdError = -1; return false; }
    auto it = g_cdFilesByKey.find(key);
    if (it != g_cdFilesByKey.end()) { out = it->second; g_lastCdError = 0; return true; }
    const std::string host = discRoot() + "/" + key;
    std::ifstream f(host, std::ios::binary);
    if (!f.is_open()) { g_lastCdError = -1; return false; }
    f.seekg(0, std::ios::end);
    const uint64_t size = (uint64_t)f.tellg();
    CdFileEntry e;
    e.hostPath = host;
    e.sizeBytes = (uint32_t)(size & 0xFFFFFFFFu);
    e.baseLbn = g_nextPseudoLbn;
    e.sectors = (uint32_t)((size + (kCdSectorSize - 1)) / kCdSectorSize);
    if (e.sectors == 0) e.sectors = 1;
    g_nextPseudoLbn += e.sectors + 1;  // oracle Support.h:395 (+1 gap)
    g_cdFilesByKey.emplace(key, e);
    out = e;
    g_lastCdError = 0;
    return true;
}

// Write sceCdlFILE {u32 lsn, u32 size, char name[16], u8 date[8]} (oracle
// Support.h writeCdSearchResult:520).
bool writeCdSearchResult(uint32_t fileAddr, const std::string& ps2Path, const CdFileEntry& e) {
    uint8_t* s = (uint8_t*)mem(fileAddr);
    if (!s) return false;
    std::memset(s, 0, 32);
    std::memcpy(s + 0, &e.baseLbn, 4);
    std::memcpy(s + 4, &e.sizeBytes, 4);
    std::string leaf = normCdPath(ps2Path);
    const size_t slash = leaf.find_last_of('/');
    if (slash != std::string::npos) leaf = leaf.substr(slash + 1);
    std::strncpy((char*)s + 8, leaf.c_str(), 15);
    return true;
}

}  // namespace

// ---- sceCdInit (CD.cpp:421-427) ----
uint32_t sceCdInit(uint32_t q0, ...) {
    g_cdInitialized = true;
    g_lastCdError = 0;
    g_cdMode = 0;
    return 1;
}
RT_STUB_FN(0x002789C0, guest::sceCdInit)

// ---- sceCdMmode (CD.cpp:458-462) ----
uint32_t sceCdMmode(uint32_t q0, ...) {
    g_cdMode = q0;  // oracle stores $a0 (SCECdDVD etc.), ignored elsewhere
    return 1;
}
RT_STUB_FN(0x00278E98, guest::sceCdMmode)

// ---- sceCdRead (CD.cpp:226-343) ----
// q0 = lbn, q1 = sectors, q2 = destination buffer (folded u64), q3 = rmode*
// (ignored by the oracle — it reads a0/a1/a2 only). Copies sectors
// SYNCHRONOUSLY into guest RAM and returns 1; 0 on an unresolved LBN.
uint32_t sceCdRead(uint32_t q0, uint32_t q1, uint64_t q2, uint32_t q3, ...) {
    const uint32_t lbn = q0, sectors = q1, buf = (uint32_t)q2;
    // oracle clampReadBytes (CD.cpp:240): bytes = sectors*2048 clamped to RAM.
    const uint64_t requested = (uint64_t)sectors * kCdSectorSize;
    const uint32_t offset = buf & 0x01FFFFFFu;
    const size_t bytes = requested == 0 ? 0
        : (size_t)(requested > (RT_GUEST_RAM_SIZE - offset) ? (RT_GUEST_RAM_SIZE - offset) : requested);
    if (bytes == 0) { g_lastCdError = 0; g_cdStreamingLbn = lbn + sectors; return 1; }
    uint8_t* dst = (uint8_t*)mem(buf);
    if (!dst) { g_lastCdError = -1; return 0; }
    if (!readCdSectors(lbn, sectors, dst, bytes)) return 0;
    g_cdStreamingLbn = lbn + sectors;  // oracle CD.cpp:337
    return 1;
}
RT_STUB_FN(0x00278F60, guest::sceCdRead)

// ---- sceCdSync (CD.cpp:350-353): 0 = completed / not busy ----
uint32_t sceCdSync(uint64_t q0, ...) { return 0; }
RT_STUB_FN(0x00278740, guest::sceCdSync)

// ---- sceCdSyncS (CD.cpp:996-999): 0 = completed / not busy ----
uint64_t sceCdSyncS(uint64_t q0, ...) { return 0; }
RT_STUB_FN(0x002787E0, guest::sceCdSyncS)

// ---- sceCdGetError (CD.cpp:355-358) ----
uint32_t sceCdGetError(uint32_t q0, ...) { return (uint32_t)g_lastCdError; }
RT_STUB_FN(0x00279140, guest::sceCdGetError)

// ---- sceCdSearchFile (CD.cpp:565-669) ----
// q0 = sceCdlFILE* out, q1 = path*. Registers the path at a pseudo-LBN and
// writes the file struct; returns 1 / 0. (The boot does NOT reach this —
// it reads hard-coded LSNs — but later game code does.)
uint32_t sceCdSearchFile(uint32_t q0, uint32_t q1, ...) {
    const std::string path = cstrGuest(q1, 260);
    if (path.empty()) { g_lastCdError = -1; return 0; }
    CdFileEntry e;
    if (!registerCdFile(path, e)) return 0;
    if (!writeCdSearchResult(q0, path, e)) { g_lastCdError = -1; return 0; }
    g_cdStreamingLbn = e.baseLbn;
    g_cdStreamingEndLbn = e.baseLbn + e.sectors;
    return 1;
}
RT_STUB_FN(0x00278238, guest::sceCdSearchFile)

// ---- sceCdNcmdDiskReady (CD.cpp:464-467): 2 = ready ----
uint32_t sceCdNcmdDiskReady(uint32_t q0, ...) { return 2; }
RT_STUB_FN(0x002786A8, guest::sceCdNcmdDiskReady)

// ---- sceCdBreak (CD.cpp:375-378) ----
uint32_t sceCdBreak(uint32_t q0, ...) { return 1; }
RT_STUB_FN(0x002791D8, guest::sceCdBreak)

// ---- sceCdCallback (CD.cpp:380-383) ----
uint32_t sceCdCallback(uint32_t q0, ...) { return 0; }
RT_STUB_FN(0x00277CD8, guest::sceCdCallback)

// ---- sceCdInitEeCB (CD.cpp:429-432) ----
uint32_t sceCdInitEeCB(uint32_t q0, uint32_t q1, uint32_t q2, ...) { return 1; }
RT_STUB_FN(0x00277E90, guest::sceCdInitEeCB)

// ---- sceCdDelayThread (CD.cpp:390-393) ----
uint32_t sceCdDelayThread(uint16_t q0, ...) { return 0; }
RT_STUB_FN(0x00277C70, guest::sceCdDelayThread)

// ---- sceSdTransToIOP (Audio.cpp:208-211): oracle leaves it TODO_NAMED ----
// libsd (sound driver), not libcdvd; the oracle's host body is
// TODO_NAMED("sceSdTransToIOP") = log-once + return 0. Real semantics belong
// to the Audio wave; keep the oracle's no-op here so the address is served.
uint32_t sceSdTransToIOP(uint32_t q0, uint32_t q1, uint32_t q2, uint32_t q3, ...) {
    static int shown = 0;
    if (shown < 8) std::fprintf(stderr, "[hostlib] sceSdTransToIOP (libsd; TODO, audio wave)\n");
    if (shown < 1000) ++shown;
    return 0;
}
RT_STUB_FN(0x00279338, guest::sceSdTransToIOP)

}  // namespace guest
