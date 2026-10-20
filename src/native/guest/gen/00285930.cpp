// guest:0x00285930 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifInvalidateDCache(uint64_t param_1, uint64_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
  }
  uint uVar1;
  
  uVar1 = Status & 0x10000;
  if (uVar1 != 0) {
    RT_CALLRA(0x285960, guest::DIntr());
  }
  RT_CALLRA(0x285974, guest::_sceIDC(param_1 & 0xffffffffffffffc0,param_2 & 0xffffffffffffffc0));
  if (uVar1 != 0) {
    EIntr();
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00285930, guest::sceSifInvalidateDCache)

}  // namespace guest
