// guest:0x002739e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadPortClose(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x20, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
  }
  undefined4 uVar1;
  long lVar2;
  
  uVar1 = 0;
  if ((*(uint32_t*)rtguest::G((uint32_t)0x1773220 + (uint32_t)(param_1 * 0x1c + param_2 * 7) * 4)) != 0) {
    (*(uint32_t*)rtguest::G(0x1773500)) = 0xe;
    (*(uint32_t*)rtguest::G(0x1773510)) = 1;
    (*(uint32_t*)rtguest::G(0x1773504)) = param_1;
    (*(uint32_t*)rtguest::G(0x1773508)) = param_2;
    lVar2 = RT_CALLRA(0x273a68, guest::sceSifCallRpc(0x17731c0,1,0,0x1773500,0x80,0x1773500,0x80,0));
    if (lVar2 < 0) {
      uVar1 = 0;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)0x1773220 + (uint32_t)(param_1 * 0x1c + param_2 * 7) * 4)) = 0;
      uVar1 = (*(uint32_t*)rtguest::G(0x177350c));
    }
  }
  return uVar1;
}


RT_GUEST_FN(0x002739e0, guest::scePadPortClose)

}  // namespace guest
