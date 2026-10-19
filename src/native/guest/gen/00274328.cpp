// guest:0x00274328 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadGetButtonMask(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  if ((*(uint32_t*)rtguest::G((uint32_t)0x1773220 + (uint32_t)((int)param_1 * 0x1c + param_2 * 7) * 4)) != 0) {
    iVar1 = scePadGetDmaStr(param_1);
    if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x72))) == '\x01') {
      iVar2 = 0;
      if ((1 < (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 100)))) && (iVar2 = 0, 1 < (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x66))))) {
        iVar2 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x79))) + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x7c))) * 0x1000000 +
                (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x7b))) * 0x10000 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x7a))) * 0x100;
      }
    }
    else {
      iVar2 = 0;
    }
  }
  return iVar2;
}


RT_GUEST_FN(0x00274328, guest::scePadGetButtonMask)

}  // namespace guest
