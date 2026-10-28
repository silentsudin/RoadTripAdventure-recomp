// W12 host services, wave 6 (libmc memory card). Coordinator-reviewed mirror
// of the oracle's ps2_stubs MemoryCard family (Kernel/Stubs/MemoryCard.cpp)
// for the 23 sceMc*/mce* entries in config/native_host_stubs.txt. Nothing
// here is compiled from ps2xRuntime (decomp hard rule); bodies transliterate
// the oracle's semantics with per-body citations.
//
// Oracle ground truth (cited inline): a host-directory card per port
// (IoPaths.mcRoot = <data>/saves/mc0, sibling mc1 — src/main.cpp:1189),
// always formatted; libmc's async protocol: each sceMc* command starts and
// completes immediately, records (cmd,result) + pending (MemoryCard.cpp:377
// setMcCommandResultLocked), and sceMcSync returns 1 with cmd/result once,
// -1 while idle (MemoryCard.cpp:1220-1258 "games rely on it").
//
// shortcut: no save-state serialization of this state (native save states
// are W13/W14; mirror serializeMemoryCardState then), and file timestamps
// use the host clock (ps2_guest_clock fake mode is not ported; upgrade when
// determinism tests feed GetDir entries into hashed state).
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include "guest_shim.h"
#include "guest_decls.inc"

namespace guest {

#define RT_STUB_FN(addr, name)                                       \
    namespace { struct RtReg_##addr { RtReg_##addr() {               \
        rtguest::registerGuest(addr,                                 \
            reinterpret_cast<void*>(&name)); } } rt_reg_##addr##_; }

