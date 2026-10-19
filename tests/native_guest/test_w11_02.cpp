// Pins below are calculated in build/scratch/port/P02/pins.py from the cited Ghidra bodies.
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t SetIndexedValueAndSum(uint32_t, uint32_t, uint32_t, ...);
uint32_t SetObjectAttribute(uint32_t, uint32_t, uint32_t, ...);
uint32_t SetFlaggedSlotBit(uint32_t, uint32_t, uint32_t, ...);
}

template <class T> static T& at(uint32_t addr) {
  return *reinterpret_cast<T*>(rtguest::G(addr));
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);

  // 0x002354f0: slots {5,7,9} sum to 21 after replacing index 1 with 7.
  constexpr uint32_t indexed = 0x1000;
  at<int32_t>(indexed + 0x8460) = 5;
  at<int32_t>(indexed + 0x8464) = -1;
  at<int32_t>(indexed + 0x8468) = 9;
  assert(guest::SetIndexedValueAndSum(indexed, 1, 7) == 21);
  assert(at<uint32_t>(indexed + 0x8458) == 1);
  assert(at<int32_t>(indexed + 0x8464) == 7);
  assert(at<int32_t>(indexed + 0x846c) == 21);

  // 0x0023c418: case 8 sets then clears 0x10 while retaining the attribute byte.
  constexpr uint32_t object = 0x2000;
  at<uint16_t>(object + 8) = 0xa5a5;
  guest::SetObjectAttribute(object, 8, 1);
  assert(at<uint8_t>(object + 0x14) == 1);
  assert(at<uint16_t>(object + 8) == 0xa5b5);
  guest::SetObjectAttribute(object, 8, 0);
  assert(at<uint16_t>(object + 8) == 0xa5a5);

  // 0x0023d2c0: group 1, slot 2, bit 3 fills word 0 then word 1.
  constexpr uint32_t flags = 0x01780390 + (2 * 5 + 0xd12) * 4;
  guest::SetFlaggedSlotBit(2, 3, 1);
  assert(at<uint32_t>(flags) == 8);
  guest::SetFlaggedSlotBit(2, 3, 1);
  assert(at<uint32_t>(flags + 4) == 8);
}
