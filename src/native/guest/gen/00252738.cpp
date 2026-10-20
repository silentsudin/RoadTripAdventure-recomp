// guest:0x00252738 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RecordGolfHoleScore(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
uint32_t piVar5;
uint32_t piVar6;
  
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8008)));
  iVar4 = param_2 + 1;
  if (99 < param_2 + 1) {
    iVar4 = 99;
  }
  iVar2 = iVar3 * 4;
  iVar1 = iVar2 + param_1 + 0xc;
  piVar5 = (uint32_t)(iVar2 + param_1 + 0x8084);
  iVar4 = iVar4 - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8000)));
  piVar6 = (uint32_t)(param_1 + 0xc + iVar2 + 0x8050);
  (*(int32_t*)rtguest::G((uint32_t)piVar5)) = iVar4;
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80a8))) + iVar4;
  iVar2 = 99;
  if (iVar4 < 100) {
    iVar2 = iVar4;
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80a8))) = iVar2;
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)piVar5));
  if (iVar4 < (*(int32_t*)rtguest::G((uint32_t)piVar6))) {
    (*(int32_t*)rtguest::G((uint32_t)piVar6)) = iVar4;
    (*(uint8_t*)rtguest::G((uint32_t)0x1780a30 + (uint32_t)(iVar3) * 1)) = (char)iVar4;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x80a0))) = 1;
  }
  if (iVar3 == 8) {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80a8)));
    if (iVar3 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8080)))) {
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8080))) = iVar3;
      (*(uint8_t*)rtguest::G(0x1780a39)) = (uint32_t)iVar3;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80d0))) = 1;
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80a8)));
    }
    if (0 < iVar3) {
      GetOrSetPresidentRaceResult(1);
      return 0;
    }
    GetOrSetPresidentRaceResult(2);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00252738, guest::RecordGolfHoleScore)

}  // namespace guest
