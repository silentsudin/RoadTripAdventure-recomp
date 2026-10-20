// guest:0x002207d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawSceneArcRibbonFaded(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
  }
  int iVar1;
  int iVar2;
  uint uVar3;
  
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x14))) - 54000U < 0x1a5e1) {
    return 0;
  }
  RT_CALLRA(0x220808, guest::vifSetBatchOffset(0x29d0b0));
  if ((*(int8_t*)rtguest::G(0x177ae4c)) != '\0') {
    RT_CALLRA(0x22082c, guest::drawArcRibbon(0x177ac50,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177add0)) + 8))),param_1));
  }
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x14)));
  if (iVar1 - 0xcaf1U < 0x7ff) {
    iVar2 = -iVar1 + 54000;
    if (iVar2 < 0) {
      iVar2 = -iVar1 + 0xd32f;
    }
    uVar3 = iVar2 >> 6;
    (*(uint32_t*)rtguest::G(0x0029d130)) = (long)(int)(uVar3 << 0x10 | uVar3 << 8 | uVar3);
    vifDirect(0x29d0f0,6);
    return 0;
  }
  if (iVar1 - 0x278d1U < 0x7ff) {
    iVar2 = iVar1 + -0x278d0;
    if (iVar1 + -0x278d0 < 0) {
      iVar2 = iVar1 + -0x27891;
    }
    uVar3 = iVar2 >> 6;
    (*(uint32_t*)rtguest::G(0x0029d130)) = (long)(int)(uVar3 << 0x10 | uVar3 << 8 | uVar3);
    vifDirect(0x29d0f0,6);
    return 0;
  }
  (*(uint32_t*)rtguest::G(0x0029d130)) = 0x202020;
  vifDirect(0x29d0f0,6);
  return 0;
}


RT_GUEST_FN(0x002207d0, guest::DrawSceneArcRibbonFaded)

}  // namespace guest
