// guest:0x002076b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiSetSpriteUV(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg;
  (*(uint64_t*)rtguest::G((uint32_t)param_1)) = (*(uint64_t*)rtguest::G((uint32_t)param_1)) & 0xffffffffc000c000 | (long)(int)(((int)param_3 & 0x3ffU) << 4) |
             (long)(int)(((int)param_4 & 0x3ffU) << 4) << 0x10;
  (*(uint64_t*)rtguest::G((uint32_t)param_2)) = (*(uint64_t*)rtguest::G((uint32_t)param_2)) & 0xffffffffc000c000 |
             (long)(int)(((int)param_3 + (int)param_5) * 0x10 + 0x10U & 0x3fff) |
             (long)(int)(((int)param_4 + (int)param_6) * 0x10 + 0x10U & 0x3fff) << 0x10;
  return 0;
}


RT_GUEST_FN(0x002076b8, guest::uiSetSpriteUV)

}  // namespace guest
