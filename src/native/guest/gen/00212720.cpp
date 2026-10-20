// guest:0x00212720 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void DrawMenuCursorAndIcons(uint32_t param_1, uint64_t param_2, uint32_t param_3, uint64_t param_4) {
  rtguest::FrameGuard rt_fg;
    uint32_t uStack_a0 = rtguest::allocFrame(8);
    uint32_t uStack_98 = rtguest::allocFrame(8);
    uint32_t uStack_90 = rtguest::allocFrame(8);
    uint32_t uStack_88 = rtguest::allocFrame(8);
    uint32_t uStack_80 = rtguest::allocFrame(4);
    uint32_t uStack_7c = rtguest::allocFrame(4);
    uint32_t iStack_78 = rtguest::allocFrame(4);
    uint32_t iStack_74 = rtguest::allocFrame(4);
    uint32_t uStack_70 = rtguest::allocFrame(4);
    uint32_t uStack_6c = rtguest::allocFrame(4);
    uint32_t uStack_68 = rtguest::allocFrame(8);
    uint32_t iStack_60 = rtguest::allocFrame(4);
  uint uVar1;
  int iVar2;
  
  iVar2 = (int)param_4 + -1;
  if (param_4 != 0) {
    (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = param_1;
    do {
      if (((*(uint32_t*)rtguest::G((uint32_t)param_3)) & 1) != 0) {
        uiArenaSetTexture(param_2,0x29c030,2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0029c360 + (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(5) * sizeof(uint32_t))) * 4))));
        uiArenaDataTag(param_2,1);
        uiArenaSprites(param_2,0x29c340,1,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t))));
        uiArenaSetTexture(param_2,0x29c030,2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0029c388 + (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(4) * sizeof(uint32_t))) * 4))));
        uiArenaDataTag(param_2,1);
        uiArenaSprites(param_2,0x29c368,1,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t))));
        uiArenaSetTexture(param_2,0x29c030,2,0xc);
        uiArenaDataTag(param_2,2);
        if ((*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(8) * sizeof(uint32_t))) == 0) {
          uVar1 = 0;
        }
        else {
          uVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 8))) >> 4;
        }
        uiArenaSprites(param_2,(uVar1 & 3) * 0x20 + 0x29c3c8,1,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))) + 0x5e,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))) + 10,
                       (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t))));
        uiArenaSprites(param_2,(uVar1 + 1 & 3) * 0x20 + 0x29c3c8,1,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))) + 0xe2,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))) + 8,
                       (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t))));
        (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_80)), 0, 0x20));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_7c)) = 10;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_80)) = 99;
        (*(int32_t*)rtguest::G((uint32_t)iStack_78)) = (int)(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(7) * sizeof(uint32_t))) / 0xb << 7;
        (*(int32_t*)rtguest::G((uint32_t)iStack_74)) = ((int)(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(7) * sizeof(uint32_t))) % 0xb) * 0x16;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = 0x80;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = 0x16;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) = 0xa00000063;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_98)) = CONCAT44((*(int32_t*)rtguest::G((uint32_t)iStack_74)),(*(int32_t*)rtguest::G((uint32_t)iStack_78)));
        (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = 0x1600000080;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_88)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_68));
        uiArenaSetTexture(param_2,0x29c030,1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0029c448 + (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(6) * sizeof(uint32_t))) * 4))));
        uiArenaDataTag(param_2,1);
        uiArenaSprites(param_2,uStack_a0,1,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t))));
      }
      iVar2 = iVar2 + -1;
      param_3 = param_3 + 9;
    } while (iVar2 != -1);
  }
  return;
}


RT_GUEST_FN(0x00212720, guest::DrawMenuCursorAndIcons)

}  // namespace guest
