// guest:0x00285b00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t enableDmac(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  undefined8 uVar1;
  uint uVar2;
  
  uVar2 = Status & 0x10000;
  if (uVar2 != 0) {
    RT_CALLRA(0x285b28, guest::DIntr());
  }
  uVar1 = RT_CALLRA(0x285b30, guest::_EnableDmac(param_1));
  SYNC(0);
  if (uVar2 != 0) {
    RT_CALLRA(0x285b48, guest::EIntr());
  }
  return uVar1;
}


RT_GUEST_FN(0x00285b00, guest::enableDmac)

}  // namespace guest
