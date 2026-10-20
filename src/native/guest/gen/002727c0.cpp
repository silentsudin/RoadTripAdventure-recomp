// guest:0x002727c0 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"

namespace guest {
using namespace rtguest::vu0;

uint32_t vecDotProductVu0(uint32_t a,uint32_t b) {
  rtguest::FrameGuard rt_fg(0);
S.vf[4]=lqc2(a); S.vf[5]=lqc2(b); vmul(14,5,4,5); vaddbc(8,1,5,5,5); vaddbc(8,2,5,5,5); uint32_t r; std::memcpy(&r,&S.vf[5].v[0],4); return r;
}

RT_GUEST_FN(0x002727c0, guest::vecDotProductVu0)

}  // namespace guest
