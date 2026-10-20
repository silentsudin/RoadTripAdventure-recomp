// guest:0x00223588 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawCarShadow(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
    uint32_t uStack_80 = rtguest::allocFrame(4);
    uint32_t uStack_7c = rtguest::allocFrame(4);
    uint32_t uStack_78 = rtguest::allocFrame(4);
    uint32_t uStack_74 = rtguest::allocFrame(4);
    uint32_t uStack_70 = rtguest::allocFrame(4);
    uint32_t uStack_6c = rtguest::allocFrame(4);
    uint32_t uStack_68 = rtguest::allocFrame(4);
    uint32_t uStack_64 = rtguest::allocFrame(4);
uint32_t auStack_60 = rtguest::allocFrame(4);
    uint32_t uStack_5c = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t uStack_4c = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t uStack_3c = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t uStack_2c = rtguest::allocFrame(4);
  
  (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x34)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 4)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_68)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_78)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) = 0x3f000000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_80)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_7c)) = 0x3f800000;
  vifUploadColours(uStack_80,uStack_70);
  vu0CopyMatrix(auStack_60,param_1);
  (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_3c)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_4c)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = 0;
  vifObjectMatrix(auStack_60);
  vifBatchRegs((long)(param_3 * 0x44 + 0x243e) | 0x1d308000U |
               (long)(param_3 * 0x44 + 0x247e) << 0x25 | 0x20000005c0000000,7,0x1fc401fc40a,9);
  vifCallModel(0x290570);
  return 0;
}


RT_GUEST_FN(0x00223588, guest::drawCarShadow)

}  // namespace guest
