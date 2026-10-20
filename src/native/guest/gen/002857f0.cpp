// guest:0x002857f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifWriteBackDCache(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
  }
  uint uVar1;
  
  uVar1 = Status & 0x10000;
  if (uVar1 != 0) {
    RT_CALLRA(0x285820, guest::DIntr());
  }
  RT_CALLRA(0x285834, guest::_sceSDC(param_1 & 0xffffffffffffffc0,param_2 & 0xffffffffffffffc0));
  if (uVar1 != 0) {
    EIntr();
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x002857f0, guest::sceSifWriteBackDCache)

}  // namespace guest
