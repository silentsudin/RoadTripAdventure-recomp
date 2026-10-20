// guest:0x0028bf50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _lf_bind(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
  }
  undefined4 uVar1;
  int iVar2;
  long lVar3;
  
  if ((*(int32_t*)rtguest::G(0x2e99a0)) < 0) {
    while (lVar3 = sceSifBindRpc(0x177a000,0xffffffff80000006,0), -1 < lVar3) {
      if ((*(uint32_t*)rtguest::G(0x177a024)) != 0) {
        (*(uint32_t*)rtguest::G(0x2e99a0)) = 0;
        lVar3 = RT_CALLRA(0x28bfc8, guest::sceSifCallRpc(0x177a000,0xff,0,0,0,0x1779e00,4,0));
        if (lVar3 < 0) {
          return 0xfffeffff;
        }
        (*(uint32_t*)rtguest::G(0x177a028)) = (*(uint32_t*)rtguest::G(0x1779e00));
        return 0;
      }
      iVar2 = 0x100000;
      do {
        iVar2 = iVar2 + -1;
      } while (iVar2 != -1);
    }
    uVar1 = 0xffffffff;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}


RT_GUEST_FN(0x0028bf50, guest::_lf_bind)

}  // namespace guest
