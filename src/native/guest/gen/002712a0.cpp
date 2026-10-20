// guest:0x002712a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textGridClear(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  int iVar1;
uint32_t puVar2;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_1)) * (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t)));
  puVar2 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int32_t)));
  while (iVar1 = iVar1 + -1, iVar1 != -1) {
    (*(uint16_t*)rtguest::G((uint32_t)puVar2)) = 0x720;
    puVar2 = puVar2 + 1;
  }
  (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(int32_t))) = 0;
  (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(int32_t))) = 0;
  return 0;
}


RT_GUEST_FN(0x002712a0, guest::textGridClear)

}  // namespace guest
