// guest:0x00272118 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t heapAllocBlock(uint32_t param_1, ...) {
  int iVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
uint32_t puVar5;
  uint uVar6;
  
  uVar2 = (*(uint32_t*)rtguest::G(0x2dce70));
  if (((*(uint32_t*)rtguest::G(0x2dce70)) & 0xfffffff) == 0xfffffff) {
    (*(uint32_t*)rtguest::G(0x2dce70)) = (*(uint32_t*)rtguest::G(0x2dce70)) & 0xffffffff00000000 | 0x27fe;
    (*(uint32_t*)rtguest::G(0x2e6e68)) = (*(uint32_t*)rtguest::G(0x2e6e68)) & 0xffffffff | 0x3000000000000000;
    uVar2 = 0;
  }
  uVar6 = param_1 + 3U >> 2;
  iVar4 = 0;
  if (uVar2 >> 0x1c != 3) {
    iVar1 = 0;
    do {
      puVar5 = (uint32_t)((int)0x002dce70 + iVar1);
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)puVar5));
      uVar3 = uVar2 & 0xfffffff;
      if ((uVar2 >> 0x1c == 0) && (uVar6 <= uVar3)) {
        if (uVar6 != uVar3) {
          (*(uint32_t*)rtguest::G((uint32_t)0x2dce70 + (uint32_t)(iVar4 + uVar6 + 1) * 4)) = (uVar3 - uVar6) - 1 & 0xfffffff;
          uVar2 = (*(uint32_t*)rtguest::G((uint32_t)puVar5)) & 0xf0000000 | uVar6 & 0xfffffff;
          (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = uVar2;
        }
        (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = uVar2 & 0xfffffff | 0x10000000;
        return iVar1 + 0x2dce74;
      }
      iVar4 = iVar4 + 1 + uVar3;
      iVar1 = iVar4 * 4;
    } while ((*(uint32_t*)rtguest::G((uint32_t)0x2dce70 + (uint32_t)(iVar4) * 4)) >> 0x1c != 3);
  }
  return 0;
}


RT_GUEST_FN(0x00272118, guest::heapAllocBlock)

}  // namespace guest
