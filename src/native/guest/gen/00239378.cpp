// guest:0x00239378 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ShowAwardMessage(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  byte bVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  long lVar4;
  int iVar5;
  int iVar6;
  
  iVar6 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    iVar5 = (int)param_2;
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))) & 0x40000) == 0) {
      RT_CALLRA(0x2393d4, guest::InitActorSlot(3,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) - 1,param_2));
    }
    else {
      RT_CALLRA(0x2393f0, guest::InitActorSlot(((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x21))) -
                    (int)(char)(*(uint8_t*)rtguest::G((uint32_t)0x2be43e + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x23))) * 8) * 1))) + 4,
                    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) - 1,param_2));
    }
    uVar2 = RT_CALLRA(0x2393fc, guest::uiMenuCreate(param_1));
    uVar3 = RT_CALLRA(0x23940c, guest::GetSlotDataPtr((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))),0));
    RT_CALLRA(0x23941c, guest::uiMenuAddItem(uVar2,uVar3,0));
    RT_CALLRA(0x239430, guest::uiMenuAddItem(uVar2,0x2ec580,0x80));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar2 + 0x5f))) = 2;
    RT_CALLRA(0x23943c, guest::PlayBgmSlot3());
    RT_CALLRA(0x239444, guest::PlayUiSound(0x303));
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26)));
    iVar5 = 1;
    while( true ) {
      RT_CALLRA(0x239478, guest::SetFlaggedSlotBit(iVar5,(*(uint8_t*)rtguest::G((uint32_t)0x177f76c + (uint32_t)(iVar5 + (uint32_t)bVar1 * 0x1c) * 1)),0));
      if (7 < iVar5 + 1) break;
      bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26)));
      iVar5 = iVar5 + 1;
    }
  }
  else {
    lVar4 = (int32_t)RT_CALLRA(0x23949c, guest::uiMenuRun((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))),param_2));
    if (lVar4 != 0) {
      RT_CALLRA(0x2394ac, guest::PlayUiSound(0x1a));
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x1c)))));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = (uint32_t)bVar1;
      RT_CALLRA(0x2394cc, guest::gs_tasks_kill(param_1));
      PlayBgmSlot6();
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x00239378, guest::ShowAwardMessage)

}  // namespace guest
