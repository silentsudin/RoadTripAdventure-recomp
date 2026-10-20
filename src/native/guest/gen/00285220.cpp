// guest:0x00285220 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FlushCache(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(0);
  syscall(100);
  return 0;
}


RT_GUEST_FN(0x00285220, guest::FlushCache)

}  // namespace guest
