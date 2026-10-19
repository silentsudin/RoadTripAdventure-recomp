// guest:0x00285a30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t enableIntc(uint32_t param_1, ...) {
  undefined8 uVar1;
  uint uVar2;
  
  uVar2 = Status & 0x10000;
  if (uVar2 != 0) {
    DIntr();
  }
  uVar1 = _EnableIntc(param_1);
  SYNC(0);
  if (uVar2 != 0) {
    EIntr();
  }
}


RT_GUEST_FN(0x00285a30, guest::enableIntc)

}  // namespace guest
