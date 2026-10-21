// guest:0x00244988 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
typedef uint32_t rt_u32u __attribute__((aligned(1)));  // unaligned guest access: defined behavior, LE result unchanged

namespace guest {

uint32_t BuildCarouselDrawPacket(uint32_t rt_bits_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  float param_1 = rtguest::fbits(rt_bits_1);  // f12.. float arg carried as bits (W13 float-param fix)
  rtguest::FrameGuard rt_fg(128);
  uint uVar1;
  ushort uVar2;
  ushort uVar3;
  int iVar4;
  ulong uVar5;
  ulong uVar6;
  int iVar7;
  ulong uVar8;
  int iVar9;
  ulong uVar10;
uint32_t auStack_80 = rtguest::allocFrame(64);
  
  iVar4 = param_5 * 0x6e;
  iVar7 = param_5 * 0x32;
  iVar9 = param_5 * 0xce;
  if (iVar4 < 0) {
    iVar4 = iVar4 + 0xff;
  }
  if (iVar7 < 0) {
    iVar7 = iVar7 + 0xff;
  }
  if (iVar9 < 0) {
    iVar9 = iVar9 + 0xff;
  }
  uVar8 = ((long)(param_4 + (iVar7 >> 8)) & 0xffffU) << 0x20;
  uVar10 = (long)(param_4 + (iVar9 >> 8)) << 0x30;
  uVar5 = (long)(param_3 - (iVar4 >> 8)) & 0xffffU |
          ((long)(param_3 + (iVar4 >> 8)) & 0xffffU) << 0x10;
  uVar6 = uVar5 | uVar8 | uVar10;
  if ((*(uint32_t*)rtguest::G(0x333288)) != 0) {
    uVar1 = (uint32_t)uVar5;
    uVar5 = (long)(((int)(uVar1 << 0x10) >> 0xe) / 5) & 0xffff;
    uVar6 = uVar8 | uVar10 | uVar5 |
            ((long)((((int)(uVar1 & 0xffff0000 | (uint32_t)uVar5) >> 0x10) << 2) / 5) & 0xffffU) << 0x10
    ;
  }
  uVar3 = (uint32_t)(uVar6 >> 0x30);
  uVar2 = (uint32_t)(uVar6 >> 0x20);
  uVar5 = uVar6 & 0xffffffff |
          ((long)((int)((int)(short)uVar2 + (uint32_t)(uVar2 >> 0xf)) >> 1) & 0xffffU) << 0x20 |
          (long)((int)((int)(short)uVar3 + (uint32_t)(uVar3 >> 0xf)) >> 1) << 0x30;
  if ((int)uVar6 << 0x10 < 0) {
    uVar5 = 0xdf0000027f0000;
  }
  RT_CALLRA(0x244b6c, guest::gifAdPair(param_2 + 0x30,uVar5,0x41));
  RT_CALLRA(0x244b7c, guest::vu0CopyMatrix(auStack_80,0x2a8b70));
  if (param_1 == 0.0) {
    iVar4 = (int)param_6;
    if (param_6 < 0x2c) {
      (*(float*)rtguest::G(0x2a8b60)) = (float)(iVar4 * 0xd) + 440.0;
    }
    else {
      (*(float*)rtguest::G(0x2a8b60)) = 1024.0;
    }
    if (param_6 < 0x2d) {
      (*(uint32_t*)rtguest::G(0x2a8b64)) = (short)param_6 * -0x100 + 0x4000;
    }
    else {
      (*(uint32_t*)rtguest::G(0x2a8b64)) = 0x1300;
    }
    (*(rt_u32u*)rtguest::G(0x2a8b66)) = (uint32_t)(iVar4 << 5);
    if (param_6 < 0x14) {
      (*(uint32_t*)rtguest::G(0x2a8b54)) = rtguest::f2bits((float)iVar4);
    }
    else {
      (*(uint32_t*)rtguest::G(0x2a8b54)) = rtguest::f2bits(20.0f);
    }
  }
  else {
    (*(uint32_t*)rtguest::G(0x2a8b54)) = 0;
    (*(float*)rtguest::G(0x2a8b60)) = 440.0;
    (*(uint32_t*)rtguest::G(0x2a8b64)) = 0x4000;
    (*(rt_u32u*)rtguest::G(0x2a8b66)) = 0;
  }
  if ((*(uint32_t*)rtguest::G(0x333288)) == 0) {
    RT_CALLRA(0x244cc4, guest::SetViewDistanceGlobals(rtguest::f2bits((float)(param_3 + 0x6c0)),rtguest::f2bits((float)((param_4 + param_5 / 2) / 2 + 0x790))));
  }
  else {
    RT_CALLRA(0x244d14, guest::SetViewDistanceGlobals(rtguest::f2bits((float)((param_3 << 2) / 5 + 0x700)),rtguest::f2bits((float)((param_4 + param_5 / 2) / 2 + 0x780))));
  }
  RT_CALLRA(0x244d28, guest::buildCamera(param_2 + 0x50,0x2a8b50,auStack_80,auStack_80));
  RT_CALLRA(0x244d30, guest::updateLightDirection(param_2 + 0x1d0));
  return 0;
}


RT_GUEST_FN(0x00244988, guest::BuildCarouselDrawPacket)

}  // namespace guest
