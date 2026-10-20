// guest:0x00226a18 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vifUploadColours(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  bool bVar1;
uint32_t puVar2;
  
  if ((*(uint32_t*)rtguest::G(0x3356ec)) == (uint32_t)0x0) {
    RT_CALLRA(0x226a3c, guest::fatal(0x2eafa0));
  }
  if ((*(uint32_t*)rtguest::G(0x3356f4)) == 0) {
    if ((*(uint32_t*)rtguest::G(0x3356ec)) + 0xc * sizeof(uint32_t) < (uint32_t)0x17523d0) {
      bVar1 = false;
    }
    else {
      RT_CALLRA(0x226a70, guest::fatal(0x2eafc0));
      bVar1 = false;
    }
  }
  else {
    bVar1 = true;
    if ((*(uint32_t*)rtguest::G(0x3356ec)) + 0xc * sizeof(uint32_t) < (uint32_t)0x1750bd0) {
      bVar1 = false;
    }
  }
  if (!bVar1) {
    puVar2 = (*(uint32_t*)rtguest::G(0x3356ec)) + 4 * sizeof(uint32_t);
    (*(uint32_t*)rtguest::G((*(uint32_t*)rtguest::G(0x3356ec)))) = 0x10000002;
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(2) * 4)) = 0x10000000;
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(3) * 4)) = 0x6c020018;
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(1) * 4)) = 0;
    (*(uint32_t*)rtguest::G(0x3356ec)) = puVar2;
    RT_CALLRA(0x226ad4, guest::vu0CopyVector(puVar2,param_1));
    (*(uint32_t*)rtguest::G(0x3356ec)) = (*(uint32_t*)rtguest::G(0x3356ec)) + 4 * sizeof(uint32_t);
    RT_CALLRA(0x226aec, guest::vu0CopyVector((*(uint32_t*)rtguest::G(0x3356ec)),param_2));
    (*(uint32_t*)rtguest::G(0x3356ec)) = (*(uint32_t*)rtguest::G(0x3356ec)) + 4 * sizeof(uint32_t);
  }
  return 0;
}


RT_GUEST_FN(0x00226a18, guest::vifUploadColours)

}  // namespace guest
