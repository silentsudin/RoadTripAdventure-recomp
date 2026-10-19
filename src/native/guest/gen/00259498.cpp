// guest:0x00259498 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CheckCarArrivedAtZone(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  float fVar2;
  
  fVar2 = (float)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177ace0 + (uint32_t)(param_2 * 0x9c) * 4));
  if ((fVar2 < 175.0) || ((float)(*(uint32_t*)rtguest::G((uint32_t)0x177ace4 + (uint32_t)(param_2 * 0x9c) * 4)) < 0.0)) {
    iVar1 = -1;
  }
  else {
    iVar1 = 0;
    if (((((*(int64_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ad40 + param_2 * 0x9c))) == 0) && ((*(uint32_t*)rtguest::G((uint32_t)0x177ad48 + (uint32_t)(param_2 * 0x9c) * 4)) == 0)
         ) && (((((*(float*)rtguest::G(0x3330ac)) <= fVar2 || (fVar2 <= (*(uint32_t*)rtguest::G(0x3330b0)))) ||
                (117.0 <= (float)(*(uint32_t*)rtguest::G((uint32_t)0x177ace8 + (uint32_t)(param_2 * 0x9c) * 4)))) ||
               (iVar1 = 1, (float)(*(uint32_t*)rtguest::G((uint32_t)0x177ace8 + (uint32_t)(param_2 * 0x9c) * 4)) <= 83.0)))) &&
       (iVar1 = -1, (*(uint32_t*)rtguest::G(0x3330b4)) <= fVar2)) {
      iVar1 = 0;
    }
  }
  if (iVar1 != 0) {
    (*(uint16_t*)rtguest::G((uint32_t)0x177ade8 + (uint32_t)(param_2 * 0x138) * 2)) = (*(uint16_t*)rtguest::G((uint32_t)0x177ade8 + (uint32_t)(param_2 * 0x138) * 2)) | 0x800;
    if (iVar1 == 1) {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) == '\0') {
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = (char)param_2 + '\x01';
        return 0;
      }
    }
    else {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) | (uint32_t)(1 << (param_2 & 0x1f));
    }
  }
  return 0;
}


RT_GUEST_FN(0x00259498, guest::CheckCarArrivedAtZone)

}  // namespace guest
