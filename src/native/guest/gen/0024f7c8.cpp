// guest:0x0024f7c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiArenaSprites(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t puVar1;
  ushort uVar2;
  int iVar3;
  ushort uVar4;
  int iVar5;
  int iVar6;
  
  iVar6 = 0;
  if (0 < param_3) {
    do {
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) + 0x50;
      iVar5 = (int)param_1 + (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) + 0x10;
      if ((*(int32_t*)rtguest::G((uint32_t)param_1)) < iVar3) {
        (*(int32_t*)rtguest::G((uint32_t)param_1)) = 0;
        iVar5 = 0;
      }
      else {
        (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) = iVar3;
      }
      if (iVar5 == 0) {
        return 0;
      }
      iVar6 = iVar6 + 1;
      uVar4 = -(*(uint16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(8) * sizeof(uint16_t)));
      if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xe))) & 1) == 0) {
        uVar4 = (*(uint16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(8) * sizeof(uint16_t)));
      }
      uVar2 = -(*(uint16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(10) * sizeof(uint16_t)));
      if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xe))) & 2) == 0) {
        uVar2 = (*(uint16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(10) * sizeof(uint16_t)));
      }
      uiSpriteWrite(iVar5,(int)(((uint32_t)(*(uint16_t*)rtguest::G((uint32_t)param_2)) + param_4) * 0x10000) >> 0x10,
                    (int)(((uint32_t)(*(uint16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(uint16_t))) + param_5) * 0x10000) >> 0x10,(*(uint16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(uint16_t))),(*(uint16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(6) * sizeof(uint16_t))),
                    uVar4,uVar2);
      puVar1 = param_2 + 0xc;
      param_2 = param_2 + 0x10;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x44))) = 0xffffff - ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)puVar1)) + param_6);
    } while (iVar6 < param_3);
  }
  return 0;
}


RT_GUEST_FN(0x0024f7c8, guest::uiArenaSprites)

}  // namespace guest
