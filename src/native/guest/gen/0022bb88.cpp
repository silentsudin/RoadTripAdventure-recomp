// guest:0x0022bb88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunSaveProgressTask(uint32_t param_1, uint32_t param_2, ...) {
  char cVar1;
  byte bVar2;
  char cVar3;
  undefined4 uVar4;
  undefined8 uVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  undefined8 uVar9;
  uint uVar10;
  uint uVar11;
  int iVar12;
uint32_t puVar13;
  int iVar14;
  uint uVar15;
  
  iVar12 = (int)param_1;
  uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x1c))) & 3;
  uVar15 = (int)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x1c))) >> 4 & 1;
  iVar8 = uVar15 * 0x3448 + uVar6 * 0x1c;
  puVar13 = (uint32_t)((int)0x177f760 + iVar8);
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 8))) == 0) {
    uVar5 = uiMenuCreate();
    uVar9 = 0xf;
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x1c))) & 3) != 0) {
      uVar9 = 0xc;
    }
    LayoutTextItems(uVar5,0x29f370,uVar9,6);
    CenterRectangle(uVar5,5);
    iVar14 = (int)uVar5;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0xe))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x5f))) = 1;
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0xc))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0xc))) | (uint32_t)uVar15;
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x24)));
  }
  else {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x24)));
  }
  iVar14 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x28)));
  if (cVar1 != '\0') {
    uVar11 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 4)));
    if ((int)uVar11 < 0) {
      iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 4))) = 0xffffffff;
      gs_tasks_kill(param_1);
      return 0;
    }
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x27))) == 0xe) {
      uVar10 = 0;
    }
    else {
      uVar10 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x27))) + 1;
    }
    if (uVar10 == 0xd) {
      gs_tasks_spawn(param_1,0x2083c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002f00ac + uVar11 * 0x10))));
      SetObjectAttribute(puVar13,0xd,uVar11);
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 2))) = 0;
    }
    else {
      if (uVar10 < 0xe) {
        if (uVar10 == 0) {
          bVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x177f760 + iVar8 + 4)));
          uVar4 = (*(uint32_t*)rtguest::G((uint32_t)puVar13));
          iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x18)));
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = uVar11 + 0x800f;
          (*(uint32_t*)rtguest::G((uint32_t)0x17804d0 + (uint32_t)(uVar15 * 0xd12 + (uint32_t)bVar2) * 4)) = uVar4;
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
          bVar2 = (*(uint8_t*)rtguest::G((uint32_t)0x335720 + (uint32_t)(uVar11) * 1));
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x177f760 + iVar8 + 4))) = (*(uint8_t*)rtguest::G((uint32_t)0x335720 + (uint32_t)(uVar11) * 1));
          (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 2))) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)puVar13)) = (*(uint32_t*)rtguest::G((uint32_t)0x17804d0 + (uint32_t)(uVar15 * 0xd12 + (uint32_t)bVar2) * 4));
          goto LAB_0022be88;
        }
      }
      else if (uVar10 == 0xe) {
        gs_tasks_spawn(param_1,0x20dc50,uVar11 | 0x3a0000);
      }
      SetObjectAttribute(puVar13,uVar10,uVar11);
      if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x2ed336u + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177f770 + iVar8))) * 7))) == 0) {
        trap(7);
      }
      ::printf((char *)rtguest::G((uint32_t)(0x2ebc90u)), (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ecedc + (uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x177f76e + (uint32_t)(iVar8) * 4)) * 0x18))) << 7) /
             (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x2ed336u + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177f770 + iVar8))) * 7)))) /
             (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed204 + (uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x177f76f + (uint32_t)(iVar8) * 4)) * 0x10))));
      iVar7 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x2eca7cu + (uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x177f76d + (uint32_t)(iVar8) * 4)) * 7))) * 0x59;
      if (iVar7 < 0) {
        iVar7 = iVar7 + 0xff;
      }
      ::printf((char *)rtguest::G((uint32_t)(0x2ebca0u)), iVar7 >> 8);
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 2))) = 0;
    }
  }
LAB_0022be88:
  uiMenuWindow(iVar14,param_2,0x29f370);
  if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 2))) == 0) {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0xe))) == '\0') {
      cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0xd)));
      goto LAB_0022bed4;
    }
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0xd)));
  }
  else {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0xd)));
  }
  cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0xd)));
  if ((long)cVar1 == (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x27)))) {
    return 0;
  }
LAB_0022bed4:
  uVar11 = 0;
  if (cVar3 != 0xe) {
    uVar11 = (int)cVar3 + 1;
  }
  (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x27))) = cVar3;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 2))) = 1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x20))) != 0) {
    gs_tasks_kill((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x20))));
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x20))) = 0;
  if (((uVar11 != 7) || (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f768 + uVar6 * 7 + uVar15 * 0xd12))) & 0x400) == 0)) &&
     ((((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 10))) & 2) == 0 || (uVar11 != 0xe)))) {
    if (uVar11 == 0) {
      uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x177f760 + iVar8 + 4))) << 8 | uVar15 << 0x14;
    }
    else {
      uVar6 = uVar11 | (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar13 + uVar11 + 0xc))) << 8 |
              (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x1c))) << 0x10;
    }
    uVar4 = gs_tasks_spawn(param_1,0x246250,uVar6);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x20))) = uVar4;
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0xe))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0xe))) | 0x80;
  }
  return 0;
}


RT_GUEST_FN(0x0022bb88, guest::RunSaveProgressTask)

}  // namespace guest
