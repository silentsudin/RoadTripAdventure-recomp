#include "../../src/native/guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t GetGridCellIndex(uint32_t, ...);
uint32_t GetCarSlotEntry(uint32_t, ...);
uint32_t GetSceneCamLookatIndex(uint32_t, ...);
uint32_t __unpack_f(uint32_t, uint32_t, ...);
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);

  constexpr uint32_t position = 0x1000;
  // 0x0021c880: x=0, z=0 maps to cell 0.
  assert(guest::GetGridCellIndex(position) == 0);
  // 0x0021c880: x=0x02000000, z=0 maps to cell 2.
  *static_cast<int32_t*>(rtguest::G(position)) = 0x02000000;
  assert(guest::GetGridCellIndex(position) == 2);
  // 0x0021c880: x=0, z=-0x02000000 maps to cell 3.
  *static_cast<int32_t*>(rtguest::G(position)) = 0;
  *static_cast<int32_t*>(rtguest::G(position + 8)) = -0x02000000;
  assert(guest::GetGridCellIndex(position) == 3);

  // 0x0021fb20: slot 3 is base + 3 * 0x20.
  assert(guest::GetCarSlotEntry(3) == 0x0029ce60);
  // 0x00226180: selector 1 takes the alternate look-at table.
  assert(guest::GetSceneCamLookatIndex(1) == 0x0029e950);
  assert(guest::GetSceneCamLookatIndex(0) == 0x0029e9b0);

  constexpr uint32_t bits = 0x1100;
  constexpr uint32_t parts = 0x1200;
  // 0x0027d6d8: 1.0f is positive, normal, exponent 0, fraction 0x40000000.
  *static_cast<uint32_t*>(rtguest::G(bits)) = 0x3f800000;
  guest::__unpack_f(bits, parts);
  assert(*static_cast<uint32_t*>(rtguest::G(parts)) == 3);
  assert(*static_cast<uint32_t*>(rtguest::G(parts + 4)) == 0);
  assert(*static_cast<uint32_t*>(rtguest::G(parts + 8)) == 0);
  assert(*static_cast<uint32_t*>(rtguest::G(parts + 12)) == 0x40000000);
}
