// Pins the SKIP2 campaign batch: newlibc SIMD trio (strcpy/strlen/strncpy/
// memcmp: 16-aligned, 8-aligned and byte paths), mat4Transpose (PCPY/PEXT
// V16 chain), updateTimeOfDayLight (dawn window DAT plumbing + light
// matrix blend via vu0Scale/AddVector).
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t strcpy(uint32_t q0 = 0, uint32_t q1 = 0, ...);
uint32_t strlen(uint32_t q0 = 0, ...);
uint32_t strncpy(uint32_t q0 = 0, uint32_t q1 = 0, uint32_t q2 = 0, ...);
uint32_t memcmp(uint32_t q0 = 0, uint32_t q1 = 0, uint32_t q2 = 0, ...);
uint32_t mat4Transpose(uint32_t q0 = 0, uint32_t q1 = 0, ...);
uint32_t updateTimeOfDayLight(uint32_t q0 = 0, uint32_t q1 = 0, uint32_t q2 = 0, ...);
}  // namespace guest

int main() {
  std::memset(rtguest::kRam, 0, sizeof(rtguest::kRam));
  const uint32_t m = 0x00400000;

  // ---- strcpy: 16-aligned across chunks (NUL inside chunk 1) ----
  uint32_t src = m + 0x10, dst = m + 0x100;
  std::memcpy(rtguest::G(src), "Hello, World! ChoroQ", 21);  // 20 chars + NUL
  guest::strcpy(dst, src);
  assert(std::memcmp(rtguest::G(dst), "Hello, World! ChoroQ", 21) == 0);

  // ---- strcpy: 8-aligned path ----
  src = m + 0x28; dst = m + 0x228;
  std::memcpy(rtguest::G(src), "eight-aligned copy string value", 32);
  guest::strcpy(dst, src);
  assert(std::memcmp(rtguest::G(dst), "eight-aligned copy string value", 32) == 0);

  // ---- strcpy: unaligned byte path ----
  src = m + 0x41; dst = m + 0x341;
  std::memcpy(rtguest::G(src), "odd odd odd", 12);
  guest::strcpy(dst, src);
  assert(std::memcmp(rtguest::G(dst), "odd odd odd", 12) == 0);

  // ---- strlen: all three paths ----
  src = m + 0x50;   // 16-aligned, NUL at 37: chunk 2
  std::memcpy(rtguest::G(src), "012345678901234501234567890123456", 34);
  assert(guest::strlen(src) == 33);
  src = m + 0x68;   // 8-aligned, NUL within first qword
  std::memcpy(rtguest::G(src), "short", 6);
  assert(guest::strlen(src) == 5);
  src = m + 0x77;   // unaligned
  std::memcpy(rtguest::G(src), "abcdef", 7);
  assert(guest::strlen(src) == 6);

  // ---- strncpy: NUL + zero fill up to n ----
  src = m + 0x80; dst = m + 0x90;
  std::memcpy(rtguest::G(src), "AAA", 4);
  std::memset(rtguest::G(dst), 0x5a, 8);
  assert(guest::strncpy(dst, src, 8) == dst);
  assert(std::memcmp(rtguest::G(dst), "AAA\0\0\0\0\0", 8) == 0);
  // truncation: n bytes copied, no NUL
  std::memset(rtguest::G(dst), 0x5a, 8);
  guest::strncpy(dst, src, 2);
  assert(((uint8_t*)rtguest::G(dst))[0] == 'A' && ((uint8_t*)rtguest::G(dst))[1] == 'A' &&
         ((uint8_t*)rtguest::G(dst))[6] == 0x5a && ((uint8_t*)rtguest::G(dst))[7] == 0x5a);
  // 16-aligned fast path (all-high-bit bytes keep the fast loop alive).
  // shortcut: fn/0027f430.c:60-66 loads a full 16-byte chunk before storing,
  // so dst must not overlap the unread source (newlibc contract); old dst sat
  // inside the src fill.
  src = m + 0xb0; dst = m + 0xe0;
  for (int i = 0; i < 34; i++) ((uint8_t*)rtguest::G(src))[i] = uint8_t(0x80 + i);
  ((uint8_t*)rtguest::G(src))[30] = 0;  // NUL in second chunk
  guest::strncpy(dst, src, 40);
  for (int i = 0; i < 30; i++) assert(((uint8_t*)rtguest::G(dst))[i] == uint8_t(0x80 + i));
  for (int i = 30; i < 40; i++) assert(((uint8_t*)rtguest::G(dst))[i] == 0);

  // ---- memcmp ----
  src = m + 0x110;
  std::memcpy(rtguest::G(src), "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 41);
  std::memcpy(rtguest::G(m + 0x150), "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 41);
  assert(guest::memcmp(src, m + 0x150, 40) == 0);
  ((uint8_t*)rtguest::G(m + 0x150))[33] = 'b';   // inside fast-path chunk 2
  assert((int32_t)guest::memcmp(src, m + 0x150, 40) < 0);
  ((uint8_t*)rtguest::G(m + 0x150))[33] = 'a';
  assert(guest::memcmp(m + 0x117, m + 0x157, 3) == 0);  // unaligned short path

  // ---- mat4Transpose: input word (row*10+col) ----
  uint32_t in = m + 0x200, out = m + 0x300;
  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
      *(uint32_t*)rtguest::G(in + r * 0x10 + c * 4) = r * 10 + c;
  guest::mat4Transpose(out, in);
  // PCPYLD/PCPYUD chain (fn/00272820.c): auVar1 stores cols 0,1,2,3 -> the
  // four output 16B vectors hold transposed columns; LE vector-to-memory
  // puts word 0 at the lowest address (previous pin mirrored each vector).
  const uint32_t want[16] = {0, 10, 20, 30,  1, 11, 21, 31,
                             2, 12, 22, 32,  3, 13, 23, 33};
  for (int i = 0; i < 16; i++)
    assert(*(uint32_t*)rtguest::G(out + i * 4) == want[i]);

  // ---- updateTimeOfDayLight: dawn window (0xc15c <= t < 0xe484) ----
  // t = 0xc15c + 4096, DAT_00332dac = 4096.0f -> fVar10 = 1.0f
  const uint32_t f1 = rtguest::f2bits(1.0f), f100 = rtguest::f2bits(100.0f);
  *(uint32_t*)rtguest::G(0x332dac) = rtguest::f2bits(4096.0f);
  *(uint32_t*)rtguest::G(0x332db0) = f1;      // fVar9 = 1.0 - 1.0 = 0
  for (uint32_t a = 0x177f620; a < 0x177f760; a += 4) *(uint32_t*)rtguest::G(a) = 0;
  // tod tables: day row i word j = (i*10+j) as float, dusk/night = 5.0 junk
  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++) {
      *(uint32_t*)rtguest::G(0x29d500 + r * 0x10 + c * 4) = rtguest::f2bits(float(r * 10 + c));
      *(uint32_t*)rtguest::G(0x29d540 + r * 0x10 + c * 4) = rtguest::f2bits(5.0f);
      *(uint32_t*)rtguest::G(0x29d580 + r * 0x10 + c * 4) = rtguest::f2bits(7.0f);
    }
  const uint32_t task = m + 0x500;
  guest::updateTimeOfDayLight(f100, task, 0xc15c + 4096);
  assert(*(uint32_t*)rtguest::G(0x177f738) == rtguest::f2bits(0.5f));       // fogFarParam
  assert(*(uint32_t*)rtguest::G(0x177f73c) == rtguest::f2bits(700.0f));     // (500+300)-100
  assert(*(uint32_t*)rtguest::G(0x177f750) == 0x7fffffff);                  // fog RGBA
  assert(*(uint32_t*)rtguest::G(0x177f754) == 0x00ffffff);                  // _DAT_0177f754
  assert(*(uint32_t*)rtguest::G(0x177f720) == f1);                          // sceneParams
  assert(*(uint32_t*)rtguest::G(0x177f724) == 0 && *(uint32_t*)rtguest::G(0x177f728) == 0);
  assert(*(uint32_t*)rtguest::G(0x177f65c) == 0x43000000);
  // lightMatrixBlend row 0 = day row 0 * 1.0 (dusk/night scaled by 0)
  assert(*(uint32_t*)rtguest::G(0x177f620) == rtguest::f2bits(0.0f));
  assert(*(uint32_t*)rtguest::G(0x177f624) == rtguest::f2bits(1.0f));
  assert(*(uint32_t*)rtguest::G(0x177f630) == rtguest::f2bits(10.0f));
  assert(*(uint32_t*)rtguest::G(0x177f63c) == rtguest::f2bits(13.0f));
  assert(*(uint32_t*)rtguest::G(task + 0x50) == 0x7fffffff);                // fog copy to task
  return 0;
}
