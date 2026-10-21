// guest:0x00277af8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scalbnf(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0x8);
  }
  int iVar1;
  float fVar2;
  
  iVar1 = ((param_1 & 0x7fffffffu) >> 0x17) + (int)param_2;  // MIPS and $2,$6,0x7fffffff @0x277b14: ABS on the f12 bits
  if (0x7fffff < (param_1 & 0x7fffffffu)) {
    if (iVar1 < 0x100) {
      if (0 < iVar1) {
        return (param_1 & 0x807fffffu) | ((uint32_t)iVar1 << 23);  // mtc1 $2,$f12 @0x277b6c: result bits
      }
      if ((int)param_2 < 0xc351) {  // MIPS slt (signed) @0x277b78
        fVar2 = 1e-30f;
      }
      else {
        fVar2 = 1e+30f;
      }
    }
    else {
      fVar2 = 1e+30f;
    }
    param_1 = rtguest::f2bits(rtguest::fbits(RT_CALLRA(0x277ba4, guest::copysignf(rtguest::f2bits(fVar2),param_1))) * fVar2);  // copysignf ends mtc1 $2,$f0 / jr ra (0x277c38): float bits; param_1 is a bits param
  }
  return param_1;
}


RT_GUEST_FN(0x00277af8, guest::scalbnf)

}  // namespace guest
