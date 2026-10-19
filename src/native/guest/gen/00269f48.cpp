// guest:0x00269f48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t resultsScene(uint32_t param_1, uint32_t param_2, ...) {
    uint32_t iStack_58 = rtguest::allocFrame(4);
    uint32_t iStack_5c = rtguest::allocFrame(4);
    uint32_t iStack_60 = rtguest::allocFrame(4);
  undefined1 uVar1;
  ushort uVar2;
  char cVar3;
uint32_t puVar4;
uint32_t puVar5;
  int iVar6;
  undefined8 uVar7;
uint32_t pbVar8;
uint32_t pcVar9;
uint32_t piVar10;
  int iVar11;
uint32_t puVar12;
uint32_t pbVar13;
  int iVar14;
  int iVar15;
  uint uVar16;
  int iVar17;
uint32_t puVar18;
  int iVar19;
uint32_t puVar20;
uint32_t auStack_110 = rtguest::allocFrame(32);
uint32_t auStack_f0 = rtguest::allocFrame(20);
uint32_t aiStack_dc = rtguest::allocFrame(92);
uint32_t abStack_80 = rtguest::allocFrame(16);
uint32_t abStack_70 = rtguest::allocFrame(16);
  
  iVar17 = 0;
  pbVar13 = abStack_80;
  (*(int8_t*)rtguest::G((uint32_t)abStack_80 + (uint32_t)(0) * 1)) = (*(int8_t*)rtguest::G(0x3353e0));
  (*(int8_t*)rtguest::G((uint32_t)abStack_80 + (uint32_t)(1) * 1)) = (*(uint32_t*)rtguest::G(0x3353e1));
  (*(int8_t*)rtguest::G((uint32_t)abStack_80 + (uint32_t)(2) * 1)) = (*(uint32_t*)rtguest::G(0x3353e2));
  iVar19 = (int)param_1;
  (*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(0) * 1)) = (*(uint32_t*)rtguest::G(0x3353e8));
  (*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(1) * 1)) = (*(uint32_t*)rtguest::G(0x3353e9));
  (*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(2) * 1)) = (*(uint32_t*)rtguest::G(0x3353ea));
  (*(int32_t*)rtguest::G((uint32_t)iStack_5c)) = 0;
  (*(int32_t*)rtguest::G((uint32_t)iStack_58)) = 0;
  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 8))) == 0) {
    pbVar8 = 0x002dc930;
    pcVar9 = 0x01772400;
    iVar15 = 0;
    do {
      iVar14 = iVar15 + 1;
      (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)(pcVar9)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be398 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)pbVar8))) * 4))))));
      pbVar8 = (uint32_t)(iVar15 + 0x2dc931);
      pcVar9 = pcVar9 + 0x16;
      iVar15 = iVar14;
    } while (iVar14 < 6);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x26))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x27))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x25))) = 0;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x30))) = 0;
  }
  iVar15 = 0;
  if ((*(uint8_t*)rtguest::G(0x177fde0)) != '\0') {
    pcVar9 = 0x0177fde0;
    iVar17 = 1;
    while( true ) {
      iVar15 = iVar15 + 1;
      pcVar9 = pcVar9 + 0x30;
      if ((0x13 < iVar15) || ((*(int8_t*)rtguest::G((uint32_t)pcVar9)) == '\0')) break;
      iVar17 = iVar17 + 1;
    }
  }
  iVar15 = 5;
  puVar4 = 0x0177246e;
  piVar10 = aiStack_dc;
  do {
    iVar15 = iVar15 + -1;
    (*(int32_t*)rtguest::G((uint32_t)piVar10)) = (int)puVar4;
    piVar10 = piVar10 + -1;
    puVar4 = puVar4 + -0x16;
  } while (-1 < iVar15);
  if (iVar17 < 1) {
    uVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x25)));
  }
  else {
    piVar10 = aiStack_dc + 3;
    puVar5 = 0x0177fde0;
    iVar15 = iVar17;
    do {
      iVar15 = iVar15 + -1;
      (*(int32_t*)rtguest::G((uint32_t)piVar10)) = (int)puVar5;
      piVar10 = piVar10 + 1;
      puVar5 = puVar5 + 0x30;
    } while (iVar15 != 0);
    uVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x25)));
  }
  switch(uVar1) {
  case 0:
    uVar7 = uiMenuCreate(param_1);
    LayoutTextItems(uVar7,0x2dc918,2,2);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar7 + 0x5f))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar7 + 0xd))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x27)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x25))) = 1;
    break;
  case 1:
    break;
  case 2:
    uVar7 = uiMenuCreate(param_1);
    LayoutTextItems(uVar7,auStack_f0,6,6);
    puVar12 = (uint32_t)uVar7;
    (*(uint16_t*)rtguest::G((uint32_t)puVar12)) = 0x28;
    (*(uint16_t*)rtguest::G((uint32_t)puVar12 + (uint32_t)(1) * sizeof(uint16_t))) = 0x16;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x5f))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x25))) = 3;
  case 3:
    iVar17 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x28)));
    uVar16 = 6;
    uiMenuWindow(iVar17,(*(int32_t*)rtguest::G((uint32_t)iStack_60)),auStack_f0);
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0xd))) < '\x05') {
      uVar16 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0xd)));
    }
    iVar15 = uVar16 * 0x20;
    uiDrawPanel9(0x28,0x6e,0x230,0x58,0);
    uiDrawText(0x8c,0x6e,0x32f410,0);
    uiDrawText(0x3c,0x79,0x335410,0);
    uiDrawText(0x3c,0x84,0x335418,0);
    uiDrawText(0x3c,0x8f,0x335420,0);
    uiDrawText(0x3c,0x9a,0x335418,0);
    uiDrawText(0x3c,0xa5,0x335428,0);
    uiDrawText(0x3c,0xb0,0x335418,0);
    puVar20 = (uint32_t)(0x01780198 + iVar15);
    iVar17 = 2;
    puVar18 = puVar20;
    do {
      if (((*(uint32_t*)rtguest::G((uint32_t)puVar18)) & 0xffffff) != 0) {
        if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar18 + 3))) == -1) {
          uVar7 = GetSlotDataPtr(0,0);
          uiDrawText(0x8c,(uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar13)) * 0xc - (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar13)),uVar7,0);
        }
        else {
          uiDrawText(0x8c,(uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar13)) * 0xc - (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar13)),
                     0x0177fde0 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar18 + 3))) * 0x30,0);
        }
      }
      iVar17 = iVar17 + -1;
      pbVar13 = pbVar13 + 1;
      puVar18 = puVar18 + 1;
    } while (-1 < iVar17);
    if (((*(uint32_t*)rtguest::G((uint32_t)puVar20)) & 0xffffff) != 0) {
      uiDrawTime(0x8c,0x79,(int)((*(uint32_t*)rtguest::G((uint32_t)puVar20)) << 8) >> 8,0);
    }
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0178019c + iVar15))) & 0xffffff) != 0) {
      uiDrawTime(0x8c,0x8f,(int)((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0178019c + iVar15))) << 8) >> 8,0);
    }
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a0 + iVar15))) & 0xffffff) != 0) {
      uiDrawTime(0x8c,0xa5,(int)((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a0 + iVar15))) << 8) >> 8,0);
    }
    uiDrawText(0x1a4,0x6e,0x32f420,0);
    uiDrawText(0x154,0x79,0x335410,0);
    pbVar13 = abStack_70;
    uiDrawText(0x154,0x84,0x335418,0);
    iVar17 = 0;
    uiDrawText(0x154,0x8f,0x335420,0);
    uiDrawText(0x154,0x9a,0x335418,0);
    uiDrawText(0x154,0xa5,0x335428,0);
    uiDrawText(0x154,0xb0,0x335418,0);
    puVar18 = (uint32_t)(0x017801a4 + iVar15);
    do {
      if (((*(uint32_t*)rtguest::G((uint32_t)puVar18 + (uint32_t)(iVar17) * sizeof(uint32_t))) & 0xffffff) != 0) {
        if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)(puVar18 + iVar17) + 3))) == -1) {
          uVar7 = GetSlotDataPtr(0,0);
          uiDrawText(0x1a4,(uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar13)) * 0xc - (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar13)),uVar7,0);
        }
        else {
          uiDrawText(0x1a4,(uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar13)) * 0xc - (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar13)),
                     0x0177fde0 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar20 + iVar17 * 4 + 3))) * 0x30,0);
        }
      }
      iVar17 = iVar17 + 1;
      pbVar13 = pbVar13 + 1;
    } while (iVar17 < 3);
    if (((*(uint32_t*)rtguest::G((uint32_t)puVar18)) & 0xffffff) != 0) {
      uiDrawTime(0x1a4,0x79,(int)((*(uint32_t*)rtguest::G((uint32_t)puVar18)) << 8) >> 8,0);
    }
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a8 + iVar15))) & 0xffffff) != 0) {
      uiDrawTime(0x1a4,0x8f,(int)((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801a8 + iVar15))) << 8) >> 8,0);
    }
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801ac + iVar15))) & 0xffffff) != 0) {
      uiDrawTime(0x1a4,0xa5,(int)((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801ac + iVar15))) << 8) >> 8,0);
    }
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x30))) & 0x10) == 0) {
      cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x26)));
      goto LAB_0026a8dc;
    }
