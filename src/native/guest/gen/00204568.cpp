// guest:0x00204568 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawPanelCells(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(16);
  uiDrawPanel9(param_1 * 0x14 + 0x1e,param_2 * 0xc + 0x11,((param_3 - param_1) + -1) * 0x14,
               ((param_4 - param_2) + -1) * 0xc);
  return 0;
}


RT_GUEST_FN(0x00204568, guest::uiDrawPanelCells)

}  // namespace guest
