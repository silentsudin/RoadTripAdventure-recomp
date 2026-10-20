// guest:0x0028d008 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _InitSys(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  supplement_crt0();
  InitAlarm();
  InitThread();
  return 0;
}


RT_GUEST_FN(0x0028d008, guest::_InitSys)

}  // namespace guest
