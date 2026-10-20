// guest:0x00234528 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ResetStateBlock(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(param_1)), 0, 0x8cb0));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8004))) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x80a4))) = 0x6400000001;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x80ac))) = 0x10000002c;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x80b4))) = 0x29f74800000004;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x80bc))) = 1;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x80c4))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x80cc))) = 0;
  return 0;
}


RT_GUEST_FN(0x00234528, guest::ResetStateBlock)

}  // namespace guest
