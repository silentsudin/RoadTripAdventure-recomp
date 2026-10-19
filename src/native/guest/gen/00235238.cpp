// guest:0x00235238 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetHudState3(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
uint32_t __s;
  int iVar1;
  
  __s = (uint32_t)(param_1 + 0x82e0);
  iVar1 = 0;
  if (param_3 != 0) {
    iVar1 = 3;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)__s)) != iVar1) {
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x2c));
    (*(int32_t*)rtguest::G((uint32_t)__s)) = iVar1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8308))) = 0x40000000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82e4))) = 0x140;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82e8))) = 0xe0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f0))) = 3;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f4))) = 0x0029fac8;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f8))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8304))) = 0x1e;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82ec))) = 1;
  }
  return 0;
}


RT_GUEST_FN(0x00235238, guest::SetHudState3)

}  // namespace guest
