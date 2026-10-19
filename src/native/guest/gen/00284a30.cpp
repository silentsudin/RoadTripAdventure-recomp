// guest:0x00284a30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _fstat_r(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  long lVar1;
  
  (*(int32_t*)rtguest::G(0x33573c)) = 0;
  lVar1 = sysFstat(param_2,param_3);
  if ((lVar1 == -1) && ((*(int32_t*)rtguest::G(0x33573c)) != 0)) {
    (*(int32_t*)rtguest::G((uint32_t)param_1)) = (*(int32_t*)rtguest::G(0x33573c));
  }
  return 0;
}


RT_GUEST_FN(0x00284a30, guest::_fstat_r)

}  // namespace guest
