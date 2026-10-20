// guest:0x00271ae8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void textGridDrawFrame(uint64_t param_1, uint64_t param_2, uint64_t param_3, uint32_t param_4, uint32_t param_5) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x70) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x74) = 0x00000000u;
  }
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  
  uVar4 = (uint32_t)param_2;
  uVar3 = uVar4 + param_4 + -1;
  uVar1 = (uint32_t)param_3;
  uVar2 = uVar1 + param_5 + -1;
  RT_CALLRA(0x271b30, guest::textBufferBlendPixel16(param_1,param_2,param_3,0xf9,7));  // MIPS 0x271b28-30: $a3=0xf9, 5th arg via $t0=7
  RT_CALLRA(0x271b48, guest::textBufferBlendPixel16(param_1,uVar3,param_3,0xf5,7));
  RT_CALLRA(0x271b60, guest::textBufferBlendPixel16(param_1,param_2,uVar2,0xfa,7));
  RT_CALLRA(0x271b78, guest::textBufferBlendPixel16(param_1,uVar3,uVar2,0xf6,7));
  while (uVar4 = uVar4 + 1, uVar4 < uVar3) {
    RT_CALLRA(0x271b9c, guest::textBufferBlendPixel16(param_1,uVar4,param_3,0xfc,7));
    RT_CALLRA(0x271bb8, guest::textBufferBlendPixel16(param_1,uVar4,uVar2,0xfc,7));
  }
  while (uVar1 = uVar1 + 1, uVar1 < uVar2) {
    RT_CALLRA(0x271bf4, guest::textBufferBlendPixel16(param_1,param_2,uVar1,0xf3,7));
    RT_CALLRA(0x271c10, guest::textBufferBlendPixel16(param_1,uVar3,uVar1,0xf3,7));
  }
  return;
}


RT_GUEST_FN(0x00271ae8, guest::textGridDrawFrame)

}  // namespace guest
