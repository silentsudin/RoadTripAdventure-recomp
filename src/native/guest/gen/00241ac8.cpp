// guest:0x00241ac8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SubmitCounterTexturePacketC(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
  }
  int iVar1;
  
  iVar1 = (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) * (int)(*(int16_t*)rtguest::G(0x2a33ca));
  if (iVar1 < 0) {
    iVar1 = iVar1 + 0xf;
  }
  (*(uint32_t*)rtguest::G(0x2a34b0)) = (long)(*(uint32_t*)rtguest::G(0x2a33b0)) | (long)(*(uint32_t*)rtguest::G(0x2a33bc)) << 0x38 |
                 (long)(*(uint32_t*)rtguest::G(0x2a33b4)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2a33bc)) << 0x18 |
                 (long)(*(uint32_t*)rtguest::G(0x2a33b8)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2a33b8)) << 0x30;
  (*(uint32_t*)rtguest::G(0x2a34d0)) = (long)(*(uint32_t*)rtguest::G(0x2a33c0)) | (long)(*(uint32_t*)rtguest::G(0x2a33c4)) << 0x20;
  (*(uint32_t*)rtguest::G(0x2a34c0)) = ((long)(iVar1 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2a33ce))) << 0x10;
  (*(uint64_t*)rtguest::G(0x2a34a0)) = (*(uint64_t*)rtguest::G(0x2a34a0)) & 0xffffffffffff8000 | 4;
  RT_CALLRA(0x241b80, guest::gifChainEndTag(0x2a3490,5));
  sendTexturePackets(0x2a3490);
  return 0;
}


RT_GUEST_FN(0x00241ac8, guest::SubmitCounterTexturePacketC)

}  // namespace guest
