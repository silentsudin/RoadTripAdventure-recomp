// guest:0x002490a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateRaceHud(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
  }
  char cVar1;
  bool bVar2;
  uint uVar3;
uint32_t piVar4;
  int iVar5;
uint32_t pbVar6;
uint32_t puVar7;
  undefined8 uVar8;
  
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 8))) == 0) {
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(0x01757bc0)), 0, 0x2020));
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x28))) & 0x12000) == 0) {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x22)));
    uVar8 = 0;
    bVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x1c))) != 0;
    if ((cVar1 == -1) || (cVar1 == '\"')) {
      uVar8 = 1;
    }
    iVar5 = 0;
    if (bVar2 != 0xffffffff) {
      piVar4 = 0x0177add4;
      puVar7 = 0x01757bc0;
      pbVar6 = (uint32_t)((int)0x0177f778 + 2);
      do {
        if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(piVar4 + 5))) & 0x200) == 0) {
          uVar3 = 10;
          if (!bVar2) {
            uVar3 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar6));
          }
          rtCall((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(0x2a8fb0u + (uint32_t)(uVar3) * 4))))), param_1, param_2, puVar7, bVar2, iVar5, (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)piVar4)) + 8))) >> 0xc & 1, uVar8);
        }
        iVar5 = iVar5 + 1;
        puVar7 = puVar7 + 0x1010;
        piVar4 = piVar4 + 0x270;  // T* element step
        pbVar6 = pbVar6 + 0x3448;
      } while (iVar5 < (int)(bVar2 + 1));
    }
  }
  return 0;
}


RT_GUEST_FN(0x002490a0, guest::UpdateRaceHud)

}  // namespace guest
