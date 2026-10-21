// guest:0x00207b50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sendTexturePackets(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  undefined8 uVar1;
  
  uVar1 = RT_CALLRA(0x207b64, guest::sceDmaGetChan(2));
  RT_CALLRA(0x207b70, guest::FlushCache(0));
  RT_CALLRA(0x207b80, guest::sceDmaSync(uVar1,0,0));
  REG_DMAC_2_GIF_QWC = 0;
  REG_DMAC_2_GIF_TADR = param_1;
  REG_DMAC_STAT = 3;
  REG_DMAC_2_GIF_CHCR = 0x105;
  return 0;
}


RT_GUEST_FN(0x00207b50, guest::sendTexturePackets)

}  // namespace guest
