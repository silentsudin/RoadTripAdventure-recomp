// guest:0x00284bc0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _Exit(uint32_t __status, ...) {
  rtguest::FrameGuard rt_fg(0);
  syscall(4);
  return 0;
}


RT_GUEST_FN(0x00284bc0, guest::_Exit)

}  // namespace guest
