// guest:0x00244800 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t BuildCarouselQuadD(uint32_t rt_bits_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  float param_1 = rtguest::fbits(rt_bits_1);  // f12.. float arg carried as bits (W13 float-param fix)
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_5);  // walk: = param_5
    rtguest::setS32(2, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(3, (uint32_t)param_4);  // walk: = param_4
  }
  short sVar1;
  int iVar2;
uint32_t puVar3;
  uint uVar4;
  short sVar5;
  int iVar6;
  int iVar7;
  float fVar8;
  
  fVar8 = 0.0;
  param_2 = param_2 + param_3 * 0xd0;
  if (param_1 == 0.0) {
    iVar2 = 0x80;
  }
  else {
    // MIPS 0x244850: jal cos with $a0 = raw 64-bit $4 (no cvt.d.w); cos result unused, f0 stays 0.0
    RT_CALLRA(0x244850, guest::cos((uint64_t)(int64_t)(int32_t)param_2));
    iVar2 = (int)(fVar8 * 16.0) + 0x60;
  }
  puVar3 = (uint32_t)(param_2 + 0x4c);
  iVar6 = 3;
  do {
    iVar6 = iVar6 + -1;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(-1) * sizeof(uint32_t))) = iVar2;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(-2) * sizeof(uint32_t))) = iVar2;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(-3) * sizeof(uint32_t))) = iVar2;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3)) = 0x80;
    puVar3 = puVar3 + 0x30;  // T* element step
  } while (-1 < iVar6);
  iVar2 = param_6 * 0x60;
  iVar7 = param_6 * 0xc;
  iVar6 = param_6 * 0x28;
  if (iVar2 < 0) {
    iVar2 = iVar2 + 0xff;
  }
  uVar4 = param_4 - (iVar2 >> 8);
  if (iVar7 < 0) {
    iVar7 = iVar7 + 0xff;
  }
  sVar1 = param_5 + (short)((uint32_t)iVar7 >> 8);
  if (iVar6 < 0) {
    iVar6 = iVar6 + 0xff;
  }
  param_5 = param_5 + (short)((uint32_t)iVar6 >> 8);
  sVar5 = (short)uVar4;
  if ((*(uint32_t*)rtguest::G(0x333288)) == 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x50))) = sVar5 * 0x10 + 0x6bf8;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x54))) = sVar1 * 8 + 0x78f8;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x54))) = sVar1 * 8 + 0x77f8;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x50))) = ((int)sVar5 << 6) / 5 + 0x6ff8;
  }
  iVar6 = (*(uint32_t*)rtguest::G(0x333288));
  param_6 = param_6 << 4;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x58))) = param_6;
  iVar2 = (int)(uVar4 & 0xffff | (param_4 + (iVar2 >> 8)) * 0x10000) >> 0x10;
  if (iVar6 == 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x80))) = iVar2 * 0x10 + 0x6bf8;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x84))) = sVar1 * 8 + 0x78f8;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x84))) = sVar1 * 8 + 0x77f8;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x80))) = (iVar2 << 6) / 5 + 0x6ff8;
  }
  iVar6 = (*(uint32_t*)rtguest::G(0x333288));
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x88))) = param_6;
  if (iVar6 == 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xb0))) = sVar5 * 0x10 + 0x6bf8;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xb4))) = param_5 * 8 + 0x78f8;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xb4))) = param_5 * 8 + 0x77f8;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xb0))) = ((int)sVar5 << 6) / 5 + 0x6ff8;
  }
  iVar6 = (*(uint32_t*)rtguest::G(0x333288));
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xb8))) = param_6;
  if (iVar6 == 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xe0))) = iVar2 * 0x10 + 0x6bf8;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xe4))) = param_5 * 8 + 0x78f8;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xe4))) = param_5 * 8 + 0x77f8;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xe0))) = (iVar2 << 6) / 5 + 0x6ff8;
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xe8))) = param_6;
  return 0;
}


RT_GUEST_FN(0x00244800, guest::BuildCarouselQuadD)

}  // namespace guest
