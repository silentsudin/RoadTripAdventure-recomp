// guest:0x0026de80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
// Widened arity (param_2 named) vs the one-arg decl: rename the stale decl.
#include "guest_decls.inc"

namespace guest {

uint32_t DrawMapCoordinateValues(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(128);
  uint64_t uVar1;
  uint64_t uVar2;
  uint64_t uVar3;
  uint64_t uVar4;
  uint32_t auStack_80 = rtguest::allocFrame(64);

  uVar1 = fptodp((*(uint32_t*)rtguest::G((uint32_t)(param_1))));
  uVar2 = fptodp((*(uint32_t*)rtguest::G((uint32_t)((uint32_t)(param_1 + (uint32_t)(1) * sizeof(uint32_t))))));
  uVar3 = fptodp((*(uint32_t*)rtguest::G((uint32_t)((uint32_t)(param_1 + (uint32_t)(2) * sizeof(uint32_t))))));
  uVar4 = fptodp((*(uint32_t*)rtguest::G((uint32_t)((uint32_t)(param_1 + (uint32_t)(3) * sizeof(uint32_t))))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_80)), (char *)rtguest::G((uint32_t)(0x32fbb8)), uVar1, uVar2, uVar3, uVar4);
  uiDebugText8(0x10, param_2 << 3, auStack_80, 0xf);
  return 0;
}


RT_GUEST_FN(0x0026de80, guest::DrawMapCoordinateValues)

}  // namespace guest
