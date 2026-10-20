// guest:0x00284af0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _read_r(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  ssize_t sVar1;
  
  (*(int32_t*)rtguest::G(0x33573c)) = 0;
  sVar1 = RT_CALLRA(0x284b18, guest::read(param_2,param_3,param_4));
  if ((sVar1 == -1) && ((*(int32_t*)rtguest::G(0x33573c)) != 0)) {
    (*(int32_t*)rtguest::G((uint32_t)param_1)) = (*(int32_t*)rtguest::G(0x33573c));
  }
  return sVar1;
}


RT_GUEST_FN(0x00284af0, guest::_read_r)

}  // namespace guest
