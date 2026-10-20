// guest:0x0023d390 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ClearFlaggedSlotBit(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  uint uVar1;
uint32_t puVar2;
  
  puVar2 = 0x01780390 + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * sizeof(undefined4);
  uVar1 = 1 << (param_2 & 0x1f);
  if ((param_1 == 0) || (param_1 == 0xf)) {
    (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)((int)param_2 >> 5) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)((int)param_2 >> 5) * sizeof(uint32_t))) & ~uVar1;
    return 0;
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)0x17803a0 + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * 4)) & uVar1) != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)0x17803a0 + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * 4)) =
         (*(uint32_t*)rtguest::G((uint32_t)0x17803a0 + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * 4)) & ~uVar1;
    return 0;
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)0x178039c + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * 4)) & uVar1) != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)0x178039c + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * 4)) =
         (*(uint32_t*)rtguest::G((uint32_t)0x178039c + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * 4)) & ~uVar1;
    return 0;
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)0x1780398 + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * 4)) & uVar1) != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)0x1780398 + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * 4)) =
         (*(uint32_t*)rtguest::G((uint32_t)0x1780398 + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * 4)) & ~uVar1;
    return 0;
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)0x1780394 + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * 4)) & uVar1) != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)0x1780394 + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * 4)) =
         (*(uint32_t*)rtguest::G((uint32_t)0x1780394 + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * 4)) & ~uVar1;
    return 0;
  }
  (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = (*(uint32_t*)rtguest::G((uint32_t)puVar2)) & ~uVar1;
  return 0;
}


RT_GUEST_FN(0x0023d390, guest::ClearFlaggedSlotBit)

}  // namespace guest
