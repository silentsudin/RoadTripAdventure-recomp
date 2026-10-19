// guest:0x002750e0 translated from Ghidra pseudocode and ELF disassembly.
// Params carry f-register bits (0x2750e4 mov.s $f1,$f12 = y; 0x2750f0 mfc1 $6,$f13
// = x; this toolchain packs float args sequentially into f12,f13).
// (uint)float reinterprets are mfc1/and 0x7fffffff bit ops; result values
// return as f-register bits.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __ieee754_atan2f(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  uint32_t uVar2;
  uint32_t puVar3;
  uint32_t uVar4;

  float y = rtguest::fbits(param_1);
  uint32_t hy = param_1 & 0x7fffffffu;
  uint32_t hx = param_2 & 0x7fffffffu;

  if (rtguest::fbits(param_2) == 1.0f) {  // 0x275108 bne $6,0x3f800000
    return atanf(param_1);
  }
  uVar2 = (param_1 >> 0x1f) | ((param_2 >> 0x1f) << 1);
  if (hy < 0x800000u) {
    if (uVar2 == 2) {
      return 0x40490fda;
    }
    if (uVar2 < 3) {
      return param_1;
    }
    if (uVar2 == 3) {
      return 0xc0490fda;
    }
  }
  if (hx < 0x800000u) {
    puVar3 = 0xbfc90fdb;
    if (-1 < (int32_t)param_1) {  // sign bit of y
      puVar3 = 0x3fc90fdb;
    }
  }
  else {
    puVar3 = 0x3fc90fdcu;
    iVar1 = (int32_t)(hy - hx) >> 0x17;
    if ((iVar1 < 0x3d) && ((-1 < (int32_t)param_2 || (puVar3 = 0, -0x3d < iVar1)))) {
      uVar4 = fabsf(rtguest::f2bits(y / rtguest::fbits(param_2)));
      puVar3 = atanf(uVar4);
    }
    if (uVar2 == 1) {
      puVar3 = puVar3 ^ 0x80000000u;
    }
    else {
      if (uVar2 < 2) {
        if (uVar2 == 0) {
          return puVar3;
        }
      }
      else if (uVar2 == 2) {
        return rtguest::f2bits(3.1415925f - (rtguest::fbits(puVar3) - 1.5099579e-07f));
      }
      puVar3 = rtguest::f2bits((rtguest::fbits(puVar3) - 1.5099579e-07f) - 3.1415925f);
    }
  }
  return puVar3;
}


RT_GUEST_FN(0x002750e0, guest::__ieee754_atan2f)

}  // namespace guest
