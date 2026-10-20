// guest:0x00237080 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawEntryRowsForState(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(80);
  byte bVar1;
  uint uVar2;
  uint uVar3;
  undefined8 uVar4;
uint32_t pbVar5;
  uint uVar6;
  undefined8 uVar7;
  int iVar8;
  uint uVar9;
  int iVar10;
uint32_t ppuVar11;
  
  iVar10 = 0x41;
  uVar9 = 0;
  uiDrawPanelCells(2,3,0x1c,0x11,0);
  uiDrawTextCell(3,4,0x333840,0);
  uiDrawTextCell(7,4,0x2ec430,0);
  uiDrawTextCell(0x10,4,0x333840,0);
  uiDrawTextCell(0x14,4,0x2ec430,0);
  uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
  ppuVar11 = 0x29fd30u;
  do {
    if ((int)(uVar9 * 4) < (int)uVar6) {
      uVar2 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10))) + 1)));
      iVar8 = 0;
      uVar3 = 0;
      if (uVar2 != 0) {
        pbVar5 = 0x0178700a;
        bVar1 = (*(uint8_t*)rtguest::G(0x178700a));
        while ((pbVar5 = pbVar5 + 4, bVar1 != uVar9 && (iVar8 = iVar8 + 1, iVar8 < (int)uVar2))) {
          bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
        }
        uVar3 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10))) + 1)));
      }
      if (iVar8 < (int)uVar3) {
        uiDrawText(0x50,iVar10,(*(uint32_t*)rtguest::G((uint32_t)ppuVar11)),0);
        uVar4 = GetPlayerScoreboardPtr(iVar8);
        uVar7 = 0;
        if ((iVar8 < 3) && (uVar7 = 6, ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 0x20) == 0)) {
          uVar7 = 0;
        }
        uiDrawText(0x96,iVar10,uVar4,uVar7);
        uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
      }
    }
    if ((int)(uVar9 * 4 + 0x30) < (int)uVar6) {
      uVar2 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10))) + 1)));
      iVar8 = 0;
      uVar3 = 0;
      if (uVar2 != 0) {
        pbVar5 = 0x0178700a;
        bVar1 = (*(uint8_t*)rtguest::G(0x178700a));
        while ((pbVar5 = pbVar5 + 4, (uint32_t)bVar1 != uVar9 + 0xc &&
               (iVar8 = iVar8 + 1, iVar8 < (int)uVar2))) {
          bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
        }
        uVar3 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10))) + 1)));
      }
      if (iVar8 < (int)uVar3) {
        uiDrawText(0x154,iVar10,(*(uint32_t*)rtguest::G((uint32_t)ppuVar11 + (uint32_t)(0xc) * sizeof(uint32_t))),0);
        uVar4 = GetPlayerScoreboardPtr(iVar8);
        uVar7 = 0;
        if ((iVar8 < 3) && (uVar7 = 6, ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 0x20) == 0)) {
          uVar7 = 0;
        }
        uiDrawText(0x19a,iVar10,uVar4,uVar7);
        uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
      }
    }
    uVar9 = uVar9 + 1;
    ppuVar11 = ppuVar11 + 1;
    iVar10 = iVar10 + 0xc;
  } while ((int)uVar9 < 0xc);
  if ((int)uVar6 < 0x60) {
    if ((uVar6 & 3) == 0) {
      PlayUiSound(0x22);
      return 0;
    }
  }
  else if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0x40) != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = 1;
    PlayUiSound(0x1a);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00237080, guest::DrawEntryRowsForState)

}  // namespace guest
