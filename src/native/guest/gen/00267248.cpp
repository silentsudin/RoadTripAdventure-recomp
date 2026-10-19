// guest:0x00267248 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceSummaryScene(uint32_t param_1, uint32_t param_2, ...) {
    uint32_t uStack_50 = rtguest::allocFrame(8);
    uint32_t uStack_48 = rtguest::allocFrame(8);
    uint32_t uStack_40 = rtguest::allocFrame(8);
    uint32_t uStack_38 = rtguest::allocFrame(8);
  byte bVar1;
  ushort uVar2;
  undefined8 uVar3;
  byte bVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  undefined8 uVar9;
  
  iVar8 = (int)param_1;
  switch((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25)))) {
  case 0:
  case 1:
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 2;
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27))) | 2;
  case 2:
    bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
    bVar1 = NavigateGridSelection(param_2,bVar4,0xe,1,0,0);
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) = bVar1;
    if (bVar4 < bVar1) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))) = 0x800;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 3;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) = 0;
      goto switchD_0026728c_caseD_4;
    }
    if (bVar1 < bVar4) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) = 0x8000;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))) = 0xfffff800;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 3;
      goto switchD_0026728c_caseD_4;
    }
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x30))) & 0x10) != 0) {
      PlayUiSound(0x1b);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 4;
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27))) & 0xfd | 1;
      goto switchD_0026728c_caseD_4;
    }
    bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
    break;
  case 3:
    uVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20)));
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) = uVar2;
    if (0x8000 < uVar2) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))) = 0;
    }
  default:
