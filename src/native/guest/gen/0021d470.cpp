// guest:0x0021d470 translated from Ghidra pseudocode and VU0 disassembly.
#include "../guest_vu0.h"
#include "guest_decls.inc"
#include <cstdarg>

namespace guest {
uint32_t vu0TransformVerts(uint32_t param_1, ...) {
  va_list ap; va_start(ap, param_1);
  uint32_t param_2 = va_arg(ap, uint32_t), param_3 = va_arg(ap, uint32_t), param_4 = va_arg(ap, uint32_t); va_end(ap);
  using namespace rtguest::vu0;
  uint32_t uVar4 = 0;
  do {
    S.vf[8]=lqc2(param_2); S.vf[24]=lqc2(param_2+16); S.vf[26]=lqc2(param_2+32); S.vf[27]=lqc2(param_2+48);
    vmulabc(15,0,4,8); vmaddabc(15,1,5,8); vmaddabc(15,2,6,8); vmaddbc(15,3,12,7,8);
    vdiv(3,0,3,12); --param_3;
    vmulabc(15,0,17,24); vmaddabc(15,1,18,24); vmaddbc(15,2,24,19,24); vmaxbc(14,0,24,24,0);
    vwaitq(); vmulq(14,12,12); vftoi4(15,13,12); ctc2(16,0);
    // VU0 ignores the vf0 destinations at 0x21d4c4/0x21d4c8.
    vior(10,0,0);
    const uint32_t index = uVar4 & 3; uVar4 = index << 1;
    if ((cfc2(16) & 0xc0) == 0) {
      if (index != 0) { vmfir(1,13,1); vior(10,1,1); if (param_3 == 0) goto store; }
      vsub(1,12,15,12); vminibc(1,1,12,12,14); vftoi4(1,13,12); vmtir(13,3,13); vior(13,13,10); vmfir(1,13,13);
      vmulabc(15,0,21,24); vmaddabc(15,1,22,24); vmaddabc(15,2,23,24); vmaddbc(15,3,24,20,0);
      vmul(14,26,24,26); vminibc(14,1,26,26,14); vmulq(14,28,27); vftoi0(15,26,26);
    } else { uVar4 |= 1; vmfir(1,13,1); }
store:
    sqc2(param_1,S.vf[28]); sqc2(param_1+16,S.vf[26]); sqc2(param_1+32,S.vf[13]);
    param_1 += 48; param_2 += param_4 ? 64 : 48;
  } while (param_3 != 0);
  return 0;
}
RT_GUEST_FN(0x0021d470, guest::vu0TransformVerts)
}  // namespace guest
