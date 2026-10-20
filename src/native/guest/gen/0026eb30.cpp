// guest:0x0026eb30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsExecLoadImage(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  bool bVar1;
  uint uVar2;
  
  uVar2 = 0;
  while ((REG_DMAC_2_GIF_CHCR & 0x100) != 0) {
    bVar1 = 0x1000000 < uVar2;
    uVar2 = uVar2 + 1;
    if (bVar1) goto LAB_0026ec64;
  }
  REG_DMAC_2_GIF_QWC = 6;
  if ((param_1 & 0x70000000) == 0x70000000) {
    REG_DMAC_2_GIF_MADR = param_1 & 0xfffffff | 0x80000000;
  }
  else {
    REG_DMAC_2_GIF_MADR = param_1 & 0xfffffff;
  }
  REG_DMAC_2_GIF_CHCR = 0x101;
  do {
    bVar1 = uVar2 < 0x1000001;
    uVar2 = uVar2 + 1;
  } while (bVar1);
LAB_0026ec64:
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x32ff00u)));
  return 0xffffffff;
}


RT_GUEST_FN(0x0026eb30, guest::sceGsExecLoadImage)

}  // namespace guest
