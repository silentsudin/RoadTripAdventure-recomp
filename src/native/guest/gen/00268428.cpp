// guest:0x00268428 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t shopPartDialogSingle(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(400);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x140, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x148, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x168, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x170, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x120) = (uint32_t)(int32_t)param_1;
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x150, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x158, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x160, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x178, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x180, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x188) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x18c) = 0x00000000u;
    *(uint32_t*)rtguest::G(rt_sv+0x124) = (uint32_t)(int32_t)param_2;
  }
uint32_t acStack_190 = rtguest::allocFrame(80);
uint32_t acStack_140 = rtguest::allocFrame(65);
uint32_t acStack_ff = rtguest::allocFrame(65);
uint32_t auStack_be = rtguest::allocFrame(78);
    uint32_t iStack_70 = rtguest::allocFrame(4);
    uint32_t uStack_6c = rtguest::allocFrame(4);
    uint32_t uStack_68 = rtguest::allocFrame(4);
    uint32_t uStack_64 = rtguest::allocFrame(4);
    uint32_t uStack_60 = rtguest::allocFrame(4);
  char cVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  long lVar5;
  undefined8 uVar6;
  uint uVar7;
  uint uVar8;
uint32_t pcVar9;
uint32_t __src;
  int iVar10;
  int iVar11;
