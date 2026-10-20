// guest:0x002354f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetIndexedValueAndSum(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(0);
  int iVar1;
uint32_t piVar2;
  int iVar3;
  
  piVar2 = (uint32_t)(param_1 + 0x8460);
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8458))) = param_2;
  iVar3 = 2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 * 4 + param_1 + 0x8460))) = param_3;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x846c))) = 0;
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar2));
  while( true ) {
    piVar2 = piVar2 + sizeof(int32_t);
    if (0 < iVar1) {
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x846c))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x846c))) + iVar1;
    }
    iVar3 = iVar3 + -1;
    if (iVar3 < 0) break;
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar2));
  }
  return (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x846c)));
}


RT_GUEST_FN(0x002354f0, guest::SetIndexedValueAndSum)

}  // namespace guest
