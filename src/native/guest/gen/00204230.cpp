// guest:0x00204230 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t uiDrawTextShade(uint64_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg;
  byte bVar1;
  
  if ((*(int8_t*)rtguest::G((uint32_t)param_4)) != 0) {
    do {
      param_1 = uiDrawGlyph(param_1,param_2 << 1,((int)param_3 << 0x11) >> 0x10,param_4);
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_4));
      if (bVar1 == 0x2f) {
        param_4 = param_4 + 2;
      }
      else if (bVar1 == 0x60) {
        param_4 = param_4 + 2;
      }
      else if (bVar1 == 0x5e) {
        param_4 = param_4 + 2;
      }
      else if (bVar1 == 0x7e) {
        param_4 = param_4 + 2;
      }
      else if ((int)((uint32_t)bVar1 << 0x18) < 0) {
        param_4 = param_4 + 2;
      }
      else {
        param_4 = param_4 + 1;
      }
    } while ((*(int8_t*)rtguest::G((uint32_t)param_4)) != 0);
  }
  return param_1;
}


RT_GUEST_FN(0x00204230, guest::uiDrawTextShade)

}  // namespace guest
