// guest:0x00239900 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ShowPlayerSelect(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  char cVar1;
  int iVar2;
  uint uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  long lVar6;
  int iVar7;
  
  iVar7 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) == 0) {
    uVar4 = RT_CALLRA(0x239924, guest::uiMenuCreate(param_1) /* $a0 = entry $a0 at the jal (MIPS: no $4 write/jal before 0x239924); pseudocode dropped the live arg */);
    uVar5 = RT_CALLRA(0x239934, guest::GetSlotDataPtr(0,0));
    RT_CALLRA(0x239944, guest::uiMenuAddItem(uVar4,uVar5,0x10));
    if ((*(int8_t*)rtguest::G(0x177fdd4)) != '\0') {
      uVar5 = RT_CALLRA(0x239960, guest::GetSlotDataPtr(1,0));
      RT_CALLRA(0x239970, guest::uiMenuAddItem(uVar4,uVar5,0x10));
      if ((*(int8_t*)rtguest::G(0x177fdd6)) != '\0') {
        uVar5 = RT_CALLRA(0x239984, guest::GetSlotDataPtr(2,0));
        RT_CALLRA(0x239994, guest::uiMenuAddItem(uVar4,uVar5,0x10));
      }
    }
    RT_CALLRA(0x2399a8, guest::uiMenuAddItem(uVar4,0x3338a8,0x10));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar4 + 0x5f))) = 2;
    RT_CALLRA(0x2399bc, guest::CenterRectangle(uVar4,5));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar4 + 0xe))) = 1;
    return 0;
  }
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28)));
  lVar6 = RT_CALLRA(0x2399d0, guest::uiMenuRun(iVar2));
  if (lVar6 == 0) {
    return 0;
  }
  if (lVar6 == -2) {
    gs_tasks_kill(param_1);
    return 0;
  }
  RT_CALLRA(0x2399ec, guest::PlayUiSound(0x1a));
  if (lVar6 == -1) {
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
  }
  else {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd)));
    if ((long)cVar1 != (long)(int)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xb))) - 1)) {
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x1c)));
      iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = uVar3 | (int)cVar1 << 8;
      goto LAB_00239a40;
    }
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = 0xffffffff;
LAB_00239a40:
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xe))) = 2;
  return 0;
}


RT_GUEST_FN(0x00239900, guest::ShowPlayerSelect)

}  // namespace guest
