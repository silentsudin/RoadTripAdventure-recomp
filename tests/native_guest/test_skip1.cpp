// Pins computed by hand from build/decomp/fn/0027e840.c (memset) and
// build/decomp/fn/002835cc.c (memchr); byte counts verified against the ELF disassembly.
#include "../../src/native/guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t memset(uint32_t, uint32_t, uint32_t, ...);
uint32_t memchr(uint32_t, uint32_t, uint32_t, ...);
}

template <class T> static T& at(uint32_t addr) {
  return *reinterpret_cast<T*>(rtguest::G(addr));
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);

  // memset A: aligned, n=48 -> 32-byte loop x1 + 8-byte loop x2 + 0 tail.
  assert(guest::memset(0x2000, 0xAB, 48) == 0x2000);
  for (uint32_t i = 0; i < 48; i++) assert(at<uint8_t>(0x2000 + i) == 0xAB);
  assert(at<uint8_t>(0x1FFF) == 0);
  assert(at<uint8_t>(0x2030) == 0);

  // memset B: unaligned base (4 mod 16) -> byte loop only, n=100.
  assert(guest::memset(0x2104, 0x11, 100) == 0x2104);
  for (uint32_t i = 0; i < 100; i++) assert(at<uint8_t>(0x2104 + i) == 0x11);
  assert(at<uint8_t>(0x2103) == 0);
  assert(at<uint8_t>(0x2168) == 0);

  // memset C: aligned n=8 -> vector path, 8-byte loop exactly once.
  assert(guest::memset(0x2200, 0x7F, 8) == 0x2200);
  for (uint32_t i = 0; i < 8; i++) assert(at<uint8_t>(0x2200 + i) == 0x7F);
  assert(at<uint8_t>(0x2208) == 0);

  // memset D: n=0 writes nothing and returns __s.
  assert(guest::memset(0x2300, 0x00, 0) == 0x2300);

  // memset E: aligned n=20 -> 8-byte loop x2 + byte loop x4 (32-loop skipped).
  assert(guest::memset(0x2400, 0x5A, 20) == 0x2400);
  for (uint32_t i = 0; i < 20; i++) assert(at<uint8_t>(0x2400 + i) == 0x5A);
  assert(at<uint8_t>(0x2414) == 0);

  // memchr A: vector break in first 16-block, match at offset 0.
  for (uint32_t i = 0; i < 32; i++) at<uint8_t>(0x3000 + i) = 0x41;
  at<uint8_t>(0x3000) = 0x42;
  assert(guest::memchr(0x3000, 0x42, 32) == 0x3000);

  // memchr B: no match in block 1, vector break in block 2, byte loop to offset 20.
  for (uint32_t i = 0; i < 32; i++) at<uint8_t>(0x3100 + i) = 0x41;
  at<uint8_t>(0x3100 + 20) = 0x42;
  assert(guest::memchr(0x3100, 0x42, 32) == 0x3100 + 20);

  // memchr C: unaligned base -> byte loop only, match at offset 5.
  for (uint32_t i = 0; i < 10; i++) at<uint8_t>(0x3205 + i) = 0x55;
  at<uint8_t>(0x3205 + 5) = 0x0A;
  assert(guest::memchr(0x3205, 0x0A, 10) == 0x3205 + 5);

  // memchr D: aligned n=16, no match -> 0 (vector pass then 16 byte misses).
  for (uint32_t i = 0; i < 16; i++) at<uint8_t>(0x3300 + i) = 0x55;
  assert(guest::memchr(0x3300, 0x0A, 16) == 0);

  // memchr E: 0x80 match through the byte loop — pins the unsigned (lbu) compare.
  for (uint32_t i = 0; i < 8; i++) at<uint8_t>(0x3405 + i) = 0x01;
  at<uint8_t>(0x3405 + 3) = 0x80;
  assert(guest::memchr(0x3405, 0x80, 8) == 0x3405 + 3);

  // memchr F: n=0 -> 0.
  assert(guest::memchr(0x3500, 0x41, 0) == 0);

  // memchr G: __c is masked to its low byte ((long)__c & 0xff).
  for (uint32_t i = 0; i < 8; i++) at<uint8_t>(0x3605 + i) = 0x42;
  assert(guest::memchr(0x3605, 0x142, 8) == 0x3605);

  // memchr H: vector break with match at the last byte of the 16-byte range.
  for (uint32_t i = 0; i < 16; i++) at<uint8_t>(0x3700 + i) = 0x41;
  at<uint8_t>(0x3700 + 15) = 0x42;
  assert(guest::memchr(0x3700, 0x42, 16) == 0x3700 + 15);

  // memchr I: 0x80 match found via the vector path (xor zero detect).
  for (uint32_t i = 0; i < 32; i++) at<uint8_t>(0x3800 + i) = 0x41;
  at<uint8_t>(0x3800 + 3) = 0x80;
  assert(guest::memchr(0x3800, 0x80, 32) == 0x3800 + 3);
}
