// guest:0x002859c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t disableIntc(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  undefined8 uVar1;
  uint uVar2;
  
  uVar2 = Status & 0x10000;
  if (uVar2 != 0) {
    RT_CALLRA(0x2859f0, guest::DIntr());
  }
  uVar1 = RT_CALLRA(0x2859f8, guest::_DisableIntc(param_1));
  SYNC(0);
  if (uVar2 != 0) {
    RT_CALLRA(0x285a10, guest::EIntr());
  }
  return uVar1;
}


RT_GUEST_FN(0x002859c8, guest::disableIntc)

}  // namespace guest