namespace {

void* memP(uint32_t a) {
    const uint32_t p = a & 0x01FFFFFFu;
    return p < RT_GUEST_RAM_SIZE ? rtguest::kRam + p : nullptr;
}
uint32_t rdW(uint32_t a) { uint32_t v = 0; if (memP(a)) std::memcpy(&v, memP(a), 4); return v; }
void wrW(uint32_t a, uint32_t v) { if (memP(a)) std::memcpy(memP(a), &v, 4); }

std::string cstrGuest(uint32_t a, uint32_t cap) {
    if (!memP(a)) return {};
    const uint32_t p = a & 0x01FFFFFFu;
    std::string s;
    for (uint32_t i = 0; i < cap && rtguest::kRam[p + i]; ++i) s += (char)rtguest::kRam[p + i];
    return s;
}

// ---- oracle constants (MemoryCard.cpp:11-46) ----
constexpr int32_t kMcCmdGetInfo = 0x01, kMcCmdOpen = 0x02, kMcCmdClose = 0x03,
    kMcCmdSeek = 0x04, kMcCmdRead = 0x05, kMcCmdWrite = 0x06, kMcCmdFlush = 0x0A,
    kMcCmdMkdir = 0x0B, kMcCmdChdir = 0x0C, kMcCmdGetDir = 0x0D,
    kMcCmdSetFileInfo = 0x0E, kMcCmdDelete = 0x0F, kMcCmdFormat = 0x10,
    kMcCmdUnformat = 0x11, kMcCmdGetEntSpace = 0x12, kMcCmdRename = 0x13;
constexpr int32_t kMcResultSucceed = 0, kMcResultChangedCard = -1,
    kMcResultNoFormat = -2, kMcResultNoEntry = -4, kMcResultDeniedPermit = -5,
    kMcResultNotEmpty = -6, kMcResultUpLimitHandle = -7;
constexpr int32_t kMcTypePs2 = 2, kMcFormatted = 1, kMcUnformatted = 0;
constexpr int32_t kMcFreeClusters = 0x2000;
constexpr uint32_t kMcMaxPathLen = 1024;
constexpr size_t kMcMaxOpenFiles = 32;
constexpr uint16_t kMcAttrReadable = 0x0001, kMcAttrWriteable = 0x0002,
    kMcAttrFile = 0x0010, kMcAttrSubdir = 0x0020, kMcAttrClosed = 0x0080,
    kMcAttrExists = 0x8000;

// PS2 fio flags (mirrors guest_libs_fs.cpp).
constexpr uint32_t kFioAccMask = 0x3, kFioAppend = 0x0100, kFioCreat = 0x0200,
    kFioTrunc = 0x0400;
constexpr int32_t kPs2SeekCur = 1, kPs2SeekEnd = 2;

// ---- state (oracle g_mc* globals; no locks: single pump thread) ----
struct McOpenFile { FILE* file = nullptr; int32_t port = 0; std::string hostPath; };
struct McPortState { std::string currentDir = "/"; bool formatted = true; bool changed = false; };
int32_t g_mcNextFd = 1;
int32_t g_mcLastCmd = 0, g_mcLastResult = 0;
bool g_mcCommandPending = false;
std::map<int32_t, McOpenFile> g_mcFiles;
McPortState g_mcPorts[2];

void setMcCommandResult(int32_t cmd, int32_t result) {   // MemoryCard.cpp:377
    g_mcLastCmd = cmd; g_mcLastResult = result; g_mcCommandPending = true;
}

// ---- host roots (oracle getMcRootPath; app mcRoot = <data>/saves/mc0) ----
std::filesystem::path mcRootPath(int32_t port) {
    std::filesystem::path data;
    if (const char* d = std::getenv("RT_DATA_DIR")) data = d;
    else if (const char* h = std::getenv("HOME"))
        data = std::string(h) + "/Library/Application Support/RoadTripRecomp";
    else data = ".";
    std::filesystem::path root = data / "saves" / "mc0";
    if (port <= 0) return root;
    return root.parent_path() / "mc1";   // MemoryCard.cpp:138-146 leaf mc0 -> mc1
}
void ensureMcRoot(int32_t port) {
    std::error_code ec;
    std::filesystem::create_directories(mcRootPath(port), ec);
}

std::string toLowerS(std::string s) {
    for (auto& c : s) if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
    return s;
}
std::vector<std::string> splitMcPath(const std::string& v) {  // :174
    std::vector<std::string> parts; std::string cur;
    for (char c : v) {
        if (c == '/' || c == '\\') { if (!cur.empty()) { parts.push_back(cur); cur.clear(); } }
        else cur.push_back(c);
    }
    if (!cur.empty()) parts.push_back(cur);
    return parts;
}
std::string joinMcPath(const std::vector<std::string>& p) {  // :192
    if (p.empty()) return "/";
    std::string j = "/";
    for (size_t i = 0; i < p.size(); ++i) { if (i) j += '/'; j += p[i]; }
    return j;
}
std::string normalizeMcPath(int32_t port, std::string path) {  // :204
    std::replace(path.begin(), path.end(), '\\', '/');
    std::string lower = toLowerS(path);
    if (lower.rfind("mc0:", 0) == 0 || lower.rfind("mc1:", 0) == 0) path = path.substr(4);
    const bool abs = !path.empty() && path.front() == '/';
    std::vector<std::string> parts;
    if (!abs && port >= 0 && port < 2) parts = splitMcPath(g_mcPorts[port].currentDir);
    for (const std::string& part : splitMcPath(path)) {
        if (part.empty() || part == ".") continue;
        if (part == "..") { if (!parts.empty()) parts.pop_back(); continue; }
        parts.push_back(part);
    }
    return joinMcPath(parts);
}
std::filesystem::path guestToHostMc(int32_t port, const std::string& guestPath) {  // :232
    std::filesystem::path r = mcRootPath(port);
    if (guestPath.size() > 1u) r /= std::filesystem::path(guestPath.substr(1));
    return r.lexically_normal();
}

void writeMcCString(uint32_t addr, const std::string& v) {  // :266
    if (!addr || !memP(addr)) return;
    std::memcpy(memP(addr), v.c_str(), v.size() + 1u);
}

struct McDateTime { uint8_t Resv2, Sec, Min, Hour, Day, Month; uint16_t Year; };
struct McTblGetDir {  // :57-67, static_assert 64
    McDateTime Create, Modify;
    uint32_t FileSizeByte; uint16_t AttrFile, Reserve1; uint32_t Reserve2, PdaAplNo;
    char EntryName[32];
};
static_assert(sizeof(McTblGetDir) == 64, "sceMcTblGetDir size mismatch");

void writeMcDateTime(McDateTime& out, std::time_t v) {  // :280
    std::tm tm{};
#if defined(_WIN32)
    if (localtime_s(&tm, &v) != 0) { std::memset(&out, 0, sizeof(out)); return; }
#else
    if (localtime_r(&v, &tm) == nullptr) { std::memset(&out, 0, sizeof(out)); return; }
#endif
    out = McDateTime{0, (uint8_t)tm.tm_sec, (uint8_t)tm.tm_min, (uint8_t)tm.tm_hour,
                     (uint8_t)tm.tm_mday, (uint8_t)(tm.tm_mon + 1), (uint16_t)(tm.tm_year + 1900)};
}
void fillDirEntry(McTblGetDir& e, const std::string& name, bool isDir,
                  uint32_t size, std::time_t mtime) {  // :296
    std::memset(&e, 0, sizeof(e));
    writeMcDateTime(e.Create, mtime); writeMcDateTime(e.Modify, mtime);
    e.FileSizeByte = isDir ? 0u : size;
    e.AttrFile = (uint16_t)(kMcAttrReadable | kMcAttrWriteable |
                            (isDir ? kMcAttrSubdir : kMcAttrFile) |
                            kMcAttrClosed | kMcAttrExists);
    std::strncpy(e.EntryName, name.c_str(), sizeof(e.EntryName) - 1u);
    e.EntryName[sizeof(e.EntryName) - 1u] = '\0';
}
bool wildcardMatch(const std::string& pat, const std::string& val) {  // :319
    size_t pp = 0, vp = 0, star = std::string::npos, match = 0;
    while (vp < val.size()) {
        if (pp < pat.size() && (pat[pp] == '?' || pat[pp] == val[vp])) { ++pp; ++vp; }
        else if (pp < pat.size() && pat[pp] == '*') { star = pp++; match = vp; }
        else if (star != std::string::npos) { pp = star + 1; vp = ++match; }
        else return false;
    }
    while (pp < pat.size() && pat[pp] == '*') ++pp;
    return pp == pat.size();
}

std::time_t fileTimeToTimeT(std::filesystem::file_time_type v) {  // :258
    auto sys = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        v - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
    return std::chrono::system_clock::to_time_t(sys);
}

void closeMcFiles() {  // :384
    for (auto& [fd, f] : g_mcFiles) if (f.file) std::fclose(f.file);
    g_mcFiles.clear();
}
void closeMcFilesForPort(int32_t port) {  // :396
    for (auto it = g_mcFiles.begin(); it != g_mcFiles.end();)
        if (it->second.port == port) {
            if (it->second.file) std::fclose(it->second.file);
            it = g_mcFiles.erase(it);
        } else ++it;
}
int32_t allocMcFd(FILE* file, int32_t port, const std::filesystem::path& host) {  // :413
    if (!file) return kMcResultDeniedPermit;
    if (g_mcFiles.size() >= kMcMaxOpenFiles) return kMcResultUpLimitHandle;
    for (int attempt = 0; attempt < 0x10000; ++attempt) {
        if (g_mcNextFd <= 0) g_mcNextFd = 1;
        const int32_t fd = g_mcNextFd++;
        if (g_mcFiles.count(fd)) continue;
        g_mcFiles.emplace(fd, McOpenFile{file, port, host.string()});
        return fd;
    }
    return kMcResultUpLimitHandle;
}
FILE* openMcHostFile(const std::filesystem::path& host, uint32_t flags) {  // :438
    const uint32_t access = flags & kFioAccMask;
    const bool createOnly = access == 0u && (flags & kFioCreat);   // :440 games open O_CREAT alone
    const bool read = createOnly || access == 0x1 || access == 0x3;
    const bool write = createOnly || access == 0x2 || access == 0x3;
    const bool app = (flags & kFioAppend) != 0, creat = (flags & kFioCreat) != 0,
               trunc = (flags & kFioTrunc) != 0;
    std::error_code ec;
    const bool exists = std::filesystem::exists(host, ec) && !ec;
    const char* mode = "rb";
    if (read && write) mode = app ? (exists ? "a+b" : "w+b")
                                  : ((trunc || (creat && !exists)) ? "w+b" : "r+b");
    else if (write) mode = app ? (exists ? "ab" : "wb")
                               : ((trunc || (creat && !exists)) ? "wb" : "r+b");
    return std::fopen(host.string().c_str(), mode);
}
bool validPort(int32_t port, int32_t slot) { return port >= 0 && port < 2 && slot == 0; }  // :109

}  // namespace

