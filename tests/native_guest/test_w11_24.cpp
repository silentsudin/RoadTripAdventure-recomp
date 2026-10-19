// Pins W11-24 audit fixes:
// 1) G() masks the R5900 uncached-low alias (0x2000_0000-0x3FFF_FFFF ->
//    physical & 0x1FFF_FFFF): pseudocode DAT_21779500 in _sceCallCode read a
//    quarantine scratch before the shim fix.
// 2) _fs_version (0x00288df8) keeps &PTR_DAT_002e9138 (address) vs
//    PTR_DAT_002e9998 (deref) exactly and ANDs three memcmp results.
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t _fs_version(uint32_t = 0, ...);
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);
  // alias identity: same backing bytes both ways
  *(uint32_t*)rtguest::G(0x21779500) = 0xfeedface;
  assert(*(uint32_t*)rtguest::G(0x1779500) == 0xfeedface);
  assert(rtguest::G(0x21779500) == rtguest::G(0x1779500));

  // all three differ -> true
  std::memcpy(rtguest::G(0x1779ba8), "A\0\0\0", 4);
  std::memcpy(rtguest::G(0x2e9138), "B\0\0\0", 4);
  *(uint32_t*)rtguest::G(0x2e9998) = 0x2e99a0;
  std::memcpy(rtguest::G(0x2e99a0), "C\0\0\0", 4);
  assert(guest::_fs_version() == 1);
  // first memcmp equal -> false (AND chain short-circuits)
  std::memcpy(rtguest::G(0x2e9138), "A\0\0\0", 4);
  assert(guest::_fs_version() == 0);
}
