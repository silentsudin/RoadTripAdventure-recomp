// guest:0x0021d310 translated from Ghidra pseudocode and VU0 disassembly.
#include "../guest_vu0.h"
#include <cmath>
#include <cstring>
#include <limits>

namespace guest {
uint32_t VecSubtractAndLength(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  using namespace rtguest::vu0;
  uint16_t a[8], b[8], difference[8];
  std::memcpy(a, rtguest::G(param_2), 16);
  std::memcpy(b, rtguest::G(param_3), 16);
  for (unsigned i = 0; i != 8; ++i) {
    int value = static_cast<int16_t>(a[i]) - static_cast<int16_t>(b[i]);
    value = value < std::numeric_limits<int16_t>::min() ? std::numeric_limits<int16_t>::min()
          : value > std::numeric_limits<int16_t>::max() ? std::numeric_limits<int16_t>::max() : value;
    difference[i] = static_cast<uint16_t>(static_cast<int16_t>(value));
  }
  std::memcpy(rtguest::G(param_1), difference, 16);
  S.vf[4] = lqc2(param_1);
  vitof15(0b1111, 4, 4);
  sqc2(param_1, S.vf[4]);
  vmul(0b1110, 4, 4, 4);
  vaddbc(0b1000, 1, 4, 4, 4);
  vaddbc(0b1000, 2, 4, 4, 4);
  float length = std::sqrt(S.vf[4].v[0]);
  std::memcpy(static_cast<uint8_t*>(rtguest::G(param_1)) + 12, &length, 4);
  return 0;
}
RT_GUEST_FN(0x0021d310, guest::VecSubtractAndLength)
}  // namespace guest
