// guest:0x00248580 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateCarouselPlayerSelect(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  int iVar1;
  bool bVar2;
  int iVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  long lVar6;
  uint uVar7;
  byte bVar8;
  int iVar9;
uint32_t puVar10;
  int iVar11;
  
  iVar9 = (int)param_1;
  if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) == '\0') {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) < 0x10) {
      RT_CALLRA(0x2485b8, guest::uiSetTexa((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) << 3));
      bVar2 = false;
    }
    else {
      bVar2 = false;
    }
  }
  else {
    bVar2 = false;
    if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) == '\x02') {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) < 0x10) {
        RT_CALLRA(0x2485e8, guest::uiSetTexa((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) * -8 + 0x80));
        bVar2 = false;
      }
      else {
        RT_CALLRA(0x2485f8, guest::gs_tasks_kill());
        bVar2 = true;
      }
    }
  }
  if (bVar2) {
    return 0;
  }
  iVar11 = (int)param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) == 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x21))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x23))) = 1;
    uVar4 = RT_CALLRA(0x248628, guest::uiMenuCreate(param_1));
    uVar5 = RT_CALLRA(0x248638, guest::GetSlotDataPtr(0,0));
    RT_CALLRA(0x248648, guest::uiMenuAddItem(uVar4,uVar5,0x10));
    if ((*(int8_t*)rtguest::G(0x177fdd4)) == '\0') {
      RT_CALLRA(0x248670, guest::uiMenuAddItem(uVar4,0x334458,0x10));
    }
    else {
      uVar5 = RT_CALLRA(0x248660, guest::GetSlotDataPtr(1,0));
      RT_CALLRA(0x24868c, guest::uiMenuAddItem(uVar4,uVar5,0x10));
    }
    if ((*(int8_t*)rtguest::G(0x177fdd6)) == '\0') {
      RT_CALLRA(0x2486b4, guest::uiMenuAddItem(uVar4,0x334458,0x10));
    }
    else {
      uVar5 = RT_CALLRA(0x2486a4, guest::GetSlotDataPtr(2,0));
      RT_CALLRA(0x2486d4, guest::uiMenuAddItem(uVar4,uVar5,0x10));
    }
    uVar5 = RT_CALLRA(0x2486e0, guest::GetSlotDataPtr(0,1));
    RT_CALLRA(0x2486f0, guest::uiMenuAddItem(uVar4,uVar5,0x10));
    if ((*(uint32_t*)rtguest::G(0x178321c)) == '\0') {
      RT_CALLRA(0x248718, guest::uiMenuAddItem(uVar4,0x334458,0x10));
    }
    else {
      uVar5 = RT_CALLRA(0x248708, guest::GetSlotDataPtr(1,1));
      RT_CALLRA(0x248734, guest::uiMenuAddItem(uVar4,uVar5,0x10));
    }
    if ((*(uint32_t*)rtguest::G(0x178321e)) == '\0') {
      RT_CALLRA(0x24875c, guest::uiMenuAddItem(uVar4,0x334458,0x10));
    }
    else {
      uVar5 = RT_CALLRA(0x24874c, guest::GetSlotDataPtr(2,1));
      RT_CALLRA(0x24877c, guest::uiMenuAddItem(uVar4,uVar5,0x10));
    }
    puVar10 = (uint32_t)uVar4;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(puVar10 + 6))) = 2;
    (*(uint16_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(1) * sizeof(uint16_t))) = 0x84;
    (*(uint16_t*)rtguest::G((uint32_t)puVar10)) = 0x40;
    goto LAB_002489ac;
  }
  bVar8 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
  if ((bVar8 == 0) && (0x10 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))))) {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28)));
    lVar6 = RT_CALLRA(0x2487bc, guest::uiMenuRun(iVar1,param_2));
    bVar8 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xd)));
    if (lVar6 == 1) {
      if ((char)bVar8 < '\x03') {
        if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x30))) & 0x40) != 0) {
          if (bVar8 == 0) {
            iVar3 = 0;
          }
          else if ((bVar8 == 1) && ((*(int8_t*)rtguest::G(0x177fdd4)) != '\0')) {
            iVar3 = 0x1000000;
          }
          else {
            iVar3 = (uint32_t)bVar8 << 0x18;
            if (bVar8 != 2) goto LAB_002488c8;
            iVar3 = 0x2000000;
            if ((*(int8_t*)rtguest::G(0x177fdd6)) == '\0') goto LAB_002488c4;
          }
          uVar7 = iVar3 >> 0x18;
LAB_002488b0:
          RT_CALLRA(0x2488b0, guest::gs_tasks_spawn(param_1,0x22bb88,uVar7));
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 1;
          bVar8 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xd)));
          goto LAB_002488c4;
        }
        iVar3 = (uint32_t)bVar8 << 0x18;
      }
      else {
        if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x32))) & 0x40) == 0) goto LAB_002488c4;
        if (((bVar8 == 3) || ((bVar8 == 4 && ((*(uint32_t*)rtguest::G(0x178321c)) != '\0')))) ||
           ((iVar3 = (uint32_t)bVar8 << 0x18, bVar8 == 5 && (iVar3 = 0x5000000, (*(uint32_t*)rtguest::G(0x178321e)) != '\0'))))
        {
          uVar7 = (int)(char)bVar8 - 3U | 0x10;
          goto LAB_002488b0;
        }
      }
    }
    else {
LAB_002488c4:
      iVar3 = (uint32_t)bVar8 << 0x18;
    }
