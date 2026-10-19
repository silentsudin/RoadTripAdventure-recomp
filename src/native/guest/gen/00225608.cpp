// guest:0x00225608 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawSkyDome(uint32_t param_1, ...) {
    uint32_t uStack_2c = rtguest::allocFrame(4);
uint32_t auStack_60 = rtguest::allocFrame(48);
uint32_t auStack_30 = rtguest::allocFrame(4);
  
  SetDrawSkyColours(0x29d490);
  (*(uint32_t*)rtguest::G(0x29d49c)) = (*(uint32_t*)rtguest::G(0x332da4));
  vifUploadCamera(param_1);
  vifDirect(0x29d4a0,6);
  vifUploadAt16(0x29d3f0);
  vifUploadColours(0x29d490,0x29d480);
  vu0UnitMatrix(auStack_60);
  vu0CopyVector(auStack_30,(int)param_1 + 0x170);
  (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) = 0;
  vifObjectMatrixSky(auStack_60);
  vifCallModel(0x290a40);
  return 0;
}


RT_GUEST_FN(0x00225608, guest::drawSkyDome)

}  // namespace guest
