// guest:0x002665b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t notebookStampGridTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
    uint32_t uStack_50 = rtguest::allocFrame(8);
    uint32_t uStack_48 = rtguest::allocFrame(8);
    uint32_t uStack_40 = rtguest::allocFrame(8);
    uint32_t uStack_38 = rtguest::allocFrame(8);
  uint uVar1;
  short sVar2;
  byte bVar3;
  ushort uVar4;
  int iVar5;
  long lVar6;
  uint uVar7;
  int iVar8;
  
  sVar2 = (*(uint32_t*)rtguest::G(0x1780a42));
  iVar8 = (int)param_1;
  iVar5 = (int)param_2;
  switch((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25)))) {
  case 0:
  case 1:
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xf))) != '\0') {
      bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
      goto LAB_0026680c;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 2;
    bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27))) | 2;
    goto LAB_00266804;
  case 2:
    (*(uint32_t*)rtguest::G(0x1780a42)) = navStampDigitByDpad(param_2,(*(uint32_t*)rtguest::G(0x1780a42)));
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) = (char)(*(uint32_t*)rtguest::G(0x1780a42));
    uVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) / 10;
    if ((int)sVar2 / 10 < (int)uVar1) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))) = 0x800;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 5;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) = 0;
    }
    else if ((int)uVar1 < (int)sVar2 / 10) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) = 0x8000;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))) = 0xfffff800;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 5;
    }
    else if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x30))) & 0x40) == 0) {
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x30))) & 0x10) != 0) {
        PlayUiSound(0x1b);
        gs_tasks_spawn(param_1,0x20dc50,0x40005);
        gs_tasks_spawn(param_1,0x20dc50,(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x0177f778 + 2)) | 0xa0000);
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 6;
      }
    }
    else {
      lVar6 = TestFlagFc68((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) + 1);
      if (lVar6 == 0) {
        PlayUiSound(0x1c);
        bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
        goto LAB_0026680c;
      }
      PlayUiSound(0x1a);
      gs_tasks_spawn(param_1,0x22bfd0,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 4;
    }
    break;
  case 3:
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xf))) != '\0') {
      bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
      goto LAB_0026680c;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 4;
    break;
  case 4:
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) == '\0') {
      bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
      goto LAB_0026680c;
    }
    PlayUiSound(0x1a);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 2;
    break;
  case 5:
    uVar4 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20)));
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) = uVar4;
    if (0x8000 < uVar4) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))) = 0;
    }
    break;
  default:
    bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27))) & 0xfd;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27))) = bVar3;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xf))) != '\0') {
      bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
      goto LAB_0026680c;
    }
    bVar3 = bVar3 | 1;
LAB_00266804:
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27))) = bVar3;
  }
  bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
LAB_0026680c:
  if ((bVar3 & 2) != 0) {
    uiArenaInit(0x1762400,0x10000);
    uiArenaBeginBatch(0x1762400,0,0);
    uiArenaSetTexture(0x1762400,0x2dbb50,0,0);
    uiArenaDataTag(0x1762400,1);
    uiArenaSpritesWrap(0x1762400,0x2dbda8,1,0,0,0xfffff0);
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) != '\x04') {
      drawBoardSpriteGrid(0x1762400,1);
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))) < 1) {
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))) < 0) {
          drawRotatedQuadPair(0x1762400,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))));
          drawStampNumberColumn(0x1762400,(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) / 10 << 1);
          drawStampNumberColumn(0x1762400,((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) / 10) * 2 + 3);
        }
        else {
          uVar7 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
          uVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) / 10;
          if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) & 0x20) != 0) {
            if (uVar1 != 0) {
              (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) = 0;
              (*(uint64_t*)rtguest::G((uint32_t)uStack_48)) = 0x9800000052;
              (*(uint64_t*)rtguest::G((uint32_t)uStack_40)) = 0x2800000016;
              (*(uint64_t*)rtguest::G((uint32_t)uStack_38)) = 0;
              uiArenaDataTag(0x1762400,1);
              uiArenaSprites(0x1762400,uStack_50,1,0x1e,0xcc,1);
            }
            if (uVar1 << 1 < 0x12) {
              (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) = 0;
              (*(uint64_t*)rtguest::G((uint32_t)uStack_48)) = 0x9800000068;
              (*(uint64_t*)rtguest::G((uint32_t)uStack_40)) = 0x2800000016;
              (*(uint64_t*)rtguest::G((uint32_t)uStack_38)) = 0;
              uiArenaDataTag(0x1762400,1);
              uiArenaSprites(0x1762400,uStack_50,1,0x24c,0xcc,1);
              uVar7 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
            }
            else {
              uVar7 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
            }
          }
          iVar5 = (uVar7 % 10) * 0xc;
          uiArenaFlatRects(0x1762400,0x2dc3c0,5,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dbd34 + iVar5))),
                           (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dbd38 + iVar5))),2);
          drawStampNumberColumn(0x1762400,(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) / 10 << 1);
          drawStampNumberColumn(0x1762400,((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) / 10) * 2 + 1);
        }
      }
      else {
        drawRotatedQuadPair(0x1762400,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))));
        drawStampNumberColumn(0x1762400,((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) / 10) * 2 + -2);
        drawStampNumberColumn(0x1762400,((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) / 10) * 2 + 1);
      }
    }
    uiArenaSubmit(0x1762400);
    bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
  }
  if ((bVar3 & 1) != 0) {
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 4))) = 0xffffffff;
    gs_tasks_kill(param_1);
  }
  return 0;
}


RT_GUEST_FN(0x002665b8, guest::notebookStampGridTask)

}  // namespace guest
