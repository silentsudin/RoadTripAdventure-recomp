// guest:0x00237878 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawLocationList(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  byte bVar1;
uint32_t puVar2;
  undefined8 uVar3;
  long lVar4;
  int iVar5;
  short sVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  int iVar10;
uint32_t pbVar11;
  
  iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10)));
  iVar10 = 0;
  iVar9 = (int)param_1;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 1))) != '\0') {
    pbVar11 = 0x0177ae97;
    do {
      if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pbVar11 + -0xaf))) != 0) {
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar11));
        iVar8 = (int)((((uint32_t)bVar1 * 2 + (uint32_t)bVar1) * 8 - (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2)))) + 0x80) / 2
        ;
        uiDrawText(0x78,iVar8,(*(uint32_t*)rtguest::G((uint32_t)(0x29fd30u + (uint32_t)(bVar1) * 4))),0);
        uVar3 = GetPlayerScoreboardPtr(iVar10);
        uiDrawText(0xb4,iVar8,uVar3,0);
        if (((*(uint32_t*)rtguest::G((uint32_t)0x178700b + (uint32_t)(iVar10 * 4) * 4)) & 0xf0) == 0) {
          lVar4 = TestStateBitFc78(0x28,iVar8,0x01787008 + iVar10 * 4,0);
          iVar5 = 0x48;
          if (lVar4 == 0) {
            iVar5 = 8;
          }
          puVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2be438 + iVar5)));
        }
        else {
          puVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be438 + (uint32_t)((char)(*(uint32_t*)rtguest::G((uint32_t)0x1787008 + (uint32_t)(iVar10 * 4) * 4)) * 2) * 4));
        }
        uiDrawText(0x17c,iVar8,puVar2,0);
        iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10)));
      }
      iVar10 = iVar10 + 1;
      pbVar11 = pbVar11 + 0x270;
    } while (iVar10 < (int)(uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 1))));
  }
  sVar6 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2)));
  uVar7 = (uint32_t)sVar6;
  if ((long)sVar6 < (long)(int)((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 1))) * 0x18 + -0x120)) {
    lVar4 = (long)sVar6;
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x2c))) & 0x4000) != 0) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = (short)(uVar7 + 2);
      lVar4 = (long)(int)(uVar7 + 2);
    }
    sVar6 = (short)lVar4;
    uVar7 = (uint32_t)lVar4;
  }
  if ((uVar7 & 0xffff) == 0) {
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8)));
  }
  else {
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x2c))) & 0x1000) != 0) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = sVar6 + -2;
    }
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8)));
  }
  if ((iVar8 != 0) && (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0x50) != 0)) {
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 4))) = 1;
    gs_tasks_kill(param_1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00237878, guest::DrawLocationList)

}  // namespace guest
