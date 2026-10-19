// guest:0x00234378 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetDashConfig(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  int iVar1;
  undefined4 uVar2;
uint32_t __s;
  int iVar3;
  
  iVar3 = 0;
  if (param_3 != 0) {
    iVar3 = 3;
  }
  iVar1 = param_1 + param_2 * 0x408;
  __s = (uint32_t)(iVar1 + 0x8614);
  param_2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8004))) + param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)__s)) != iVar3) {
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x2c));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8618))) = 0x140;
    uVar2 = 0x70;
    (*(int32_t*)rtguest::G((uint32_t)__s)) = iVar3;
    if ((param_2 != 0) && (param_2 != 1)) {
      uVar2 = 0x150;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x861c))) = uVar2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x863c))) = 0x40000000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8624))) = 3;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8628))) = 0x0029f7a8;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x862c))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8620))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8638))) = 0;
  }
  return 0;
}


RT_GUEST_FN(0x00234378, guest::SetDashConfig)

}  // namespace guest
