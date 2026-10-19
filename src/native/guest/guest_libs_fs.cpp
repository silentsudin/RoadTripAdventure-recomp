// W12 host services, wave 4 (libfs / libc file I/O). Reviewed against the
// oracle by the coordinator (FileIO.cpp + Syscalls/FileIO.cpp citations
// spot-checked; addresses matched against config/native_host_stubs.txt).
//
// Mirrors the oracle's ps2_stubs FileIO family (Kernel/Stubs/FileIO.cpp) and
// its syscall layer (Kernel/Syscalls/FileIO.cpp: fioOpen/fioClose/fioRead/
// fioWrite/fioLseek) for the 9 FileIO entries in config/native_host_stubs.txt.
// The oracle routes them through PS2Vfs with mounts {hostRoot, cdRoot, mcRoot}
// (Syscalls/FileIO.cpp:6-10); native resolves to the extracted disc tree
// (RT_DISC) with the same prefix rules. Nothing here is compiled from
// ps2xRuntime (decomp hard rule).
//
// NOTE on scope: config/native_host_stubs.txt contains NO sceDread/sceGetstat/
// sceDopen/sceDread/sceChangeDir entries — those are the IOP fio syscalls
// (handled by the syscall dispatcher, a separate wave), not host-stubbed
// libc/libfs wrappers. This file covers only the 9 listed symbols:
// _sceFsSemInit, sceFsInit, sceFsReset, sceOpen, sceClose, sceLseek, sceRead,
// sceWrite, sceIoctl.
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <map>
#include <string>
#include "guest_shim.h"
#include "guest_decls.inc"

namespace guest {

#define RT_STUB_FN(addr, name)                                       \
    namespace { struct RtReg_##addr { RtReg_##addr() {               \
        rtguest::registerGuest(addr,                                 \
            reinterpret_cast<void*>(&name)); } } rt_reg_##addr##_; }

namespace {

void* mem(uint32_t a) {
    const uint32_t p = a & 0x01FFFFFFu;
    return p < RT_GUEST_RAM_SIZE ? rtguest::kRam + p : nullptr;
}
std::string cstrGuest(uint32_t a, uint32_t cap) {
    if (!mem(a)) return {};
    const uint32_t p = a & 0x01FFFFFFu;
    std::string s;
    for (uint32_t i = 0; i < cap && rtguest::kRam[p + i]; ++i) s += (char)rtguest::kRam[p + i];
    return s;
}

// PS2 fio open flags (ps2sdk ioman.h) -> host fopen mode.
constexpr uint32_t kFioRdOnly = 0x0001, kFioWrOnly = 0x0002, kFioRdWr = 0x0003;
constexpr uint32_t kFioAppend = 0x0100, kFioCreat = 0x0200, kFioTrunc = 0x0400;

// ---- fd table (oracle Support.h g_file_map / generate_file_handle) ----
std::map<uint32_t, FILE*> g_files;
uint32_t g_nextFd = 1;
uint32_t allocFd() {
    uint32_t fd = 0;
    do { fd = g_nextFd++; if (g_nextFd == 0) g_nextFd = 1; } while (fd == 0 || g_files.count(fd));
    return fd;
}

std::string discRoot() {
    if (const char* d = std::getenv("RT_DISC")) return d;
    if (const char* h = std::getenv("HOME"))
        return std::string(h) + "/Library/Application Support/RoadTripRecomp/disc";
    return ".";
}

// ---- PS2 path -> host path (oracle PS2Vfs over {hostRoot,cdRoot,mcRoot}) ----
// TODO(deviation): mc0:/mc1: has no native substrate yet — the oracle mounts
// them at mcRoot (a host directory of the PS2 memory-card filesystem). The MC
// wave must provide one; until then mc: paths fail closed (return empty), so
// the game's save/load takes the "no card" branch rather than corrupting.
std::string resolvePath(const std::string& ps2) {
    std::string p = ps2;
    for (auto& c : p) if (c == '\\') c = '/';
    std::string lower = p;
    for (auto& c : lower) if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
    if (lower.rfind("cdrom0:", 0) == 0) p.erase(0, 7);
    else if (lower.rfind("cdrom:", 0) == 0) p.erase(0, 6);
    else if (lower.rfind("cdfs:", 0) == 0) p.erase(0, 5);
    else if (lower.rfind("host:", 0) == 0) p.erase(0, 5);
    else if (lower.rfind("mc0:", 0) == 0 || lower.rfind("mc1:", 0) == 0) return {};  // RESEARCH (MC wave)
    while (!p.empty() && p.front() == '/') p.erase(p.begin());
    return discRoot() + "/" + p;
}

const char* fopenMode(uint32_t flags) {
    const uint32_t acc = flags & 0x3u;
    const bool app = (flags & kFioAppend) != 0, creat = (flags & kFioCreat) != 0,
               trunc = (flags & kFioTrunc) != 0;
    if (acc == kFioWrOnly) return app ? "ab" : (trunc || creat ? "wb" : "wb");
    if (acc == kFioRdWr) return app ? "a+b" : (creat || trunc ? "w+b" : "r+b");
    return "rb";  // O_RDONLY (default)
}

}  // namespace

