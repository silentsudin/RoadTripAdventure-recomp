// guest:0x00288368 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifGetNextRequest(uint32_t param_1, ...) {
  int iVar1;
  undefined4 uVar2;
  
  DIntr();
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc)));
  if (iVar1 == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4))) = 0;
  }
  else {
    uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x3c)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = uVar2;
  }
  EIntr();
}


RT_GUEST_FN(0x00288368, guest::sceSifGetNextRequest)

}  // namespace guest
