// Guest-image shim for transliterated Ghidra pseudocode (compiled-game program, W10+).
// See docs/native-reference/compiled-game-plan.md. Transliterated TUs compile ~1:1
// against this shim; guest pointers stay 32-bit addresses, every deref routes via G().
#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cmath>

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

// Frame arena = the EE's own stack, mirrored: the ELF declares the main
// thread stack as [0x1F80000,0x2000000); transliterated frames push down from
// its top with the guest's exact frame sizes (FrameGuard(size) at each
// generated function entry, allocFrame carving the emitted slot chunks).
// Absolute frame addresses then match the oracle's real stack, so residue-
// reading code (the attract camera chain reads aliased slots written by other
// frames at the same depth) and stack pointers stored in RAM agree bit-exactly.
// Extra green threads get pool regions below the main stack (the game's own
// tasks run scheduler-level on the main stack, like the oracle).
#define RT_EE_STACK_TOP   0x02000000u
#define RT_EE_STACK_FLOOR 0x01F80000u
#define RT_FRAME_BASE     0x01E00000u  // thread pool low bound
#define RT_FRAME_END      0x01F80000u  // thread pool high bound
uint32_t allocFrame(uint32_t bytes);
uint32_t frameTop();
void setFrameTop(uint32_t t);
void setFrameFloor(uint32_t f);
void framePush(uint32_t size);
void framePop(uint32_t saved);
uint32_t allocTaskFrameRegion(uint32_t capBytes);  // returns region end (top)
// Guest link register model: the machine jal PC of the last call (set by
// callers via callRA below); callees' prologue $ra saves store it, matching
// the oracle's saved-return-address residue in frame slots.
uint32_t pendingRA();
void setPendingRA(uint32_t r);
#define RT_CALLRA(pc, ...) (rtguest::setPendingRA(pc), (__VA_ARGS__))
struct FrameGuard {
    uint32_t saved;
    uint32_t rtRA;  // guest return address of the call that entered this frame
    explicit FrameGuard(uint32_t size) : saved(frameTop()), rtRA(pendingRA()) { framePush(size); }
    ~FrameGuard() { framePop(saved); }
};

// Guest float register bits as float, for float-typed callee params
// (pseudocode prints raw bits; a plain int literal would value-convert).
inline float fbits(uint32_t b) { float f; __builtin_memcpy(&f, &b, 4); return f; }

// Reverse: host float value as guest float-register bits. Guest math TUs take
// and return f-register bit patterns (register transfers never value-convert);
// C++ float params would truncate, so callers carry bits through uint32_t.
inline uint32_t f2bits(float f) { uint32_t b; __builtin_memcpy(&b, &f, 4); return b; }

// Same pair for doubles ($f12/$f13 = 64-bit bit pattern): math TUs carry
// doubles as uint64_t bits, never as C++ double params/returns.
inline double dbits(uint64_t b) { double d; __builtin_memcpy(&d, &b, 8); return d; }
inline uint64_t d2bits(double d) { uint64_t b; __builtin_memcpy(&b, &d, 8); return b; }

// DIV.S rounds to NEAREST on the oracle (ps2xRuntime ps2_runtime_macros.h:872-889):
// under the thread's round-toward-zero FPCR the double quotient is truncated;
// adding half a float ulp to the double's bits and truncating to float gives the
// correctly-rounded float result. ADD/SUB/MUL stay RZ plain operators.
inline float f_round_nearest(double d) {
  uint64_t u; __builtin_memcpy(&u, &d, 8); u += (1ull << 28); __builtin_memcpy(&d, &u, 8);
  return static_cast<float>(d);
}
inline float fdiv(float a, float b) { return f_round_nearest((double)a / (double)b); }

// Translate a guest address to a host pointer (cached/uncached aliases, scratchpad).
inline void* G(uint32_t a) {
    if (a >= 0x80000000u) a -= 0x80000000u;      // KSEG1 uncached alias
    // R5900 uncached-low alias: 0x2000_0000-0x3FFF_FFFF is physical & 0x1FFF_FFFF
    // (0x2177xxxx = FS RPC bufs; 0x3F80xxxx = DMAC regs → same kRam holes).
    if (a >= 0x20000000u && a < 0x40000000u) a &= 0x1FFFFFFFu;
    if (a < RT_GUEST_RAM_SIZE) return kRam + a;
    if (a >= 0x70000000u && a < 0x70000000u + RT_SCRATCH_SIZE)
        return kScratch + (a - 0x70000000u);
    noteFault(a);
    return kScratch;  // quarantine: reads stay deterministic, hashes will expose it
}

// Load the boot ELF's sections into the image (user's own file; never committed).
bool loadImage(const uint8_t* elfFile, size_t size);

// Host printf for guest debug strings: fmt is host-mapped already; each %s
// argument is a GUEST address (the corpus passes u32/u64 values), mapped via
// G(). shortcut: covers the conversion set the game's debug strings use
// (diuxXoclpsf with l modifiers); revisit if a scene prints exotic specs.
int rtPrintf(const char* fmt, ...);
// sprintf whose varargs live in a guest u64-slot block (Ghida's spilled
// vararg area; see gen/00271310 textGridPrintf). dst/fmt are host pointers.
int rtSprintfGargs(char* dst, const char* fmt, uint32_t gargs);

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

// W12 MMIO staging (docs/native-reference/w12-kernel-design.md): a DMAC CHCR
// slot with a write trap, so the guest's own kick stores (sendTexturePackets
// 0x207b50, vifKick 0x2271f0 writing CHCR.STR through the Ghidra-named reg
// globals) complete the transfer — the oracle sees those stores in
// PS2Memory's DMAC device model. Completion auto-clears STR, which is what
// unblocks the guest wait-loops polling CHCR & 0x100. Body in
// guest_libs_dma.cpp; the rest of the register file stays plain uint32_t
// staging slots.
struct MmioReg {
    uint32_t addr;
    uint32_t value = 0;
    operator uint32_t() const { return value; }
    MmioReg& operator=(uint32_t x);
};

}  // namespace rtguest

namespace guest { void eeDmaChcrTrap(uint32_t regAddr, uint32_t x); }

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

// Ghidra prints MIPS intrinsics (sqrt.s etc.) as calls; the generated decl
// file's uint32 pass-through stubs would value-truncate float arguments, so
// exact-match float overloads here win resolution. sqrt.s/ABS follow the
// thread's EE float mode like the oracle's SSE ops.
namespace guest {
inline float SQRT(float x) { return sqrtf(x); }
inline float ABS(float x) { return x < 0.0f ? -x : x; }

// W12 EE libkernel: the syscall stub TUs forward their register args (a0..a7
// = $4..$7) to the host dispatcher; num is the raw li-v0 value from the stub
// (negative render = sign-extended = interruptSafe "i*" variant). Implemented
// in guest_kernel.cpp, semantics mirrored from the oracle ps2xRuntime
// EeScheduler + Syscalls (oracle = reference only, never linked here).
int32_t syscall(uint64_t num, uint32_t a0 = 0, uint32_t a1 = 0, uint32_t a2 = 0,
                uint32_t a3 = 0, uint32_t a4 = 0, uint32_t a5 = 0,
                uint32_t a6 = 0, uint32_t a7 = 0);
}
