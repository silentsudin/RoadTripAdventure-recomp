// guest:0x002271f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vifKick(uint32_t param_1, ...) {
  FlushCache(0);
  do {
  } while ((REG_DMAC_1_VIF1_CHCR & 0x100) != 0);
  REG_DMAC_1_VIF1_QWC = 0;
  REG_DMAC_1_VIF1_TADR = param_1;
  REG_DMAC_STAT = 2;
  REG_DMAC_1_VIF1_CHCR = 0x145;
  return 0;
}


RT_GUEST_FN(0x002271f0, guest::vifKick)

}  // namespace guest
