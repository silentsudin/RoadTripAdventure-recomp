// VU0-A pins: instruction sequences from 0x0021d2d8, 0x0021d3b8 and 0x0021d3e0.
#include "../../src/native/guest/guest_vu0.h"
#include <cassert>
#include <cstring>

using namespace rtguest;
using namespace rtguest::vu0;

int main() {
  std::memset(kRam, 0, RT_GUEST_RAM_SIZE);
  S = State{}; S.vf[0] = {{0.f, 0.f, 0.f, 1.f}};
  // 0x0021d2d8: identity rows times (1,2,3,1), fixed-point F15 output.
  sqc2(0x100, {{32768.f, 65536.f, 98304.f, 32768.f}});
  sqc2(0x200, {{1.f, 0.f, 0.f, 0.f}}); sqc2(0x210, {{0.f, 1.f, 0.f, 0.f}});
  sqc2(0x220, {{0.f, 0.f, 1.f, 0.f}});
  S.vf[8] = lqc2(0x100); vitof15(0b1111, 8, 8);
  S.vf[4] = lqc2(0x200); S.vf[5] = lqc2(0x210); S.vf[6] = lqc2(0x220);
  vmulabc(0b1111, 0, 4, 8); vmaddabc(0b1111, 1, 5, 8); vmaddbc(0b1111, 2, 9, 6, 8);
  vftoi15(0b1111, 9, 9);
  assert(S.vf[9].v[0] == 32768.f && S.vf[9].v[1] == 65536.f && S.vf[9].v[2] == 98304.f);
  // 0x0021d3b8: three full quads land at indexed VU addresses 4, 5 and 6.
  S.vi[1] = 4; S.vf[1] = {{1,2,3,4}}; S.vf[2] = {{5,6,7,8}}; S.vf[3] = {{9,10,11,12}};
  vsqi(0b1111, 1, 1); vsqi(0b1111, 2, 1); vsqi(0b1111, 3, 1);
  assert(lqc2(64).v[0] == 1.f && lqc2(80).v[0] == 5.f && lqc2(96).v[0] == 9.f && S.vi[1] == 7);
  // 0x0021d470: vf0 destinations are architecturally ignored.
  const Vu4 vf0 = S.vf[0];
  vsub(0b1011, 12, 15, 12);
  assert(S.vf[0].v[0] == vf0.v[0] && S.vf[0].v[3] == vf0.v[3]);
  return 0;
}
