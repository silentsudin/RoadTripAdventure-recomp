// guest:0x00248bf0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateCarouselSubMenu(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  bool bVar1;
  byte bVar2;
  char cVar3;
  undefined8 uVar4;
  long lVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  
  iVar7 = (int)param_1;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) == '\0') {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) < 0x10) {
      RT_CALLRA(0x248c28, guest::uiSetTexa((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) << 3));
      bVar1 = false;
    }
    else {
      bVar1 = false;
    }
  }
  else {
    bVar1 = false;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) == '\x02') {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) < 0x10) {
        RT_CALLRA(0x248c58, guest::uiSetTexa((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) * -8 + 0x80));
        bVar1 = false;
      }
      else {
        RT_CALLRA(0x248c68, guest::gs_tasks_kill());
        bVar1 = true;
      }
    }
  }
  if (bVar1) {
    return 0;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) == 0) {
    uVar4 = RT_CALLRA(0x248c88, guest::uiMenuCreate(param_1));
    RT_CALLRA(0x248ca0, guest::uiMenuAddItem(uVar4,0x327750,0x10));
    RT_CALLRA(0x248cb4, guest::uiMenuAddItem(uVar4,0x327778,0x10));
    RT_CALLRA(0x248cc8, guest::uiMenuAddItem(uVar4,0x3277a0,0x10));
    RT_CALLRA(0x248cd4, guest::CenterRectangle(uVar4,6));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar4 + 0xc))) = 2;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar4 + 0xe))) = 1;
    cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24)));
  }
  else {
    cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24)));
  }
  iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28)));
  if (cVar3 != '\0') {
    if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) == '\x03') && ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) == 0)) {
      RT_CALLRA(0x248d14, guest::PlayUiSound(0x1a));
      iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 4))) = 2;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) = 0;
    }
    else {
      RT_CALLRA(0x248d40, guest::PlayUiSound(0x1b));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xe))) & 0x7f;
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) == '\x01') {
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xd))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xd))) + '\x01';
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 0;
      }
      else {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 0;
      }
    }
  }
  lVar5 = (int32_t)RT_CALLRA(0x248d78, guest::uiMenuRun(iVar6,param_2));
  iVar8 = (int)param_2;
  if (lVar5 == 1) {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xd))) != '\x02') {
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xd))) * 2 + iVar8 + 0x30))) & 0x40) == 0) {
        RT_CALLRA(0x248da0, guest::PlayUiSound(0x1c));
        cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25)));
        goto LAB_00248e3c;
      }
      RT_CALLRA(0x248dec, guest::PlayUiSound(0x1a));
      RT_CALLRA(0x248db8, guest::gs_tasks_spawn(param_1,0x229758,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xd)))));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 1;
      bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xe))) | 0x80;
      goto LAB_00248e34;
    }
    RT_CALLRA(0x248e18, guest::PlayUiSound(0x1a));
    RT_CALLRA(0x248e00, guest::gs_tasks_spawn(param_1,0x239150,0x327740));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xe))) | 0x80;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 3;
  }
  else {
    if (lVar5 != -1) {
      cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25)));
      goto LAB_00248e3c;
    }
    bVar2 = 2;
LAB_00248e34:
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xe))) = bVar2;
  }
  cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25)));
LAB_00248e3c:
  if (cVar3 == '\0') {
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x30))) & 0x10) == 0) {
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x32))) & 0x10) == 0) {
        return 0;
      }
      if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 10))) & 2) == 0) {
        return 0;
      }
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
    }
    else {
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 0xffffffff;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) = 0;
    RT_CALLRA(0x248e94, guest::PlayUiSound(0x1b));
  }
  return 0;
}


RT_GUEST_FN(0x00248bf0, guest::UpdateCarouselSubMenu)

}  // namespace guest
