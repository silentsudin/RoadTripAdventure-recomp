// guest:0x0023d818 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PopByteQueue(uint32_t q0, ...) {
  undefined1 uVar1;
uint32_t puVar2;
  int iVar3;
  
  uVar1 = (*(uint32_t*)rtguest::G(0x1782a64));
  if ((*(int32_t*)rtguest::G(0x1782a60)) != 0) {
    (*(int32_t*)rtguest::G(0x1782a60)) = (*(int32_t*)rtguest::G(0x1782a60)) + -1;
    iVar3 = 0;
    if (0 < (*(int32_t*)rtguest::G(0x1782a60))) {
      puVar2 = 0x01782a64;
      do {
        iVar3 = iVar3 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)puVar2)) = (*(uint8_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint8_t)));
        puVar2 = puVar2 + 1;
      } while (iVar3 < (*(int32_t*)rtguest::G(0x1782a60)));
    }
  }
  return 0;
}


RT_GUEST_FN(0x0023d818, guest::PopByteQueue)

}  // namespace guest
