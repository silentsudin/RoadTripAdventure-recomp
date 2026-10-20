// guest:0x0027c1b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t decode_sleb128(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  byte bVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  
  uVar3 = 0;
  uVar4 = 0;
  do {
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_1));
    param_1 = param_1 + 1;
    uVar2 = uVar3 & 0x1f;
    uVar3 = uVar3 + 7;
    uVar4 = uVar4 | (bVar1 & 0x7f) << uVar2;
  } while ((bVar1 & 0x80) != 0);
  if (uVar3 < 0x20) {
    if ((bVar1 & 0x40) == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)param_2)) = uVar4;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)param_2)) = uVar4 | -1 << (uVar3 & 0x1f);
    }
  }
  else {
    (*(uint32_t*)rtguest::G((uint32_t)param_2)) = uVar4;
  }
  return param_1;
}


RT_GUEST_FN(0x0027c1b0, guest::decode_sleb128)

}  // namespace guest
