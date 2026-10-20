// guest:0x00211f68 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void DrawMenuListItems(uint64_t param_1, uint64_t param_2, uint32_t param_3, uint64_t param_4, uint32_t param_5) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  
  iVar5 = (int)param_4 + -1;
  if (param_4 != 0) {
    do {
      if (((*(uint32_t*)rtguest::G((uint32_t)param_3)) & 1) != 0) {
        uiArenaSetTexture(param_2,0x29c030,0,0);
        uiArenaDataTag(param_2,7);
        uiArenaSprites(param_2,0x29c0c0,7,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t))));
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_5 + 0x28))) & 0x1000) == 0) {
          uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(4) * sizeof(uint32_t)));
          iVar4 = 0;
        }
        else {
          uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(4) * sizeof(uint32_t)));
          if ((((((uVar3 == 1) || (uVar3 == 0xc)) || (uVar3 == 0xd)) ||
               ((uVar3 == 0xe || (uVar3 == 0xf)))) ||
              ((uVar3 == 0x10 || ((uVar3 == 0x11 || (uVar3 == 0x12)))))) ||
             ((uVar3 == 0x13 || ((uVar3 == 0x14 || (iVar4 = 0, uVar3 == 0x15)))))) {
            DrawTextHalfY((*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))) + 0x1e,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))) + 0x1c,0x2eaae8,6);
            DrawTextHalfY((*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))) + 0x1e,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))) + 0x34,0x333548,0);
            DrawTextHalfY((*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))) + 0x1e,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))) + 0x4c,0x2eaaf8,0);
            DrawTextHalfY((*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))) + 0x1e,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))) + 100,0x333550,0);
            goto LAB_00212150;
          }
        }
        while( true ) {
          iVar2 = uVar3 * 4 + iVar4;
          iVar1 = iVar4 * 0x18;
          iVar4 = iVar4 + 1;
          DrawTextHalfY((*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))) + 0x1e,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))) + iVar1 + 0x28,(*(uint32_t*)rtguest::G((uint32_t)(0x29c1a0u + (uint32_t)(iVar2) * 4))),0
                       );
          if (3 < iVar4) break;
          uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(4) * sizeof(uint32_t)));
        }
      }
LAB_00212150:
      iVar5 = iVar5 + -1;
      param_3 = param_3 + 5;
    } while (iVar5 != -1);
  }
  return;
}


RT_GUEST_FN(0x00211f68, guest::DrawMenuListItems)

}  // namespace guest
