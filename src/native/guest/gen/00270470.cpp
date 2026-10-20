// guest:0x00270470 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDmaGetEnv(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(80);
  undefined8 uVar1;
  undefined4 uVar2;
uint32_t puVar3;
  
  uVar2 = (*(uint32_t*)rtguest::G(0x2dce68));
  uVar1 = (*(uint64_t*)rtguest::G(0x2dce60));
  puVar3 = (uint32_t)param_1;
  (*(uint64_t*)rtguest::G((uint32_t)puVar3)) = (*(uint64_t*)rtguest::G(0x2dce58));
  (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t))) = uVar1;
  (*(uint32_t*)rtguest::G((uint32_t)puVar3 + 2 * sizeof(uint64_t))) = uVar2;
  return param_1;
}


RT_GUEST_FN(0x00270470, guest::sceDmaGetEnv)

}  // namespace guest

