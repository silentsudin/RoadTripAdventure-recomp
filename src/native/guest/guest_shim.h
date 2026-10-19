// Guest-image shim for transliterated Ghidra pseudocode (compiled-game program, W10+).
// See docs/native-reference/compiled-game-plan.md. Transliterated TUs compile ~1:1
// against this shim; guest pointers stay 32-bit addresses, every deref routes via G().
#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>

// Ghidra pseudocode types (r5900:LE:32). long/ulong stay host-native (64-bit on
// macOS, matching MIPS64 registers); fix from the token inventory if Ghidra emitted
// 32-bit long semantics.
typedef unsigned char undefined;
typedef signed char undefined1;   // Ghidra prints byte loads signed where it matters
typedef uint16_t undefined2;
typedef uint32_t undefined3;
typedef uint32_t undefined4;
typedef uint64_t undefined5;
typedef uint64_t undefined6;
typedef uint64_t undefined7;
typedef uint64_t undefined8;
typedef signed char byte;
typedef uint8_t uchar;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef uint64_t ulong;
typedef uint32_t code;  // Ghidra's function-pointer type: guest code = u32 address

#define RT_GUEST_RAM_SIZE 0x02000000u  // 32 MB main RAM
#define RT_SCRATCH_SIZE 0x4000u        // EE scratchpad at 0x70000000

namespace rtguest {

extern uint8_t kRam[RT_GUEST_RAM_SIZE];
extern uint8_t kScratch[RT_SCRATCH_SIZE];

// One fault counter per unmapped address class; G() never traps the host.
struct Fault { uint32_t addr; uint32_t count; };
const Fault* faults(size_t* n);
void noteFault(uint32_t a);

// Bump arena inside guest RAM for transliterated stack arrays (auStack_* locals get
// guest addresses so they can be passed to other guest fns).
// shortcut: never freed or unwound; replace with the real EE stack model at W12 boot.
#define RT_FRAME_BASE 0x01E00000u
#define RT_FRAME_END  0x02000000u
uint32_t allocFrame(uint32_t bytes);

// Guest float register bits as float, for float-typed callee params
// (pseudocode prints raw bits; a plain int literal would value-convert).
inline float fbits(uint32_t b) { float f; __builtin_memcpy(&f, &b, 4); return f; }

// Translate a guest address to a host pointer (cached/uncached aliases, scratchpad).
inline void* G(uint32_t a) {
    if (a >= 0x80000000u) a -= 0x80000000u;      // KSEG1 uncached alias
    if (a < RT_GUEST_RAM_SIZE) return kRam + a;
    if (a >= 0x70000000u && a < 0x70000000u + RT_SCRATCH_SIZE)
        return kScratch + (a - 0x70000000u);
    noteFault(a);
    return kScratch;  // quarantine: reads stay deterministic, hashes will expose it
}

// Load the boot ELF's sections into the image (user's own file; never committed).
bool loadImage(const uint8_t* elfFile, size_t size);

// Indirect-call registry: OUR table of translated functions (plan doc, decision 2).
// Entries stored as void* (dlsym pattern): signatures vary per function as ports
// widen 64-bit params, so no function-pointer-to-function-pointer cast is made.
void registerGuest(uint32_t addr, void* fn);
typedef uint64_t (*GuestFnPtr)(uint64_t, uint64_t, uint64_t, uint64_t,
                               uint64_t, uint64_t, uint64_t, uint64_t);
uint64_t callGuest(uint32_t addr, uint64_t a0 = 0, uint64_t a1 = 0,
                   uint64_t a2 = 0, uint64_t a3 = 0, uint64_t a4 = 0,
                   uint64_t a5 = 0, uint64_t a6 = 0, uint64_t a7 = 0);
uint64_t fnEntry(uint32_t addr);  // lookup only, 0 if absent

}  // namespace rtguest

// Transliterator-emitted registration.
#define RT_GUEST_FN(addr, name)                                        \
    namespace { struct RtReg_ { RtReg_() {                             \
        rtguest::registerGuest(addr, reinterpret_cast<void*>(&name)); \
    } } rt_reg_; }

