// guest:0x00269c80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FlagIndexBitsUpTo(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  
  if (param_3 == 0xffffffff) {
    iVar1 = CountSetBitsInByteArray(param_1,param_2);  // tail j 0x24f3b8, $4/$5 passthrough
    return iVar1;
  }
  if (param_3 < param_2 << 5) {
    iVar1 = 0;
    uVar4 = 0;
    if (param_2 >> 2 != 0) {
      do {
        uVar3 = 0;
        uVar2 = uVar4 * 0x20;
        do {
          if (param_3 == uVar2) {
            if (((*(int32_t*)rtguest::G((uint32_t)param_1)) >> (uVar3 & 0x1f) & 1U) != 0) {
              return iVar1;
            }
            return -1;
          }
          uVar2 = uVar3 & 0x1f;
          uVar3 = uVar3 + 1;
          if (((*(int32_t*)rtguest::G((uint32_t)param_1)) >> uVar2 & 1U) != 0) {
            iVar1 = iVar1 + 1;
          }
          uVar2 = uVar4 * 0x20 + uVar3;
        } while (uVar3 < 0x20);
        uVar4 = uVar4 + 1;
        param_1 = param_1 + 1;
      } while (uVar4 < param_2 >> 2);
    }
  }
  return -1;
}


RT_GUEST_FN(0x00269c80, guest::FlagIndexBitsUpTo)

}  // namespace guest
