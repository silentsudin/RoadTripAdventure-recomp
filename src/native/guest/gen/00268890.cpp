// guest:0x00268890 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t shopPartDialogYesNo(uint32_t param_1, uint32_t param_2, ...) {
uint32_t acStack_200 = rtguest::allocFrame(80);
uint32_t acStack_1b0 = rtguest::allocFrame(65);
uint32_t acStack_16f = rtguest::allocFrame(65);
uint32_t acStack_12e = rtguest::allocFrame(65);
    uint32_t cStack_ed = rtguest::allocFrame(1);
    rtguest::allocFrame(64);  // guest frame gap
    uint32_t cStack_ac = rtguest::allocFrame(1);
    rtguest::allocFrame(75);  // guest frame gap
    uint32_t iStack_60 = rtguest::allocFrame(4);
    uint32_t uStack_5c = rtguest::allocFrame(4);
    uint32_t uStack_58 = rtguest::allocFrame(4);
    uint32_t uStack_54 = rtguest::allocFrame(4);
  char cVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined8 uVar6;
  long lVar7;
  uint uVar8;
uint32_t pcVar9;
uint32_t __src;
  uint uVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
uint32_t psVar14;
  uint uVar15;
  
  uVar15 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
  uVar13 = (int)uVar15 >> 8 & 0xff;
  uVar8 = (int)uVar15 >> 0x10 & 0xff;
  uVar10 = uVar15 >> 0x18;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_54)) = (uint32_t)(uVar13 < 2);
  uVar15 = uVar15 & 1;
  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = param_1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = param_2;
  if (uVar13 < 2) {
    iVar2 = DispatchByIndex15(uVar8,uVar10);
    if (uVar13 == 0) {
      iVar2 = iVar2 / 2;
    }
    uVar6 = DispatchByIndex16(uVar8,uVar10);
    ::sprintf((char *)rtguest::G((uint32_t)(acStack_1b0)), (char *)rtguest::G((uint32_t)(0x335368)), uVar6);
    (*(uint32_t*)rtguest::G((uint32_t)uStack_58)) = 0;
    iVar12 = 4;
    (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)(acStack_16f)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(0x2dc818u + (uint32_t)(uVar13 * 2) * 4)))))));
    (*(int8_t*)rtguest::G((uint32_t)acStack_12e + (uint32_t)(0) * 1)) = (*(int8_t*)rtguest::G(0x335360));
  }
  else {
    if (uVar13 < 3) {
      iVar2 = ComputeScaledPartValue(uVar8,uVar10,uVar15,1);
    }
    else {
      uVar15 = uVar15 ^ 1;
      iVar2 = ComputeScaledPartValue(uVar8,uVar10,uVar15,0);
    }
    (*(uint32_t*)rtguest::G((uint32_t)uStack_58)) = 0x96;
    iVar12 = 5;
    ::sprintf((char *)rtguest::G((uint32_t)(acStack_1b0)), (char *)rtguest::G((uint32_t)(0x335368)), 0x0177fd8c + uVar15 * 0xd12);
    uVar6 = DispatchByIndex16(uVar8,uVar10);
    ::sprintf((char *)rtguest::G((uint32_t)(acStack_16f)), (char *)rtguest::G((uint32_t)(0x335368)), uVar6);
    (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)(acStack_12e)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(0x2dc818u + (uint32_t)(uVar13 * 2) * 4)))))));
    (*(int8_t*)rtguest::G((uint32_t)cStack_ac)) = (*(int8_t*)rtguest::G(0x335360));
    if (uVar15 == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)uStack_58)) = 0xffffff6a;
    }
  }
  iVar4 = 0;
  (*(int8_t*)rtguest::G((uint32_t)cStack_ed)) = (*(int8_t*)rtguest::G(0x335360));
  if (iVar12 != 0) {
    iVar11 = iVar12;
    pcVar9 = acStack_1b0;
    do {
      iVar3 = uiTextWidth(pcVar9);
      lVar7 = uiTextWidth(0x335358);
      if (lVar7 == 0) {
        trap(7);
      }
      if (iVar4 < iVar3 / (int)lVar7) {
        iVar4 = uiTextWidth(pcVar9);
        lVar7 = uiTextWidth(0x335358);
        iVar4 = iVar4 / (int)lVar7;
        if (lVar7 == 0) {
          trap(7);
        }
      }
      iVar11 = iVar11 + -1;
      pcVar9 = pcVar9 + 0x41;
    } while (iVar11 != 0);
  }
  if (iVar12 != 0) {
    iVar3 = 0;
    iVar11 = iVar12;
    pcVar9 = acStack_1b0;
    do {
      __src = acStack_1b0 + iVar3;
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(acStack_200)), 0, 0x41));
      iVar5 = uiTextWidth(pcVar9);
      lVar7 = uiTextWidth(0x335358);
      if (lVar7 == 0) {
        trap(7);
      }
      iVar11 = iVar11 + -1;
      iVar3 = iVar3 + 0x41;
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(acStack_200)), 0x20, (long)((iVar4 - iVar5 / (int)lVar7) / 2)));
      (uint32_t)(uintptr_t)(::strcat((char *)rtguest::G((uint32_t)(acStack_200)), (char *)rtguest::G((uint32_t)(__src))));
      (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)(__src)), (char *)rtguest::G((uint32_t)(acStack_200))));
      pcVar9 = pcVar9 + 0x41;
    } while (iVar11 != 0);
  }
  uVar6 = uiMenuCreate((*(int32_t*)rtguest::G((uint32_t)iStack_60)));
  if (iVar12 != 0) {
    pcVar9 = acStack_1b0;
    do {
      iVar12 = iVar12 + -1;
      uiMenuAddItem(uVar6,pcVar9,0);
      pcVar9 = pcVar9 + 0x41;
    } while (iVar12 != 0);
  }
  uiMenuAddItem(uVar6,0x335370,0x10);
  uiMenuAddItem(uVar6,0x335378,0x10);
  psVar14 = (uint32_t)uVar6;
  (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(psVar14 + 6))) = (char)uVar15;
  iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 8)));
  (*(int16_t*)rtguest::G((uint32_t)psVar14 + (uint32_t)(1) * sizeof(int16_t))) = 0x70 - (short)((int)((int)(*(int16_t*)rtguest::G((uint32_t)psVar14 + (uint32_t)(3) * sizeof(int16_t))) + (uint32_t)((uint32_t)(*(int16_t*)rtguest::G((uint32_t)psVar14 + (uint32_t)(3) * sizeof(int16_t))) >> 0xf)) >> 1);
  (*(int16_t*)rtguest::G((uint32_t)psVar14)) = ((short)(*(uint32_t*)rtguest::G((uint32_t)uStack_58)) -
             (short)((int)((int)(*(int16_t*)rtguest::G((uint32_t)psVar14 + (uint32_t)(2) * sizeof(int16_t))) + (uint32_t)((uint32_t)(*(int16_t*)rtguest::G((uint32_t)psVar14 + (uint32_t)(2) * sizeof(int16_t))) >> 0xf)) >> 1)) + 0x140;
  if (iVar12 == 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar14 + 0xd))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x27))) = 1;
  }
  else {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar14 + 0xd))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x27)));
    psVar14 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x28)));
    lVar7 = uiMenuRun(psVar14,(*(uint32_t*)rtguest::G((uint32_t)uStack_5c)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x27))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar14 + 0xd)));
    if ((*(uint32_t*)rtguest::G((uint32_t)uStack_54)) == 0) {
      iVar12 = CountDecimalDigits(iVar2);
      iVar4 = uiTextWidth(0x0177fd9f + uVar15 * 0xd12);
      iVar11 = CountDecimalDigits(iVar2);
      iVar12 = ((int)(*(int16_t*)rtguest::G((uint32_t)psVar14 + (uint32_t)(2) * sizeof(int16_t))) - (iVar12 * 0x10 + iVar4 + 2)) / 2 + iVar11 * 0x10;
      uiDrawNumber16Y2((*(int16_t*)rtguest::G((uint32_t)psVar14)) + iVar12,(*(int16_t*)rtguest::G((uint32_t)psVar14 + (uint32_t)(1) * sizeof(int16_t))) * 2 + 0x4a,iVar2,0);
      uiDrawText((*(int16_t*)rtguest::G((uint32_t)psVar14)) + iVar12 + 2,(*(int16_t*)rtguest::G((uint32_t)psVar14 + (uint32_t)(1) * sizeof(int16_t))) + 0x24,0x0177fd9f + uVar15 * 0xd12,0);
    }
    else {
      iVar12 = CountDecimalDigits(iVar2);
      iVar4 = uiTextWidth(0x0177fd9f + uVar15 * 0xd12);
      iVar11 = CountDecimalDigits(iVar2);
      iVar12 = ((int)(*(int16_t*)rtguest::G((uint32_t)psVar14 + (uint32_t)(2) * sizeof(int16_t))) - (iVar12 * 0x10 + iVar4 + 2)) / 2 + iVar11 * 0x10;
      uiDrawNumber16Y2((*(int16_t*)rtguest::G((uint32_t)psVar14)) + iVar12,(*(int16_t*)rtguest::G((uint32_t)psVar14 + (uint32_t)(1) * sizeof(int16_t))) * 2 + 0x32,iVar2,0);
      uiDrawText((*(int16_t*)rtguest::G((uint32_t)psVar14)) + iVar12 + 2,(*(int16_t*)rtguest::G((uint32_t)psVar14 + (uint32_t)(1) * sizeof(int16_t))) + 0x18,0x0177fd9f + uVar15 * 0xd12,0);
    }
    if (lVar7 != 0) {
      if (lVar7 == -1) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar14 + 0xd))) = 1;
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar14 + 0xd)));
      }
      else {
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar14 + 0xd)));
      }
      if (cVar1 == '\0') {
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x18))) + 0x20))) =
             (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x18))) + 0x20))) + -1;
        PlayUiSound(0x1a);
      }
      else {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x18))) + 0x20))) = 0xffffffff;
        PlayUiSound(0x1b);
      }
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar14 + 0xd)));
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = (int)cVar1;
      gs_tasks_kill();
    }
  }
  return 0;
}


RT_GUEST_FN(0x00268890, guest::shopPartDialogYesNo)

}  // namespace guest
