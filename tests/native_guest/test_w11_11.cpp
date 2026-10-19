// Pins hand-computed from the cited Ghidra bodies (build/decomp/fn/<addr>.c);
// each block names the addr and the inputs. 0x00200fe8 wrap cases pin the ushort
// truncation before %/0x19 (sums exceed 0x10000); 0x002063c8 pins the ulong*
// element scaling of `param_1 + 4/8` (= +0x20/+0x40 bytes).
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t uiKanaGlyphUV(uint32_t, uint32_t, uint32_t, uint32_t, ...);
uint32_t uiKanjiGlyphUV(uint32_t, uint32_t, uint32_t, uint32_t, ...);
uint32_t uiSpriteWrite(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, ...);
uint32_t uiSetSpriteXY(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, ...);
uint32_t uiSetSpriteUV(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, ...);
uint32_t uiSetSpriteClamp(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, ...);
uint32_t uiEnvCenterPos(uint32_t, uint32_t, uint32_t, uint32_t, ...);
uint32_t uiEnvCenterPosB(uint32_t, uint32_t, uint32_t, uint32_t, ...);
}

template <class T> static T& at(uint32_t addr) {
  return *reinterpret_cast<T*>(rtguest::G(addr));
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);
  constexpr uint32_t u = 0x1000, v = 0x1004;

  // 0x00200fe8 uiKanaGlyphUV(&u, &v, code, v0): switch cases and range branches.
  struct { uint32_t code, v0, uExp, vExp; } kana[] = {
    {0x8141, 0x1234, 0x1a4, 0x12d4},  // case 0x8141, break path (+0xa0)
    {0x8340, 0x0500, 0x0a0, 0x053c}, // 0x833f<: (0x8340+0x7cc8)&0xffff=8, +0x3c
    {0x8260, 0x0010, 0x050, 0x009c}, // 0x825f<: (0x8260+0x7da4)&0xffff=4, +0x8c
    {0x82a0, 0x0064, 0x014, 0x0064}, // 0x829e<: (0x82a0+0x7d61)&0xffff=1, no bias
    {0x8240, 0x0100, 0x050, 0x0178}, // default: (0x8240+0x7dc4)&0xffff=4, LAB_2e8 (+0x78)
    {0x889f, 0x0200, 0x1a54, 0x02b4}, // >=0x8754: (0x889f+0x78ac)*0x14+0x78 truncated to short
    {0x8176, 0x0300, 0x0000, 0x03b4}, // case 0x8176: u=0, v+=0xb4
    {0x817d, 0x0050, 0x00dc, 0x0190}, // case 0x817d: u=0xdc, v+=0x140
  };
  for (const auto& k : kana) {
    guest::uiKanaGlyphUV(u, v, k.code, k.v0);
    assert(at<uint16_t>(u) == k.uExp);
    assert(at<int16_t>(v) == (int16_t)k.vExp);
  }

  // 0x002012f8 uiKanjiGlyphUV(&u, &v, code, v0).
  struct { uint32_t code, v0, uExp, vExp; } kanji[] = {
    {0x8fe3, 0x0040, 0x003c, 0x0180}, // case 0x8fe3: u=0x3c, v+=0x140
    {0x88c8, 0x0000, 0x0028, 0x0140}, // case 0x88c8
    {0x95fb, 0x0020, 0x0000, 0x0160}, // case 0x95fb: u=0 (assignment)
    {0x989f, 0x0777, 0x00b4, 0x00a0}, // default LAB_1410: v = 0xa0 (assign, not +=)
  };
  for (const auto& k : kanji) {
    guest::uiKanjiGlyphUV(u, v, k.code, k.v0);
    assert(at<uint16_t>(u) == k.uExp);
    assert(at<int16_t>(v) == (int16_t)k.vExp);
  }

  // 0x002063c8 uiSpriteWrite(rec, x=100, y=50, u0=40, v0=30, w=20, h=10), videoMode 0.
  constexpr uint32_t rec = 0x2000;
  at<uint32_t>(0x333288) = 0;  // videoMode
  guest::uiSpriteWrite(rec, 100, 50, 40, 30, 20, 10);
  assert(at<uint64_t>(rec + 0x00) == 0x000009c1e0ec28a);
  assert(at<uint64_t>(rec + 0x08) == 8);
  assert(at<uint64_t>(rec + 0x10) == 0x001e00280);
  assert(at<uint64_t>(rec + 0x18) == 3);
  assert(at<uint16_t>(rec + 0x20) == 0x7240);  // param_1 + 4 (ulong*) = +0x20
  assert(at<uint16_t>(rec + 0x22) == 0x7a90);
  assert(at<uint64_t>(rec + 0x28) == 5);
  assert(at<uint64_t>(rec + 0x30) == 0x029003d0);
  assert(at<uint64_t>(rec + 0x38) == 3);
  assert(at<uint16_t>(rec + 0x40) == 0x7380);  // param_1 + 8 (ulong*) = +0x40
  assert(at<uint16_t>(rec + 0x42) == 0x7ae0);
  assert(at<uint32_t>(rec + 0x44) == 0x00fffffe);
  assert(at<uint64_t>(rec + 0x48) == 5);

  // videoMode 1: 6/5 scaling, 0x7800/0x7000 bases.
  std::memset(rtguest::G(rec), 0, 0x70);
  at<uint32_t>(0x333288) = 1;
  guest::uiSpriteWrite(rec, 100, 50, 40, 30, 20, 10);
  assert(at<uint16_t>(rec + 0x20) == 0x7500);
  assert(at<uint16_t>(rec + 0x22) == 0x7990);
  assert(at<uint16_t>(rec + 0x40) == 0x7600);
  assert(at<uint16_t>(rec + 0x42) == 0x79e0);

  // negative w=-8: sVar2 = u0-w = 48, |w| folded into the max fields.
  std::memset(rtguest::G(rec), 0, 0x70);
  at<uint32_t>(0x333288) = 0;
  guest::uiSpriteWrite(rec, 100, 50, 40, 30, (uint32_t)-8, 10);
  assert(at<uint64_t>(rec + 0x00) == 0x000009c1e0bc28a);
  assert(at<uint64_t>(rec + 0x10) == 0x001e00300);
  assert(at<uint64_t>(rec + 0x30) == 0x02900290);  // [6] written before the |w| fold: (48-8)*0x10+0x10
  assert(at<uint16_t>(rec + 0x20) == 0x7240);
  assert(at<uint16_t>(rec + 0x40) == 0x72c0);
  assert(at<uint16_t>(rec + 0x42) == 0x7ae0);

  // 0x002076b8 uiSetSpriteUV: identical fields to uiSpriteWrite's [2]/[6] entries.
  std::memset(rtguest::G(rec), 0, 0x70);
  guest::uiSetSpriteUV(rec + 0x10, rec + 0x30, 40, 30, 20, 10);
  assert(at<uint64_t>(rec + 0x10) == 0x001e00280);
  assert(at<uint64_t>(rec + 0x30) == 0x029003d0);

  // 0x002078a0 uiSetSpriteClamp: identical field to uiSpriteWrite's PRIM entry.
  std::memset(rtguest::G(rec), 0, 0x70);
  guest::uiSetSpriteClamp(rec, 40, 30, 20, 10);
  assert(at<uint64_t>(rec) == 0x000009c1e0ec28a);

  // 0x002075f0 uiSetSpriteXY (videoMode 0): the two XYZ2 halves of uiSpriteWrite's writes.
  at<uint32_t>(0x333288) = 0;
  std::memset(rtguest::G(rec), 0, 0x50);
  guest::uiSetSpriteXY(rec + 0x20, rec + 0x40, 100, 50, 20, 10);
  assert(at<uint16_t>(rec + 0x20) == 0x7240);
  assert(at<uint16_t>(rec + 0x22) == 0x7a90);
  assert(at<uint16_t>(rec + 0x40) == 0x7380);
  assert(at<uint16_t>(rec + 0x42) == 0x7ae0);

  // 0x0026f340 / 0x0026f3c8 uiEnvCenterPos(B)(env, x=100, y=50, flag):
  // sizes at +0x30 = (0x21 << 48) | (0x45 << 16) -> half sizes 17/35.
  constexpr uint32_t env = 0x3000;
  at<uint64_t>(env + 0x30) = 0x0021000000450000;
  guest::uiEnvCenterPos(env, 100, 50, 0);
  assert(at<uint64_t>(env + 0x20) == 0x0000021000000410);
  at<uint64_t>(env + 0x20) = 0;
  guest::uiEnvCenterPosB(env, 100, 50, 1);
  assert(at<uint64_t>(env + 0x20) == 0x0000021800000410);
}
