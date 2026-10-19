// Guest-image memory + indirect-call registry (see guest_shim.h).
#include "guest_shim.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

namespace rtguest {

uint8_t kRam[RT_GUEST_RAM_SIZE] = {};
uint8_t kScratch[RT_SCRATCH_SIZE] = {};

// printf shim for the corpus (see guest_shim.h): %s args are guest addresses.
int rtPrintf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int total = 0;
    for (const char* p = fmt; *p;) {
        const char* pct = static_cast<const char*>(memchr(p, '%', strlen(p)));
        if (!pct) { fputs(p, stdout); total += (int)strlen(p); break; }
        fwrite(p, 1, (size_t)(pct - p), stdout);
        total += (int)(pct - p);
        const char* spec = pct + 1;
        if (*spec == '%') { fputc('%', stdout); ++total; p = spec + 1; continue; }
        while (*spec && (strchr("-+ #0123456789.*", *spec))) ++spec;
        bool lng = false;
        while (*spec == 'l' || *spec == 'h') { lng = *spec == 'l' ? true : lng; ++spec; }
        const char specCh = *spec ? *spec++ : 0;
        std::string piece(pct, spec);  // "%...spec"
        char buf[640];
        int n = -1;
        switch (specCh) {
        case 's': {
            const uint32_t a = (uint32_t)(uintptr_t)va_arg(ap, void*);
            n = snprintf(buf, sizeof buf, piece.c_str(), (const char*)G(a));
            break;
        }
        case 'd': case 'i': case 'u': case 'x': case 'X': case 'o': {
            if (lng) n = snprintf(buf, sizeof buf, piece.c_str(), va_arg(ap, long long));
            else n = snprintf(buf, sizeof buf, piece.c_str(), va_arg(ap, int));
            break;
        }
        case 'c': n = snprintf(buf, sizeof buf, piece.c_str(), va_arg(ap, int)); break;
        case 'f': case 'F': case 'g': case 'G': case 'e': case 'E':
            n = snprintf(buf, sizeof buf, piece.c_str(), va_arg(ap, double)); break;
        case 'p': n = snprintf(buf, sizeof buf, piece.c_str(), va_arg(ap, void*)); break;
        case 'n': (void)va_arg(ap, int*); break;  // not printed; guest won't use it
        case 0: break;
        default:  // unknown: echo literally, no arg consumed
            piece.assign(pct, 2); n = snprintf(buf, sizeof buf, "%%%c", specCh); break;
        }
        if (n > 0) { fwrite(buf, 1, (size_t)n, stdout); total += n; }
        p = spec;
    }
    va_end(ap);
    return total;
}

static std::unordered_map<uint32_t, uint32_t>& faultMap() {  // Meyers: RT_GUEST_FN ctors may run first
    static std::unordered_map<uint32_t, uint32_t> m;
    return m;
}
void noteFault(uint32_t a) { ++faultMap()[a]; }
const Fault* faults(size_t* n) {
    static std::vector<Fault> out;
    out.clear();
    for (auto& kv : faultMap()) out.push_back({kv.first, kv.second});
    *n = out.size();
    return out.empty() ? nullptr : out.data();
}

static std::unordered_map<uint32_t, void*>& g_table() {
    static std::unordered_map<uint32_t, void*> t;
    return t;
}
void registerGuest(uint32_t addr, void* fn) { g_table()[addr] = fn; }
uint64_t fnEntry(uint32_t addr) {
    auto it = g_table().find(addr);
    return it == g_table().end() ? 0 : reinterpret_cast<uint64_t>(it->second);
}
uint64_t callGuest(uint32_t addr, uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3,
                   uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7) {
    uint64_t e = fnEntry(addr);
    if (!e) { noteFault(addr | 0xF0000000u); return 0; }
    return reinterpret_cast<GuestFnPtr>(e)(a0, a1, a2, a3, a4, a5, a6, a7);
}

// ELF32 section copy into the image (boot ELF is the user's own; never committed).
bool loadImage(const uint8_t* f, size_t sz) {
    if (sz < 0x34 || memcmp(f, "\x7f" "ELF\x01\x01\x01", 6) != 0) return false;
    auto rd = [&](size_t o) { uint32_t v; memcpy(&v, f + o, 4); return v; };
    uint32_t shoff = rd(0x20);
    uint16_t shentsize, shnum, shstrndx;
    memcpy(&shentsize, f + 0x2e, 2); memcpy(&shnum, f + 0x30, 2); memcpy(&shstrndx, f + 0x32, 2);
    uint32_t strtab = rd(shoff + shstrndx * shentsize + 0x10);
    for (uint16_t i = 0; i < shnum; ++i) {
        size_t e = shoff + i * (size_t)shentsize;
        uint32_t type = rd(e + 4), addr = rd(e + 0xc), off = rd(e + 0x10), size = rd(e + 0x14);
        if ((type == 1 /*S_PROGBITS*/ || type == 8 /*S_NOBITS*/) && size &&
            addr >= 0x80000000u) addr -= 0x80000000u;
        if (type != 1 && type != 8) continue;
        if (addr + size > RT_GUEST_RAM_SIZE || (type == 1 && off + size > sz)) return false;
        if (type == 1) memcpy(kRam + addr, f + off, size);  // .bss stays zero
    }
    return true;
}

static uint32_t g_frameTop = RT_FRAME_BASE;

uint32_t allocFrame(uint32_t bytes) {
    bytes = (bytes + 15) & ~15u;
    uint32_t a = g_frameTop;
    if (a + bytes > RT_FRAME_END) {
        fprintf(stderr, "[rtguest] frame arena exhausted\n");
        abort();
    }
    g_frameTop = a + bytes;
    return a;
}

}  // namespace rtguest
