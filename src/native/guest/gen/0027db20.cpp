// guest:0x0027db20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t abort(uint32_t q0, ...) {
  int iVar1;
uint32_t puVar2;
  undefined4 extraout_a0_lo;
uint32_t puVar3;
  
  raise(6);
  Ps2ExitThreadId(1);
  puVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x002e8cc0)) + 0x148)));
  if (puVar3 == (uint32_t)0x0) {
    puVar3 = (uint32_t)((*(uint32_t*)rtguest::G(0x002e8cc0)) + 0x14c);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x002e8cc0)) + 0x148))) = puVar3;
  }
  if (0x1f < (int)(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t)))) {
    puVar3 = malloc(0x88);
    puVar2 = (*(uint32_t*)rtguest::G(0x002e8cc0));
    if (puVar3 == (uint32_t)0x0) {
      return 0xffffffff;
    }
    (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x148)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x148))) = puVar3;
  }
  iVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t)));
  (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t))) = iVar1 + 1;
  (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(iVar1 + 2) * sizeof(uint32_t))) = extraout_a0_lo;
  return 0;
}


RT_GUEST_FN(0x0027db20, guest::abort)

}  // namespace guest
