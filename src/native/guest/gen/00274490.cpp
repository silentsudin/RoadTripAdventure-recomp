// guest:0x00274490 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadInfoPressMode(uint32_t param_1, uint32_t param_2, ...) {
  bool bVar1;
  long lVar2;
  
  bVar1 = false;
  if ((*(uint32_t*)rtguest::G((uint32_t)0x1773220 + (uint32_t)((int)param_1 * 0x1c + param_2 * 7) * 4)) != 0) {
    lVar2 = scePadGetButtonMask(param_1);
    bVar1 = lVar2 == 0x3ffff;
  }
}


RT_GUEST_FN(0x00274490, guest::scePadInfoPressMode)

}  // namespace guest
