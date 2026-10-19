// guest:0x00241840 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SubmitAnimatedTexturePacketC(uint32_t param_1, ...) {
  int iVar1;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) * (int)(*(int16_t*)rtguest::G(0x2a2f2a));
  if (iVar1 < 0) {
    iVar1 = iVar1 + 0xf;
  }
  (*(uint32_t*)rtguest::G(0x2a34b0)) = (long)(*(uint32_t*)rtguest::G(0x2a2f10)) | (long)(*(uint32_t*)rtguest::G(0x2a2f1c)) << 0x38 |
                 (long)(*(uint32_t*)rtguest::G(0x2a2f14)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2a2f1c)) << 0x18 |
                 (long)(*(uint32_t*)rtguest::G(0x2a2f18)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2a2f18)) << 0x30;
  (*(uint32_t*)rtguest::G(0x2a34d0)) = (long)(*(uint32_t*)rtguest::G(0x2a2f20)) | (long)(*(uint32_t*)rtguest::G(0x2a2f24)) << 0x20;
  (*(uint32_t*)rtguest::G(0x2a34c0)) = ((long)(iVar1 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2a2f2e))) << 0x10;
  (*(uint64_t*)rtguest::G(0x2a34a0)) = (*(uint64_t*)rtguest::G(0x2a34a0)) & 0xffffffffffff8000 | 4;
  gifChainEndTag(0x2a3490,5);
  sendTexturePackets(0x2a3490);
  return 0;
}


RT_GUEST_FN(0x00241840, guest::SubmitAnimatedTexturePacketC)

}  // namespace guest
