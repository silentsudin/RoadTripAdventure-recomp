// guest:0x002750e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __ieee754_atan2f(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  uint uVar2;
uint32_t puVar3;
  undefined4 uVar4;
  
  if (param_2 == 1.0) {
    puVar3 = (uint32_t)atanf();
  }
  uVar2 = (uint32_t)param_1 >> 0x1f | (int)param_2 >> 0x1e & 2U;
  if ((uint32_t)ABS((float)param_1) < 0x800000) {
    if (uVar2 == 2) {
      return (uint32_t)0x40490fda;
    }
    if (uVar2 < 3) {
    }
    if (uVar2 == 3) {
      return (uint32_t)0xc0490fda;
    }
  }
  if ((uint32_t)ABS(param_2) < 0x800000) {
    puVar3 = 0xbfc90fdb;
    if (-1 < (int)param_1) {
      puVar3 = (uint32_t)0x3fc90fdb;
    }
  }
  else {
    puVar3 = (uint32_t)0x3fc90fdc;
    iVar1 = (int)ABS((float)param_1) - (int)ABS(param_2) >> 0x17;
    if ((iVar1 < 0x3d) && ((-1 < (int)param_2 || (puVar3 = (uint32_t)0x0, -0x3d < iVar1)))) {
      uVar4 = fabsf((float)param_1 / param_2);
      puVar3 = (uint32_t)atanf(uVar4);
    }
    if (uVar2 == 1) {
      puVar3 = (uint32_t)((uint32_t)puVar3 ^ 0x80000000);
    }
    else {
      if (uVar2 < 2) {
        if (uVar2 == 0) {
        }
      }
      else if (uVar2 == 2) {
        return (uint32_t)(3.1415925 - ((float)puVar3 - 1.5099579e-07));
      }
      puVar3 = (uint32_t)(((float)puVar3 - 1.5099579e-07) - 3.1415925);
    }
  }
}


RT_GUEST_FN(0x002750e0, guest::__ieee754_atan2f)

}  // namespace guest
