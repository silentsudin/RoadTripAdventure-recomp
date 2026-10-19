// guest:0x002419f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SubmitCounterTexturePacketB(uint32_t param_1, ...) {
  int iVar1;
  
  iVar1 = (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) * (int)(*(int16_t*)rtguest::G(0x2a33aa));
  if (iVar1 < 0) {
    iVar1 = iVar1 + 0xf;
  }
  (*(uint32_t*)rtguest::G(0x2a34b0)) = (long)(*(uint32_t*)rtguest::G(0x2a3390)) | (long)(*(uint32_t*)rtguest::G(0x2a339c)) << 0x38 |
                 (long)(*(uint32_t*)rtguest::G(0x2a3394)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2a339c)) << 0x18 |
                 (long)(*(uint32_t*)rtguest::G(0x2a3398)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2a3398)) << 0x30;
  (*(uint32_t*)rtguest::G(0x2a34d0)) = (long)(*(uint32_t*)rtguest::G(0x2a33a0)) | (long)(*(uint32_t*)rtguest::G(0x2a33a4)) << 0x20;
  (*(uint32_t*)rtguest::G(0x2a34c0)) = ((long)(iVar1 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2a33ae))) << 0x10;
  (*(uint64_t*)rtguest::G(0x2a34a0)) = (*(uint64_t*)rtguest::G(0x2a34a0)) & 0xffffffffffff8000 | 4;
  gifChainEndTag(0x2a3490,5);
  sendTexturePackets(0x2a3490);
  return 0;
}


RT_GUEST_FN(0x002419f0, guest::SubmitCounterTexturePacketB)

}  // namespace guest
