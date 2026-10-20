// guest:0x0021fae8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t LoadCameraPreset(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(80);
  param_2 = param_2 * 0x20;
  (*(uint64_t*)rtguest::G((uint32_t)param_1)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x0029ce00 + param_2)));
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x0029ce08 + param_2)));
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x0029ce10 + param_2)));
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x0029ce18 + param_2)));
  return 0;
}


RT_GUEST_FN(0x0021fae8, guest::LoadCameraPreset)

}  // namespace guest
