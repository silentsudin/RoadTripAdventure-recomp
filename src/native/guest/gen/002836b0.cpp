// guest:0x002836b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _Balloc(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  int iVar1;
uint32_t piVar2;
uint32_t piVar3;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x4c)));
  if (iVar1 == 0) {
    iVar1 = _calloc_r(param_1,4,0x10);
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x4c))) = iVar1;
    if (iVar1 == 0) {
      return 0;
    }
  }
  piVar3 = (uint32_t)(param_2 * 4 + iVar1);
  piVar2 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)piVar3));
  if (piVar2 == (uint32_t)0x0) {
    iVar1 = 1 << (param_2 & 0x1f);
    piVar2 = (uint32_t)_calloc_r(param_1,1,iVar1 * 4 + 0x14);
    if (piVar2 == (uint32_t)0x0) {
      return 0;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar2 + 4))) = param_2;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar2 + 8))) = iVar1;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)piVar3)) = (*(int32_t*)rtguest::G((uint32_t)piVar2));
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar2 + 0x10))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar2 + 0xc))) = 0;
  return (int)piVar2;
}


RT_GUEST_FN(0x002836b0, guest::_Balloc)

}  // namespace guest
