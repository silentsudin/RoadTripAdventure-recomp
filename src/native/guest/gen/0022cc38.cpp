// guest:0x0022cc38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawModelAt177F358(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(96);
  undefined8 uVar1;
uint32_t auStack_60 = rtguest::allocFrame(64);
  
  vifBeginPacket(1);
  ResetShowroomLighting();
  vifUploadAt4(0x177e6d0);
  vu0UnitMatrix(auStack_60);
  vifObjectMatrix(auStack_60);
  vifBatchRegs(0x20045645e53221b2,7,0,0xf);
  vifCallModel((*(int32_t*)rtguest::G(0x177f358)) + 0x10);
  vifUploadColours(0x29f420,0x29f430);
  drawModelParts((*(int32_t*)rtguest::G(0x177f358)),2);
  UploadCarAt16Env();
  DrawCarShowroomRig(0x1752660,0);
  ResetShowroomLighting();
  DrawCarVariant0(0x1752660,0);
  vifDirect(param_1,0xc5);
  uVar1 = vifEndPacket(1);
  vifKick(uVar1);
  return 0;
}


RT_GUEST_FN(0x0022cc38, guest::drawModelAt177F358)

}  // namespace guest
