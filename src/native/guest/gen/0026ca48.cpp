// guest:0x0026ca48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t ApplyTimeOfDayLighting(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  undefined4 uVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  
  iVar3 = (int)param_1;
  iVar4 = (int)param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) == 0) {
    RT_CALLRA(0x26ca78, guest::SpawnTownEntities((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c))) + 0x16));
    RT_CALLRA(0x26ca88, guest::StartTownModeTask(param_1,param_2,1));
    cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25)));
  }
  else {
    cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25)));
    if ((cVar2 == '\0') && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xf))) == '\0')) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 1;
      RT_CALLRA(0x26cab4, guest::ClearTaskPoolFlag(param_1,0,param_1,param_2));
      RT_CALLRA(0x26cac8, guest::gs_tasks_spawn(param_1,0x208270,0));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x28))) & 0xfffffbff | 0x10000;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x14))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dcd28 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c))) * 4)));
      cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25)));
    }
  }
  if (cVar2 == '\x01') {
    RT_CALLRA(0x26cb20, guest::updateTimeOfDayLight((*(uint32_t*)rtguest::G(0x177ace4)),param_2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x14)))));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x22))) == -1) {
      uVar1 = RT_CALLRA(0x26cb40, guest::DecomposeWorldPosition(iVar4 + 0x58,iVar4 + 0x5c,0x177acf0));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x54))) = uVar1;
    }
  }
  return 0;
}


RT_GUEST_FN(0x0026ca48, guest::ApplyTimeOfDayLighting)

}  // namespace guest
