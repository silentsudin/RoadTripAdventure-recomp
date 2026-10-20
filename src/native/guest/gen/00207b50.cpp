// guest:0x00207b50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sendTexturePackets(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  undefined8 uVar1;
  
  uVar1 = sceDmaGetChan(2);
  FlushCache(0);
  sceDmaSync(uVar1,0,0);
  REG_DMAC_2_GIF_QWC = 0;
  REG_DMAC_2_GIF_TADR = param_1;
  REG_DMAC_STAT = 3;
  REG_DMAC_2_GIF_CHCR = 0x105;
  return 0;
}


RT_GUEST_FN(0x00207b50, guest::sendTexturePackets)

}  // namespace guest
