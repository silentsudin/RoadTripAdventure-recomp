// guest:0x0026e3f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsResetPath(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
  undefined8 uVar1;
  undefined4 in_vc12;
  
  REG_VIF1_FBRST = 1;
  REG_VIF1_ERR = 2;
  SYNC(0);
  uVar1 = _cfc2(in_vc12);
  _ctc2((uint32_t)uVar1 | 0x200);
  SYNC(0x10);
  REG_VIF1_FIFO = (*(uint32_t*)rtguest::G(0x2dcda0));
  (*(uint32_t*)rtguest::G(0x10005004)) = (*(uint32_t*)rtguest::G(0x2dcda4));
  (*(uint32_t*)rtguest::G(0x10005008)) = (*(uint32_t*)rtguest::G(0x2dcda8));
  (*(uint32_t*)rtguest::G(0x1000500c)) = (*(uint32_t*)rtguest::G(0x2dcdac));
  REG_GIF_CTRL = 1;
  return 0;
}


RT_GUEST_FN(0x0026e3f0, guest::sceGsResetPath)

}  // namespace guest
