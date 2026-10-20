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

// ELF32 PT_LOAD segment copy into the image, as the oracle's LoadExec does
// (section walking skipped SREGINFO/.mdebug bytes between segments, leaving
// 0x29a840-0x29a847 stale). Boot ELF is the user's own; never committed.
bool loadImage(const uint8_t* f, size_t sz) {
    if (sz < 0x34 || memcmp(f, "\x7f" "ELF\x01\x01\x01", 6) != 0) return false;
    auto rd = [&](size_t o) { uint32_t v; memcpy(&v, f + o, 4); return v; };
    uint32_t phoff = rd(0x1c);
    uint16_t phentsize, phnum;
    memcpy(&phentsize, f + 0x2a, 2); memcpy(&phnum, f + 0x2c, 2);
    for (uint16_t i = 0; i < phnum; ++i) {
        size_t e = phoff + i * (size_t)phentsize;
        if (rd(e) != 1 /*PT_LOAD*/) continue;
        uint32_t off = rd(e + 4), addr = rd(e + 8), filesz = rd(e + 0x10), memsz = rd(e + 0x14);
        if (addr >= 0x80000000u) addr -= 0x80000000u;
        if (addr + memsz > RT_GUEST_RAM_SIZE || off + filesz > sz || filesz > memsz) return false;
        std::memset(kRam + addr + filesz, 0, memsz - filesz);  // .bss stays zero
        std::memcpy(kRam + addr, f + off, filesz);
    }
    return true;
}

// EE stack mirror: frames push down from the ELF's declared main-stack top;
// see guest_shim.h. Per green thread: [frameFloor, frameTop); the game's own
// tasks run scheduler-level on the main thread's stack like the oracle.
static uint32_t g_frameTop = RT_EE_STACK_TOP;
static uint32_t g_frameCursor = RT_EE_STACK_TOP;
static uint32_t g_frameCeil = RT_EE_STACK_TOP;
static uint32_t g_frameFloor = RT_EE_STACK_FLOOR;

uint32_t frameTop() { return g_frameTop; }
void setFrameTop(uint32_t t) { g_frameTop = t; g_frameCursor = t; g_frameCeil = t; }
void setFrameFloor(uint32_t f) { g_frameFloor = f; }

static uint32_t g_pendingRA = 0;
uint32_t pendingRA() { return g_pendingRA; }
void setPendingRA(uint32_t r) { g_pendingRA = r + 8u; }  // MIPS return = jal+8 (oracle stack residue: jal@0x2089c0 leaves ra 0x2089c8)

void framePush(uint32_t size) {
    uint32_t base = g_frameTop - size;
    if (size > g_frameTop || base < g_frameFloor) {
        fprintf(stderr, "[rtguest] EE stack underflow (top=%08x size=%x)\n", g_frameTop, size);
        abort();
    }
    g_frameCeil = g_frameTop;
    g_frameTop = base;
    g_frameCursor = base;
}

void framePop(uint32_t saved) { g_frameTop = saved; g_frameCursor = saved; g_frameCeil = saved; }

uint32_t allocTaskFrameRegion(uint32_t capBytes) {  // returns the region's top
    static uint32_t s_pool = RT_FRAME_END;
    if (s_pool < RT_FRAME_BASE + capBytes) {
        fprintf(stderr, "[rtguest] frame region pool exhausted\n");
        abort();
    }
    s_pool -= capBytes;
    return s_pool + capBytes;
}

uint32_t allocFrame(uint32_t bytes) {
    // Chunks carve up from the frame base in emitted (ascending-offset) order;
    // FrameGuard(size) pre-pushed the exact guest frame size, so slot addresses
    // equal the oracle's sp+offset. No chunk padding here: the transliterator's
    // gap chunks encode exact guest frame offsets, and adjacent slots (e.g. a
    // float[5] followed by the scalar matrix elements buildCamera reads as one
    // 4x4) must stay contiguous; FrameGuard(size) already carries the frame's
    // own alignment.
    uint32_t a = g_frameCursor;
    g_frameCursor = a + bytes;
    if (g_frameCursor > g_frameCeil) {
        fprintf(stderr, "[rtguest] frame chunks overflow pushed frame (%x)\n", bytes);
        abort();
    }
    return a;
}

}  // namespace rtguest