LAB_0026a8cc:
    PlayUiSound(0x1b);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x25))) = 0;
    goto LAB_0026a8d8;
  case 4:
    uVar7 = uiMenuCreate(param_1);
    iVar15 = 10;
    if (iVar17 < 0xb) {
      iVar15 = iVar17;
    }
    LayoutTextItems(uVar7,aiStack_dc + 3,iVar17,iVar15);
    puVar12 = (uint32_t)uVar7;
    (*(uint16_t*)rtguest::G((uint32_t)puVar12)) = 0x14;
    (*(uint16_t*)rtguest::G((uint32_t)puVar12 + (uint32_t)(1) * sizeof(uint16_t))) = 0x16;
    (*(uint16_t*)rtguest::G((uint32_t)puVar12 + (uint32_t)(2) * sizeof(uint16_t))) = 200;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x5f))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x25))) = 5;
    goto LAB_0026a2a4;
  case 5:
LAB_0026a2a4:
    iVar17 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x28)));
    iVar15 = 0;
    uiMenuWindow(iVar17,(*(int32_t*)rtguest::G((uint32_t)iStack_60)),aiStack_dc + 3);
    uiDrawPanel9(0xf0,0x16,0x17c,0xb0,0);
    uiDrawText(0x168,0x16,0x3353f0,0);
    uiDrawText(0x1b8,0x16,0x3353f8,0);
    iVar14 = 0;
    do {
      iVar11 = iVar14 / 2 + 0x21;
      iVar14 = iVar14 + 0x1d;
      uiDrawText(0x104,iVar11,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be398 + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x2dc920 + (uint32_t)(iVar15) * 4))) * 4)),0);
      iVar6 = iVar15 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0xd))) * 0x30;
      ::sprintf((char *)rtguest::G((uint32_t)(auStack_110)), (char *)rtguest::G((uint32_t)(0x335400)), (*(uint8_t*)rtguest::G((uint32_t)0x177fdf4 + (uint32_t)(iVar6) * 1)), (*(uint8_t*)rtguest::G((uint32_t)0x177fdfe + (uint32_t)(iVar6) * 1)));
      iVar6 = uiTextWidth(auStack_110);
      uiDrawText(600 - iVar6,iVar11,auStack_110,0);
      iVar6 = iVar15 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0xd))) * 0x30;
      iVar15 = iVar15 + 1;
      (*(int32_t*)rtguest::G((uint32_t)iStack_5c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_5c)) + (uint32_t)(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x177fdf4 + (uint32_t)(iVar6) * 1));
      (*(int32_t*)rtguest::G((uint32_t)iStack_58)) = (*(int32_t*)rtguest::G((uint32_t)iStack_58)) + (uint32_t)(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x177fdfe + (uint32_t)(iVar6) * 1));
    } while (iVar15 < 10);
    uiDrawText(0x104,0xb2,0x335408,0);
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_110)), (char *)rtguest::G((uint32_t)(0x335400)), (*(int32_t*)rtguest::G((uint32_t)iStack_5c)), (*(int32_t*)rtguest::G((uint32_t)iStack_58)));
    iVar17 = uiTextWidth(auStack_110);
    uiDrawText(600 - iVar17,(iVar15 * 0x1d) / 2 + 0x21,auStack_110,0);
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x30))) & 0x10) == 0) {
      cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x26)));
      goto LAB_0026a8dc;
    }
    goto LAB_0026a8cc;
  default:
    cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x26)));
    goto LAB_0026a8dc;
  }
  iVar15 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x28)));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x27))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0xd)));
  uiMenuWindow(iVar15,(*(int32_t*)rtguest::G((uint32_t)iStack_60)),0x2dc918);
  if (iVar17 == 0) {
    uiDrawText(0x120,0x65,(*(uint32_t*)rtguest::G(0x002dc918)),6);
  }
  uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x30)));
  if ((uVar2 & 0x40) != 0) {
    cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0xd)));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0xd))) == '\0') {
      if (iVar17 == 0) {
        PlayUiSound(0x1c);
      }
      else {
        PlayUiSound(0x1a);
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x25))) = 4;
      }
      cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0xd)));
    }
    if (cVar3 == '\x01') {
      PlayUiSound(0x1a);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x25))) = 2;
      uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x30)));
    }
    else {
      uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x30)));
    }
  }
  if ((uVar2 & 0x10) == 0) {
    cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x26)));
  }
  else {
    PlayUiSound(0x1b);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x26))) = 1;
LAB_0026a8d8:
    cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x26)));
  }
LAB_0026a8dc:
  if (cVar3 != '\0') {
    iVar17 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 4))) = 1;
    gs_tasks_kill(param_1);
  }
  return 0;
}


RT_GUEST_FN(0x00269f48, guest::resultsScene)

}  // namespace guest
