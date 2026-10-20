// guest:0x0026fe80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsPutDrawEnv(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  bool bVar1;
  uint uVar2;
  
  uVar2 = 0;
  do {
    if ((REG_DMAC_2_GIF_CHCR & 0x100) == 0) {
      if (((uint32_t)param_1 & 0x70000000) == 0x70000000) {
        REG_DMAC_2_GIF_MADR = (uint32_t)param_1 & 0xfffffff | 0x80000000;
      }
      else {
        REG_DMAC_2_GIF_MADR = (uint32_t)param_1 & 0xfffffff;
      }
      REG_DMAC_2_GIF_CHCR = 0x101;
      REG_DMAC_2_GIF_QWC = ((uint32_t)(*(uint64_t*)rtguest::G((uint32_t)param_1)) & 0x7fff) + 1;
      return 0;
    }
    bVar1 = uVar2 < 0x1000001;
    uVar2 = uVar2 + 1;
  } while (bVar1);
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x330148u)));
  return 0xffffffff;
}


RT_GUEST_FN(0x0026fe80, guest::sceGsPutDrawEnv)

}  // namespace guest
