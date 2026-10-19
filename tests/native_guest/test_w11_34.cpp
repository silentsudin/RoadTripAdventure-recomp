// Pins W11-34: HasQueuedByteFlag (0x23dfe0) bit-7 of the &0xff-indexed ring
// byte; dialogueCmdYesNoMenu (0x23bc90) — regression for the fnptr-store fix
// (ELF 23bcbc sw $2,0xc($16): block+0xc must hold the 0x239260 ADDRESS, not
// a byte read from it) and the param_1 forward into uiMenuCreate (delay-slot
// $4 liveness proven at 23bc9c; pool sentinel 0x34a3c0 scanned from a zeroed
// slot table).
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t HasQueuedByteFlag(uint32_t q0 = 0, ...);
uint32_t dialogueCmdYesNoMenu(uint32_t q0 = 0, ...);
}  // namespace guest

int main() {
  std::memset(rtguest::kRam, 0, sizeof(rtguest::kRam));

  *(uint8_t*)rtguest::G(0x0177fd08 + 70) = 0x81;
  assert(guest::HasQueuedByteFlag(70) == 0x80);
  assert(guest::HasQueuedByteFlag(0x146) == 0x80);  // &0xff index wrap
  assert(guest::HasQueuedByteFlag(71) == 0);

  // dialogueCmdYesNoMenu over a zeroed pool: head non-sentinel, slot0 entry
  // holds the first window sentinel -> window at 0x34a3c0.
  *(uint32_t*)rtguest::G(0x0177a0f8) = 0;
  *(uint32_t*)rtguest::G(0x0177a0f8 + 0xc) = 0x34a3c0;
  const uint32_t blk = 0x00400000;
  guest::dialogueCmdYesNoMenu(blk);
  assert(*(uint32_t*)rtguest::G(blk + 0x28) == 0x34a3c0);  // uiMenuCreate(block)
  assert(*(uint16_t*)rtguest::G(0x34a3c0) == 0x140);
  assert(*(uint16_t*)rtguest::G(0x34a3c0 + 2) == 0x70);
  assert(*(uint8_t*)rtguest::G(0x34a3c0 + 0x5f) == 2);
  assert(*(uint8_t*)rtguest::G(0x34a3c0 + 0xd) == 1);      // cursor store
  assert(*(uint32_t*)rtguest::G(blk + 0xc) == 0x239260);   // fn address literal
  return 0;
}
