// guest:0x0024f928 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiArenaSpritesWrap(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x58) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x5c) = 0x00000000u;
  }
uint32_t puVar1;
  ushort uVar2;
  int iVar3;
  ushort uVar4;
  long lVar5;
  int iVar6;
  int iVar7;
  
  iVar7 = 0;
  if (0 < param_3) {
    do {
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t)));
      iVar3 = iVar6 + 0x50;
      if ((*(int32_t*)rtguest::G((uint32_t)param_1)) < iVar3) {
        (*(int32_t*)rtguest::G((uint32_t)param_1)) = 0;
        iVar6 = 0;
      }
      else {
        (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) = iVar3;
        iVar6 = (int)param_1 + iVar6 + 0x10;
      }
      if (iVar6 == 0) {
        return 0;
      }
      uVar2 = -(*(uint16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(8) * sizeof(uint16_t)));
      if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x12))) & 1) == 0) {
        uVar2 = (*(uint16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(8) * sizeof(uint16_t)));
      }
      uVar4 = -(*(uint16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(10) * sizeof(uint16_t)));
      if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x12))) & 2) == 0) {
        uVar4 = (*(uint16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(10) * sizeof(uint16_t)));
      }
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xc))) == 0) {
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 8)));
        if (iVar3 < 0) {
          iVar3 = -iVar3;
        }
        lVar5 = (long)(int)(short)iVar3;
      }
      else {
        lVar5 = (long)(short)(*(uint16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(0xc) * sizeof(uint16_t)));
      }
      RT_CALLRA(0x24fa70, guest::uiSpriteWriteWrap(iVar6,(int)(((uint32_t)(*(uint16_t*)rtguest::G((uint32_t)param_2)) + param_4) * 0x10000) >> 0x10,
                        (int)(((uint32_t)(*(uint16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(uint16_t))) + param_5) * 0x10000) >> 0x10,(*(uint16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(uint16_t))),(*(uint16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(6) * sizeof(uint16_t))),
                        uVar2,uVar4,lVar5));
      puVar1 = param_2 + 0x10;
      param_2 = param_2 + 0x14;
      iVar7 = iVar7 + 1;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x44))) = 0xffffff - ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)puVar1)) + param_6);
    } while (iVar7 < param_3);
  }
  return 0;
}


RT_GUEST_FN(0x0024f928, guest::uiArenaSpritesWrap)

}  // namespace guest
