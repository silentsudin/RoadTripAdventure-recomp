// guest:0x00284920 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _calloc_r(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  long lVar1;
uint32_t puVar2;
  ulong __n;
uint32_t __s;
  
  lVar1 = _malloc_r(param_1,param_2 * param_3);
  if (lVar1 != 0) {
    __s = (uint32_t)lVar1;
    __n = (uint32_t)(int)(((*(uint32_t*)rtguest::G((uint32_t)__s + (uint32_t)(-1) * sizeof(uint32_t))) & 0xfffffffc) - 4);
    if (__n < 0x25) {
      puVar2 = __s;
      if (0x13 < __n) {
        (*(uint32_t*)rtguest::G((uint32_t)__s)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)__s + (uint32_t)(1) * sizeof(uint32_t))) = 0;
        puVar2 = __s + 2;
        if (0x1b < __n) {
          (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0;
          puVar2 = __s + 4;
          (*(uint32_t*)rtguest::G((uint32_t)__s + (uint32_t)(3) * sizeof(uint32_t))) = 0;
          if (0x23 < __n) {
            (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0;
            puVar2 = __s + 6;
            (*(uint32_t*)rtguest::G((uint32_t)__s + (uint32_t)(5) * sizeof(uint32_t))) = 0;
          }
        }
      }
      (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint32_t))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
    }
    else {
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, __n));
    }
  }
  return lVar1;
}


RT_GUEST_FN(0x00284920, guest::_calloc_r)

}  // namespace guest
