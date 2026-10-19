// Pins computed from 0x00271fc0, 0x00272010, and 0x00272060 pseudocode.
#include "guest/guest_shim.h"
#include <cassert>
#include <cstring>

namespace guest {
void textBufferSetPixel16(uint32_t, uint32_t, uint32_t, uint16_t);
uint16_t textBufferGetPixel16(uint32_t, uint32_t, uint32_t);
void textBufferBlendPixel16(uint32_t, uint32_t, uint32_t, uint16_t, int16_t);
}

template <class T> static T& at(uint32_t addr) {
  return *reinterpret_cast<T*>(rtguest::G(addr));
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);
  constexpr uint32_t grid = 0x1000, pixels = 0x1100;
  at<uint32_t>(grid) = 3;
  at<uint32_t>(grid + 4) = 2;
  at<uint32_t>(grid + 8) = pixels;

  // 0x00271fc0: (x=2,y=1) writes at pixels + (1*3+2)*2.
  guest::textBufferSetPixel16(grid, 2, 1, 0xabcd);
  assert(at<uint16_t>(pixels + 10) == 0xabcd);
  guest::textBufferSetPixel16(grid, uint32_t(-1), 0, 0x1234);
  assert(at<uint16_t>(pixels) == 0);

  // 0x00272010: in-range reads; any out-of-range coordinate yields 0x720.
  assert(guest::textBufferGetPixel16(grid, 2, 1) == 0xabcd);
  assert(guest::textBufferGetPixel16(grid, 3, 1) == 0x720);

  // 0x00272060: low byte > 0xf0 is retained ORed with tile; palette is high byte.
  at<uint16_t>(pixels) = 0x12f5;
  guest::textBufferBlendPixel16(grid, 0, 0, 0x0a, 7);
  assert(at<uint16_t>(pixels) == 0x07ff);
  at<uint16_t>(pixels + 2) = 0x12e0;
  guest::textBufferBlendPixel16(grid, 1, 0, 0x1ab, 3);
  assert(at<uint16_t>(pixels + 2) == 0x03ab);
}
