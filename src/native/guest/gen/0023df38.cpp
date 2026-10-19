// guest:0x0023df38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AppendQueuedByte(uint32_t param_1, ...) {
uint32_t puVar1;
  int iVar2;
  
  if ((*(int32_t*)rtguest::G(0x177fd88)) == 0x80) {
    puVar1 = 0x0177fd08;
    iVar2 = 0;
    do {
      iVar2 = iVar2 + 1;
      (*(uint8_t*)rtguest::G((uint32_t)puVar1)) = (*(uint8_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint8_t)));
      puVar1 = puVar1 + 1;
    } while (iVar2 < 0x7f);
    (*(uint32_t*)rtguest::G(0x177fd87)) = param_1;
    return 0;
  }
  (*(uint8_t*)rtguest::G((uint32_t)0x177fd08 + (uint32_t)((*(int32_t*)rtguest::G(0x177fd88))) * 1)) = param_1;
  (*(int32_t*)rtguest::G(0x177fd88)) = (*(int32_t*)rtguest::G(0x177fd88)) + 1;
  return 0;
}


RT_GUEST_FN(0x0023df38, guest::AppendQueuedByte)

}  // namespace guest
