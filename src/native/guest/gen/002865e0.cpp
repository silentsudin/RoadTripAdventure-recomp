// guest:0x002865e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePrintfPutChar(uint32_t param_1, ...) {
  int iVar1;
  
  iVar1 = (*(int32_t*)rtguest::G(0x2e98f4));
  if (0x7d < (*(int32_t*)rtguest::G(0x2e98f4))) {
    (*(int32_t*)rtguest::G(0x2e98f4)) = 0;
    (*(uint32_t*)rtguest::G(0x1776dbf)) = 0;
    kputs(0x1776d40);
    iVar1 = (*(int32_t*)rtguest::G(0x2e98f4));
  }
  if (param_1 != 10) {
    (*(int32_t*)rtguest::G(0x2e98f4)) = iVar1 + 1;
    (*(uint32_t*)rtguest::G((uint32_t)0x1776d40 + (uint32_t)(iVar1) * 4)) = (char)param_1;
    return 0;
  }
  (*(int32_t*)rtguest::G(0x2e98f4)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)0x1776d40 + (uint32_t)(iVar1) * 4)) = 10;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x1776d41))) = 0;
  kputs(0x1776d40);  // merge path of jal 286618; $4=base (lui/addiu 286608)
  return 0;
}


RT_GUEST_FN(0x002865e0, guest::scePrintfPutChar)

}  // namespace guest
