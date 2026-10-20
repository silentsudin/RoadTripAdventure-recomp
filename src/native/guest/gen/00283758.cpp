// guest:0x00283758 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _Bfree(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(0);
uint32_t puVar1;
uint32_t puVar2;
  
  puVar2 = (uint32_t)param_2;
  if (param_2 != 0) {
    puVar1 = (uint32_t)((*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint32_t))) * 4 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x4c))));
    (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = (*(uint32_t*)rtguest::G((uint32_t)puVar1));
    (*(uint32_t*)rtguest::G((uint32_t)puVar1)) = puVar2;
  }
  return 0;
}


RT_GUEST_FN(0x00283758, guest::_Bfree)

}  // namespace guest
