// Pins W11-27: LookupPlayerNameSlot (0x00237ac0), authored MIPS-grounded (no draft
// TU — the cancelled wave-7 worker never wrote it). MIPS facts the pseudocode got
// wrong and this test pins: the table bases are DIRECT (0x177fdd8/0x177fde0 and
// 0x1783220/0x1783228, 0x30-byte entries), the "ANYBODY" assignments are u64 loads
// of the 8 string bytes at 0x333870 (0x237b78/0x237c20 ld+sd), part-2 pid is
// DAT_01782b98, and the return is v0's leftover word (strcpy dst / matched id /
// ANYBODY low word — the char[8] return is Ghidra fiction).
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t LookupPlayerNameSlot(uint32_t q0 = 0, ...);
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);
  *(uint64_t*)rtguest::G(0x01785fe0) = 0x1122334455667788ull;  // pid1 (player table)
  *(uint64_t*)rtguest::G(0x01782b98) = 0x99aabbccddeeff00ull;  // pid2 (name table)
  std::strcpy((char*)rtguest::G(0x017831d4), "FRED");          // name source 1
  std::strcpy((char*)rtguest::G(0x0177fd8c), "BAR");           // name source 2
  const uint32_t car = 0x00004000;

  uint32_t r = guest::LookupPlayerNameSlot(car);               // create in slot 0 (both tables)
  assert(r == 0x01783228u);                                   // v0 = part-2 strcpy dst
  assert(*(uint16_t*)rtguest::G(car + 0x1c) == 0);
  assert(*(uint16_t*)rtguest::G(car + 0x1e) == 0);
  assert(*(uint64_t*)rtguest::G(0x0177fdd8) == 0x1122334455667788ull);
  assert(std::strcmp((char*)rtguest::G(0x0177fde0), "FRED") == 0);
  assert(*(uint64_t*)rtguest::G(0x01783220) == 0x99aabbccddeeff00ull);
  assert(std::strcmp((char*)rtguest::G(0x01783228), "BAR") == 0);

  r = guest::LookupPlayerNameSlot(car);                       // same pids: found, slot 0
  assert(r == 0xddeeff00u);                                   // v0 = low word of matched id
  assert(*(uint16_t*)rtguest::G(car + 0x1c) == 0);
  assert(*(uint16_t*)rtguest::G(car + 0x1e) == 0);

  *(uint64_t*)rtguest::G(0x01785fe0) = 5;                     // new pid1 -> next empty slot 1
  guest::LookupPlayerNameSlot(car);
  assert(*(uint16_t*)rtguest::G(car + 0x1c) == 1);
  assert(*(uint64_t*)rtguest::G(0x0177fdd8 + 0x30) == 5);
  assert(std::strcmp((char*)rtguest::G(0x0177fde0 + 0x30), "FRED") == 0);

  for (int k = 0; k < 20; ++k) {                              // fill both tables, unmatched pids
    *(uint64_t*)rtguest::G(0x0177fdd8 + k * 0x30) = 0x1000 + k;
    std::strcpy((char*)rtguest::G(0x0177fde0 + k * 0x30), "N");
    *(uint64_t*)rtguest::G(0x01783220 + k * 0x30) = 0x2000 + k;
    std::strcpy((char*)rtguest::G(0x01783228 + k * 0x30), "M");
  }
  *(uint64_t*)rtguest::G(0x01785fe0) = 0x7777;
  *(uint64_t*)rtguest::G(0x01782b98) = 0x8888;
  guest::LookupPlayerNameSlot(car);                           // exhausted -> ANYBODY fallback
  assert(*(uint16_t*)rtguest::G(car + 0x1c) == 0x13);
  assert(*(uint16_t*)rtguest::G(car + 0x1e) == 0x13);
  assert(*(uint64_t*)rtguest::G(0x01780170) == *(uint64_t*)rtguest::G(0x00333870));
  assert(*(uint64_t*)rtguest::G(0x017835b8) == *(uint64_t*)rtguest::G(0x00333870));
}
