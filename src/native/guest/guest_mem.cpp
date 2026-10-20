// Guest-image memory + indirect-call registry (see guest_shim.h).
#include "guest_shim.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

#include <dlfcn.h>   // RT_FAULT_BT symbolization (same as cd_trace_bt)
#include <execinfo.h>

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

// vsprintf variant where the args are u64 slots in a guest block at gargs.
// shortcut: same directive coverage as rtPrintf (game uses %d %02d %X %s %c);
// upgrade to a full parser if a caller needs %*$/wide chars.
int rtSprintfGargs(char* dst, const char* fmt, uint32_t gargs) {
    char* out = dst;
    int nargs = 0;
    for (const char* p = fmt; *p;) {
        const char* pct = static_cast<const char*>(memchr(p, '%', strlen(p)));
        if (!pct) { strcpy(out, p); out += strlen(p); break; }
        memcpy(out, p, (size_t)(pct - p));
        out += pct - p;
        const char* spec = pct + 1;
        if (*spec == '%') { *out++ = '%'; p = spec + 1; continue; }
        while (*spec && (strchr("-+ #0123456789.", *spec))) ++spec;
        while (*spec == 'l' || *spec == 'h') ++spec;  // host gets 32-bit values anyway
        const char* flagsEnd = spec;  // end of flags/width, before conv char
        const char specCh = *spec ? *spec++ : 0;
        std::string piece(pct, flagsEnd);
        piece += specCh;
        uint64_t a = 0;
        std::memcpy(&a, G(gargs + (uint32_t)nargs * 8), 8);
        ++nargs;
        int n = -1;
        switch (specCh) {
        case 's': n = sprintf(out, piece.c_str(), (const char*)G((uint32_t)a)); break;
        case 'd': case 'i': n = sprintf(out, piece.c_str(), (int32_t)a); break;
        case 'u': case 'x': case 'X': case 'o': n = sprintf(out, piece.c_str(), (uint32_t)a); break;
        case 'c': n = sprintf(out, piece.c_str(), (int)(a & 0xFF)); break;
        case 'f': case 'F': case 'g': case 'G': case 'e': case 'E':
            n = sprintf(out, piece.c_str(), (double)a); break;  // guest never fp here; slot read as int64
        case 0: break;
        default: n = sprintf(out, "%%%c", specCh); break;  // echo, no guest slot consumed below
        }
        if (n < 0) n = 0;
        out += n;
        p = spec;
    }
    *out = 0;
    return (int)(out - dst);
}

static std::unordered_map<uint32_t, uint32_t>& faultMap() {  // Meyers: RT_GUEST_FN ctors may run first
    static std::unordered_map<uint32_t, uint32_t> m;
    return m;
}
void noteFault(uint32_t a) {
    uint32_t& c = ++faultMap()[a];
    if (c == 1 && std::getenv("RT_FAULT_BT")) {
        std::fprintf(stderr, "[faultbt] key=0x%x\n", a);
        void* frames[14];
        const int nFr = backtrace(frames, 14);
        static uintptr_t slide = 0;
        if (!slide) {
            Dl_info self;
            if (dladdr((void*)&noteFault, &self)) {
                slide = (uintptr_t)self.dli_fbase;
                std::fprintf(stderr, "    slide=%p\n", (void*)slide);
            }
        }
        for (int i = 1; i < nFr; ++i) {
            Dl_info di;
            const char* nm = (dladdr(frames[i], &di) && di.dli_sname) ? di.dli_sname : "?";
            std::fprintf(stderr, "    #%d %s off=%#lx\n", i, nm,
                         (unsigned long)((uintptr_t)frames[i] - slide));
        }
    }
}
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

// shortcut (W12): the bump arena is released wholesale per vblank instead of
// per function return (real per-thread EE stack model is W13). Arena pointers
// must not outlive their vblank; det pair + soak watch this.
void resetFrameArena() { g_frameTop = RT_FRAME_BASE; }

uint32_t frameTop() { return g_frameTop; }
void setFrameTop(uint32_t t) { g_frameTop = t; }

uint32_t allocTaskFrameRegion(uint32_t capBytes) {
    static uint32_t s_poolTop = RT_FRAME_BASE;
    uint32_t a = s_poolTop;
    if (a + capBytes > RT_FRAME_END) {
        fprintf(stderr, "[rtguest] frame region pool exhausted\n");
        abort();
    }
    s_poolTop = a + capBytes;
    return a;
}

uint32_t allocFrame(uint32_t bytes) {
    // 4-byte align only: the transliterator's per-TU gap chunks encode exact
    // guest frame offsets, and adjacent slots (e.g. a float[5] followed by the
    // scalar matrix elements buildCamera reads as one 4x4) must stay contiguous.
    // 16-byte padding split them and silently zeroed projection terms.
    bytes = (bytes + 3) & ~3u;
    uint32_t a = g_frameTop;
    if (a + bytes > RT_FRAME_END) {
        fprintf(stderr, "[rtguest] frame arena exhausted\n");
        abort();
    }
    g_frameTop = a + bytes;
    return a;
}

}  // namespace rtguest
