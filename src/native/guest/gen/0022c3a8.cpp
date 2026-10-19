// guest:0x0022c3a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t getShopPacketArea(uint32_t param_1, ...) {
  int iVar1;
  
  iVar1 = param_1;
  if (param_1 < 0) {
    iVar1 = param_1 + 0xf;
  }
  ::printf((char *)rtguest::G((uint32_t)(0x2ebbe0u)), iVar1 >> 4);
  if (0x1000 < param_1) {
    fatal(0x2ebbf8,iVar1 >> 4);
  }
  return 0x1785ff0;
}


RT_GUEST_FN(0x0022c3a8, guest::getShopPacketArea)

}  // namespace guest
