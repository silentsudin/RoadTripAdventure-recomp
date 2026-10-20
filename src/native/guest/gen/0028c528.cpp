// guest:0x0028c528 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifUnloadModule(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
  }
  undefined4 uVar1;
  long lVar2;
  
  lVar2 = RT_CALLRA(0x28c538, guest::_lf_bind());
  uVar1 = 0xffff0000;
  if (-1 < lVar2) {
    lVar2 = RT_CALLRA(0x28c548, guest::lfVersionCheck());
    if (lVar2 == 0) {
      (*(uint32_t*)rtguest::G(0x1779e00)) = param_1;
      lVar2 = RT_CALLRA(0x28c58c, guest::sceSifCallRpc(0x177a000,8,0,0x1779e00,4,0x1779e00,4,0));
      uVar1 = (*(uint32_t*)rtguest::G(0x1779e00));
      if (lVar2 < 0) {
        uVar1 = 0xfffeffff;
      }
    }
    else {
      uVar1 = 0xfffefffc;
    }
  }
  return uVar1;
}


RT_GUEST_FN(0x0028c528, guest::sceSifUnloadModule)

}  // namespace guest
