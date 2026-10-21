// guest:0x00265128 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawStampIcon(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(176);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x90) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x94) = 0x00000000u;
  }
uint32_t aiStack_b0 = rtguest::allocFrame(32);
    uint32_t fStack_90 = rtguest::allocFrame(4);
    uint32_t fStack_8c = rtguest::allocFrame(4);
    uint32_t uStack_88 = rtguest::allocFrame(4);
    uint32_t uStack_84 = rtguest::allocFrame(4);
    uint32_t fStack_80 = rtguest::allocFrame(4);
    uint32_t fStack_7c = rtguest::allocFrame(4);
    uint32_t uStack_78 = rtguest::allocFrame(4);
    uint32_t uStack_74 = rtguest::allocFrame(4);
    uint32_t fStack_70 = rtguest::allocFrame(4);
    uint32_t fStack_6c = rtguest::allocFrame(4);
    uint32_t uStack_68 = rtguest::allocFrame(4);
    uint32_t uStack_64 = rtguest::allocFrame(4);
    uint32_t fStack_60 = rtguest::allocFrame(4);
    uint32_t fStack_5c = rtguest::allocFrame(4);
    uint32_t uStack_58 = rtguest::allocFrame(4);
    uint32_t uStack_54 = rtguest::allocFrame(4);
uint32_t auStack_50 = rtguest::allocFrame(16);
    uint32_t uStack_40 = rtguest::allocFrame(8);
    uint32_t uStack_38 = rtguest::allocFrame(8);
  undefined4 uVar1;
  float fVar2;
  
  uVar1 = 0x3f800000;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_40)) = (long)(param_2 * 9 + 0x1fe2) | 0x19408000U | (long)(param_2 * 9 + 0x1fea) << 0x25;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_38)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_40)) | 0x2000000d80000000;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_40)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_40)) | 0xd80000000;
  fVar2 = (rtguest::fdiv((float)param_3, 5.0) + 1.0) * 64.0;
  RT_CALLRA(0x2651d8, guest::uiArenaSetTexture(param_1,uStack_40,0,0));
  (*(float*)rtguest::G((uint32_t)fStack_90)) = -fVar2;
  (*(int32_t*)rtguest::G((uint32_t)aiStack_b0 + (uint32_t)(5) * 4)) = 0x40;
  (*(int32_t*)rtguest::G((uint32_t)aiStack_b0 + (uint32_t)(4) * 4)) = 0x40;
  (*(int32_t*)rtguest::G((uint32_t)aiStack_b0 + (uint32_t)(0) * 4)) = (param_2 % 2) * 0x120 + 0xb0;
  (*(int32_t*)rtguest::G((uint32_t)aiStack_b0 + (uint32_t)(1) * 4)) = 0xa6;
  (*(int32_t*)rtguest::G((uint32_t)aiStack_b0 + (uint32_t)(2) * 4)) = 0;
  (*(int32_t*)rtguest::G((uint32_t)aiStack_b0 + (uint32_t)(3) * 4)) = 0;
  (*(int32_t*)rtguest::G((uint32_t)aiStack_b0 + (uint32_t)(6) * 4)) = 0;
  (*(int32_t*)rtguest::G((uint32_t)aiStack_b0 + (uint32_t)(7) * 4)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_78)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_68)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_58)) = 0;
  (*(float*)rtguest::G((uint32_t)fStack_8c)) = (*(float*)rtguest::G((uint32_t)fStack_90));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_84)) = uVar1;
  (*(float*)rtguest::G((uint32_t)fStack_80)) = fVar2;
  (*(float*)rtguest::G((uint32_t)fStack_7c)) = (*(float*)rtguest::G((uint32_t)fStack_90));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) = uVar1;
  (*(float*)rtguest::G((uint32_t)fStack_70)) = (*(float*)rtguest::G((uint32_t)fStack_90));
  (*(float*)rtguest::G((uint32_t)fStack_6c)) = fVar2;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = uVar1;
  (*(float*)rtguest::G((uint32_t)fStack_60)) = fVar2;
  (*(float*)rtguest::G((uint32_t)fStack_5c)) = fVar2;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_54)) = uVar1;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_50)), 0, 0x10));
  RT_CALLRA(0x265294, guest::uiArenaRotQuads(param_1,aiStack_b0,1,0,0,0));
  return 0;
}


RT_GUEST_FN(0x00265128, guest::drawStampIcon)

}  // namespace guest
