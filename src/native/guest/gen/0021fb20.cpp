// guest:0x0021fb20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetCarSlotEntry(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(80);
  return 0x0029ce00 + param_1 * 0x20;
}


RT_GUEST_FN(0x0021fb20, guest::GetCarSlotEntry)

}  // namespace guest
