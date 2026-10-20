// guest:0x00226f70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vifDirect(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  bool bVar1;
  
  if ((*(uint32_t*)rtguest::G(0x3356ec)) == (uint32_t)0x0) {
    fatal(0x2eafa0);
  }
  if ((*(uint32_t*)rtguest::G(0x3356f4)) == 0) {
    if ((*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(4) * sizeof(uint) < (uint32_t)0x17523d0) {
      bVar1 = false;
    }
    else {
      fatal(0x2eafc0);
      bVar1 = false;
    }
  }
  else {
    bVar1 = true;
    if ((*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(4) * sizeof(uint) < (uint32_t)0x1750bd0) {
      bVar1 = false;
    }
  }
  if (!bVar1) {
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)))) = param_2 | 0x30000000;
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(1) * 4)) = param_1 & 0xfffffff;
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(2) * 4)) = 0x11000000;
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(3) * 4)) = param_2 | 0x50000000;
    ( *(uint32_t*)rtguest::G(0x3356ec)) = (*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(4) * sizeof(uint);
  }
  return 0;
}


RT_GUEST_FN(0x00226f70, guest::vifDirect)

}  // namespace guest
