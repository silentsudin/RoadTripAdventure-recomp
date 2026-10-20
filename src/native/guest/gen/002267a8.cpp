// guest:0x002267a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vifUploadAt16(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  bool bVar1;
  
  if ((*(uint32_t*)rtguest::G(0x3356ec)) == (uint32_t)0x0) {
    fatal(0x2eafa0);
  }
  if ((*(uint32_t*)rtguest::G(0x3356f4)) == 0) {
    if ((*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(4) * sizeof(undefined4) < (uint32_t)0x17523d0) {
      bVar1 = false;
    }
    else {
      fatal(0x2eafc0);
      bVar1 = false;
    }
  }
  else {
    bVar1 = true;
    if ((*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(4) * sizeof(undefined4) < (uint32_t)0x1750bd0) {
      bVar1 = false;
    }
  }
  if (!bVar1) {
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)))) = 0x30000008;
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(1) * 4)) = param_1;
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(2) * 4)) = 0x10000000;
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(3) * 4)) = 0x6c080010;
    ( *(uint32_t*)rtguest::G(0x3356ec)) = (*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(4) * sizeof(undefined4);
  }
  return 0;
}


RT_GUEST_FN(0x002267a8, guest::vifUploadAt16)

}  // namespace guest