// ---- the 23 host bodies (order = config/native_host_stubs.txt) ----

uint32_t sceMcInit(uint32_t q0, ...) {                 // MemoryCard.cpp:940
    closeMcFiles();
    g_mcNextFd = 1; g_mcLastCmd = 0; g_mcLastResult = 0; g_mcCommandPending = false;
    for (McPortState& s : g_mcPorts) { s.currentDir = "/"; s.formatted = true; }
    ensureMcRoot(0); ensureMcRoot(1);
    return 0;
}
uint64_t sceMcChangeThreadPriority(uint32_t q0, ...) { return 0; }   // :529
uint32_t sceMcGetSlotMax(uint32_t q0, ...) { return 1; }             // :935

int64_t sceMcOpen(uint32_t q0, uint32_t q1, int64_t q2, uint32_t q3, ...) {  // :1007
    const std::string path = cstrGuest((uint32_t)q2, kMcMaxPathLen);
    int32_t result = kMcResultNoEntry;
    if (validPort(q0, q1)) {
        McPortState& s = g_mcPorts[q0];
        if (!s.formatted) result = kMcResultNoFormat;
        else {
            const std::string gp = normalizeMcPath(q0, path);
            const auto host = guestToHostMc(q0, gp);
            std::error_code ec;
            const bool create = (q3 & kFioCreat) != 0;
            const bool exists = std::filesystem::exists(host, ec) && !ec;
            if (gp == "/") result = kMcResultDeniedPermit;
            else if (exists && std::filesystem::is_directory(host, ec)) result = kMcResultDeniedPermit;
            else if (!exists && !create) result = kMcResultNoEntry;
            else if (!std::filesystem::exists(host.parent_path(), ec) || ec) result = kMcResultNoEntry;
            else {
                FILE* f = openMcHostFile(host, q3);
                if (!f) result = exists ? kMcResultDeniedPermit : kMcResultNoEntry;
                else { result = allocMcFd(f, q0, host); if (result < 0) std::fclose(f); }
            }
        }
    }
    setMcCommandResult(kMcCmdOpen, result);
    return 0;
}

