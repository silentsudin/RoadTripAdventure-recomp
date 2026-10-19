// guest:0x0020e250 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t cdReadEntryPart(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  long lVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  
  (*(uint32_t*)rtguest::G(0x3356c4)) = 0;
  iVar3 = (param_3 & 0xfffff800) + 0x7ff;
  uVar4 = (param_3 & 0x7ff) + param_4 + 0x7ff & 0xfffff800;
  iVar2 = uVar4 + 0x7ff;
  (*(uint32_t*)rtguest::G((uint32_t)0x01727b80 + 4)) = (int)((*(uint64_t*)rtguest::G((uint32_t)param_1)) >> 0x20);
  if (iVar3 < 0) {
    iVar3 = (param_3 & 0xfffff800) + 0xffe;
  }
  if (iVar2 < 0) {
    iVar2 = uVar4 + 0xffe;
  }
  (*(uint32_t*)rtguest::G(0x1727b88)) = iVar2 >> 0xb;
  iVar3 = (*(uint32_t*)rtguest::G((uint32_t)0x01727b80 + 4)) + (iVar3 >> 0xb);
  (*(uint64_t*)rtguest::G(0x1727b80)) = CONCAT44(iVar3,(int)(*(uint64_t*)rtguest::G((uint32_t)param_1)));
  (*(uint32_t*)rtguest::G(0x3356cc)) = 0x01727b80;
  (*(uint32_t*)rtguest::G(0x3356c8)) = (uint32_t)param_2;
  lVar1 = sceCdRead(iVar3,(*(uint32_t*)rtguest::G(0x1727b88)),param_2,0x333380);
  if (lVar1 == 0) {
    (*(uint32_t*)rtguest::G(0x3356c4)) = 0;
  }
  else {
    (*(uint32_t*)rtguest::G(0x3356c4)) = 2;
  }
  return param_3 & 0x7ff;
}


RT_GUEST_FN(0x0020e250, guest::cdReadEntryPart)

}  // namespace guest
