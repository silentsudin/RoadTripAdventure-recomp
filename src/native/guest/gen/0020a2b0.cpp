// guest:0x0020a2b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DispatchMenuTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  int iVar1;
  undefined4 uVar2;
  undefined8 uVar3;
  int iVar4;
  
  iVar4 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) == 0) {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c)));
    if (iVar1 == 0) {
      RT_CALLRA(0x20a2e4, guest::gs_tasks_spawn(param_1,0x20a520,0));
      return 0;
    }
    if (iVar1 == 2) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 5;
      RT_CALLRA(0x20a310, guest::gs_tasks_spawn(param_1,0x209cb0,5));
      return 0;
    }
    if (iVar1 == 3) {
      RT_CALLRA(0x20a330, guest::gs_tasks_spawn(param_1,0x20dc50,0x410000));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 4;
      uVar3 = RT_CALLRA(0x20a34c, guest::gs_tasks_spawn(param_1,0x208a88,0));
      RT_CALLRA(0x20a354, guest::SetTaskFlag(uVar3));
      return 0;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 2;
    uVar3 = 0x208848;
  }
  else {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) == '\0') {
      if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) == '\0') && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) == '\x03')) {
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) == 0) {
          return 0;
        }
        RT_CALLRA(0x20a470, guest::PlayBgmSlot2());
        RT_CALLRA(0x20a478, guest::DestroyTaskRecord(param_1));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 4;
        RT_CALLRA(0x20a40c, guest::gs_tasks_spawn(param_1,0x208a88,4));
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = 0;
        RT_CALLRA(0x20a4a0, guest::PlayUiSound(0x1f));
        return 0;
      }
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) != '\x04') {
        return 0;
      }
      if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0x8000800) == 0) {
        return 0;
      }
      RT_CALLRA(0x20a4d4, guest::PlayBgmSlot2());
      RT_CALLRA(0x20a4dc, guest::DestroyTaskRecord(param_1));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 5;
      RT_CALLRA(0x20a494, guest::gs_tasks_spawn(param_1,0x209cb0,5));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = 0;
      RT_CALLRA(0x20a504, guest::PlayUiSound(0x1f));
      return 0;
    }
    switch((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25)))) {
    case 0:
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 1;
      uVar3 = 0x2086b0;
      break;
    case 1:
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 2;
      uVar3 = 0x208848;
      break;
    case 2:
    case 3:
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 4;
      uVar3 = 0x208a88;
      break;
    case 4:
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 3;
      uVar3 = 0x20a150;
      break;
    case 5:
      iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 4))) = uVar2;
      gs_tasks_kill(param_1);
      return 0;
    default:
      goto switchD_0020a3a4_default;
    }
  }
  RT_CALLRA(0x20a4f8, guest::gs_tasks_spawn(param_1,uVar3,0));
switchD_0020a3a4_default:
  return 0;
}


RT_GUEST_FN(0x0020a2b0, guest::DispatchMenuTask)

}  // namespace guest