switchD_0026728c_caseD_4:
    bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
  }
  if ((bVar4 & 2) == 0) goto LAB_0026789c;
  uiArenaInit(0x1762400,0x10000);
  uiArenaBeginBatch(0x1762400,0,0);
  uiArenaSetTexture(0x1762400,0x2dbb50,0,0);
  uiArenaDataTag(0x1762400,1);
  uiArenaSpritesWrap(0x1762400,0x2dbda8,1,0,0,0xfffff0);
  drawBoardSpriteGrid(0x1762400,3);
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))) == 0) {
    bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) & 0x20) != 0) {
      if (bVar4 != 0) {
        (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) = 0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_48)) = 0x9800000052;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_40)) = 0x2800000016;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_38)) = 0;
        uiArenaDataTag(0x1762400,1);
        uiArenaSprites(0x1762400,uStack_50,1,0x1e,0xcc,1);
      }
      if (bVar4 < 0xd) {
        (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) = 0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_48)) = 0x9800000068;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_40)) = 0x2800000016;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_38)) = 0;
        uiArenaDataTag(0x1762400,1);
        uiArenaSprites(0x1762400,uStack_50,1,0x24c,0xcc,1);
      }
    }
  }
  else {
    drawRotatedQuadPair(0x1762400,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))));
  }
  uiArenaSubmit(0x1762400);
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))) < 1) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))) < 0) {
      uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
      if ((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) < 0x4001) {
        uVar5 = uVar6 * 2;
        if (uVar5 < 0x1a) {
          DrawRaceSummaryRow(uVar5,0,param_2);
          bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
          goto LAB_0026789c;
        }
        uVar9 = 0x32;
        if (uVar6 == 0xd) {
          iVar7 = raceSummaryDrawFigure8Row(0x32,0x3a);
          iVar7 = raceSummaryDrawSkiJumpRow(0x32,iVar7 + 0x18);
          raceSummaryDrawCurlingRow(0x32,iVar7 + 0x18);
          bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
          goto LAB_0026789c;
        }
        if (uVar5 != 0x1b) {
          bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
          goto LAB_0026789c;
        }
      }
      else {
        uVar5 = uVar6 * 2 + 3;
        if (uVar5 < 0x1a) {
          DrawRaceSummaryRow(uVar5,uVar5 - (uVar5 & 0x3fe),param_2);
          bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
          goto LAB_0026789c;
        }
        uVar9 = 0x15e;
        if ((uVar5 & 1) == 0) {
          uVar9 = 0x32;
        }
        if (uVar6 * 2 == 0x17) {
          iVar7 = raceSummaryDrawFigure8Row(uVar9,0x3a);
          iVar7 = raceSummaryDrawSkiJumpRow(uVar9,iVar7 + 0x18);
          raceSummaryDrawCurlingRow(uVar9,iVar7 + 0x18);
          bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
          goto LAB_0026789c;
        }
        if (uVar6 != 0xc) {
          bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
          goto LAB_0026789c;
        }
      }
    }
    else {
      uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
      uVar5 = uVar6 * 2;
      if (uVar5 < 0x1a) {
        DrawRaceSummaryRow(uVar5,0,param_2);
        uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
      }
      else if (uVar6 == 0xd) {
        iVar7 = raceSummaryDrawFigure8Row(0x32,0x3a);
        iVar7 = raceSummaryDrawSkiJumpRow(0x32,iVar7 + 0x18);
        raceSummaryDrawCurlingRow(0x32,iVar7 + 0x18);
        uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
      }
      else if (uVar5 == 0x1b) {
        uVar9 = raceSummaryDrawGolfRow(0x32,0x3a);
        raceSummaryDrawRallyRow(0x32,uVar9);
        uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
      }
      uVar5 = uVar6 * 2 + 1;
      if (uVar5 < 0x1a) {
        DrawRaceSummaryRow(uVar5,uVar5 - (uVar5 & 0x3fe),param_2);
        bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
        goto LAB_0026789c;
      }
      uVar9 = 0x15e;
      if ((uVar5 & 1) == 0) {
        uVar9 = 0x32;
      }
      if (uVar6 * 2 == 0x19) {
        iVar7 = raceSummaryDrawFigure8Row(uVar9,0x3a);
        iVar7 = raceSummaryDrawSkiJumpRow(uVar9,iVar7 + 0x18);
        raceSummaryDrawCurlingRow(uVar9,iVar7 + 0x18);
        bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
        goto LAB_0026789c;
      }
      if (uVar6 != 0xd) {
        bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
        goto LAB_0026789c;
      }
    }
  }
  else {
    uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
    if ((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) < 0x4000) {
      iVar7 = uVar6 * 2 + -2;
      if (iVar7 < 0x1a) {
        DrawRaceSummaryRow(iVar7,0,param_2);
        bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
        goto LAB_0026789c;
      }
      uVar9 = 0x32;
      if (uVar6 == 0xe) {
        iVar7 = raceSummaryDrawFigure8Row(0x32,0x3a);
        iVar7 = raceSummaryDrawSkiJumpRow(0x32,iVar7 + 0x18);
        raceSummaryDrawCurlingRow(0x32,iVar7 + 0x18);
        bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
        goto LAB_0026789c;
      }
      if (uVar6 * 2 != 0x1d) {
        bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
        goto LAB_0026789c;
      }
    }
    else {
      uVar5 = uVar6 * 2 + 1;
      if (uVar5 < 0x1a) {
        DrawRaceSummaryRow(uVar5,uVar5 - (uVar5 & 0x3fe),param_2);
        bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
        goto LAB_0026789c;
      }
      uVar9 = 0x15e;
      if ((uVar5 & 1) == 0) {
        uVar9 = 0x32;
      }
      if (uVar6 * 2 == 0x19) {
        iVar7 = raceSummaryDrawFigure8Row(uVar9,0x3a);
        iVar7 = raceSummaryDrawSkiJumpRow(uVar9,iVar7 + 0x18);
        raceSummaryDrawCurlingRow(uVar9,iVar7 + 0x18);
        bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
        goto LAB_0026789c;
      }
      if (uVar6 != 0xd) {
        bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
        goto LAB_0026789c;
      }
    }
  }
  uVar3 = raceSummaryDrawGolfRow(uVar9,0x3a);
  raceSummaryDrawRallyRow(uVar9,uVar3);
  bVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27)));
LAB_0026789c:
  if ((bVar4 & 1) != 0) {
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 4))) = 0xffffffff;
    gs_tasks_kill(param_1);
  }
  return 0;
}


RT_GUEST_FN(0x00267248, guest::raceSummaryScene)

}  // namespace guest
