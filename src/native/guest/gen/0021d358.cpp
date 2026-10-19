// guest:0x0021d358 translated from Ghidra pseudocode and VU0 disassembly.
#include "../guest_vu0.h"
#include <cmath>
#include <cstring>

namespace guest {
uint32_t GetSpeedFromF15Vector(uint32_t param_1, ...) {
  using namespace rtguest::vu0;
  S.vf[4] = lqc2(param_1);
  vitof15(0b1111, 4, 4);
  vmul(0b1110, 4, 4, 4);
  vaddbc(0b1000, 2, 4, 4, 4);
  float result = std::sqrt(S.vf[4].v[0]) * 216.0f;
  uint32_t bits;
  std::memcpy(&bits, &result, 4);
  return bits;
}
RT_GUEST_FN(0x0021d358, guest::GetSpeedFromF15Vector)
}  // namespace guest
