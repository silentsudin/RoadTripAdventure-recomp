// guest:0x002256a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawSkyLayers(uint32_t param_1, uint32_t param_2, ...) {
  (*(uint32_t*)rtguest::G(0x29d47c)) = (*(uint32_t*)rtguest::G(0x177f728));
  vifUploadColours(0x29d470,0x29d480);
  vifBatchRegs(0x1fc00000009,9,0x2007df45e9443dfa,7);
  vifCallModel(0x2962e0);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22))) != '\"') {
    (*(uint32_t*)rtguest::G(0x29d47c)) = (*(uint32_t*)rtguest::G(0x177f720));
    vifUploadColours(0x29d490,0x29d480);
    vifCallModel(0x298a50);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x002256a8, guest::drawSkyLayers)

}  // namespace guest
