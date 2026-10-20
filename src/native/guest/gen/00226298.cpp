// guest:0x00226298 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t TransformMatrixAndSubmit(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
  }
uint32_t auStack_50 = rtguest::allocFrame(64);
  
  RT_CALLRA(0x2262ac, guest::vu0MulMatrix(auStack_50,0x177f5e0,param_1));
  RT_CALLRA(0x2262b4, guest::vu0StoreMatrixQuadsAt4(auStack_50));
  return 0;
}


RT_GUEST_FN(0x00226298, guest::TransformMatrixAndSubmit)

}  // namespace guest
