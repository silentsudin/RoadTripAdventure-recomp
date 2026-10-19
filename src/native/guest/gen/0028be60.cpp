// guest:0x0028be60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifLoadIopHeap(uint32_t param_1, uint32_t param_2, ...) {
  char cVar1;
  undefined4 uVar2;
  long lVar3;
  int iVar4;
  
  if ((*(uint32_t*)rtguest::G(0x2e999c)) < 0) {
    uVar2 = 0;
  }
  else {
    (*(uint32_t*)rtguest::G(0x1779d04)) = (*(int8_t*)rtguest::G((uint32_t)param_1));
    iVar4 = 0;
    if ((*(uint32_t*)rtguest::G(0x1779d04)) != '\0') {
      for (iVar4 = 1;
          (iVar4 < 0xfc && (cVar1 = (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(iVar4) * sizeof(int8_t))), (*(uint32_t*)rtguest::G((uint32_t)0x1779d04 + (uint32_t)(iVar4) * 4)) = cVar1, cVar1 != '\0'));
          iVar4 = iVar4 + 1) {
      }
    }
    if (iVar4 == 0xfc) {
      iVar4 = 0xfb;
    }
    (*(uint32_t*)rtguest::G(0x1779dff)) = 0;
    (*(uint32_t*)rtguest::G(0x1779d00)) = param_2;
    lVar3 = sceSifCallRpc(0x1779c40,3,0,0x1779d00,iVar4 + 5,0x1779c80,4,0);
    uVar2 = (*(uint32_t*)rtguest::G(0x1779c80));
    if (lVar3 < 0) {
      uVar2 = 0xffffffff;
    }
  }
}


RT_GUEST_FN(0x0028be60, guest::sceSifLoadIopHeap)

}  // namespace guest
