// guest:0x00269bd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FlagIndexGetNth(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  
  if (param_3 == 0xffffffff) {
    iVar1 = CountSetBitsInByteArray();
  }
  if (param_3 < param_2 << 5) {
    uVar4 = 0;
    uVar5 = 0;
    if (param_2 >> 2 != 0) {
      do {
        uVar3 = 0;
        uVar2 = (*(uint32_t*)rtguest::G((uint32_t)param_1));
        do {
          if ((uVar2 & 1) != 0) {
            if (param_3 == uVar4) {
              return uVar5 * 0x20 + uVar3;
            }
            uVar4 = uVar4 + 1;
          }
          uVar3 = uVar3 + 1;
          uVar2 = (int)(*(uint32_t*)rtguest::G((uint32_t)param_1)) >> (uVar3 & 0x1f);
        } while (uVar3 < 0x20);
        uVar5 = uVar5 + 1;
        param_1 = param_1 + 1;
      } while (uVar5 < param_2 >> 2);
    }
  }
  return -1;
}


RT_GUEST_FN(0x00269bd0, guest::FlagIndexGetNth)

}  // namespace guest