int64_t sceMcMkdir(uint32_t q0, ...) {  // :960  ($a1 slot, $a2 path via va)
    va_list ap; va_start(ap, q0);
    const uint64_t q1 = va_arg(ap, uint64_t);
    const uint64_t q2 = va_arg(ap, uint64_t);
    va_end(ap);
    const std::string path = cstrGuest((uint32_t)q2, kMcMaxPathLen);
    int32_t result = kMcResultNoEntry;
    if (validPort((int32_t)q0, (int32_t)q1)) {
        McPortState& s = g_mcPorts[q0];
        if (!s.formatted) result = kMcResultNoFormat;
        else {
            ensureMcRoot(q0);
            const auto host = guestToHostMc(q0, normalizeMcPath(q0, path));
            std::error_code ec;
            if (std::filesystem::exists(host, ec) && !ec)
                result = std::filesystem::is_directory(host, ec) ? kMcResultSucceed : kMcResultDeniedPermit;
            else if (std::filesystem::create_directory(host, ec) && !ec) result = kMcResultSucceed;
            else result = (std::filesystem::exists(host.parent_path(), ec) && !ec)
                              ? kMcResultDeniedPermit : kMcResultNoEntry;
        }
    }
    setMcCommandResult(kMcCmdMkdir, result);
    return 0;
}

uint64_t sceMcClose(uint32_t q0, ...) {  // :581
    int32_t result = kMcResultNoEntry;
    auto it = g_mcFiles.find(q0);
    if (it != g_mcFiles.end()) {
        if (!it->second.file || std::fclose(it->second.file) == 0) result = kMcResultSucceed;
        g_mcFiles.erase(it);
    }
    setMcCommandResult(kMcCmdClose, result);
    return 0;
}

