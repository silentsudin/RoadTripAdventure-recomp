// guest:0x0023fcb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SubmitConditionalTexturePackets(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x38) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x3c) = 0x00000000u;
  }
  long lVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  lVar1 = RT_CALLRA(0x23fcd8, guest::TestFlagBitFc98(0x50));
  if (lVar1 == 0) {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) * (int)(*(int16_t*)rtguest::G(0x2a336a));
    if (iVar2 < 0) {
      iVar2 = iVar2 + 0xf;
    }
    (*(uint32_t*)rtguest::G(0x2a34b0)) = (long)(*(uint32_t*)rtguest::G(0x2a3350)) | (long)(*(uint32_t*)rtguest::G(0x2a335c)) << 0x38 |
                   (long)(*(uint32_t*)rtguest::G(0x2a3354)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2a335c)) << 0x18 |
                   (long)(*(uint32_t*)rtguest::G(0x2a3358)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2a3358)) << 0x30;
    (*(uint32_t*)rtguest::G(0x2a34d0)) = (long)(*(uint32_t*)rtguest::G(0x2a3360)) | (long)(*(uint32_t*)rtguest::G(0x2a3364)) << 0x20;
    (*(uint32_t*)rtguest::G(0x2a34f0)) = (long)(*(uint32_t*)rtguest::G(0x2a3370)) | (long)(*(uint32_t*)rtguest::G(0x2a337c)) << 0x38 |
                   (long)(*(uint32_t*)rtguest::G(0x2a3374)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2a337c)) << 0x18 |
                   (long)(*(uint32_t*)rtguest::G(0x2a3378)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2a3378)) << 0x30;
    (*(uint32_t*)rtguest::G(0x2a3510)) = (long)(*(uint32_t*)rtguest::G(0x2a3380)) | (long)(*(uint32_t*)rtguest::G(0x2a3384)) << 0x20;
    (*(uint32_t*)rtguest::G(0x2a34c0)) = ((long)(iVar2 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2a336e))) << 0x10;
    (*(uint32_t*)rtguest::G(0x2a3500)) = 0;
  }
  else {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) * (int)(*(int16_t*)rtguest::G(0x2a336a));
    iVar3 = (*(uint32_t*)rtguest::G(0x2a3368)) * 0x40;
    iVar4 = (short)(*(uint32_t*)rtguest::G(0x002a338a)) * 0x80;
    if (iVar2 < 0) {
      iVar2 = iVar2 + 0xf;
    }
    if (iVar3 < 0) {
      iVar3 = iVar3 + 0xf;
    }
    if (iVar4 < 0) {
      iVar4 = iVar4 + 0xf;
    }
    (*(uint32_t*)rtguest::G(0x2a34d0)) = (long)(*(uint32_t*)rtguest::G(0x2a3360)) | (long)(*(uint32_t*)rtguest::G(0x2a3364)) << 0x20;
    (*(uint32_t*)rtguest::G(0x2a34c0)) = (long)(iVar3 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2a336c)) |
                   ((long)(iVar2 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2a336e))) << 0x10;
    (*(uint32_t*)rtguest::G(0x2a34b0)) = (long)(*(uint32_t*)rtguest::G(0x2a3350)) | (long)(*(uint32_t*)rtguest::G(0x2a335c)) << 0x38 |
                   (long)(*(uint32_t*)rtguest::G(0x2a3354)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2a335c)) << 0x18 |
                   (long)(*(uint32_t*)rtguest::G(0x2a3358)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2a3358)) << 0x30;
    (*(uint32_t*)rtguest::G(0x2a34f0)) = (long)(*(uint32_t*)rtguest::G(0x2a3370)) | (long)(*(uint32_t*)rtguest::G(0x2a337c)) << 0x38 |
                   (long)(*(uint32_t*)rtguest::G(0x2a3374)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2a337c)) << 0x18 |
                   (long)(*(uint32_t*)rtguest::G(0x2a3378)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2a3378)) << 0x30;
    (*(uint32_t*)rtguest::G(0x2a3510)) = (long)(*(uint32_t*)rtguest::G(0x2a3380)) | (long)(*(uint32_t*)rtguest::G(0x2a3384)) << 0x20;
    (*(uint32_t*)rtguest::G(0x2a3500)) = ((long)(iVar4 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2a338e))) << 0x10;
  }
  (*(uint64_t*)rtguest::G(0x2a34a0)) = (*(uint64_t*)rtguest::G(0x2a34a0)) & 0xffffffffffff8000 | 8;
  RT_CALLRA(0x23ff24, guest::gifChainEndTag(0x2a3490,9));
  sendTexturePackets(0x2a3490);
  return 0;
}


RT_GUEST_FN(0x0023fcb0, guest::SubmitConditionalTexturePackets)

}  // namespace guest