uint32_t psVar12;
  uint uVar13;
  uint uVar14;
  
  uVar13 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
  uVar14 = (int)uVar13 >> 8 & 0xff;
  uVar7 = (int)uVar13 >> 0x10 & 0xff;
  uVar8 = uVar13 >> 0x18;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = (uint32_t)(uVar14 < 4);
  uVar13 = uVar13 & 1;
  (*(int32_t*)rtguest::G((uint32_t)iStack_70)) = param_1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = param_2;
  if (uVar14 < 4) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = RT_CALLRA(0x268488, guest::DispatchByIndex15(uVar7,uVar8));
    iVar11 = 1;
    (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)(acStack_140)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(0x2dc7f8u + (uint32_t)(uVar14) * 4)))))));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_68)) = 0;
  }
  else {
    if (uVar14 < 6) {
      iVar11 = 2;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = RT_CALLRA(0x2684e0, guest::ComputeScaledPartValue(uVar7,uVar8,uVar13,1));
    }
    else {
      uVar13 = uVar13 ^ 1;
      iVar11 = 3;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = RT_CALLRA(0x268500, guest::ComputeScaledPartValue(uVar7,uVar8,uVar13,0));
      if (uVar14 < 7) {
        (*(uint8_t*)rtguest::G((uint32_t)auStack_be + (uint32_t)(0) * 1)) = (*(uint8_t*)rtguest::G(0x335360));
      }
      else {
        uVar6 = RT_CALLRA(0x268534, guest::DispatchByIndex16(uVar7,uVar8));
        ::sprintf((char *)rtguest::G((uint32_t)(auStack_be)), (char *)rtguest::G((uint32_t)(0x335368)), uVar6);
      }
    }
    (*(uint32_t*)rtguest::G((uint32_t)uStack_68)) = 0x96;
    ::sprintf((char *)rtguest::G((uint32_t)(acStack_140)), (char *)rtguest::G((uint32_t)(0x335368)), 0x0177fd8c + uVar13 * 0xd12);
    (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)(acStack_ff)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(0x2dc7f8u + (uint32_t)(uVar14) * 4)))))));
    if (uVar13 == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)uStack_68)) = 0xffffff6a;
    }
  }
  iVar3 = 0;
  if (iVar11 != 0) {
    iVar10 = iVar11;
    pcVar9 = acStack_140;
    do {
      iVar2 = RT_CALLRA(0x2685b8, guest::uiTextWidth(pcVar9));
      lVar5 = RT_CALLRA(0x2685c4, guest::uiTextWidth(0x335358));
      if (lVar5 == 0) {
        trap(7);
      }
      if (iVar3 < iVar2 / (int)lVar5) {
        iVar3 = RT_CALLRA(0x2685ec, guest::uiTextWidth(pcVar9));
        lVar5 = RT_CALLRA(0x2685f8, guest::uiTextWidth(0x335358));
        iVar3 = iVar3 / (int)lVar5;
        if (lVar5 == 0) {
          trap(7);
        }
      }
      iVar10 = iVar10 + -1;
      pcVar9 = pcVar9 + 0x41;
    } while (iVar10 != 0);
  }
  if (iVar11 != 0) {
    iVar2 = 0;
    iVar10 = iVar11;
    pcVar9 = acStack_140;
    do {
      __src = acStack_140 + iVar2;
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(acStack_190)), 0, 0x41));
      iVar4 = RT_CALLRA(0x268650, guest::uiTextWidth(pcVar9));
      lVar5 = RT_CALLRA(0x268660, guest::uiTextWidth(0x335358));
      if (lVar5 == 0) {
        trap(7);
      }
      iVar10 = iVar10 + -1;
      iVar2 = iVar2 + 0x41;
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(acStack_190)), 0x20, (long)((iVar3 - iVar4 / (int)lVar5) / 2)));
      (uint32_t)(uintptr_t)(::strcat((char *)rtguest::G((uint32_t)(acStack_190)), (char *)rtguest::G((uint32_t)(__src))));
      (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)(__src)), (char *)rtguest::G((uint32_t)(acStack_190))));
      pcVar9 = pcVar9 + 0x41;
    } while (iVar10 != 0);
  }
  uVar6 = RT_CALLRA(0x2686bc, guest::uiMenuCreate((*(int32_t*)rtguest::G((uint32_t)iStack_70))));
  if (iVar11 != 0) {
    pcVar9 = acStack_140;
    do {
      iVar11 = iVar11 + -1;
      RT_CALLRA(0x2686ec, guest::uiMenuAddItem(uVar6,pcVar9,0));
      pcVar9 = pcVar9 + 0x41;
    } while (iVar11 != 0);
  }
  RT_CALLRA(0x268708, guest::uiMenuAddItem(uVar6,0x335370,0x10));
  psVar12 = (uint32_t)uVar6;
  (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(psVar12 + 6))) = (char)uVar13;
  iVar11 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_70)) + 8)));
  (*(int16_t*)rtguest::G((uint32_t)psVar12 + (uint32_t)(1) * sizeof(int16_t))) = 0x70 - (short)((int)((int)(*(int16_t*)rtguest::G((uint32_t)psVar12 + (uint32_t)(3) * sizeof(int16_t))) + (uint32_t)((uint32_t)(*(int16_t*)rtguest::G((uint32_t)psVar12 + (uint32_t)(3) * sizeof(int16_t))) >> 0xf)) >> 1);
  (*(int16_t*)rtguest::G((uint32_t)psVar12)) = ((short)(*(uint32_t*)rtguest::G((uint32_t)uStack_68)) -
             (short)((int)((int)(*(int16_t*)rtguest::G((uint32_t)psVar12 + (uint32_t)(2) * sizeof(int16_t))) + (uint32_t)((uint32_t)(*(int16_t*)rtguest::G((uint32_t)psVar12 + (uint32_t)(2) * sizeof(int16_t))) >> 0xf)) >> 1)) + 0x140;
  if (iVar11 != 0) {
    psVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_70)) + 0x28)));
    lVar5 = (int32_t)RT_CALLRA(0x268774, guest::uiMenuRun(psVar12,(*(uint32_t*)rtguest::G((uint32_t)uStack_6c))));
    if ((((*(uint32_t*)rtguest::G((uint32_t)uStack_60)) == 0) && (5 < uVar14)) && (uVar14 < 7)) {
      iVar11 = RT_CALLRA(0x2687a0, guest::CountDecimalDigits((*(uint32_t*)rtguest::G((uint32_t)uStack_64))));
      iVar3 = RT_CALLRA(0x2687bc, guest::uiTextWidth(0x0177fd9f + uVar13 * 0xd12));
      iVar10 = RT_CALLRA(0x2687cc, guest::CountDecimalDigits((*(uint32_t*)rtguest::G((uint32_t)uStack_64))));
      iVar11 = ((int)(*(int16_t*)rtguest::G((uint32_t)psVar12 + (uint32_t)(2) * sizeof(int16_t))) - (iVar11 * 0x10 + iVar3 + 2)) / 2 + iVar10 * 0x10;
      RT_CALLRA(0x268808, guest::uiDrawNumber16Y2((*(int16_t*)rtguest::G((uint32_t)psVar12)) + iVar11,(*(int16_t*)rtguest::G((uint32_t)psVar12 + (uint32_t)(1) * sizeof(int16_t))) * 2 + 0x32,(*(uint32_t*)rtguest::G((uint32_t)uStack_64)),0));
      RT_CALLRA(0x268828, guest::uiDrawText((*(int16_t*)rtguest::G((uint32_t)psVar12)) + iVar11 + 2,(*(int16_t*)rtguest::G((uint32_t)psVar12 + (uint32_t)(1) * sizeof(int16_t))) + 0x18,0x0177fd9f + uVar13 * 0xd12,0));
    }
    if (lVar5 != 0) {
      RT_CALLRA(0x268838, guest::PlayUiSound(0x1a));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar12 + 0xd)));
      iVar11 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_70)) + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x24))) = 1;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 4))) = (int)cVar1;
      RT_CALLRA(0x268858, guest::gs_tasks_kill((*(int32_t*)rtguest::G((uint32_t)iStack_70))));
    }
  }
  return 0;
}


RT_GUEST_FN(0x00268428, guest::shopPartDialogSingle)

}  // namespace guest
