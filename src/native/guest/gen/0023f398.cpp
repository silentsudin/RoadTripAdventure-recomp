// guest:0x0023f398 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SubmitDualAnimatedTextureLayout(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x38, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
  }
  int iVar1;
  int iVar2;
  
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) == 0) {
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0;
  }
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) * (int)(*(int16_t*)rtguest::G(0x2a302a));
  iVar2 = (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) * (int)(*(uint32_t*)rtguest::G(0x2a304a));
  if (iVar1 < 0) {
    iVar1 = iVar1 + 0xf;
  }
  if (iVar2 < 0) {
    iVar2 = iVar2 + 0xf;
  }
  (*(uint32_t*)rtguest::G(0x2a34d0)) = (long)(*(uint32_t*)rtguest::G(0x2a3020)) | (long)(*(uint32_t*)rtguest::G(0x2a3024)) << 0x20;
  (*(uint32_t*)rtguest::G(0x2a34f0)) = (long)(*(uint32_t*)rtguest::G(0x2a3030)) | (long)(*(uint32_t*)rtguest::G(0x2a303c)) << 0x38 |
                 (long)(*(uint32_t*)rtguest::G(0x2a3034)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2a303c)) << 0x18 |
                 (long)(*(uint32_t*)rtguest::G(0x2a3038)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2a3038)) << 0x30;
  (*(uint32_t*)rtguest::G(0x2a3510)) = (long)(*(uint32_t*)rtguest::G(0x2a3040)) | (long)(*(uint32_t*)rtguest::G(0x2a3044)) << 0x20;
  (*(uint32_t*)rtguest::G(0x2a34b0)) = (long)(*(uint32_t*)rtguest::G(0x2a3010)) | (long)(*(uint32_t*)rtguest::G(0x2a301c)) << 0x38 |
                 (long)(*(uint32_t*)rtguest::G(0x2a3014)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2a301c)) << 0x18 |
                 (long)(*(uint32_t*)rtguest::G(0x2a3018)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2a3018)) << 0x30;
  (*(uint32_t*)rtguest::G(0x2a34c0)) = ((long)(iVar1 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2a302e))) << 0x10;
  (*(uint32_t*)rtguest::G(0x2a3500)) = ((long)(iVar2 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2a304e))) << 0x10;
  RT_CALLRA(0x23f4e0, guest::AdvanceOscillatingValue(param_1 + 2));
  (*(uint64_t*)rtguest::G(0x2a34a0)) = (*(uint64_t*)rtguest::G(0x2a34a0)) & 0xffffffffffff8000 | 8;
  RT_CALLRA(0x23f504, guest::gifChainEndTag(0x2a3490,9));
  sendTexturePackets(0x2a3490);
  return 0;
}


RT_GUEST_FN(0x0023f398, guest::SubmitDualAnimatedTextureLayout)

}  // namespace guest