LAB_002488c8:
    uVar7 = iVar3 >> 0x18;
    if (uVar7 == (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)))) {
      bVar8 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
    }
    else {
      if (uVar7 == 0) {
LAB_00248954:
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) = bVar8;
      }
      else if ((uVar7 == 1) && ((*(int8_t*)rtguest::G(0x177fdd4)) != '\0')) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) = bVar8;
      }
      else if ((uVar7 == 2) && ((*(int8_t*)rtguest::G(0x177fdd6)) != '\0')) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) = bVar8;
      }
      else {
        if (uVar7 == 3) goto LAB_00248954;
        if ((uVar7 != 4) || ((*(uint32_t*)rtguest::G(0x178321c)) == '\0')) {
          if (uVar7 == 5) {
            if ((*(uint32_t*)rtguest::G(0x178321e)) != '\0') goto LAB_00248954;
            bVar8 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
          }
          else {
            bVar8 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
          }
          goto LAB_002489b0;
        }
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) = bVar8;
      }
LAB_002489ac:
      bVar8 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
    }
  }
  else {
    bVar2 = bVar8 < 2;
    if (bVar8 != 1) goto LAB_002489b4;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) != '\0') {
      uVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4)));
      if (uVar7 != 0xffffffff) {
        if ((uVar7 & 0x8000) == 0) {
          bVar2 = true;
          goto LAB_002489b4;
        }
        RT_CALLRA(0x2489a4, guest::copyCarModel(uVar7 & 0xff,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))) + 0xd)))));
        goto LAB_002489ac;
      }
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 0;
      bVar8 = 0;
    }
  }
LAB_002489b0:
  bVar2 = bVar8 < 2;
LAB_002489b4:
  if ((bVar2) && (0x10 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))))) {
    RT_CALLRA(0x2489d4, guest::RenderShowcaseView(param_1,param_2,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)))));
    bVar8 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
  }
  if ((bVar8 == 0) &&
     ((((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x30))) & 0x10) != 0 ||
      ((((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x32))) & 0x10) != 0 && (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 10))) & 2) != 0)))))) {
    RT_CALLRA(0x248a24, guest::gs_tasks_spawn(param_1,0x20dc50,0x300006));
    iVar11 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 4))) = 0xffffffff;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x24))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) = 0;
    RT_CALLRA(0x248a4c, guest::PlayUiSound(0x1b));
  }
  return 0;
}


RT_GUEST_FN(0x00248580, guest::UpdateCarouselPlayerSelect)

}  // namespace guest
