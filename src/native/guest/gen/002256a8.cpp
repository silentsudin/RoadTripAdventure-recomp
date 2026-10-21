// guest:0x002256a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawSkyLayers(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::gS[0] = 0x000000000029d480ull;  // walk const
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
    rtguest::gS[2] = 0x000000000029d470ull;  // walk const
    rtguest::gS[3] = 0x000000000177f5e0ull;  // walk const
  }
  (*(uint32_t*)rtguest::G(0x29d47c)) = (*(uint32_t*)rtguest::G(0x177f728));
  RT_CALLRA(0x2256e4, guest::vifUploadColours(0x29d470,0x29d480));
  RT_CALLRA(0x225714, guest::vifBatchRegs(0x1fc00000009,9,0x2007df45e9443dfa,7));
  vifCallModel(0x2962e0);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22))) != '\"') {
    (*(uint32_t*)rtguest::G(0x29d47c)) = (*(uint32_t*)rtguest::G(0x177f720));
    RT_CALLRA(0x225740, guest::vifUploadColours(0x29d490,0x29d480));
    vifCallModel(0x298a50);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x002256a8, guest::drawSkyLayers)

}  // namespace guest
