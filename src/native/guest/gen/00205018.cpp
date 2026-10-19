// guest:0x00205018 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DestroyTaskTree(uint32_t param_1, ...) {
  int iVar1;
  
  if ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x2c))) == (uint32_t)0x0) {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
  }
  else {
    // MIPS 0x205028/34: lw $v0,0x2c($s0); jalr $v0 — node+0x2c IS the handler
    // pointer ($a0 = node held from 0x205020). Ghidra's *(code**) double-deref
    // mistranslation made the kill call jump into code bytes (garbage handler).
    rtCall((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x2c))), param_1);
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
  }
  (*(uint8_t*)rtguest::G((uint32_t)param_1)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = 0;
  if (iVar1 != 0) {
    DestroyTaskTree(iVar1);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) = 0;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14))) != 0) {
    DestroyTaskTree((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14))));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14))) = 0;
  }
  return 0;
}


RT_GUEST_FN(0x00205018, guest::DestroyTaskTree)

}  // namespace guest
