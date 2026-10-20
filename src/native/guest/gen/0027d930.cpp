// guest:0x0027d930 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __fpcmp_parts_d(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  uint uVar1;
  int iVar2;
  uint uVar3;
  
  uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_1));
  if ((uVar3 < 2) || (uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_2)), uVar1 < 2)) {
    return 1;
  }
  if (uVar3 == 4) {
    if ((uVar1 ^ 4) == 0) {
      return (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint32_t))) - (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t)));
    }
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t)));
  }
  else {
    if ((uVar1 ^ 4) == 0) {
      iVar2 = 1;
      if ((*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint32_t))) == 0) {
        iVar2 = -1;
      }
      return iVar2;
    }
    if (uVar3 == 2) {
      if (uVar1 == 2) {
        return 0;
      }
      iVar2 = -1;
      if ((*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint32_t))) != 0) {
        iVar2 = 1;
      }
      return iVar2;
    }
    if (uVar1 != 2) {
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t)));
      if (uVar3 == (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint32_t)))) {
        if ((int)(*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) <= (int)(*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(uint32_t)))) {
          if ((int)(*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(uint32_t))) <= (int)(*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t)))) {
            if ((*(uint64_t*)rtguest::G((uint32_t)(param_2 + 4 * sizeof(uint32_t)))) < (*(uint64_t*)rtguest::G((uint32_t)(param_1 + 4 * sizeof(uint32_t))))) goto LAB_0027da24;
            if ((*(uint64_t*)rtguest::G((uint32_t)(param_2 + 4 * sizeof(uint32_t)))) <= (*(uint64_t*)rtguest::G((uint32_t)(param_1 + 4 * sizeof(uint32_t))))) {
              return 0;
            }
          }
          iVar2 = 1;
          if (uVar3 == 0) {
            iVar2 = -1;
          }
          return iVar2;
        }
      }
LAB_0027da24:
      iVar2 = -1;
      if (uVar3 == 0) {
        iVar2 = 1;
      }
      return iVar2;
    }
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t)));
  }
  iVar2 = -1;
  if (uVar3 == 0) {
    iVar2 = 1;
  }
  return iVar2;
}


RT_GUEST_FN(0x0027d930, guest::__fpcmp_parts_d)

}  // namespace guest
