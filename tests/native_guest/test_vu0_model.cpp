// Pins for the VU0 macro model (src/native/guest/guest_vu0.h).
// Idiom sequences are copied from build/scratch/port/vu0_disasm.txt for the
// real game functions vecDotProductVu0 (0x2727c0) and vecNormalizeVu0
// (0x2727e8); expected values computed by hand from PS2 VU0 semantics.
#include "../../src/native/guest/guest_vu0.h"

#include <cassert>
#include <cmath>
#include <cstring>

using namespace rtguest;
using namespace rtguest::vu0;

static float fbits(uint32_t b) { float f; std::memcpy(&f, &b, 4); return f; }

int main() {
    std::memset(kRam, 0, RT_GUEST_RAM_SIZE);

    // vecDotProductVu0 replay: a=(1,2,3,1) b=(4,5,6,1) -> x = 1*4+2*5+3*6 = 32
    {
        Vu4 a{{1.f, 2.f, 3.f, 1.f}}, b{{4.f, 5.f, 6.f, 1.f}};
        sqc2(0x1000, a);
        sqc2(0x1010, b);
        S.vf[4] = lqc2(0x1000);                       // lqc2 vf4, 0(a0)
        S.vf[5] = lqc2(0x1010);                       // lqc2 vf5, 0(a1)
        vmul(0b1110, 5, 4, 5);                        // vmul.xyz vf5, vf4, vf5
        vaddbc(0b1000, 0b01, 5, 5, 5);                // vaddbc.y.x vf5, vf5, vf5 (bc=y)
        vaddbc(0b1000, 0b10, 5, 5, 5);                // vaddbc.z.x vf5, vf5, vf5 (bc=z)
        assert(S.vf[5].v[0] == 32.f);
    }

    // vecNormalizeVu0 replay on (3,4,0): len=5, reciprocal path via Q.
    {
        S = State{};
        S.vf[0] = Vu4{{0.f, 0.f, 0.f, 1.f}};
        Vu4 a{{3.f, 4.f, 0.f, 1.f}};
        sqc2(0x2000, a);
        S.vf[4] = lqc2(0x2000);                       // lqc2 vf4, 0(a1)
        vmul(0b1110, 5, 4, 4);                        // vmul.xyz vf5 = vf4*vf4 lane-wise
        vaddbc(0b1000, 0b01, 5, 5, 5);                // vaddbc.y.x: x += y  (25)
        vaddbc(0b1000, 0b10, 5, 5, 5);                // vaddbc.z.x: x += z
        vsqrt(0, 5);                                  // vsqrt Q, vf5.x  (Q = 5)
        assert(S.q == 5.f);
        vaddq(0b1000, 5, 0);                          // vaddq.x vf5, vf0, Q
        vdiv(0b11, 0, 0b00, 5);                       // vdiv Q, vf0.w, vf5.x (Q = 1/5)
        vwaitq();
        assert(S.q == 0.2f);
        vmulq(0b1110, 6, 4);                          // vmulq.xyz vf6 = vf4 * Q
        assert(S.vf[6].v[0] == 0.6f && S.vf[6].v[1] == 0.8f && S.vf[6].v[2] == 0.f);
    }

    // vf0 hard-wired init value.
    assert(S.vf[0].v[0] == 0.f && S.vf[0].v[3] == 1.f);

    // vdiv by zero -> sign | 0x7F7FFFFF (oracle ps2_vu_div).
    {
        S.vf[7] = Vu4{{1.f, -1.f, 0.f, 0.f}};
        vdiv(0, 7, 0, 0);                             // 1 / vf0.x(=0)
        assert(std::signbit(S.q) == false);
        uint32_t b; std::memcpy(&b, &S.q, 4);
        assert(b == 0x7F7FFFFFu);
        vdiv(0b01, 7, 0, 0);                          // -1 / 0
        std::memcpy(&b, &S.q, 4);
        assert(b == 0xFF7FFFFFu);
    }

    // ACC chain: vmulabc then vmaddabc (matrix-transform idiom from 0x2731c8).
    {
        S = State{};
        S.vf[0] = Vu4{{0.f, 0.f, 0.f, 1.f}};
        S.vf[4] = Vu4{{1.f, 2.f, 3.f, 1.f}};
        S.vf[5] = Vu4{{4.f, 5.f, 6.f, 1.f}};
        S.vf[8] = Vu4{{7.f, 8.f, 9.f, 10.f}};
        vmulabc(0b1111, 0b00, 4, 8);                  // ACC = vf4 * vf8.x (broadcast)
        assert(S.acc.v[0] == 7.f && S.acc.v[3] == 7.f);
        vmaddabc(0b1111, 0b01, 5, 8);                 // ACC += vf5 * vf8.y
        assert(S.acc.v[0] == 7.f + 32.f && S.acc.v[1] == 14.f + 40.f);
    }

    // dest-mask blend: .xz writes lanes x,z only.
    {
        S = State{};
        S.vf[0] = Vu4{{0.f, 0.f, 0.f, 1.f}};
        S.vf[1] = Vu4{{10.f, 20.f, 30.f, 40.f}};
        S.vf[2] = Vu4{{1.f, 1.f, 1.f, 1.f}};
        vadd(0b1010, 1, 1, 2);                        // vf1 += vf2, mask .xz
        assert(S.vf[1].v[0] == 11.f && S.vf[1].v[1] == 20.f);
        assert(S.vf[1].v[2] == 31.f && S.vf[1].v[3] == 40.f);
    }

    // vftoi4: scale 16 then trunc; >= 2^31 pins 0x7FFFFFFF bits.
    {
        S.vf[3] = Vu4{{1.5f, -1.5f, 1e30f, 268435455.9f}};  // 2^31/16 = 134217728
        vftoi4(0b1111, 10, 3);
        uint32_t b; std::memcpy(&b, &S.vf[10].v[0], 4);
        assert(b == 24u);                              // FTOI stores integer BITS
        std::memcpy(&b, &S.vf[10].v[1], 4);
        assert(b == 0xFFFFFFE8u);                      // -24 as bits
        std::memcpy(&b, &S.vf[10].v[2], 4);
        assert(b == 0x7FFFFFFFu);
        // 268435455.9 * 16 = 4.29e9 >= 2^31 -> also pinned high
        std::memcpy(&b, &S.vf[10].v[3], 4);
        assert(b == 0x7FFFFFFFu);
    }

    // vmr32: SSE shuffle (0,3,2,1) -> lanes (y,z,w,x).
    {
        S.vf[1] = Vu4{{1.f, 2.f, 3.f, 4.f}};
        vmr32(0b1111, 2, 1);
        assert(S.vf[2].v[0] == 2.f && S.vf[2].v[1] == 3.f && S.vf[2].v[2] == 4.f && S.vf[2].v[3] == 1.f);
    }

    // vmtir/vmfir round-trip through vi (oracle memcpy tricks).
    {
        S.vf[1] = Vu4{{0.f, 0.f, 0.f, 0.f}};
        float s = fbits(0x40001234u);                 // some float whose low16 = 0x1234
        S.vf[1].v[1] = s;
        vmtir(5, 0b01, 1);                            // vi5 = low16 bits of vf1.y
        assert(S.vi[5] == 0x1234);
        vmfir(0b1111, 3, 5);                          // vf3 = (int32)(int16)vi5 as float bits
        uint32_t b; std::memcpy(&b, &S.vf[3].v[0], 4);
        assert(b == 0x00001234u);
    }

    // viadd wraps at 16 bits.
    {
        S.vi[1] = 0xFFFF;
        S.vi[2] = 3;
        viadd(1, 1, 2);
        assert(S.vi[1] == 2);
    }

    // vlqi/vsqi: index (vi & 0x3FF) << 4 with post-increment.
    {
        S = State{};
        S.vf[0] = Vu4{{0.f, 0.f, 0.f, 1.f}};
        Vu4 q{{5.f, 6.f, 7.f, 8.f}};
        sqc2(16 * 3, q);                              // store raw for load-back
        S.vi[3] = 3;
        S.vf[9] = vlqi(9, 3);                         // vf9 = (vi3); vi3 -> 4
        assert(S.vf[9].v[0] == 5.f && S.vi[3] == 4);
        S.vf[1] = Vu4{{1.f, 1.f, 1.f, 1.f}};
        vsqi(0b1000, 1, 3);                           // store vf1 .x lane into (vi3=4)
        Vu4 back = lqc2(16 * 4);
        assert(back.v[0] == 1.f && back.v[1] == 0.f); // merged into the (zero) old quad
        assert(S.vi[3] == 5);
    }

    // cfc2/ctc2: vi cells and Q.
    {
        S = State{};
        S.vf[0] = Vu4{{0.f, 0.f, 0.f, 1.f}};
        ctc2(1, 0x1FFF);
        assert(cfc2(1) == 0x1FFF);
        ctc2(0, 7);                                   // vi0 writes ignored (oracle)
        assert(cfc2(0) == 0);
        float qq = 2.5f;
        uint32_t qb; std::memcpy(&qb, &qq, 4);
        ctc2(22, qb);
        assert(S.q == 2.5f);
    }

    return 0;
}