uint64_t sceMcSeek(uint32_t q0, uint32_t q1, uint32_t q2, ...) {  // :1149
    int32_t result = kMcResultNoEntry;
    auto it = g_mcFiles.find(q0);
    if (it != g_mcFiles.end() && it->second.file) {
        const int whence = q2 == kPs2SeekCur ? SEEK_CUR : q2 == kPs2SeekEnd ? SEEK_END : SEEK_SET;
        if (std::fseek(it->second.file, q1, whence) == 0) {
            const long pos = std::ftell(it->second.file);
            result = pos >= 0 ? (int32_t)pos : kMcResultDeniedPermit;
        } else result = kMcResultDeniedPermit;
    }
    setMcCommandResult(kMcCmdSeek, result);
    return 0;
}

uint32_t mceIntrReadFixAlign(uint32_t q0, ...) { return 0; }    // :1433

uint64_t sceMcRead(uint32_t q0, uint64_t q1, uint64_t q2, ...) {  // :1073
    uint8_t* dst = q2 > 0 ? (uint8_t*)memP(q1) : nullptr;
    int32_t result = kMcResultNoEntry;
    auto it = g_mcFiles.find(q0);
    if (q2 <= 0) result = 0;
    else if (it == g_mcFiles.end() || !it->second.file) result = kMcResultNoEntry;
    else if (!dst) result = kMcResultDeniedPermit;
    else {
        const size_t n = std::fread(dst, 1u, (size_t)q2, it->second.file);
        result = std::ferror(it->second.file) ? kMcResultDeniedPermit : (int32_t)n;
        if (std::ferror(it->second.file)) std::clearerr(it->second.file);
    }
    setMcCommandResult(kMcCmdRead, result);
    return 0;
}

uint64_t sceMcWrite(uint32_t q0, uint32_t q1, uint64_t q2, ...) {  // :1294
    const uint8_t* src = q2 > 0 ? (const uint8_t*)memP(q1) : nullptr;
    int32_t result = kMcResultNoEntry;
    auto it = g_mcFiles.find(q0);
    if (q2 <= 0) result = 0;
    else if (it == g_mcFiles.end() || !it->second.file) result = kMcResultNoEntry;
    else if (!src) result = kMcResultDeniedPermit;
    else {
        const size_t n = std::fwrite(src, 1u, (size_t)q2, it->second.file);
        result = std::ferror(it->second.file) ? kMcResultDeniedPermit : (int32_t)n;
        if (!std::ferror(it->second.file)) std::fflush(it->second.file);
        else std::clearerr(it->second.file);
    }
    setMcCommandResult(kMcCmdWrite, result);
    return 0;
}

uint32_t sceMcSync(uint64_t q0, uint64_t q1, uint64_t q2, ...) {  // :1220
    const bool had = g_mcCommandPending;
    g_mcCommandPending = false;
    if (!had) return 0xFFFFFFFFu;  // -1: libmc idle signal (games rely on it)
    if (q1) wrW((uint32_t)q1, (uint32_t)g_mcLastCmd);
    if (q2) wrW((uint32_t)q2, (uint32_t)g_mcLastResult);
    return 1;
}

uint32_t mceGetInfoApdx(uint32_t q0, ...) { return 0; }          // :1438

uint64_t sceMcGetInfo(uint32_t q0, uint32_t q1, uint64_t q2, uint64_t q3, uint64_t q4, ...) {  // :876
    int32_t cardType = 0, freeBlocks = 0, format = kMcUnformatted;
    int32_t result = kMcResultNoEntry;
    if (validPort((int32_t)q0, (int32_t)q1)) {
        McPortState& s = g_mcPorts[q0];
        cardType = kMcTypePs2;
        freeBlocks = s.formatted ? kMcFreeClusters : 0;
        format = s.formatted ? kMcFormatted : kMcUnformatted;
        result = s.formatted ? kMcResultSucceed : kMcResultNoFormat;
        if (s.changed && s.formatted) result = kMcResultChangedCard;  // :899 report swap once
        s.changed = false;
    }
    if (q2) wrW((uint32_t)q2, (uint32_t)cardType);
    if (q3) wrW((uint32_t)q3, (uint32_t)freeBlocks);
    if (q4) wrW((uint32_t)q4, (uint32_t)format);
    setMcCommandResult(kMcCmdGetInfo, result);
    return 0;
}

