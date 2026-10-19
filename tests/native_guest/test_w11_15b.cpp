// Pins computed from build/decomp/fn/0021fae8.c and 002180c0.c.
#include "../../src/native/guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t LoadCameraPreset(uint32_t, uint32_t, ...);
uint32_t LoadCarPerformanceSpecs(uint32_t, ...);
}

template <class T> static T& at(uint32_t addr) {
  return *reinterpret_cast<T*>(rtguest::G(addr));
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);

  // The preset is four consecutive 64-bit words at 0x29ce00 + index * 0x20.
  for (uint32_t i = 0; i != 4; ++i)
    at<uint64_t>(0x29ce00 + 0x40 + i * 8) = 0x1122334455667788ULL + i;
  assert(guest::LoadCameraPreset(0x1000, 2) == 0);
  for (uint32_t i = 0; i != 4; ++i)
    assert(at<uint64_t>(0x1000 + i * 8) == 0x1122334455667788ULL + i);

  // Pin the table offsets and both performance-adjustment branches by hand.
  constexpr uint32_t car = 0x2000, spec = 0x3000;
  at<uint32_t>(car + 0x180) = spec;
  at<uint8_t>(spec + 1) = 2;
  at<uint8_t>(spec + 2) = 3;
  at<uint8_t>(spec + 3) = 4;
  at<uint8_t>(spec + 4) = 5;
  at<uint8_t>(spec + 5) = 6;
  at<uint8_t>(spec + 6) = 7;
  at<uint8_t>(spec + 0xd) = 8;
  at<uint16_t>(car + 0x198) = 0x31;
  for (uint32_t i = 0; i != 6; ++i) at<int16_t>(0x2eca7c + (2 * 14 + i) * 2) = 16 + i * 16;
  at<uint32_t>(0x2ecedc + 3 * 0x18) = 0x12345678;
  at<uint16_t>(0x2ecee0 + 3 * 0x18) = 0x2468;
  at<int32_t>(0x2ed204 + 4 * 0x10) = 100;
  at<int16_t>(0x2eca8a + 2 * 0x1c) = 1;
  at<uint16_t>(0x2ed4f4 + 6 * 0x10) = 0xabcd;
  at<uint32_t>(0x2f00ac + 8 * 0x40) = 9;
  for (uint32_t i = 0; i != 7; ++i) at<uint16_t>(0x2ed334 + 5 * 0x1c + i * 2) = 0x100 + i;
  assert(guest::LoadCarPerformanceSpecs(car) == 0);
  for (uint32_t i = 0; i != 6; ++i) assert(at<int16_t>(car + 0x21c + i * 2) == 17 + i * 17);
  assert(at<uint32_t>(car + 0x214) == 0x12345678);
  assert(at<uint16_t>(car + 0x240) == 0x2468);
  assert(at<int32_t>(car + 0x218) == 80); // (100 - 25) + 5
  assert(at<uint16_t>(car + 0x242) == 0xabcd);
  assert(at<uint32_t>(car + 0x200) == 0x2ed5dc + 7 * 0x2c);
  assert(at<uint8_t>(car + 0x245) == 10);
  for (uint32_t i = 0; i != 7; ++i) assert(at<uint16_t>(car + 0x22c + i * 2) == 0x100 + i);
}
