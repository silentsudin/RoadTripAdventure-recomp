// guest:0x0022cc38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawModelAt177F358(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x48, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x50) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x54) = 0x00000000u;
    rtguest::gS[0] = 0x000000000177e6d0ull;  // walk const
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  undefined8 uVar1;
uint32_t auStack_60 = rtguest::allocFrame(64);
  
  RT_CALLRA(0x22cc54, guest::vifBeginPacket(1));
  RT_CALLRA(0x22cc5c, guest::ResetShowroomLighting());
  RT_CALLRA(0x22cc64, guest::vifUploadAt4(0x177e6d0));
  RT_CALLRA(0x22cc74, guest::vu0UnitMatrix(auStack_60));
  RT_CALLRA(0x22cc7c, guest::vifObjectMatrix(auStack_60));
  RT_CALLRA(0x22cca4, guest::vifBatchRegs(0x20045645e53221b2,7,0,0xf));
  RT_CALLRA(0x22ccb0, guest::vifCallModel((*(int32_t*)rtguest::G(0x177f358)) + 0x10));
  RT_CALLRA(0x22ccc0, guest::vifUploadColours(0x29f420,0x29f430));
  RT_CALLRA(0x22ccd4, guest::drawModelParts((*(int32_t*)rtguest::G(0x177f358)),2));
  RT_CALLRA(0x22ccdc, guest::UploadCarAt16Env());
  RT_CALLRA(0x22cce8, guest::DrawCarShowroomRig(0x1752660,0));
  RT_CALLRA(0x22ccf0, guest::ResetShowroomLighting());
  RT_CALLRA(0x22ccfc, guest::DrawCarVariant0(0x1752660,0));
  RT_CALLRA(0x22cd08, guest::vifDirect(param_1,0xc5));
  uVar1 = RT_CALLRA(0x22cd10, guest::vifEndPacket(1));
  RT_CALLRA(0x22cd18, guest::vifKick(uVar1));
  return 0;
}


RT_GUEST_FN(0x0022cc38, guest::drawModelAt177F358)

}  // namespace guest
