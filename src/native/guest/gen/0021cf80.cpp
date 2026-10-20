// guest:0x0021cf80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t Scene3dTownHandler(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg;
  short sVar1;
uint32_t psVar2;
  
  if ((param_3 & 4) == 0) {
    if (param_1 == 0) {
      param_1 = (uint32_t)((param_3 & 5) == 1);
    }
    if (((param_3 & 1) == 0) || (param_1 == 0)) {
      if (((param_3 & 2) != 0) && (1 < (int)param_1)) {
        sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + param_1 * 2 + -2)));
        if (sVar1 == 0) {
          trap(7);
        }
        param_1 = param_1 - ((param_2 << 7) / (int)sVar1 < 8000);
      }
    }
    else {
      param_2 = param_2 << 7;
      psVar2 = (uint32_t)(param_1 * 2 + param_4);
      sVar1 = (*(int16_t*)rtguest::G((uint32_t)psVar2));
      if (sVar1 == 0) {
        trap(7);
      }
      if ((param_2 / (int)sVar1 < 0x1389) || ((param_3 & 2) != 0)) {
        if (2 < (int)param_1) {
          psVar2 = (uint32_t)(param_1 * 2 + param_4);
          sVar1 = (*(int16_t*)rtguest::G((uint32_t)psVar2));
          if (sVar1 == 0) {
            trap(7);
          }
          if (param_2 / (int)sVar1 < 3000) {
            sVar1 = (*(int16_t*)rtguest::G((uint32_t)psVar2 + (uint32_t)(-1) * sizeof(int16_t)));
            if (sVar1 == 0) {
              trap(7);
            }
            if (3999 < param_2 / (int)sVar1) {
              return param_1;
            }
            param_1 = param_1 - 1;
          }
        }
      }
      else if ((int)param_1 < 6) {
        if ((*(int16_t*)rtguest::G((uint32_t)psVar2 + (uint32_t)(1) * sizeof(int16_t))) < 1) {
          return param_1;
        }
        param_1 = param_1 + 1;
      }
    }
  }
  else {
    param_1 = 0;
  }
  return param_1;
}


RT_GUEST_FN(0x0021cf80, guest::Scene3dTownHandler)

}  // namespace guest