int64_t sceMcGetDir(uint32_t q0, uint32_t q1, int64_t q2, uint32_t q3, int64_t q4, uint64_t q5, ...) {  // :714
    const std::string raw = cstrGuest((uint32_t)q2, kMcMaxPathLen);
    const int32_t maxEntries = q4;
    const uint32_t tableAddr = (uint32_t)q5;
    std::vector<McTblGetDir> entries;
    int32_t result = kMcResultNoEntry;
    if (validPort(q0, q1)) {
        McPortState& s = g_mcPorts[q0];
        if (!s.formatted) result = kMcResultNoFormat;
        else {
            ensureMcRoot(q0);
            const std::string q = normalizeMcPath(q0, raw.empty() ? "." : raw);
            const bool wild = q.find('*') != std::string::npos || q.find('?') != std::string::npos;
            std::filesystem::path queryRel = q.size() > 1u ? std::filesystem::path(q.substr(1))
                                                           : std::filesystem::path{};
            std::filesystem::path parentRel; std::string pattern;
            if (wild) { parentRel = queryRel.parent_path(); pattern = queryRel.filename().string(); }
            else {
                const auto qh = guestToHostMc(q0, q);
                std::error_code qec;
                if (std::filesystem::exists(qh, qec) && !qec && std::filesystem::is_directory(qh, qec))
                    { parentRel = queryRel; pattern = "*"; }
                else { parentRel = queryRel.parent_path(); pattern = queryRel.filename().string(); }
            }
            if (pattern.empty()) pattern = "*";
            std::filesystem::path hostDir = mcRootPath(q0);
            if (!parentRel.empty()) hostDir /= parentRel;
            hostDir = hostDir.lexically_normal();
            std::error_code ec;
            if (std::filesystem::exists(hostDir, ec) && !ec && std::filesystem::is_directory(hostDir, ec)) {
                const std::time_t now = std::time(nullptr);
                auto add = [&](const std::string& name) {
                    if (!wildcardMatch(pattern, name)) return;
                    McTblGetDir e{}; fillDirEntry(e, name, true, 0u, now); entries.push_back(e);
                };
                add("."); add("..");
                std::vector<std::filesystem::directory_entry> dirEntries;
                for (const auto& e : std::filesystem::directory_iterator(
                         hostDir, std::filesystem::directory_options::skip_permission_denied, ec)) {
                    if (ec) break;
                    dirEntries.push_back(e);
                }
                std::sort(dirEntries.begin(), dirEntries.end(),
                          [](const std::filesystem::directory_entry& a,
                             const std::filesystem::directory_entry& b) {
                              return toLowerS(a.path().filename().string()) <
                                     toLowerS(b.path().filename().string());
                          });
                for (const auto& e : dirEntries) {
                    const std::string name = e.path().filename().string();
                    if (!wildcardMatch(pattern, name)) continue;
                    std::error_code eec;
                    const bool isDir = e.is_directory(eec) && !eec;
                    const uint32_t size = isDir ? 0u : (uint32_t)e.file_size(eec);
                    eec.clear();
                    const std::time_t mt = fileTimeToTimeT(e.last_write_time(eec));
                    McTblGetDir te{}; fillDirEntry(te, name, isDir, size, eec ? now : mt);
                    entries.push_back(te);
                }
                const size_t n = std::min(entries.size(),
                                         maxEntries > 0 ? (size_t)maxEntries : 0u);
                if (n == 0u || tableAddr == 0u) result = (int32_t)n;
                else if (uint8_t* dst = (uint8_t*)memP(tableAddr)) {
                    std::memcpy(dst, entries.data(), n * sizeof(McTblGetDir));
                    result = (int32_t)n;
                } else result = kMcResultDeniedPermit;
            }
        }
    }
    setMcCommandResult(kMcCmdGetDir, result);
    return 0;
}

