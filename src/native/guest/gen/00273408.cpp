// guest:0x00273408 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceVpu0Reset(uint32_t q0, ...) {
  undefined8 uVar1;
  undefined4 in_vc12;
  
  REG_VIF0_MARK = 0;
  REG_VIF0_ERR = 0;
  REG_VIF0_FBRST = 1;
  uVar1 = _cfc2(in_vc12);
  _ctc2((uint32_t)uVar1 | 2);
  SYNC(0x10);
  REG_VIF0_FIFO = (*(uint32_t*)rtguest::G(0x2e6ea0));
  (*(uint32_t*)rtguest::G(0x10004004)) = (*(uint32_t*)rtguest::G(0x2e6ea4));
  (*(uint32_t*)rtguest::G(0x10004008)) = (*(uint32_t*)rtguest::G(0x2e6ea8));
  (*(uint32_t*)rtguest::G(0x1000400c)) = (*(uint32_t*)rtguest::G(0x2e6eac));
  return 0;
}


RT_GUEST_FN(0x00273408, guest::sceVpu0Reset)

}  // namespace guest
