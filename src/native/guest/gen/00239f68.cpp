// guest:0x00239f68 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ShowRankedMessage(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  char cVar1;
  byte bVar2;
uint32_t pcVar3;
  int iVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  long lVar7;
  int iVar8;
  
  iVar8 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) != 0) {
    lVar7 = (int32_t)RT_CALLRA(0x23a0e0, guest::uiMenuRun((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),param_2));
    if (lVar7 != 0) {
      PlayUiSound(0x1a);
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
      bVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c))) + 2)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = (uint32_t)bVar2;
      RT_CALLRA(0x23a110, guest::gs_tasks_kill(param_1));
      PlayBgmSlot6();
      return 0;
    }
    return 0;
  }
  pcVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c)));
  uVar5 = RT_CALLRA(0x239f88, guest::uiMenuCreate(param_1) /* $a0 = entry $a0 at the jal (MIPS: no $4 write/jal before 0x239f88); pseudocode dropped the live arg */);
  RT_CALLRA(0x239fa0, guest::uiMenuAddItem(uVar5,0x3338e0,0));
  if ((*(int8_t*)rtguest::G((uint32_t)pcVar3)) == '\0') {
    RT_CALLRA(0x239fc0, guest::uiMenuAddItem(uVar5,0x3338e8,0x80));
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar3));
  }
  else {
    RT_CALLRA(0x239fd8, guest::uiMenuAddItem(uVar5,0x3338f0,0x80));
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar3));
  }
  uVar6 = RT_CALLRA(0x239fe4, guest::DispatchByIndex16(cVar1,(*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(1) * sizeof(int8_t)))));
  RT_CALLRA(0x239ff4, guest::uiMenuAddItem(uVar5,uVar6,0x80));
  switch((*(int8_t*)rtguest::G((uint32_t)pcVar3))) {
  case '\x01':
    uVar6 = 0x3338f8;
    break;
  case '\x02':
    uVar6 = 0x2ec650;
    break;
  case '\x03':
    uVar6 = 0x2ec660;
    break;
  case '\x04':
    uVar6 = 0x333900;
    break;
  case '\x05':
    uVar6 = 0x2ec670;
    break;
  case '\x06':
  default:
    RT_CALLRA(0x23a084, guest::uiMenuAddItem(uVar5,0x3338f0,0x80));
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(1) * sizeof(int8_t)));
    goto LAB_0023a0b0;
  case '\a':
    uVar6 = 0x2ec680;
  }
  RT_CALLRA(0x23a0a4, guest::uiMenuAddItem(uVar5,uVar6,0x80));
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(1) * sizeof(int8_t)));
LAB_0023a0b0:
  RT_CALLRA(0x23a0b4, guest::SetFlaggedSlotBit((*(int8_t*)rtguest::G((uint32_t)pcVar3)),cVar1,0));
  RT_CALLRA(0x23a0bc, guest::PlayBgmSlot3());
  PlayUiSound(0x302);
  return 0;
}


RT_GUEST_FN(0x00239f68, *static_cast<uint32_t (*)(uint32_t, uint32_t, ...)>(&guest::ShowRankedMessage))

}  // namespace guest