uint32_t mceStorePwd(uint32_t q0, ...) { return 0; }             // :1443

int64_t sceMcChdir(uint32_t q0, uint32_t q1, int64_t q2, ...) {  // :534
    // $a3 (currentDir out ptr) is unnamed in the .inc; the va list reaches it.
    va_list ap; va_start(ap, q2);
    const uint32_t outAddr = (uint32_t)va_arg(ap, uint64_t);
    va_end(ap);
    const std::string requested = q2 ? cstrGuest((uint32_t)q2, kMcMaxPathLen) : std::string{};
    std::string currentDir = "/";
    int32_t result = kMcResultNoEntry;
    if (validPort((int32_t)q0, (int32_t)q1)) {
        McPortState& s = g_mcPorts[q0];
        currentDir = s.currentDir;
        if (!s.formatted) result = kMcResultNoFormat;
        else {
            ensureMcRoot(q0);
            const std::string resolved = requested.empty() ? s.currentDir
                                                           : normalizeMcPath(q0, requested);
            const auto hostDir = guestToHostMc(q0, resolved);
            std::error_code ec;
            if (std::filesystem::exists(hostDir, ec) && !ec && std::filesystem::is_directory(hostDir, ec)) {
                s.currentDir = resolved; currentDir = resolved; result = kMcResultSucceed;
            }
        }
    }
    setMcCommandResult(kMcCmdChdir, result);
    writeMcCString(outAddr, currentDir);
    return 0;
}

uint64_t sceMcFormat(uint32_t q0, uint32_t q1, ...) {  // :798
    int32_t result = kMcResultNoEntry;
    if (validPort((int32_t)q0, (int32_t)q1)) {
        closeMcFilesForPort(q0);
        const auto root = mcRootPath(q0);
        std::error_code ec;
        std::filesystem::remove_all(root, ec); ec.clear();
        std::filesystem::create_directories(root, ec);
        if (!ec) {
            McPortState& s = g_mcPorts[q0];
            s.currentDir = "/"; s.formatted = true; result = kMcResultSucceed;
        }
    }
    setMcCommandResult(kMcCmdFormat, result);
    return 0;
}

int64_t sceMcDelete(uint32_t q0, uint32_t q1, int64_t q2, ...) {  // :601
    const std::string path = cstrGuest((uint32_t)q2, kMcMaxPathLen);
    int32_t result = kMcResultNoEntry;
    if (validPort((int32_t)q0, (int32_t)q1)) {
        McPortState& s = g_mcPorts[q0];
        if (!s.formatted) result = kMcResultNoFormat;
        else {
            const std::string gp = normalizeMcPath(q0, path);
            if (gp != "/") {
                const auto host = guestToHostMc(q0, gp);
                std::error_code ec;
                if (std::filesystem::exists(host, ec) && !ec) {
                    if (std::filesystem::is_directory(host, ec) && !std::filesystem::is_empty(host, ec))
                        result = kMcResultNotEmpty;
                    else if (std::filesystem::remove(host, ec) && !ec) result = kMcResultSucceed;
                    else result = kMcResultDeniedPermit;
                }
            }
        }
    }
    setMcCommandResult(kMcCmdDelete, result);
    return 0;
}

uint64_t sceMcFlush(uint32_t q0, ...) {  // :668
    int32_t result = kMcResultNoEntry;
    auto it = g_mcFiles.find(q0);
    if (it != g_mcFiles.end() && it->second.file)
        result = std::fflush(it->second.file) == 0 ? kMcResultSucceed : kMcResultDeniedPermit;
    setMcCommandResult(kMcCmdFlush, result);
    return 0;
}

