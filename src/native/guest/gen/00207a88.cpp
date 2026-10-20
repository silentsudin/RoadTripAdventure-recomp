// guest:0x00207a88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiFlatVertexWrite(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, uint32_t param_8, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  
  (*(uint8_t*)rtguest::G((uint32_t)param_1)) = param_5;
  (*(uint8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint8_t))) = param_6;
  (*(uint8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint8_t))) = param_7;
  (*(uint8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint8_t))) = param_8;
  iVar1 = (*(uint32_t*)rtguest::G(0x333288));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4))) = 0;
  if (iVar1 == 0) {
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) = (short)((int)param_2 << 4) + 0x6c00;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12))) = param_3 * 8 + 0x7900;
  }
  else {
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12))) = param_3 * 8 + 0x7800;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) = (short)(((int)param_2 << 6) / 5) + 0x7000;
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14))) = ~(int)param_4;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18))) = 5;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = 1;
  return 0;
}


RT_GUEST_FN(0x00207a88, guest::uiFlatVertexWrite)

}  // namespace guest
