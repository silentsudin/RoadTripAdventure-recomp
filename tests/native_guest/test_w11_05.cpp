#include "../../src/native/guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t GetConst500(uint32_t, ...);
uint32_t GetLabelEntry_ed4e8(uint64_t, uint32_t, ...);
uint32_t GetPartValueU16(uint32_t, uint32_t, ...);
uint32_t GetVehicleTransmissionMode(uint32_t, ...);
uint32_t GetRaceTimeRank(uint32_t, ...);
uint64_t sysFstat(uint64_t, uint32_t, ...);
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);

  // 0x00243698, no inputs: pseudocode returns literal 500.
  assert(guest::GetConst500(0) == 500);

  // 0x00243528, param_2=2: table index is 2 * 4 pointers (0x20 bytes).
  *static_cast<uint32_t*>(rtguest::G(0x002ed4e8 + 0x20)) = 0x00123456;
  assert(guest::GetLabelEntry_ed4e8(0, 2) == 0x00123456);

  // 0x00243a00, category=11/index=2 selects the u16 at 0x2f03ee + 2*0x14.
  *static_cast<uint16_t*>(rtguest::G(0x002f03ee + 0x28)) = 0xbeef;
  assert(guest::GetPartValueU16(11, 2) == 0xbeef);
  // 0x00243a00, category=12 returns literal 11; category=9 returns zero.
  assert(guest::GetPartValueU16(12, 0) == 11);
  assert(guest::GetPartValueU16(9, 0) == 0);

  constexpr uint32_t vehicle = 0x1000;
  // 0x002495c0: shift-mode -1, 1, and other map to 1, 2, and 0.
  *static_cast<int8_t*>(rtguest::G(vehicle + 0x244)) = -1;
  assert(guest::GetVehicleTransmissionMode(vehicle) == 1);
  *static_cast<int8_t*>(rtguest::G(vehicle + 0x244)) = 1;
  assert(guest::GetVehicleTransmissionMode(vehicle) == 2);
  *static_cast<int8_t*>(rtguest::G(vehicle + 0x244)) = 0;
  assert(guest::GetVehicleTransmissionMode(vehicle) == 0);

  // 0x00249670: elapsed times 81000, 144000, 162001 produce ranks 1, 2, 0.
  *static_cast<int32_t*>(rtguest::G(vehicle + 0x14)) = 81000;
  assert(guest::GetRaceTimeRank(vehicle) == 1);
  *static_cast<int32_t*>(rtguest::G(vehicle + 0x14)) = 144000;
  assert(guest::GetRaceTimeRank(vehicle) == 2);
  *static_cast<int32_t*>(rtguest::G(vehicle + 0x14)) = 162001;
  assert(guest::GetRaceTimeRank(vehicle) == 0);

  // 0x002856b0, buf=0x1800: stores a zero u64 at +0x48 and S_IFCHR at +4.
  *static_cast<uint64_t*>(rtguest::G(0x1848)) = ~uint64_t{0};
  assert(guest::sysFstat(0, 0x1800) == 0);
  assert(*static_cast<uint64_t*>(rtguest::G(0x1848)) == 0);
  assert(*static_cast<uint32_t*>(rtguest::G(0x1804)) == 0x2000);
}
