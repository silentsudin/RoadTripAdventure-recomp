// guest:0x0022c280 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t RenderShowcaseView(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x58, 8);
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0x70);
    *(uint32_t*)rtguest::G(rt_sv+0x68) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x6c) = 0x00000000u;
    rtguest::gS[0] = 0x00000000017525d0ull;  // walk const
    rtguest::gS[1] = 0x000000000029f320ull;  // walk const
    rtguest::gS[2] = 0x000000000177e6d0ull;  // walk const
    rtguest::gS[4] = 0x0000000001750000ull;  // walk const
  }
uint32_t auStack_80 = rtguest::allocFrame(64);
  
  RT_CALLRA(0x22c2bc, guest::vu0UnitMatrix(0x17525d0));
  RT_CALLRA(0x22c2d0, guest::vu0RotMatrixY((*(uint32_t*)rtguest::G(0x29f36c)),0x17525d0,0x17525d0));
  RT_CALLRA(0x22c2dc, guest::vu0CopyVectorXYZ(0x1752600,0x29f360));
  (*(uint32_t*)rtguest::G(0x1752658)) = 0;
  (*(uint32_t*)rtguest::G(0x1752654)) = 0;
  (*(uint32_t*)rtguest::G(0x1752650)) = 0;
  RT_CALLRA(0x22c2f4, guest::vu0UnitMatrix(0x1752610));
  (*(uint32_t*)rtguest::G(0x1752644)) = (*(uint32_t*)rtguest::G(0x332e24));
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)0x177ea50 + (uint32_t)((int)param_3) * 4)) + 8))) & 0x400) == 0) {
    (*(uint32_t*)rtguest::G(0x1752644)) = 0;
  }
  RT_CALLRA(0x22c328, guest::UpdateSceneLightsAndFog(1,0));
  RT_CALLRA(0x22c330, guest::vu0UnitMatrix(auStack_80));
  RT_CALLRA(0x22c340, guest::SetViewDistanceGlobals(0x45000000,0x45000000));
  RT_CALLRA(0x22c358, guest::buildCamera(0x177e6d0,0x29f3d0,auStack_80,auStack_80));
  RT_CALLRA(0x22c360, guest::updateLightDirection(0x177e850));
  RT_CALLRA(0x22c368, guest::vifUploadAt4(0x177e6d0));
  RT_CALLRA(0x22c370, guest::ResetShowroomLighting());
  RT_CALLRA(0x22c37c, guest::DrawCarVariant0(0x17525d0,param_3));
  return 0;
}


RT_GUEST_FN(0x0022c280, guest::RenderShowcaseView)

}  // namespace guest
