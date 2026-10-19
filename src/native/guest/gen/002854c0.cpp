// guest:0x002854c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t write(uint32_t __fd, uint32_t __buf, uint32_t __n, ...) {
  ssize_t sVar1;
  long lVar2;
  
  if (__fd - 1U < 2) {
    if ((*(int32_t*)rtguest::G(0x2e913c)) == 0) {
      lVar2 = sceTtyInit(__buf);
      if (lVar2 == 0) goto LAB_00285520;
      (*(int32_t*)rtguest::G(0x2e913c)) = 1;
    }
    sVar1 = sceTtyWrite(__buf,__n);
  }
  else {
LAB_00285520:
    sVar1 = -1;
  }
}


RT_GUEST_FN(0x002854c0, guest::write)

}  // namespace guest
