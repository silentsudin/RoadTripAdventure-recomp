// guest:0x0021e070 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ResolveCameraTerrainCollision(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(112);
  short sVar1;
  long lVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  float fVar6;
uint32_t auStack_70 = rtguest::allocFrame(4);
uint32_t auStack_6c = rtguest::allocFrame(12);
  
  iVar4 = (int)param_1;
  iVar5 = iVar4 + 0x100;
  fVar6 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x104)));
  lVar2 = rtCall((*(uint32_t*)rtguest::G(0x335710)), iVar5, auStack_70, auStack_6c, param_6, param_3);
  iVar3 = (int)param_2;
  if ((lVar2 == -1) || (fVar6 < (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x104))))) {
    sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x16)));
    while( true ) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x16))) = sVar1 + -0x80;
      RT_CALLRA(0x21e118, guest::buildCamera(param_1,param_2,param_4,param_5));
      if (0 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x16)))) break;
      fVar6 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x104)));
      lVar2 = rtCall((*(uint32_t*)rtguest::G(0x335710)), iVar5, auStack_70, auStack_6c, param_6, param_3);
      if (lVar2 == -1) {
        sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x16)));
      }
      else {
        if ((*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x104))) <= fVar6) break;
        sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x16)));
      }
    }
    do {
      fVar6 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x114)));
      lVar2 = rtCall((*(uint32_t*)rtguest::G(0x335710)), iVar4 + 0x110, auStack_70, auStack_6c, param_6, param_3);
      if ((lVar2 != -1) && ((*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x114))) <= fVar6)) {
        return 0;
      }
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x16))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x16))) + -0x80;
      RT_CALLRA(0x21e1bc, guest::buildCamera(param_1,param_2,param_4,param_5));
    } while ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x16))) < 1);
  }
  else {
    fVar6 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x114)));
    lVar2 = rtCall((*(uint32_t*)rtguest::G(0x335710)), iVar4 + 0x110, auStack_70, auStack_6c, param_6, param_3);
    if ((lVar2 == -1) || (fVar6 < (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x114))))) {
      sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x16)));
      while( true ) {
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x16))) = sVar1 + 0x80;
        RT_CALLRA(0x21e238, guest::buildCamera(param_1,param_2,param_4,param_5));
        if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x16))) < 0) break;
        fVar6 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x114)));
        lVar2 = rtCall((*(uint32_t*)rtguest::G(0x335710)), iVar4 + 0x110, auStack_70, auStack_6c, param_6, param_3);
        if (lVar2 == -1) {
          sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x16)));
        }
        else {
          if ((*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x114))) <= fVar6) break;
          sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x16)));
        }
      }
      do {
        fVar6 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x104)));
        lVar2 = rtCall((*(uint32_t*)rtguest::G(0x335710)), iVar5, auStack_70, auStack_6c, param_6, param_3);
        if ((lVar2 != -1) && ((*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x104))) <= fVar6)) {
          return 0;
        }
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x16))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x16))) + 0x80;
        RT_CALLRA(0x21e2dc, guest::buildCamera(param_1,param_2,param_4,param_5));
      } while (-1 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x16))));
    }
  }
  return 0;
}


RT_GUEST_FN(0x0021e070, guest::ResolveCameraTerrainCollision)

}  // namespace guest
