// guest:0x0023a140 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ShowPrizeScreen(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  byte bVar1;
uint32_t pbVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  
  iVar5 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) == 0) {
    pbVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c)));
    sVar3 = (short)((int)(*(uint32_t*)rtguest::G((uint32_t)0x177fdbc + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)pbVar2))) * 4)) / 0x649);
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) = sVar3;
    iVar4 = (int)sVar3;
    (*(uint32_t*)rtguest::G((uint32_t)0x177fdbc + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)pbVar2))) * 4)) = (*(uint32_t*)rtguest::G((uint32_t)0x177fdbc + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)pbVar2))) * 4)) + iVar4 * -0x649;
    if (iVar4 == 0) {
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) + 1)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = (uint32_t)bVar1;
      gs_tasks_kill(param_1);  // delay-slot move $4,$16 loads the arg (Ghidra elides)
      return 0;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20))) = iVar4 * ((uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar2)) * 10 + 10);
    RT_CALLRA(0x23a210, guest::PlayBgmSlot3());
    RT_CALLRA(0x23a220, guest::SpendTimerBudget(-(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20))),0));
  }
  else {
    RT_CALLRA(0x23a23c, guest::uiDrawPanelCells(8,6,0x16,10,1));
    RT_CALLRA(0x23a254, guest::uiDrawTextCell(9,7,0x333908,0));
    RT_CALLRA(0x23a26c, guest::uiDrawTextCell(0x10,7,0x177fd9f,0));
    RT_CALLRA(0x23a280, guest::uiDrawNumber16(0x14a,0x59,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20))),0));
    RT_CALLRA(0x23a298, guest::uiDrawTextCell(9,8,0x2ec6b0,0));
    RT_CALLRA(0x23a2ac, guest::uiDrawNumber16(0x172,0x65,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))),0));
    RT_CALLRA(0x23a2c4, guest::uiDrawTextCell(0x12,8,0x333910,0));
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0x40) != 0) {
      RT_CALLRA(0x23a2dc, guest::PlayUiSound(0x1a));
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) + 1)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = (uint32_t)bVar1;
      gs_tasks_kill(param_1);
      RT_CALLRA(0x23a304, guest::PlayBgmSlot6());
    }
  }
  return 0;
}


RT_GUEST_FN(0x0023a140, guest::ShowPrizeScreen)

}  // namespace guest
