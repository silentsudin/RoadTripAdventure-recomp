// Pins the typed-pointer-step fix on uiSpriteWriteWrap (0x00206040):
// pseudocode *(short*)(param_1 + 4) with ulong* param_1 is a qword-ELEMENT
// step = +0x20 bytes (MIPS: addiu $9,$13,0x20 then sh 0x0/0x2($9)), not the
// raw +4 the transliterator first emitted. Same fix applied to 00206650,
// 002068d0, 00206b00 (identical sprite-record prologues) and __smakebuf
// 00283440 (+3 int* = +0xc, MIPS lhu 0xc($16)); the sprite pin here covers
// the class end-to-end.
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t uiSpriteWriteWrap(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                          uint32_t, uint32_t, uint32_t, uint32_t, ...);
}

static uint16_t sh_(uint32_t a) { return *(uint16_t*)rtguest::G(a); }
static uint64_t q_(uint32_t a) { return *(uint64_t*)rtguest::G(a); }

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);  // videoMode G(0x333288) = 0
  constexpr uint32_t s = 0x1000;
  // param_2=0x12 p3=0x21 p4=5 p5=6 p6=7 p7=8 p8=0 p9=0
  guest::uiSpriteWriteWrap(s, 0x12, 0x21, 5, 6, 7, 8, 0, 0);
  assert(sh_(s + 0x20) == 0x6d20);  // (0x12<<4)+0x6c00 — the fixed +4-elem site
  assert(sh_(s + 0x22) == 0x7a08);  // (0x21<<3)+0x7900
  assert(sh_(s + 0x40) == 0x6d90);  // (0x12+7)*0x10+0x6c00 — the +8-elem site
  assert(sh_(s + 0x42) == 0x7a48);  // (0x21+8)*8+0x7900
  assert(q_(s + 0x10) == 0x600050);  // param_1[2] qword RMW
  assert(q_(s + 0x30) == 0xf000d0);  // param_1[6] qword RMW
  // fn/00206040.c:53-74: the record prologue really writes qwords param_1[0]
  // (RMW, spills into s+4) and param_1[1] = 8; the "untouched" premise was wrong.
  // fn/00206040.c:53-74: the record prologue really writes qword *param_1
  // (RMW 0x1bff017fff for these params, spilling 0x1b into s+4) and
  // param_1[1] = 8 at byte +8; the "untouched" premise was wrong.
  assert(sh_(s + 4) == 0x1b && sh_(s + 8) == 8);
}
