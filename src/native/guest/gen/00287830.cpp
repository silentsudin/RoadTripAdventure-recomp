// guest:0x00287830 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sifAllocRpcPacket(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  int iVar2;
  int iVar3;
  
  DIntr();
  iVar1 = 0;
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t)));
  if (0 < (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int32_t)))) {
    do {
      if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x10))) & 1) == 0) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x10))) = iVar1 << 0x10 | 5;
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_1));
        iVar2 = iVar1 + 1;
        (*(int32_t*)rtguest::G((uint32_t)param_1)) = iVar2;
        if (iVar2 == 1) {
          iVar2 = 1;
          (*(int32_t*)rtguest::G((uint32_t)param_1)) = iVar1 + 2;
        }
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x14))) = iVar3;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18))) = iVar2;
        EIntr();
        return iVar3;
      }
      iVar1 = iVar1 + 1;
      iVar3 = iVar3 + 0x40;
    } while (iVar1 < (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int32_t))));
  }
  EIntr();
  return 0;
}


RT_GUEST_FN(0x00287830, guest::sifAllocRpcPacket)

}  // namespace guest
