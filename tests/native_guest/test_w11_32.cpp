// Pins W11-32: FormatRaceTimeBCD (0x2495f0) MM:SS digit pack and
// ReleaseStampFlag (0x23c7b0) bit-clear + house-stamp 64-bit set path.
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t FormatRaceTimeBCD(uint32_t q0 = 0, ...);
uint32_t ReleaseStampFlag(uint32_t q0 = 0, ...);
}  // namespace guest

int main() {
  std::memset(rtguest::kRam, 0, sizeof(rtguest::kRam));

  // FormatRaceTimeBCD: ticks/150 = 3725 s -> min 62 sec-unit digit math
  *(uint32_t*)rtguest::G(0x00400014) = 3725 * 150;
  assert(guest::FormatRaceTimeBCD(0x00400000) == 0x62A05u);
  *(uint32_t*)rtguest::G(0x00400014) = 0;
  assert(guest::FormatRaceTimeBCD(0x00400000) == 0xA00u);

  // ReleaseStampFlag: slot idx +0x21 = 3, char +0x23 = 2
  *(uint8_t*)rtguest::G(0x00400021) = 3;
  *(uint8_t*)rtguest::G(0x00400023) = 2;
  *(uint32_t*)rtguest::G(0x01780338 + 2 * 4) = 0xFFFFFFFFu;
  // house stamp table[2] low byte 0x41 -> uVar2 = 0x164 -> word[1] |= 1<<0x24
  *(uint32_t*)rtguest::G(0x002A2B20 + 2 * 4) = 0x41;
  guest::ReleaseStampFlag(0x00400000);
  assert(*(uint32_t*)rtguest::G(0x01780338 + 2 * 4) == 0xFFFFFFF7u);
  assert(*(uint32_t*)rtguest::G(0x0177FC88 + 8) == 0x10000000u);
  // a3a branch untouched (idx != 0): bit 2 not cleared there
  assert(*(uint16_t*)rtguest::G(0x01780A3A) == 0);
  // guard is unsigned (ELF lbu+sltiu): +0x21 = 0x81 skips the whole body
  *(uint8_t*)rtguest::G(0x00400021) = 0x81;
  guest::ReleaseStampFlag(0x00400000);
  assert(*(uint32_t*)rtguest::G(0x01780338 + 2 * 4) == 0xFFFFFFF7u);

  return 0;
}
