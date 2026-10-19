// guest:0x00265b30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t notebookStampListTask(uint32_t param_1, uint32_t param_2, ...) {
    uint32_t uStack_50 = rtguest::allocFrame(8);
    uint32_t uStack_48 = rtguest::allocFrame(8);
    uint32_t uStack_40 = rtguest::allocFrame(8);
    uint32_t uStack_38 = rtguest::allocFrame(8);
  undefined1 uVar1;
  byte bVar2;
  ushort uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  
  iVar7 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) == 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x26))) = (uint32_t)(*(uint8_t*)rtguest::G(0x1780a40));
  }
  iVar9 = (int)param_2;
  switch((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25)))) {
  case 0:
  case 1:
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0xf))) == '\0') {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 2;
      bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x27))) | 2;
      goto LAB_00265ce4;
    }
    bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x27)));
    goto LAB_00265cec;
  case 2:
    iVar8 = (int)((int)(short)(*(uint8_t*)rtguest::G(0x1780a40)) + (uint32_t)((*(uint8_t*)rtguest::G(0x1780a40)) >> 0xf)) >> 1;
    iVar4 = NavigateGridSelection(param_2,iVar8,0x32,1,0,0);
    iVar5 = (iVar4 << 0x11) >> 0x11;
    (*(uint8_t*)rtguest::G(0x1780a40)) = (uint32_t)(iVar4 << 1);
    if (iVar8 < iVar5) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))) = 0x800;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 3;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 2))) = 0;
    }
    else if (iVar5 < iVar8) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 2))) = 0x8000;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))) = 0xfffff800;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 3;
    }
    else {
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x30))) & 0x10) == 0) {
        bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x27)));
        goto LAB_00265cec;
      }
      PlayUiSound(0x1b);
      gs_tasks_spawn(param_1,0x20dc50,0x40005);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 4;
    }
    break;
  case 3:
    uVar3 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 2))) + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20)));
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 2))) = uVar3;
    if (0x8000 < uVar3) {
      uVar1 = (uint32_t)(*(uint8_t*)rtguest::G(0x1780a40));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 2;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x26))) = uVar1;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 2))) = 0;
    }
    break;
  case 4:
    bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x27)));
    goto LAB_00265ccc;
  default:
    bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x27)));
LAB_00265ccc:
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x27))) = bVar2 & 0xfd;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0xf))) == '\0') {
      bVar2 = bVar2 & 0xfd | 1;
LAB_00265ce4:
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x27))) = bVar2;
      break;
    }
    bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x27)));
    goto LAB_00265cec;
  }
  bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x27)));
LAB_00265cec:
  if ((bVar2 & 2) == 0) goto LAB_00265f9c;
  uiArenaInit(0x1762400,0x10000);
  uiArenaBeginBatch(0x1762400,0,0);
  uiArenaSetTexture(0x1762400,0x2dbb50,0,0);
  uiArenaDataTag(0x1762400,1);
  uiArenaSpritesWrap(0x1762400,0x2dbda8,1,0,0,0xfffff0);
  drawBoardSpriteGrid(0x1762400,0);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) == '\x03') {
    drawRotatedQuadPair(0x1762400,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 2))));
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))) < 1) {
      if (0x4000 < (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 2)))) {
        uVar6 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x26)));
        goto LAB_00265de4;
      }
      drawStampCard(0x1762400,
                    ((int)((int)(short)(*(uint8_t*)rtguest::G(0x1780a40)) + (uint32_t)((*(uint8_t*)rtguest::G(0x1780a40)) >> 0xf)) >> 1) << 1);
    }
    else if ((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 2))) < 0x4000) {
      drawStampCard(0x1762400,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x26))) & 0xfe);
    }
    else {
      uVar6 = (int)(short)(*(uint8_t*)rtguest::G(0x1780a40)) + (uint32_t)((*(uint8_t*)rtguest::G(0x1780a40)) >> 0xf);
LAB_00265de4:
      drawStampCard(0x1762400,uVar6 | 1);
    }
  }
  else {
    iVar9 = ((int)((int)(short)(*(uint8_t*)rtguest::G(0x1780a40)) + (uint32_t)((*(uint8_t*)rtguest::G(0x1780a40)) >> 0xf)) >> 1) << 1;
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) & 0x20) != 0) {
      if (0 < iVar9) {
        (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) = 0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_48)) = 0x9800000052;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_40)) = 0x2800000016;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_38)) = 0;
        uiArenaDataTag(0x1762400,1);
        uiArenaSprites(0x1762400,uStack_50,1,0x1e,0xcc,1);
      }
      if (iVar9 < 0x62) {
        (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) = 0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_48)) = 0x9800000068;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_40)) = 0x2800000016;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_38)) = 0;
        uiArenaDataTag(0x1762400,1);
        uiArenaSprites(0x1762400,uStack_50,1,0x24c,0xcc,1);
      }
    }
    drawStampCard(0x1762400,
                  ((int)((int)(short)(*(uint8_t*)rtguest::G(0x1780a40)) + (uint32_t)((*(uint8_t*)rtguest::G(0x1780a40)) >> 0xf)) >> 1) << 1);
    drawStampCard(0x1762400,(int)(short)(*(uint8_t*)rtguest::G(0x1780a40)) + (uint32_t)((*(uint8_t*)rtguest::G(0x1780a40)) >> 0xf) | 1);
  }
  uiArenaSubmit(0x1762400);
  bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x27)));
LAB_00265f9c:
  if ((bVar2 & 1) != 0) {
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = 0xffffffff;
    gs_tasks_kill(param_1);
  }
  return 0;
}


RT_GUEST_FN(0x00265b30, guest::notebookStampListTask)

}  // namespace guest
