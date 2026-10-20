#pragma once
// EE FPU ops whose rounding differs from the game thread's RZ+FZ mode.
#include <cstdint>

namespace rtguest {

// EE DIV.S rounds to NEAREST while add/sub/mul round toward zero (oracle
// ps2_runtime_macros.h ps2_fpu_div_s / ps2_fpu_round_nearest, matched bit for
// bit to PCSX2). Plain '/' in the RZ thread mode truncates one ulp low (W13-DIV:
// buildCamera frustum corners at cameraBlocks+0x100, 0x177e7d0..). x/0 = +-MAX
// (sign fs^ft, 0/0 too), 0/x = signed 0, denormals are zero. Rounded with
// integer ops after the double divide, so any host rounding mode gives the same.
inline float div_s(float fs, float ft) {
    uint32_t bs, bt;
    __builtin_memcpy(&bs, &fs, 4);
    __builtin_memcpy(&bt, &ft, 4);
    uint32_t r;
    if ((bt & 0x7F800000u) == 0u) {
        r = ((bs ^ bt) & 0x80000000u) | 0x7F7FFFFFu;
    } else if ((bs & 0x7F800000u) == 0u) {
        r = (bs ^ bt) & 0x80000000u;
    } else {
        const double d = (double)fs / (double)ft;
        uint64_t u;
        __builtin_memcpy(&u, &d, 8);
        u += 1ull << 28;  // half a float ulp in the double's encoding: truncating below = nearest
        const uint32_t sign = (uint32_t)(u >> 32) & 0x80000000u;
        const int e = (int)((u >> 52) & 0x7FF) - 1023 + 127;
        if (e <= 0) r = sign;                         // FZ: no denormals
        else if (e >= 255) r = sign | 0x7F7FFFFFu;    // EE overflow clamps to MAX
        else r = sign | ((uint32_t)e << 23) | (uint32_t)((u >> 29) & 0x7FFFFFu);
    }
    float f;
    __builtin_memcpy(&f, &r, 4);
    return f;
}

}  // namespace rtguest
