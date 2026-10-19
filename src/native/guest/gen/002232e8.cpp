// guest:0x002232e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldProps(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
uint32_t puVar1;
uint32_t pfVar2;
  uint uVar3;
  
  uVar3 = 0;
  vifUploadAt4();
  vifDirect(0x29d240,5);
  vifUploadLightAt16(0x177f5e0);
  (*(uint32_t*)rtguest::G(0x3356e8)) = 0;
  vifUploadLightMatrix(0x177f620);
  puVar1 = 0x0177eb10;
  pfVar2 = (uint32_t)0x0177eb4c;
  do {
    if ((*(float*)rtguest::G((uint32_t)pfVar2)) != 0.0) {
      vifObjectMatrix(puVar1);
      vifBatchRegs((*(uint64_t*)rtguest::G((uint32_t)0x177ea30 + (uint32_t)(uVar3 & 3) * 8)),7,0,0xf);
      vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177f580 + (uint32_t)((uVar3 & 3) * 6) * 4)) + 0x10);
      if (param_3 != 0) {
        (*(float*)rtguest::G((uint32_t)pfVar2)) = 0.0;
      }
    }
    uVar3 = uVar3 + 1;
    pfVar2 = pfVar2 + 0x10;
    puVar1 = puVar1 + 0x40;
  } while ((int)uVar3 < 0x20);
  return 0;
}


RT_GUEST_FN(0x002232e8, guest::drawFieldProps)

}  // namespace guest
