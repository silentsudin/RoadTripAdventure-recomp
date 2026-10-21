// guest:0x002084d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t TestMeshIntersection(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
    uint32_t fStack_30 = rtguest::allocFrame(4);
    uint32_t uStack_2c = rtguest::allocFrame(4);
  int iVar1;
  long lVar2;
uint32_t pfVar3;
uint32_t afStack_28 = rtguest::allocFrame(8);
  
  pfVar3 = (uint32_t)param_1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) = (*(uint32_t*)rtguest::G(0x332b24));
  (*(float*)rtguest::G((uint32_t)afStack_28 + (uint32_t)(0) * 4)) = 0.0;
  (*(float*)rtguest::G((uint32_t)fStack_30)) = (*(float*)rtguest::G(0x332b28));
  iVar1 = ((int)(rtguest::fdiv((*(float*)rtguest::G((uint32_t)pfVar3)), 100.0)) & 0xfU) + ((int)(rtguest::fdiv((*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(2) * sizeof(float))), 100.0)) & 0xfU) * 0x10;
  lVar2 = RT_CALLRA(0x208578, guest::IntersectPointWithMesh((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 * 4 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_5 + 0x78)))))),param_1,param_3,
                     (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 * 2 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_5 + 0x78))) + 0x400))),fStack_30,
                     uStack_2c,afStack_28,0xffffffffffffffff));
  if (-1 < lVar2) {
    (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(1) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)fStack_30));
    (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(3) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)afStack_28 + (uint32_t)(0) * 4));
  }
  (*(uint32_t*)rtguest::G((uint32_t)param_2)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_2c));
  return 0;
}


RT_GUEST_FN(0x002084d0, guest::TestMeshIntersection)

}  // namespace guest
