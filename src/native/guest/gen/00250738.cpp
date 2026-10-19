// guest:0x00250738 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawWaypointDebugGrid(uint32_t param_1, ...) {
  byte bVar1;
  ulong uVar2;
  undefined8 uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  int iVar10;
uint32_t auStack_70 = rtguest::allocFrame(32);
  
  uVar7 = 0;
  iVar8 = 0;
  iVar9 = 0x18;
  iVar10 = 0;
  do {
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x24a)));
    if (((int)(bVar1 / 0x14 - 1) <= (int)uVar7) && ((int)uVar7 <= (int)(bVar1 / 0x14 + 1))) {
      iVar6 = 0;
      while( true ) {
        uVar5 = iVar8 + iVar6;
        if (bVar1 == uVar5) {
          uiDebugText8(iVar9,iVar6 * 8 + 0x40,0x3344c0,0xf);
          iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 600)));
        }
        else {
          iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 600)));
        }
        if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + uVar5))) != '\0') {
          uVar2 = (long)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25c))) + uVar5))) & 0xc0;
          uVar3 = 0x4f;
          if (uVar2 != 0x40) {
            uVar3 = 0x58;
            if (uVar2 != 0x80) {
              uVar3 = 0x20;
            }
          }
          ::sprintf((char *)rtguest::G((uint32_t)(auStack_70)), (char *)rtguest::G((uint32_t)(0x328050)), uVar3, (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + iVar10 + iVar6))) * 5, ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25c))) + iVar10 + iVar6))) & 0x3f) * 5);
          uiDebugText8((uVar7 & 7) * 0x50 + 0x20,iVar6 * 8 + 0x40,auStack_70,0xf);
        }
        iVar6 = iVar6 + 1;
        if (0x13 < iVar6) break;
        bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x24a)));
      }
    }
    uVar7 = uVar7 + 1;
    iVar10 = iVar10 + 0x14;
    iVar9 = iVar9 + 0x50;
    iVar8 = iVar8 + 0x14;
  } while ((int)uVar7 < 0xd);
  return 0;
}


RT_GUEST_FN(0x00250738, guest::DrawWaypointDebugGrid)

}  // namespace guest
