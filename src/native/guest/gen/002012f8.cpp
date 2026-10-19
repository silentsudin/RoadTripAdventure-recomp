// guest:0x002012f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiKanjiGlyphUV(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  undefined2 uVar1;
  
  (*(int16_t*)rtguest::G((uint32_t)param_2)) = param_4;
  if (param_3 == 0x8fe3) {
    uVar1 = 0x3c;
  }
  else if (param_3 < 0x8fe4) {
    if (param_3 == 0x8bf3) {
      uVar1 = 0x50;
    }
    else if (param_3 < 0x8bf4) {
      if (param_3 == 0x88c8) {
        uVar1 = 0x28;
      }
      else {
        if (param_3 != 0x8af9) {
LAB_00201410:
          (*(uint16_t*)rtguest::G((uint32_t)param_1)) = 0xb4;
          (*(int16_t*)rtguest::G((uint32_t)param_2)) = 0xa0;
          return 0;
        }
        uVar1 = 0xb4;
      }
    }
    else if (param_3 == 0x8cfc) {
      uVar1 = 0x14;
    }
    else {
      if (param_3 != 0x8ea9) goto LAB_00201410;
      uVar1 = 200;
    }
  }
  else if (param_3 == 0x96a2) {
    uVar1 = 0x8c;
  }
  else if (param_3 < 0x96a3) {
    if (param_3 != 0x937b) {
      if (param_3 != 0x95fb) goto LAB_00201410;
      (*(uint16_t*)rtguest::G((uint32_t)param_1)) = 0;
      goto LAB_00201400;
    }
    uVar1 = 0xa0;
  }
  else if (param_3 == 0x9765) {
    uVar1 = 100;
  }
  else {
    if (param_3 != 0x97ca) goto LAB_00201410;
    uVar1 = 0x78;
  }
  (*(uint16_t*)rtguest::G((uint32_t)param_1)) = uVar1;
LAB_00201400:
  (*(int16_t*)rtguest::G((uint32_t)param_2)) = (*(int16_t*)rtguest::G((uint32_t)param_2)) + 0x140;
  return 0;
}


RT_GUEST_FN(0x002012f8, guest::uiKanjiGlyphUV)

}  // namespace guest
