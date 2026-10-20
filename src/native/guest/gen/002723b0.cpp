// guest:0x002723b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textBuildStringGifPacket(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg;
  byte bVar1;
  uint uVar2;
  uint uVar3;
uint32_t puVar4;
uint32_t aiStack_90 = rtguest::allocFrame(16);
  
  gifPkBufferInit(aiStack_90,param_1);
  gifPkAppendTagNreg1(aiStack_90,0,0,0);
  puVar4 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)aiStack_90 + (uint32_t)(0) * 4)) + -4);
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_5));
  uVar3 = param_2;
  while (uVar2 = (uint32_t)bVar1, uVar2 != 0) {
    param_5 = param_5 + 1;
    if (uVar2 == 10) {
      param_3 = param_3 + 0x60;
      uVar3 = param_2;
    }
    gifEmitBlock8Planes(aiStack_90,(uint32_t)uVar3 | (uint32_t)param_3 << 0x10 | 0xffffffff00000000,0xc,
                        0xffffffff80ffffff,(*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x00330190 + uVar2 * 8))),param_6);
    uVar3 = uVar3 + 0x80;
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_5));
  }
  (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = (uint32_t)(((*(int32_t*)rtguest::G((uint32_t)aiStack_90 + (uint32_t)(0) * 4)) + -4) - (int)puVar4 >> 2) >> 2 | 0x50000000;
  gifPkAppendTagNreg7(aiStack_90,0,0,0);
  return (*(int32_t*)rtguest::G((uint32_t)aiStack_90 + (uint32_t)(0) * 4)) + -0x10;
}


RT_GUEST_FN(0x002723b0, guest::textBuildStringGifPacket)

}  // namespace guest
