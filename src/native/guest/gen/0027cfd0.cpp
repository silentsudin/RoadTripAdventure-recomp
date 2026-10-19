// guest:0x0027cfd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t decode_stack_op(uint32_t param_1, uint32_t param_2, ...) {
  byte bVar1;
  long lVar2;
  long extraout_a0;
uint32_t aiStack_30 = rtguest::allocFrame(16);
  
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_1));
  lVar2 = (long)(int)(param_1 + 1);
  if (bVar1 < 0x70) {
    if (0x4f < bVar1) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3f8))) = bVar1 - 0x50;
    }
    if (bVar1 == 6) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x477))) = 1;
    }
    if (bVar1 == 0x23) {
      lVar2 = decode_uleb128(lVar2,aiStack_30);
      (*(int64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 8))) = (long)(*(int32_t*)rtguest::G((uint32_t)aiStack_30 + (uint32_t)(0) * 4));
    }
LAB_0027d0d0:
    abort();
  }
  if (bVar1 == 0x90) {
    lVar2 = decode_sleb128(lVar2,aiStack_30);
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3f8))) = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)aiStack_30 + (uint32_t)(0) * 4));
  }
  else if (0x8f < bVar1) {
    if (bVar1 != 0x92) goto LAB_0027d0d0;
    lVar2 = decode_sleb128(lVar2,aiStack_30);
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3f8))) = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)aiStack_30 + (uint32_t)(0) * 4));
    goto LAB_0027d090;
  }
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3f8))) = bVar1 - 0x70;
LAB_0027d090:
  lVar2 = decode_sleb128(lVar2,aiStack_30);
  (*(int64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10))) = (long)(*(int32_t*)rtguest::G((uint32_t)aiStack_30 + (uint32_t)(0) * 4));
}


RT_GUEST_FN(0x0027cfd0, guest::decode_stack_op)

}  // namespace guest
