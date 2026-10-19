// guest:0x0027cac8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t add_fdes(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  uint uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_1));
  uVar4 = (*(uint32_t*)rtguest::G((uint32_t)param_3));
  uVar5 = (*(uint32_t*)rtguest::G((uint32_t)param_4));
  if (iVar3 != 0) {
    do {
      if ((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) != 0) {
        uVar1 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int32_t)));
        if (uVar1 != 0) {
          if ((*(int32_t*)rtguest::G((uint32_t)param_2)) == 0) {
            iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int32_t)));
          }
          else {
            iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t)));
            (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))) = iVar3 + 1;
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 * 4 + (*(int32_t*)rtguest::G((uint32_t)param_2))))) = param_1;
            iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_1));
            iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int32_t)));
          }
          if (uVar1 < uVar4) {
            uVar4 = uVar1;
          }
          if (uVar5 < uVar1 + iVar2) {
            uVar5 = uVar1 + iVar2;
          }
        }
      }
      param_1 = (uint32_t)((int)param_1 + iVar3 + 4);
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_1));
    } while (iVar3 != 0);
  }
  (*(uint32_t*)rtguest::G((uint32_t)param_3)) = uVar4;
  (*(uint32_t*)rtguest::G((uint32_t)param_4)) = uVar5;
  return 0;
}


RT_GUEST_FN(0x0027cac8, guest::add_fdes)

}  // namespace guest
