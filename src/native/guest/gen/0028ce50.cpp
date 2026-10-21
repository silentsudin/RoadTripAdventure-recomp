// guest:0x0028ce50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t sceSifRebootIop(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x50, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x70) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x74) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  byte bVar1;
  uint uVar2;
  undefined8 uVar3;
uint32_t pcVar4;
  char cVar5;
uint32_t pbVar6;
  ulong uVar7;
uint32_t pcVar8;
uint32_t acStack_80 = rtguest::allocFrame(80);
  
  pcVar4 = acStack_80;
  pcVar8 = 0x3327c0u;
  if ((*(int8_t*)rtguest::G((uint32_t)param_1)) == 0) {
    uVar2 = 0xb;
  }
  else {
    pbVar6 = param_1;
    do {
      pbVar6 = pbVar6 + 1;
    } while ((*(int8_t*)rtguest::G((uint32_t)pbVar6)) != 0);
    uVar2 = (int)pbVar6 - (int)(param_1 + -0xb);
  }
  if (uVar2 < 0x51) {
    RT_CALLRA(0x28cec4, guest::sceSifInitRpc(0));
    RT_CALLRA(0x28cecc, guest::sceSifExitRpc());
    cVar5 = 'r';
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_1));
    do {
      (*(int8_t*)rtguest::G((uint32_t)pcVar4)) = cVar5;
      pcVar8 = pcVar8 + 1;
      pcVar4 = pcVar4 + 1;
      cVar5 = (*(int8_t*)rtguest::G((uint32_t)pcVar8));
    } while ((*(int8_t*)rtguest::G((uint32_t)pcVar8)) != '\0');
    uVar7 = (ulong)bVar1;
    if (uVar7 == 0) {
      (*(int8_t*)rtguest::G((uint32_t)pcVar4)) = '\0';
    }
    else {
      do {
        (*(int8_t*)rtguest::G((uint32_t)pcVar4)) = (char)uVar7;
        param_1 = param_1 + 1;
        pcVar4 = pcVar4 + 1;
        uVar7 = (ulong)(char)(*(int8_t*)rtguest::G((uint32_t)param_1));
      } while (uVar7 != 0);
      (*(int8_t*)rtguest::G((uint32_t)pcVar4)) = '\0';
    }
    uVar3 = RT_CALLRA(0x28cf44, guest::sceSifResetIop(acStack_80,0));
  }
  else {
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x3327d0u)), param_1);
    uVar3 = 0;
  }
  return uVar3;
}


RT_GUEST_FN(0x0028ce50, guest::sceSifRebootIop)

}  // namespace guest
