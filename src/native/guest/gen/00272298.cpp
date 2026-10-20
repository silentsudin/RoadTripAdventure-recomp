// guest:0x00272298 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t heapFreeBlock(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  uint uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
uint32_t puVar7;
  int iVar8;
  
  if ((((*(uint32_t*)rtguest::G(0x2dce70)) & 0xfffffff) != 0xfffffff) && (param_1 != 0)) {
    if ((*(uint32_t*)rtguest::G(0x2dce70)) >> 0x1c != 3) {
      iVar3 = 0;
      iVar2 = 0;
      iVar8 = 0;
      do {
        iVar6 = iVar2;
        puVar7 = (uint32_t)((int)0x002dce70 + iVar3);
        uVar4 = (*(uint32_t*)rtguest::G((uint32_t)puVar7));
        iVar5 = (uVar4 & 0xfffffff) + 1;
        iVar2 = iVar6 + iVar5;
        if (param_1 == iVar3 + 0x2dce74) {
          if ((*(uint32_t*)rtguest::G((uint32_t)0x2dce70 + (uint32_t)(iVar2) * 4)) >> 0x1c == 0) {
            uVar4 = uVar4 & 0xf0000000 | iVar5 + ((*(uint32_t*)rtguest::G((uint32_t)0x2dce70 + (uint32_t)(iVar2) * 4)) & 0xfffffff) & 0xfffffff;
            (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = uVar4;
          }
          uVar1 = (*(uint32_t*)rtguest::G((uint32_t)0x2dce70 + (uint32_t)(iVar8) * 4));
          if (uVar1 >> 0x1c != 0) {
            (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = uVar4 & 0xfffffff;
            return 0;
          }
          (*(uint32_t*)rtguest::G((uint32_t)0x2dce70 + (uint32_t)(iVar8) * 4)) =
               uVar1 & 0xf0000000 | (uVar1 & 0xfffffff) + 1 + (uVar4 & 0xfffffff) & 0xfffffff;
          return 0;
        }
        iVar3 = iVar2 * 4;
        iVar8 = iVar6;
      } while ((*(uint32_t*)rtguest::G((uint32_t)0x2dce70 + (uint32_t)(iVar2) * 4)) >> 0x1c != 3);
    }
  }
  return 0;
}


RT_GUEST_FN(0x00272298, guest::heapFreeBlock)

}  // namespace guest
