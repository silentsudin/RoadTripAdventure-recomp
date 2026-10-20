// guest:0x0025c818 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t drawFieldProp06Pickup(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x70) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x74) = 0x00000000u;
  }
  long lVar1;
  float fVar2;
uint32_t auStack_80 = rtguest::allocFrame(48);
uint32_t auStack_50 = rtguest::allocFrame(16);
uint32_t auStack_40 = rtguest::allocFrame(16);
  
  RT_CALLRA(0x25c858, guest::vifFixedRegs(0x200696c5dd30b476,7,0,0xf));
  lVar1 = RT_CALLRA(0x25c868, guest::GetFlaggedSlotCount(0xf,0x22,0));
  if ((lVar1 == 0) && (lVar1 = TestStateBitFc78(9), lVar1 == 0)) {
    RT_CALLRA(0x25c89c, guest::vu0SubVector(auStack_40,0x2c41d0,0x177ace0));
    fVar2 = rtguest::fbits(RT_CALLRA(0x25c8a8, guest::vecDotProductVu0(auStack_40,auStack_40)));
    if (SQRT(fVar2) < 1.5) {
      RT_CALLRA(0x25c8f4, guest::PlayUiSound(0x302));
      RT_CALLRA(0x25c904, guest::SetFlaggedSlotBit(0xf,0x22,0));
    }
    RT_CALLRA(0x25c90c, guest::vu0UnitMatrix(auStack_80));
    RT_CALLRA(0x25c91c, guest::tileSlotOrigin(auStack_50,0x2c41d0,2));
    RT_CALLRA(0x25c924, guest::vifObjectMatrix(auStack_80));
    RT_CALLRA(0x25c934, guest::vifCallModel((*(int32_t*)rtguest::G(0x177f5b0)) + 0x10));
  }
  if (0x2327f < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14))) - 36000U) {
    RT_CALLRA(0x25c960, guest::vu0UnitMatrix(auStack_80));
    RT_CALLRA(0x25c9b4, guest::vu0RotMatrixY(((float)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) % 0x168 + -0xb4) * (*(float*)rtguest::G(0x3330d0))) / 180.0,auStack_80
                  ,auStack_80));
    RT_CALLRA(0x25c9c8, guest::tileSlotOrigin(auStack_50,0x2c41e0,2));
    RT_CALLRA(0x25c9d0, guest::vifObjectMatrix(auStack_80));
    RT_CALLRA(0x25c9e0, guest::vifCallModel((*(uint32_t*)rtguest::G(0x177f5b4)) + 0x10));
  }
  return 0;
}


RT_GUEST_FN(0x0025c818, guest::drawFieldProp06Pickup)

}  // namespace guest
