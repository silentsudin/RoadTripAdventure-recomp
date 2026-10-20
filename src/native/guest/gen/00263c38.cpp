// guest:0x00263c38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t yThresholdGoalTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  short sVar1;
  int iVar2;
  int iVar3;
uint32_t pvVar4;
uint32_t puVar5;
  undefined4 uVar6;
  
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) == 0) {
    iVar3 = getExtraPacketArea(0x8cc0);
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = iVar3;
    pvVar4 = (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)((uint32_t)(iVar3 + 0x8cb0))), 0, 0x10));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) + 0x8cb0))) = 0;
    return pvVar4;
  }
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
  puVar5 = (uint32_t)(iVar3 + 0x8cb0);
  switch((*(uint32_t*)rtguest::G((uint32_t)puVar5))) {
  case 0:
    uVar6 = (*(uint32_t*)rtguest::G(0x3331c8));
    StoreIndexedPosition((*(uint32_t*)rtguest::G(0x3331c8)),0x43960000,(*(uint32_t*)rtguest::G(0x3331cc)),0,0xffffffffffffc000);
    StoreIndexedPosition(uVar6,0x43960000,0x442f0000,1,0xffffffffffffc000);
    (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = 1;
  case 1:
    if ((*(uint32_t*)rtguest::G(0x177ace4)) < 280.0) {
      GetOrSetPresidentRaceResult(1);
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
      (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = 2;
    }
    if (280.0 <= (*(uint32_t*)rtguest::G(0x177af54))) {
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4)));
      goto LAB_00263e04;
    }
    GetOrSetPresidentRaceResult(2);
    (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
    (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = 2;
    break;
  case 2:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0xfffffeff | 8;
    (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = 3;
    break;
  case 3:
    sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2)));
    if (0x40 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2)))) {
      uiFade(((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) + -0x40) * 0x1000000);
      sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2)));
    }
    if (sVar1 < 0xc0) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = sVar1 + 1;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = 0xffffffff;
    }
    break;
  case 0xffffffff:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4))) = 1;
    break;
  default:
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4)));
    goto LAB_00263e04;
  }
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4)));
LAB_00263e04:
  pvVar4 = (uint32_t)0x0;
  if (iVar3 != 0) {
    uiFade(0xffffffff80000000);
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18)));
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 8;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 0xffffffff;
    pvVar4 = (uint32_t)gs_tasks_kill(iVar3);
  }
  return pvVar4;
}


RT_GUEST_FN(0x00263c38, guest::yThresholdGoalTask)

}  // namespace guest
