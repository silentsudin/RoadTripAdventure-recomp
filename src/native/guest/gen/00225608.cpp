// guest:0x00225608 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawSkyDome(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x50) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x54) = 0x00000000u;
  }
uint32_t auStack_60 = rtguest::allocFrame(48);
uint32_t auStack_30 = rtguest::allocFrame(4);
    uint32_t uStack_2c = rtguest::allocFrame(4);
  
  RT_CALLRA(0x225624, guest::SetDrawSkyColours(0x29d490));
  (*(uint32_t*)rtguest::G(0x29d49c)) = (*(uint32_t*)rtguest::G(0x332da4));
  RT_CALLRA(0x225634, guest::vifUploadCamera(param_1));
  RT_CALLRA(0x225644, guest::vifDirect(0x29d4a0,6));
  RT_CALLRA(0x225650, guest::vifUploadAt16(0x29d3f0));
  RT_CALLRA(0x22565c, guest::vifUploadColours(0x29d490,0x29d480));
  RT_CALLRA(0x225664, guest::vu0UnitMatrix(auStack_60));
  RT_CALLRA(0x225670, guest::vu0CopyVector(auStack_30,(int)param_1 + 0x170));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) = 0;
  RT_CALLRA(0x22567c, guest::vifObjectMatrixSky(auStack_60));
  RT_CALLRA(0x225688, guest::vifCallModel(0x290a40));
  return 0;
}


RT_GUEST_FN(0x00225608, guest::drawSkyDome)

}  // namespace guest
