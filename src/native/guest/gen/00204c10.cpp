// guest:0x00204c10 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ClearTaskPool(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t puVar1;
  int iVar2;
  
  iVar2 = 0x2f;
  puVar1 = 0x177a0d0;
  do {
    iVar2 = iVar2 + -1;
    (*(uint8_t*)rtguest::G((uint32_t)puVar1)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x28))) = 0;
    puVar1 = puVar1 + 0x30;
  } while (-1 < iVar2);
  return 0;
}


RT_GUEST_FN(0x00204c10, guest::ClearTaskPool)

}  // namespace guest
