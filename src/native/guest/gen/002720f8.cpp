// guest:0x002720f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gifWriteTag(uint32_t param_1, uint64_t param_2, uint64_t param_3, ...) {
  rtguest::FrameGuard rt_fg(0);
uint32_t puVar1;
  
  puVar1 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)param_1));
  (*(uint64_t*)rtguest::G((uint32_t)puVar1)) = param_2;
  (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int32_t))) = (int)puVar1;
  (*(uint64_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint64_t))) = param_3;
  (*(int32_t*)rtguest::G((uint32_t)param_1)) = (int)(puVar1 + 2);
  return 0;
}


RT_GUEST_FN(0x002720f8, guest::gifWriteTag)

}  // namespace guest
