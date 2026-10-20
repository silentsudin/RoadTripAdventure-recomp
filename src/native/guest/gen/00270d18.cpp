// guest:0x00270d18 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textGridFreeAllSlots(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(0);
uint32_t puVar1;
  uint uVar2;
  
  uVar2 = 0;
  puVar1 = 0x01773048;
  do {
    (*(uint32_t*)rtguest::G((uint32_t)puVar1)) = 0;
    uVar2 = uVar2 + 1;
    puVar1 = puVar1 + 0x58;  // T* element step
  } while (uVar2 < 4);
  return 0;
}


RT_GUEST_FN(0x00270d18, guest::textGridFreeAllSlots)

}  // namespace guest
