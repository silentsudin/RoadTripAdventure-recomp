// guest:0x00223238 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawShowroomCarModel(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  vifUploadLightMatrix(0x177f620);
  vifObjectMatrix(param_2);
  vifBatchRegs((long)(param_3 * 0x44 + 0x243e) | 0x1d308000U |
               (long)(param_3 * 0x44 + 0x247e) << 0x25 | 0x20000005c0000000,7,0,0xf);
  vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177f310 + (uint32_t)(param_3 * 6) * 4)) + 0x10);
  return 0;
}


RT_GUEST_FN(0x00223238, guest::DrawShowroomCarModel)

}  // namespace guest
