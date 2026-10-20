// guest:0x00243698 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetConst500(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(0);
  return 500;
}


RT_GUEST_FN(0x00243698, guest::GetConst500)

}  // namespace guest
