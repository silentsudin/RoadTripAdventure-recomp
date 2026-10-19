// Pins W11-33: IntToPackedBCD (0x24f4a0) digit packing, and SumPlayerPoints
// (0x236238) point-table lookup with ELF-cited guard widths (0x178700a/
// 0x178700e lbu unsigned, 0x177fdd4/6 lb flags) incl. the 0x80 guard byte.
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t IntToPackedBCD(uint32_t q0 = 0, ...);
uint32_t SumPlayerPoints(uint32_t q0 = 0, ...);
}  // namespace guest

int main() {
  std::memset(rtguest::kRam, 0, sizeof(rtguest::kRam));

  assert(guest::IntToPackedBCD(1234) == 0x1234);
  assert(guest::IntToPackedBCD(0) == 0);
  assert(guest::IntToPackedBCD(90) == 0x90);

  // course byte +3 = 4; p1 slot idx 4*6+1, p2 idx 4*6+2, p3 flag off
  *(uint8_t*)rtguest::G(0x00400003) = 4;
  *(uint8_t*)rtguest::G(0x0178700a) = 1;
  *(uint8_t*)rtguest::G(0x0178700e) = 2;
  *(uint8_t*)rtguest::G(0x0177fdd4) = 1;
  *(uint8_t*)rtguest::G(0x0177fdd6) = 0;
  *(uint32_t*)rtguest::G(0x01787012) = 0;
  *(int32_t*)rtguest::G(0x0029fcb8 + (4 * 6 + 1) * 4) = 0x111;
  *(int32_t*)rtguest::G(0x0029fcb8 + (4 * 6 + 2) * 4) = 0x222;
  assert(guest::SumPlayerPoints(0x00400000) == 0x333);

  // guard is unsigned (lbu + sltiu): 6 excludes p1 term
  *(uint8_t*)rtguest::G(0x0178700a) = 6;
  assert(guest::SumPlayerPoints(0x00400000) == 0x222);
  // 0x80 guard byte: unsigned 128 >= 6 excludes (a signed-byte model would
  // include it and read idx 4*6+128 garbage)
  *(uint8_t*)rtguest::G(0x0178700a) = 0x80;
  assert(guest::SumPlayerPoints(0x00400000) == 0x222);
  return 0;
}
