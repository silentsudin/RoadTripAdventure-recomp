// guest:0x00235460 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitObjectType20(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  uint uVar1;
uint32_t puVar2;
  
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(param_1)), 0, 0x8cb0));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8008))) = 0x20;
  uVar1 = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8004))) = 0;
  puVar2 = (uint32_t)((int)param_1 + 0x8460);
  do {
    uVar1 = uVar1 + 1;
    (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0xffffffff;
    puVar2 = puVar2 + (uint32_t)(1) * sizeof(uint32_t);
  } while (uVar1 < 4);
  return 0;
}


RT_GUEST_FN(0x00235460, guest::InitObjectType20)

}  // namespace guest
