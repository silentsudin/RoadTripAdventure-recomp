// guest:0x00280f88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t exponent(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(320);
  int iVar1;
  char cVar2;
  undefined1 uVar3;
uint32_t pcVar4;
uint32_t pcVar5;
uint32_t pcVar6;
uint32_t pcVar7;
uint32_t acStack_e = rtguest::allocFrame(14);
  
  (*(uint8_t*)rtguest::G((uint32_t)param_1)) = param_3;
  if (param_2 < 0) {
    uVar3 = 0x2d;
    param_2 = -param_2;
  }
  else {
    uVar3 = 0x2b;
  }
  (*(uint8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint8_t))) = uVar3;
  pcVar4 = param_1 + 2;
  pcVar5 = acStack_e + 2;
  pcVar6 = pcVar5;
  if (param_2 < 10) {
    (*(int8_t*)rtguest::G((uint32_t)pcVar4)) = '0';
    (*(uint8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint8_t))) = (char)param_2 + '0';
    pcVar4 = param_1 + 4;
  }
  else {
    do {
      pcVar7 = pcVar6;
      iVar1 = param_2 / 10;
      pcVar6 = pcVar7 + -1;
      (*(int8_t*)rtguest::G((uint32_t)pcVar6)) = (char)(param_2 % 10) + '0';
      param_2 = iVar1;
    } while (9 < iVar1);
    pcVar7 = pcVar7 + -2;
    (*(int8_t*)rtguest::G((uint32_t)pcVar7)) = (char)iVar1 + '0';
    if (pcVar7 < pcVar5) {
      do {
        cVar2 = (*(int8_t*)rtguest::G((uint32_t)pcVar7));
        pcVar7 = pcVar7 + 1;
        (*(int8_t*)rtguest::G((uint32_t)pcVar4)) = cVar2;
        pcVar4 = pcVar4 + 1;
      } while (pcVar7 < pcVar5);
      return (int)pcVar4 - (int)param_1;
    }
  }
  return (int)pcVar4 - (int)param_1;
}


RT_GUEST_FN(0x00280f88, guest::exponent)

}  // namespace guest
