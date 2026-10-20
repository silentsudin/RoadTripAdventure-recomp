// guest:0x00226610 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vifSetBatchOffset(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  bool bVar1;
uint32_t puVar2;
  
  if ((*(uint32_t*)rtguest::G(0x3356ec)) == (uint32_t)0x0) {
    RT_CALLRA(0x22662c, guest::fatal(0x2eafa0));
  }
  if ((*(uint32_t*)rtguest::G(0x3356f4)) == 0) {
    if ((*(uint32_t*)rtguest::G(0x3356ec)) + 8 * sizeof(uint32_t) < (uint32_t)0x17523d0) {
      bVar1 = false;
    }
    else {
      RT_CALLRA(0x226660, guest::fatal(0x2eafc0));
      bVar1 = false;
    }
  }
  else {
    bVar1 = true;
    if ((*(uint32_t*)rtguest::G(0x3356ec)) + 8 * sizeof(uint32_t) < (uint32_t)0x1750bd0) {
      bVar1 = false;
    }
  }
  if (!bVar1) {
    puVar2 = (*(uint32_t*)rtguest::G(0x3356ec)) + 4 * sizeof(uint32_t);
    (*(uint32_t*)rtguest::G((*(uint32_t*)rtguest::G(0x3356ec)))) = 0x10000001;
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(2) * 4)) = 0x10000000;
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(3) * 4)) = 0x6c01001a;
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(1) * 4)) = 0;
    (*(uint32_t*)rtguest::G(0x3356ec)) = puVar2;
    RT_CALLRA(0x2266c4, guest::vu0CopyVector(puVar2,param_1));
    (*(uint32_t*)rtguest::G(0x3356ec)) = (*(uint32_t*)rtguest::G(0x3356ec)) + 4 * sizeof(uint32_t);
  }
  return 0;
}


RT_GUEST_FN(0x00226610, guest::vifSetBatchOffset)

}  // namespace guest