// ---- sceFsInit (FileIO.cpp:68-71) ----
uint32_t sceFsInit(uint32_t q0, ...) { return 0; }
RT_STUB_FN(0x00288C30, guest::sceFsInit)

// ---- sceFsReset (FileIO.cpp:73-76) ----
uint32_t sceFsReset(uint32_t q0, ...) { return 0; }
RT_STUB_FN(0x00288E88, guest::sceFsReset)

// ---- _sceFsSemInit (FileIO.cpp:21-24 sceFsSemInit = TODO_NAMED -> 0) ----
uint32_t _sceFsSemInit(uint32_t q0, ...) { return 0; }
RT_STUB_FN(0x00288AE0, guest::_sceFsSemInit)

// ---- sceOpen (FileIO.cpp:106-109 -> fioOpen, Syscalls/FileIO.cpp:21-42) ----
// q0 = path*, q1 = flags, q2 = mode(ignored). Returns a host fd (>=1) or -1.
uint32_t sceOpen(uint32_t q0, uint32_t q1, uint32_t q2, ...) {
    const std::string ps2 = cstrGuest(q0, 260);
    const std::string host = resolvePath(ps2);
    if (host.empty()) return (uint32_t)-1;
    FILE* f = std::fopen(host.c_str(), fopenMode(q1));
    if (!f) return (uint32_t)-1;
    const uint32_t fd = allocFd();
    g_files.emplace(fd, f);
    return fd;
}
RT_STUB_FN(0x00288EC0, guest::sceOpen)

// ---- sceClose (FileIO.cpp:63-66 -> fioClose, Syscalls/FileIO.cpp:44-90) ----
uint32_t sceClose(uint32_t q0, ...) {
    auto it = g_files.find(q0);
    if (it == g_files.end()) return (uint32_t)-1;
    std::fclose(it->second);
    g_files.erase(it);
    return 0;
}
RT_STUB_FN(0x00289148, guest::sceClose)

// ---- sceLseek (FileIO.cpp:101-104 -> fioLseek, Syscalls/FileIO.cpp:197-245) ----
uint32_t sceLseek(uint32_t q0, uint32_t q1, uint32_t q2, ...) {
    auto it = g_files.find(q0);
    if (it == g_files.end()) return (uint32_t)-1;
    int whence;
    switch (q2) {  // PS2 FIO_SEEK_SET/CUR/END == host SEEK_SET/CUR/END
    case 0: whence = SEEK_SET; break;
    case 1: whence = SEEK_CUR; break;
    case 2: whence = SEEK_END; break;
    default: return (uint32_t)-1;
    }
    if (std::fseek(it->second, (int32_t)q1, whence) != 0) return (uint32_t)-1;
    const long pos = std::ftell(it->second);
    return pos < 0 ? (uint32_t)-1 : (uint32_t)pos;
}
RT_STUB_FN(0x002892C8, guest::sceLseek)

// ---- sceRead (FileIO.cpp:111-114 -> fioRead, Syscalls/FileIO.cpp:92-159) ----
uint32_t sceRead(uint32_t q0, uint32_t q1, uint32_t q2, ...) {
    auto it = g_files.find(q0);
    if (it == g_files.end()) return (uint32_t)-1;
    uint8_t* buf = (uint8_t*)mem(q1);
    if (!buf) return (uint32_t)-1;
    if (q2 == 0) return 0;
    const size_t n = std::fread(buf, 1, q2, it->second);
    return (uint32_t)n;
}
RT_STUB_FN(0x00289500, guest::sceRead)

// ---- sceWrite (FileIO.cpp:116-119 -> fioWrite, Syscalls/FileIO.cpp:161-195) ----
uint32_t sceWrite(uint32_t q0, uint32_t q1, uint32_t q2, ...) {
    auto it = g_files.find(q0);
    if (it == g_files.end()) return (uint32_t)-1;
    const uint8_t* buf = (const uint8_t*)mem(q1);
    if (!buf) return (uint32_t)-1;
    if (q2 == 0) return 0;
    const size_t n = std::fwrite(buf, 1, q2, it->second);
    return (uint32_t)n;
}
RT_STUB_FN(0x00289770, guest::sceWrite)

// ---- sceIoctl (FileIO.cpp:78-99): cmd==1 (HTCI wait) writes 0 to arg ----
uint32_t sceIoctl(uint32_t q0, uint32_t q1, uint32_t q2, ...) {
    const int32_t cmd = (int32_t)q1;
    if (cmd == 1 && q2 != 0u) {
        uint8_t* arg = (uint8_t*)mem(q2);
        if (!arg) return (uint32_t)-1;
        const uint32_t ready = 0;
        std::memcpy(arg, &ready, 4);
    }
    return 0;
}
RT_STUB_FN(0x00289A30, guest::sceIoctl)

}  // namespace guest
