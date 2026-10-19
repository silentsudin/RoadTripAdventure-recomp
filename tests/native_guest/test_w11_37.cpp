// Pins W11-37: ContinueRaceEntityLoad (0x26d5e0) — regression for the
// corpus fnptr-store fix (blk+0xc = 0x26d628 literal; the draft loaded a
// byte from 0x26d628) plus the gs_tasks_spawn chain it runs. And
// _lmcGetClientPtr (0x279960) libmc client-pointer contract.
// pauseMenuTask (0x26bc08) first frame calls uiFade -> vifDirect (MMIO):
// unpinnable pre-W12; its uiMenuCreate(param_1) forward is disasm-proven
// ($4 untouched 26bc18->26bc30).
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t ContinueRaceEntityLoad(uint32_t q0 = 0, ...);
uint32_t _lmcGetClientPtr(uint32_t q0 = 0, uint32_t q1 = 0, ...);
}  // namespace guest

int main() {
  std::memset(rtguest::kRam, 0, sizeof(rtguest::kRam));

  // fptr-store + spawn path
  const uint32_t blk = 0x00400000;
  *(uint8_t*)rtguest::G(blk + 0x24) = 1;
  guest::ContinueRaceEntityLoad(blk);
  const uint32_t slot = 0x0177a0d0;  // first task-pool slot (zero RAM)
  assert(*(uint8_t*)rtguest::G(slot) == 1);
  assert(*(uint32_t*)rtguest::G(slot + 0xc) == 0x25bcf0);  // spawned handler
  assert(*(uint32_t*)rtguest::G(slot + 0x1c) == 1);        // spawn arg
  assert(*(uint32_t*)rtguest::G(slot + 0x18) == blk);      // owner back-link
  assert(*(uint32_t*)rtguest::G(blk + 0x10) == slot);      // block child chain
  assert(*(uint32_t*)rtguest::G(blk + 0xc) == 0x26d628);   // literal, not a load

  // flag clear: no spawn, no handler swap
  const uint32_t blk2 = 0x00400100;
  guest::ContinueRaceEntityLoad(blk2);
  assert(*(uint32_t*)rtguest::G(blk2 + 0xc) == 0);

  // libmc client pointers
  const uint32_t a = 0x00400200, b = 0x00400204;
  *(uint32_t*)rtguest::G(0x002e89bc) = 0xdeadbeef;
  assert(guest::_lmcGetClientPtr(a, b) == 0x01774b40);
  assert(*(uint32_t*)rtguest::G(a) == 0x01776100);
  assert(*(uint32_t*)rtguest::G(b) == 0x002e89b8);
  assert(*(uint32_t*)rtguest::G(0x0177613c) == 0xdeadbeef);
  return 0;
}
