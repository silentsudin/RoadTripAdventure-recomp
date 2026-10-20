// guest:0x0022c280 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RenderShowcaseView(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t auStack_80 = rtguest::allocFrame(64);
  
  vu0UnitMatrix(0x17525d0);
  vu0RotMatrixY((*(uint32_t*)rtguest::G(0x29f36c)),0x17525d0,0x17525d0);
  vu0CopyVectorXYZ(0x1752600,0x29f360);
  (*(uint32_t*)rtguest::G(0x1752658)) = 0;
  (*(uint32_t*)rtguest::G(0x1752654)) = 0;
  (*(uint32_t*)rtguest::G(0x1752650)) = 0;
  vu0UnitMatrix(0x1752610);
  (*(uint32_t*)rtguest::G(0x1752644)) = (*(uint32_t*)rtguest::G(0x332e24));
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)0x177ea50 + (uint32_t)((int)param_3) * 4)) + 8))) & 0x400) == 0) {
    (*(uint32_t*)rtguest::G(0x1752644)) = 0;
  }
  UpdateSceneLightsAndFog(1,0);
  vu0UnitMatrix(auStack_80);
  SetViewDistanceGlobals(0x45000000,0x45000000);
  buildCamera(0x177e6d0,0x29f3d0,auStack_80,auStack_80);
  updateLightDirection(0x177e850);
  vifUploadAt4(0x177e6d0);
  ResetShowroomLighting();
  DrawCarVariant0(0x17525d0,param_3);
  return 0;
}


RT_GUEST_FN(0x0022c280, guest::RenderShowcaseView)

}  // namespace guest
