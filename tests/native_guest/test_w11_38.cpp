// Pins W11-38: ttyRing control block (libcdvd tty ring: 0x286088/0x2860B0/
// 0x2860F0) — init/push/pop wrap arithmetic on the DAT block — and the SIMD
// strcmp (0x27f09c) byte-diff contract incl. the unaligned scalar path.
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t ttyRingInit(uint32_t q0 = 0, ...);
uint32_t ttyRingPush(uint32_t q0 = 0, ...);
uint32_t ttyRingPop(uint32_t q0 = 0, ...);
uint32_t strcmp(uint32_t q0 = 0, uint32_t q1 = 0, ...);
}  // namespace guest

int main() {
  std::memset(rtguest::kRam, 0, sizeof(rtguest::kRam));

  // tty ring: cb at 0x1776980 (cap,count,head,tail), data at 0x1776990
  const uint32_t cb = 0x01776980, data = 0x01776990;
  assert(guest::ttyRingInit(8) == cb);
  assert(*(uint32_t*)rtguest::G(cb) == 8);
  assert(*(uint32_t*)rtguest::G(cb + 4) == 0);
  assert(*(uint32_t*)rtguest::G(cb + 8) == data);
  assert(*(uint32_t*)rtguest::G(cb + 0xc) == data);

  // fn/002860b0.c/002860f0.c: the ring advances its pointers ONE BYTE per
  // call (tty byte ring, registry note); with cap 8 the 8th step hits
  // data+cap and wraps back to data.
  for (int i = 0; i < 8; ++i) guest::ttyRingPush(cb);
  assert(*(uint32_t*)rtguest::G(cb + 0xc) == data);
  assert(*(uint32_t*)rtguest::G(cb + 4) == 8);
  for (int i = 0; i < 8; ++i) guest::ttyRingPop(cb);
  assert(*(uint32_t*)rtguest::G(cb + 8) == data);
  assert(*(uint32_t*)rtguest::G(cb + 4) == 0);

  // strcmp
  const uint32_t s = 0x00400400;  // 16-aligned
  std::memcpy(rtguest::kRam + s, "abc", 4);
  std::memcpy(rtguest::kRam + s + 0x20, "abd", 4);
  std::memcpy(rtguest::kRam + s + 0x40, "ab", 3);
  const uint32_t u = 0x00400800;  // unaligned pair, separate buffer
  std::memcpy(rtguest::kRam + u + 1, "xabc", 5);
  std::memcpy(rtguest::kRam + u + 0x21, "xabd", 5);
  assert((int32_t)guest::strcmp(s, s) == 0);
  assert((int32_t)guest::strcmp(s, s + 0x20) == -1);
  assert((int32_t)guest::strcmp(s + 0x20, s) == 1);
  // prefix 'c' vs NUL: words differ at byte 2 -> byte tail returns 'c'-0
  // (disasm 0x27f1a4 -> tail 0x27f1ac). Equal-word-with-NUL returns 0 at
  // 0x27f184 via jr ra (never reached here).
  assert((int32_t)guest::strcmp(s, s + 0x40) == 'c');
  assert((int32_t)guest::strcmp(u + 1, u + 0x21) == -1);  // unaligned path
  return 0;
}
