// guest:0x00236d28 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawRowsAndUpdateState(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = (uint32_t)(int32_t)param_2;
  }
  undefined1 uVar1;
  int iVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  
  uiDrawPanelCells(2,4,0x1c,0xe,0);
  uiDrawTextCell(6,5,0x2ec4c0,0);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) == '\x01') {
    uiDrawTextCell(0x14,5,0x3337f8,0);
  }
  else {
    uiDrawTextCell(0x14,5,0x333858,0);
  }
  iVar10 = 0;
  iVar8 = 0;
  iVar9 = 0;
  iVar7 = 0x4d;
  do {
    uVar4 = 0;
    if ((iVar10 == 0) && (uVar4 = 6, ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 0x20) == 0)) {
      uVar4 = 0;
    }
    uVar4 = uiDrawText(0x5a,iVar7,0x333850,uVar4);
    uVar3 = GetPlayerScoreboardPtr(iVar8);
    if ((iVar10 != 0) || (uVar5 = 6, ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 0x20) == 0)) {
      uVar5 = 0;
    }
    uiDrawText(uVar4,iVar7,uVar3,uVar5);
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) == '\x01') {
      if ((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x178700a + (uint32_t)(iVar9) * 1)) != 0xff) {
        uiDrawText(0x15e,iVar7,(*(uint32_t*)rtguest::G((uint32_t)(0x29fd30u + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x0178700au + (uint32_t)(iVar9) * 4))) * 4))),0);
      }
      if ((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x178700e + (uint32_t)(iVar9) * 4)) != 0xff) {
        uiDrawText(0x19a,iVar7,(*(uint32_t*)rtguest::G((uint32_t)(0x29fd30u + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x0178700eu + (uint32_t)(iVar9) * 4))) * 4))),0);
      }
      if ((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x1787012 + (uint32_t)(iVar9) * 1)) != 0xff) {
        uiDrawText(0x1d6,iVar7,(*(uint32_t*)rtguest::G((uint32_t)(0x29fd30u + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x01787012u + (uint32_t)(iVar9) * 4))) * 4))),0);
      }
    }
    else {
      iVar6 = iVar7 << 1;
      uiDrawNumber10(0x17c,iVar6,(*(uint32_t*)rtguest::G((uint32_t)0x1782a74 + (uint32_t)(iVar8 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) * 0x18) * 4)),0);
      uiDrawText(0x186,iVar7,0x333860,0);
      uiDrawNumber10(0x1b8,iVar6,
                     (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) * 0x18 + 0x1782a75))),0);
      uiDrawText(0x1c2,iVar7,0x333860,0);
      uiDrawNumber10(500,iVar6,
                     (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) * 0x18 + 0x1782a76))),0);
      iVar2 = iVar8 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) * 0x18;
      uiDrawNumber10(0x230,iVar6,
                     (int)(char)(*(uint32_t*)rtguest::G((uint32_t)0x1782a74 + (uint32_t)(iVar2) * 4)) + (int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1782a75))) +
                     (int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1782a76))),0);
    }
    iVar10 = iVar10 + 1;
    iVar8 = iVar8 + 3;
    iVar9 = iVar9 + 0xc;
    iVar7 = iVar7 + 0xc;
  } while (iVar10 < 8);
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0x40) != 0) {
    PlayUiSound(0x1a);
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) == '\x01') {
      uVar1 = 2;
    }
    else {
      uVar1 = 3;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = 0;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = uVar1;
  }
  return 0;
}


RT_GUEST_FN(0x00236d28, guest::DrawRowsAndUpdateState)

}  // namespace guest