// Ghidra expression tokens. CONCATxy(a,b): a is x bytes (high), b is y bytes (low).
#define CONCAT11(a, b) ((uint32_t)(uint8_t)(a) << 8 | (uint32_t)(uint8_t)(b))
#define CONCAT12(a, b) ((uint32_t)(uint8_t)(a) << 16 | (uint32_t)(uint16_t)(b))
#define CONCAT13(a, b) ((uint32_t)(uint8_t)(a) << 24 | ((uint32_t)(b) & 0xffffffu))
#define CONCAT14(a, b) ((uint64_t)(uint8_t)(a) << 32 | (uint32_t)(b))
#define CONCAT15(a, b) ((uint64_t)(uint8_t)(a) << 40 | ((uint64_t)(b) & 0xffffffffffull))
#define CONCAT16(a, b) ((uint64_t)(uint8_t)(a) << 48 | ((uint64_t)(b) & 0xffffffffffffull))
#define CONCAT17(a, b) ((uint64_t)(uint8_t)(a) << 56 | ((uint64_t)(b) & 0xffffffffffffffull))
#define CONCAT18(a, b) ((uint64_t)(uint8_t)(a) << 56 | (uint64_t)(b))
#define CONCAT21(a, b) ((uint32_t)(uint16_t)(a) << 8 | (uint32_t)(uint8_t)(b))
#define CONCAT22(a, b) ((uint32_t)(uint16_t)(a) << 16 | (uint32_t)(uint16_t)(b))
#define CONCAT23(a, b) ((uint32_t)(uint16_t)(a) << 24 | ((uint32_t)(b) & 0xffffffu))
#define CONCAT24(a, b) ((uint64_t)(uint16_t)(a) << 32 | (uint32_t)(b))
#define CONCAT26(a, b) ((uint64_t)(uint16_t)(a) << 48 | ((uint64_t)(b) & 0xffffffffffffull))
#define CONCAT28(a, b) ((uint64_t)(uint16_t)(a) << 64 | (uint64_t)(b))
#define CONCAT31(a, b) ((uint32_t)((uint32_t)(a) & 0xffffffu) << 8 | (uint32_t)(uint8_t)(b))
#define CONCAT32(a, b) ((uint32_t)((uint32_t)(a) & 0xffffffu) << 16 | (uint32_t)(uint16_t)(b))
#define CONCAT34(a, b) ((uint64_t)((uint32_t)(a) & 0xffffffu) << 32 | (uint32_t)(b))
#define CONCAT41(a, b) ((uint64_t)(uint32_t)(a) << 8 | (uint32_t)(uint8_t)(b))
#define CONCAT42(a, b) ((uint64_t)(uint32_t)(a) << 16 | (uint32_t)(uint16_t)(b))
#define CONCAT43(a, b) ((uint64_t)(uint32_t)(a) << 24 | ((uint32_t)(b) & 0xffffffu))
#define CONCAT44(a, b) ((uint64_t)(uint32_t)(a) << 32 | (uint32_t)(b))
#define CONCAT48(a, b) ((uint64_t)(uint32_t)(a) << 64 | (uint64_t)(b))
#define CONCAT51(a, b) ((uint64_t)((uint64_t)(a) & 0xffffffffffull) << 8 | (uint32_t)(uint8_t)(b))
#define CONCAT61(a, b) ((uint64_t)((uint64_t)(a) & 0xffffffffffffull) << 8 | (uint32_t)(uint8_t)(b))
#define CONCAT62(a, b) ((uint64_t)((uint64_t)(a) & 0xffffffffffffull) << 16 | (uint32_t)(uint16_t)(b))
#define CONCAT71(a, b) ((uint64_t)((uint64_t)(a) & 0x7fffffffffffffull) << 8 | (uint32_t)(uint8_t)(b))
#define CONCAT81(a, b) ((uint64_t)(uint64_t)(a) << 8 | (uint32_t)(uint8_t)(b))
#define CONCAT82(a, b) ((uint64_t)(uint64_t)(a) << 16 | (uint64_t)(uint16_t)(b))
#define CONCAT84(a, b) ((uint64_t)(uint64_t)(a) << 32 | (uint32_t)(b))
#define CONCAT88(a, b) ((uint64_t)(uint64_t)(a) << 64 | (uint64_t)(b))

// SUBxy(a, i): i-th x-byte slice of a (y bytes wide) — byte/halfword extraction.
#define SUB11(a, i) ((uint32_t)(uint8_t)(a))
#define SUB21(a, i) ((uint32_t)((uint16_t)(a) >> (8 * ((i) & 1))) & 0xffu)
#define SUB22(a, i) ((uint32_t)(uint16_t)(a))
#define SUB41(a, i) (((uint32_t)(a) >> (8 * ((i) & 3))) & 0xffu)
#define SUB42(a, i) (((uint32_t)(a) >> (16 * ((i) & 1))) & 0xffffu)
#define SUB44(a, i) ((uint32_t)(a))
#define SUB81(a, i) (((uint64_t)(a) >> (8 * ((i) & 7))) & 0xffu)
#define SUB82(a, i) (((uint64_t)(a) >> (16 * ((i) & 3))) & 0xffffu)
#define SUB84(a, i) (((uint64_t)(a) >> (32 * ((i) & 1))) & 0xffffffffu)
#define SUB88(a, i) ((uint64_t)(a))
// shortcut: 16-byte packed vectors unmodeled; scalar slice-0 shape, W11 packed ports
#define SUB168(a, i) ((uint64_t)(a))

// Ghidra's NaN token: a compare against an actual NaN constant (== is false, != true).
static const uint32_t kGhNaNBits = 0x7fc00000u;
inline float ghNaN() { float f; memcpy(&f, &kGhNaNBits, 4); return f; }
#define NaN (ghNaN())

// Ghidra sign/zero-extension display tokens (compile as plain casts when present).
#define ZEXT(x) (x)
#define SEXT(x) (x)

// Indirect-call rewriting helper: *(code *)(EXPR)(...) -> rtCall(EXPR, ...).
inline uint64_t rtCall(uint32_t target, uint64_t a0 = 0, uint64_t a1 = 0,
                       uint64_t a2 = 0, uint64_t a3 = 0, uint64_t a4 = 0,
                       uint64_t a5 = 0, uint64_t a6 = 0, uint64_t a7 = 0) {
    return rtguest::callGuest(target, a0, a1, a2, a3, a4, a5, a6, a7);
}
