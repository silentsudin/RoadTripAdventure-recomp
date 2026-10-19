#include "../../src/native/guest/guest_vu0.h"
#include <cassert>
#include <cmath>
#include <cstring>

namespace guest {
uint32_t vecDotProductVu0(uint32_t, uint32_t);
void vecNormalizeVu0(uint32_t, uint32_t);
void vu0RotTransPers(uint32_t, uint32_t, uint32_t, uint32_t);
}
using namespace rtguest;
using namespace rtguest::vu0;

static uint32_t bits(float x) { uint32_t b; std::memcpy(&b, &x, 4); return b; }
int main() {
  std::memset(kRam, 0, RT_GUEST_RAM_SIZE);
  S = State{}; S.vf[0] = {{0.f, 0.f, 0.f, 1.f}};
  sqc2(0x1000, {{1.f,2.f,3.f,4.f}}); sqc2(0x1010, {{5.f,6.f,7.f,8.f}});
  assert(bits(70.f) == guest::vecDotProductVu0(0x1000, 0x1010));
  sqc2(0x1020, {{3.f,4.f,0.f,13.f}}); guest::vecNormalizeVu0(0x1030, 0x1020);
  const Vu4 n = lqc2(0x1030); assert(std::fabs(n.v[0] - 3.f/std::sqrt(194.f)) < 1e-6f);
  assert(std::fabs(n.v[1] - 4.f/std::sqrt(194.f)) < 1e-6f && n.v[2] == 0.f && n.v[3] == 0.f);
  sqc2(0x1100, {{1,0,0,0}}); sqc2(0x1110, {{0,1,0,0}}); sqc2(0x1120, {{0,0,1,0}}); sqc2(0x1130, {{0,0,0,1}});
  sqc2(0x1140, {{2,4,1,2}}); guest::vu0RotTransPers(0x1150,0x1100,0x1140,0);
  const Vu4 p=lqc2(0x1150); assert(bits(p.v[0])==16 && bits(p.v[1])==32 && bits(p.v[2])==0 && bits(p.v[3])==2);
}
