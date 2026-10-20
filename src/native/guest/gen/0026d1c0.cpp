// guest:0x0026d1c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AdvanceDayCycleStep(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  byte bVar1;
  int iVar2;
  char cVar3;
  undefined4 uVar4;
  uint64_t lVar5;
  int iVar6;
  
  iVar6 = (int)param_1;
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27)));
  lVar5 = (long)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8)));
  if (lVar5 == (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x002dcd50 + (uint32_t)bVar1 * 2))) + -0x40) {
    gs_tasks_spawn(param_1,0x2082d8,0);
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27)));
    lVar5 = (long)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8)));
  }
  if (lVar5 == (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x002dcd50 + (uint32_t)bVar1 * 2)))) {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x50))) = 0x80000000;
    if (iVar2 != 0) {
      gs_tasks_kill(iVar2);
    }
    cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27))) + '\x01';
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27))) == '\t') {
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 0;
      gs_tasks_kill(param_1);
      return 0;
    }
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27))) = cVar3;
    uVar4 = gs_tasks_spawn(param_1,0x26ca48,cVar3);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = uVar4;
  }
  return 0;
}


RT_GUEST_FN(0x0026d1c0, guest::AdvanceDayCycleStep)

}  // namespace guest
