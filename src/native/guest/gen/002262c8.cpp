// guest:0x002262c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetSceneFogParamsPtr(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  return 0x0177f730;
}


RT_GUEST_FN(0x002262c8, guest::GetSceneFogParamsPtr)

}  // namespace guest
