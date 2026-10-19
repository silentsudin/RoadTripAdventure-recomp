// guest:0x002848c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _write_r(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  ssize_t sVar1;
  
  (*(int32_t*)rtguest::G(0x33573c)) = 0;
  sVar1 = write(param_2,param_3,param_4);
  if ((sVar1 == -1) && ((*(int32_t*)rtguest::G(0x33573c)) != 0)) {
    (*(int32_t*)rtguest::G((uint32_t)param_1)) = (*(int32_t*)rtguest::G(0x33573c));
  }
}


RT_GUEST_FN(0x002848c0, guest::_write_r)

}  // namespace guest
