// guest:0x0020ad18 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t copyCarModel(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
    rtguest::setS32(3, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(4, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
  undefined8 uVar2;
uint32_t piVar3;
  int iVar4;
uint32_t puVar5;
  int iVar6;
  
  iVar6 = (int)param_2;
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ea470u)), param_1, param_2);
  (uint32_t)(uintptr_t)(::memmove((char *)rtguest::G((uint32_t)(0x3fbb80 + iVar6 * 0x34000)), (char *)rtguest::G((uint32_t)(0x3fbb80 + (int)param_1 * 0x34000)), 0x34000));
  puVar5 = 0x177f310 + iVar6 * 6;
  iVar4 = 5;
  piVar3 = (uint32_t)(0x3fbb80 + iVar6 * 0x34000);
  do {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar3));
    piVar3 = piVar3 + sizeof(int32_t);
    iVar4 = iVar4 + -1;
    (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = 0x3fbb80 + iVar6 * 0x34000 + iVar1;
    puVar5 = puVar5 + sizeof(uint32_t);
  } while (-1 < iVar4);
  iVar4 = (int)param_1 * 0x44;
  (*(uint64_t*)rtguest::G(0x29adb0)) = (long)(iVar4 + 0x243e) | 0x1300000000000000U |
                 (long)(iVar6 * 0x44 + 0x243e) << 0x20 | 0x13000000U | 0x2000000020000;
  (*(uint64_t*)rtguest::G(0x29add0)) = 0x8000000080;
  (*(uint32_t*)rtguest::G(0x29adc0)) = 0;
  RT_CALLRA(0x20ae68, guest::gifChainEndTag(0x29ad90,5));
  sendTexturePackets(0x29ad90);
  uVar2 = RT_CALLRA(0x20ae78, guest::sceDmaGetChan(2));
  RT_CALLRA(0x20ae88, guest::sceDmaSync(uVar2,0,0));
  (*(uint32_t*)rtguest::G(0x29adc0)) = 0;
  (*(uint64_t*)rtguest::G(0x29adb0)) = (long)(iVar4 + 0x247e) | (long)(iVar6 * 0x44 + 0x247e) << 0x20 | 0x1000000010000;
  (*(uint64_t*)rtguest::G(0x29add0)) = 0x1000000010;
  RT_CALLRA(0x20aec0, guest::gifChainEndTag(0x29ad90,5));
  sendTexturePackets(0x29ad90);
  return 0;
}


RT_GUEST_FN(0x0020ad18, guest::copyCarModel)

}  // namespace guest
