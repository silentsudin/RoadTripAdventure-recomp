// guest:0x00265568 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t HouseVisitedMenuTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(112);
  short sVar1;
  undefined1 uVar2;
  byte bVar3;
  ushort uVar4;
  int iVar5;
  long lVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  int iVar10;
uint32_t auStack_70 = rtguest::allocFrame(32);
  
  iVar9 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) == 0) {
    lVar6 = PopByteQueue();
    iVar5 = (int)lVar6 + -1;
    if (lVar6 == 0) {
      iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4))) = 0xffffffff;
      gs_tasks_kill(param_1);
      return 0;
    }
    (*(uint8_t*)rtguest::G(0x1780a40)) = (uint32_t)iVar5 & 0xff;
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x26))) = (char)iVar5;
  }
  if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) & 2) != 0) {
    uiArenaInit(0x1762400,0x10000);
    uiArenaBeginBatch(0x1762400,0,0);
    uiArenaSetTexture(0x1762400,0x2dbb50,0,0);
    uiArenaDataTag(0x1762400,1);
    uiArenaSpritesWrap(0x1762400,0x2dbda8,1,0,0,0xfffff0);
    drawBoardSpriteGrid(0x1762400,0);
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) == '\x03') {
      iVar10 = 0;
      drawStampCard(0x1762400,
                    ((int)((int)(short)(*(uint8_t*)rtguest::G(0x1780a40)) + (uint32_t)((*(uint8_t*)rtguest::G(0x1780a40)) >> 0xf)) >> 1) << 1);
      drawStampCard(0x1762400,(int)(short)(*(uint8_t*)rtguest::G(0x1780a40)) + (uint32_t)((*(uint8_t*)rtguest::G(0x1780a40)) >> 0xf) | 1);
      drawStampIcon(0x1762400,(*(uint8_t*)rtguest::G(0x1780a40)),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))));
      uVar4 = (*(uint8_t*)rtguest::G(0x1780a40));
      iVar5 = (int)(short)(*(uint8_t*)rtguest::G(0x1780a40));
      while( true ) {
        lVar6 = ExtractDelimitedToken
                          (auStack_70,(*(uint32_t*)rtguest::G((uint32_t)(0x2dbba0u + (uint32_t)(iVar5) * 4))),
                           0x335300,iVar10);
        if (lVar6 == 0) break;
        iVar7 = iVar10 * 0x18;
        iVar10 = iVar10 + 1;
        DrawTextHalfY(((int)(short)uVar4 + ((int)(short)uVar4 - (iVar5 >> 0x1f) >> 1) * -2) * 0x120
                      + 0x3c,iVar7 + 0x110,auStack_70,3);
      }
    }
    else if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) == '\x05') {
      drawRotatedQuadPair(0x1762400,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))));
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) < 1) {
        if (0x4000 < (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2)))) {
          uVar8 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x26)));
          goto LAB_002657dc;
        }
        drawStampCard(0x1762400,
                      ((int)((int)(short)(*(uint8_t*)rtguest::G(0x1780a40)) + (uint32_t)((*(uint8_t*)rtguest::G(0x1780a40)) >> 0xf)) >> 1) << 1);
      }
      else if ((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) < 0x4000) {
        drawStampCard(0x1762400,(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x26))) & 0xfe);
      }
      else {
        uVar8 = (int)(short)(*(uint8_t*)rtguest::G(0x1780a40)) + (uint32_t)((*(uint8_t*)rtguest::G(0x1780a40)) >> 0xf);
LAB_002657dc:
        drawStampCard(0x1762400,uVar8 | 1);
      }
    }
    else {
      drawStampCard(0x1762400,
                    ((int)((int)(short)(*(uint8_t*)rtguest::G(0x1780a40)) + (uint32_t)((*(uint8_t*)rtguest::G(0x1780a40)) >> 0xf)) >> 1) << 1);
      drawStampCard(0x1762400,(int)(short)(*(uint8_t*)rtguest::G(0x1780a40)) + (uint32_t)((*(uint8_t*)rtguest::G(0x1780a40)) >> 0xf) | 1);
    }
    uiArenaSubmit(0x1762400);
  }
  switch((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)))) {
  case 0:
    gs_tasks_spawn(param_1,0x20dc50,0x40017);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 1;
    break;
  case 1:
    bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) != '\0') goto LAB_00265adc;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 2;
    bVar3 = bVar3 | 2;
    goto LAB_00265ad4;
  case 2:
    sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2)));
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = sVar1 + 1;
    if (10 < sVar1) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) = 10;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 3;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = 0;
    }
    break;
  case 3:
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) + -1;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) = iVar5;
    if (iVar5 == 0) {
      SetStateBitFc78AndFc88((uint32_t)(*(uint8_t*)rtguest::G(0x1780a40)));
      PlayUiSound(0x21);
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 4;
    }
    break;
  case 4:
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0x50) == 0) {
      bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
      goto LAB_00265adc;
    }
    PlayUiSound(0x1a);
    lVar6 = PopByteQueue();
    if (lVar6 == 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 6;
    }
    else {
      iVar7 = (int)lVar6 + -1;
      iVar10 = iVar7 / 2;
      iVar5 = (int)((int)(short)(*(uint8_t*)rtguest::G(0x1780a40)) + (uint32_t)((*(uint8_t*)rtguest::G(0x1780a40)) >> 0xf)) >> 1;
      if (iVar5 < iVar10) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) = 0x800;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 5;
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = 0;
      }
      else if (iVar10 < iVar5) {
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = 0x8000;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) = 0xfffff800;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 5;
      }
      else {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 2;
      }
      (*(uint8_t*)rtguest::G(0x1780a40)) = (uint32_t)iVar7;
    }
    break;
  case 5:
    uVar4 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20)));
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = uVar4;
    if (0x8000 < uVar4) {
      uVar2 = (uint32_t)(*(uint8_t*)rtguest::G(0x1780a40));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 2;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x26))) = uVar2;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = 0;
    }
    break;
  case 6:
    sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2)));
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = sVar1 + 1;
    if (sVar1 < 9) break;
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x1c))) == 0) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 8;
      break;
    }
    bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
    goto LAB_00265acc;
  case 7:
  case 8:
    bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
    goto LAB_00265a8c;
  default:
    bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
LAB_00265a8c:
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) = bVar3 & 0xfd;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x400000;
    uiFade(0x6000000);
    sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2)));
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = sVar1 + 1;
    if (0x7f < sVar1) {
      bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
LAB_00265acc:
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = 0;
      bVar3 = bVar3 | 1;
LAB_00265ad4:
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) = bVar3;
    }
  }
  bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
LAB_00265adc:
  if ((bVar3 & 1) != 0) {
    iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4))) = 0xffffffff;
    gs_tasks_kill(param_1);
  }
  return 0;
}


RT_GUEST_FN(0x00265568, guest::HouseVisitedMenuTask)

}  // namespace guest
