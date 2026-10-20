// guest:0x00241760 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SubmitAnimatedTexturePacketB(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  int iVar1;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) * (int)(*(int16_t*)rtguest::G(0x2a2c8a));
  if (iVar1 < 0) {
    iVar1 = iVar1 + 0xf;
  }
  (*(uint32_t*)rtguest::G(0x2a34b0)) = (long)(*(uint32_t*)rtguest::G(0x2a2c70)) | (long)(*(uint32_t*)rtguest::G(0x2a2c7c)) << 0x38 |
                 (long)(*(uint32_t*)rtguest::G(0x2a2c74)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2a2c7c)) << 0x18 |
                 (long)(*(uint32_t*)rtguest::G(0x2a2c78)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2a2c78)) << 0x30;
  (*(uint32_t*)rtguest::G(0x2a34d0)) = (long)(*(uint32_t*)rtguest::G(0x2a2c80)) | (long)(*(uint32_t*)rtguest::G(0x2a2c84)) << 0x20;
  (*(uint32_t*)rtguest::G(0x2a34c0)) = ((long)(iVar1 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2a2c8e))) << 0x10;
  (*(uint64_t*)rtguest::G(0x2a34a0)) = (*(uint64_t*)rtguest::G(0x2a34a0)) & 0xffffffffffff8000 | 4;
  gifChainEndTag(0x2a3490,5);
  sendTexturePackets(0x2a3490);
  return 0;
}


RT_GUEST_FN(0x00241760, guest::SubmitAnimatedTexturePacketB)

}  // namespace guest