int64_t sceMcSetFileInfo(uint32_t q0, uint32_t q1, int64_t q2, uint32_t q3, uint32_t q4, ...) {  // :1187
    const std::string path = cstrGuest((uint32_t)q2, kMcMaxPathLen);
    int32_t result = kMcResultNoEntry;
    if (validPort((int32_t)q0, (int32_t)q1)) {
        McPortState& s = g_mcPorts[q0];
        if (!s.formatted) result = kMcResultNoFormat;
        else {
            const auto host = guestToHostMc(q0, normalizeMcPath(q0, path));
            std::error_code ec;
            if (std::filesystem::exists(host, ec) && !ec) result = kMcResultSucceed;
        }
    }
    setMcCommandResult(kMcCmdSetFileInfo, result);
    return 0;
}

int64_t sceMcRename(uint32_t q0, uint32_t q1, int64_t q2, int64_t q3, ...) {  // :1111
    const std::string oldP = cstrGuest((uint32_t)q2, kMcMaxPathLen);
    const std::string newP = cstrGuest((uint32_t)q3, kMcMaxPathLen);
    int32_t result = kMcResultNoEntry;
    if (validPort((int32_t)q0, (int32_t)q1)) {
        McPortState& s = g_mcPorts[q0];
        if (!s.formatted) result = kMcResultNoFormat;
        else {
            const auto oh = guestToHostMc(q0, normalizeMcPath(q0, oldP));
            const auto nh = guestToHostMc(q0, normalizeMcPath(q0, newP));
            std::error_code ec;
            if (std::filesystem::exists(oh, ec) && !ec &&
                std::filesystem::exists(nh.parent_path(), ec) && !ec) {
                std::filesystem::rename(oh, nh, ec);
                result = ec ? kMcResultDeniedPermit : kMcResultSucceed;
            }
        }
    }
    setMcCommandResult(kMcCmdRename, result);
    return 0;
}

uint64_t sceMcUnformat(uint32_t q0, uint32_t q1, ...) {  // :1264
    int32_t result = kMcResultNoEntry;
    if (validPort((int32_t)q0, (int32_t)q1)) {
        closeMcFilesForPort(q0);
        const auto root = mcRootPath(q0);
        std::error_code ec;
        std::filesystem::remove_all(root, ec); ec.clear();
        std::filesystem::create_directories(root, ec);
        if (!ec) {
            McPortState& s = g_mcPorts[q0];
            s.currentDir = "/"; s.formatted = false; result = kMcResultSucceed;
        }
    }
    setMcCommandResult(kMcCmdUnformat, result);
    return 0;
}

int64_t sceMcGetEntSpace(uint32_t q0, uint32_t q1, int64_t q2, ...) {  // :867
    setMcCommandResult(kMcCmdGetEntSpace, 1024);
    return 0;
}

// ---- registrations (config/native_host_stubs.txt addresses) ----
RT_STUB_FN(0x002797a8, sceMcInit)
RT_STUB_FN(0x00279990, sceMcChangeThreadPriority)
RT_STUB_FN(0x00279a48, sceMcGetSlotMax)
RT_STUB_FN(0x00279b08, sceMcOpen)
RT_STUB_FN(0x00279c40, sceMcMkdir)
RT_STUB_FN(0x00279c78, sceMcClose)
RT_STUB_FN(0x00279d30, sceMcSeek)
RT_STUB_FN(0x00279e08, mceIntrReadFixAlign)
RT_STUB_FN(0x00279e98, sceMcRead)
RT_STUB_FN(0x00279fb8, sceMcWrite)
RT_STUB_FN(0x0027a1a8, sceMcSync)
RT_STUB_FN(0x0027a288, mceGetInfoApdx)
RT_STUB_FN(0x0027a2e0, sceMcGetInfo)
RT_STUB_FN(0x0027a468, sceMcGetDir)
RT_STUB_FN(0x0027a5c0, mceStorePwd)
RT_STUB_FN(0x0027a648, sceMcChdir)
RT_STUB_FN(0x0027a798, sceMcFormat)
RT_STUB_FN(0x0027a860, sceMcDelete)
RT_STUB_FN(0x0027a988, sceMcFlush)
RT_STUB_FN(0x0027aa40, sceMcSetFileInfo)
RT_STUB_FN(0x0027ac18, sceMcRename)
RT_STUB_FN(0x0027ad80, sceMcUnformat)
RT_STUB_FN(0x0027ae48, sceMcGetEntSpace)

}  // namespace guest
