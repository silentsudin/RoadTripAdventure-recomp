// Pins W11-28: strcat (0x0027ef70) — authored this pass (no draft TU). All three
// newlib scan paths: 16-byte MMI block scan, 8-aligned word scan, byte scan —
// must land on the terminator and append exactly, leaving the byte past the
// result untouched, and return __dest.
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>
#include <string>

namespace guest {
uint32_t strcat(uint32_t q0 = 0, uint32_t q1 = 0, ...);
}

static void setstr(uint32_t a, const char *s) { std::strcpy((char*)rtguest::G(a), s); }

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);

  const uint32_t d16 = 0x00400000;  // 16-aligned, long dest crosses two MMI blocks
  setstr(d16, std::string(25, 'A').c_str());
  setstr(0x00400100, "xyz");
  assert(guest::strcat(d16, 0x00400100) == d16);
  assert(std::strcmp((char*)rtguest::G(d16), std::string(25, 'A').append("xyz").c_str()) == 0);
  assert(*(uint8_t*)rtguest::G(d16 + 28) == 0);

  const uint32_t d8 = 0x00400208;  // 8-aligned only
  setstr(d8, "abc");
  assert(guest::strcat(d8, 0x00400100) == d8);
  assert(std::strcmp((char*)rtguest::G(d8), "abcxyz") == 0);
  assert(*(uint8_t*)rtguest::G(d8 + 6) == 0);

  const uint32_t du = 0x00400303;  // unaligned -> byte path
  setstr(du, "hi");
  assert(guest::strcat(du, 0x00400100) == du);
  assert(std::strcmp((char*)rtguest::G(du), "hixyz") == 0);
  assert(*(uint8_t*)rtguest::G(du + 5) == 0);
}
