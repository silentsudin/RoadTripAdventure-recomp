// guest:0x0023bf28 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FindFlaggedSlotIndex(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  bool bVar1;
  int iVar2;
  uint uVar3;
  long lVar4;
uint32_t puVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  
  iVar7 = (int)param_1;
  if ((param_1 == 0) || (uVar8 = 0, param_1 == 0xf)) {
    uVar8 = 0;
    iVar2 = 0;
    while( true ) {
      if (((*(uint32_t*)rtguest::G((uint32_t)0x1780390 + (uint32_t)(iVar7 * 5 + param_2 * 0xd12 + iVar2) * 4)) & 1 << (uVar8 & 0x1f)) != 0) {
        if (param_3 == 0) {
          return uVar8;
        }
        param_3 = param_3 + -1;
      }
      uVar8 = uVar8 + 1;
      if (0x9f < (int)uVar8) break;
      iVar2 = (int)uVar8 >> 5;
    }
  }
  else {
    while ((int)uVar8 < 0x10) {
      uVar6 = 0;
      if ((uVar8 != 0) || (bVar1 = true, param_1 < 8)) {
        if ((param_4 & 1) != 0) {
          uVar6 = (uint32_t)((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x177f76c + (uint32_t)(param_2 * 0x3448 + iVar7) * 1)) == uVar8);
        }
        if ((((param_4 & 2) != 0) && ((*(int8_t*)rtguest::G((uint32_t)0x177fdd4 + (uint32_t)(param_2 * 0x3448) * 1)) != '\0')) &&
           ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177f788 + iVar7 + param_2 * 0x3448))) == uVar8)) {
          uVar6 = uVar6 + 1;
        }
        if ((((param_4 & 4) != 0) && ((*(int8_t*)rtguest::G((uint32_t)0x177fdd6 + (uint32_t)(param_2 * 0x3448) * 1)) != '\0')) &&
           ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177f7a0 + iVar7 + param_2 * 0x3448 + 4))) == uVar8)) {
          uVar6 = uVar6 + 1;
        }
        bVar1 = uVar6 < 5;
      }
      if (bVar1) {
        puVar5 = 0x01780390 + iVar7 * 5 + param_2 * 0xd12 + uVar6;
        uVar3 = (*(uint32_t*)rtguest::G((uint32_t)puVar5));
        while( true ) {
          puVar5 = puVar5 + 0x4;  // T* element step
          if (((uVar3 & 1 << (uVar8 & 0x1f)) != 0) &&
             (((param_4 & 8) == 0 || (lVar4 = DispatchByIndex15(param_1,uVar8), lVar4 != 0)))) {
            if (param_3 == 0) {
              return uVar8;
            }
            param_3 = param_3 + -1;
          }
          uVar6 = uVar6 + 1;
          if (4 < (int)uVar6) break;
          uVar3 = (*(uint32_t*)rtguest::G((uint32_t)puVar5));
        }
        uVar8 = uVar8 + 1;
      }
      else {
        uVar8 = uVar8 + 1;
      }
    }
  }
  return 0xffffffff;
}


RT_GUEST_FN(0x0023bf28, guest::FindFlaggedSlotIndex)

}  // namespace guest
