// guest:0x00204460 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawNumberAuto(uint32_t param_1, uint64_t param_2, uint64_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
  }
  int iVar1;
  
  if (param_3 < 1000000000) {
    if (param_3 < 100000000) {
      if (param_3 < 10000000) {
        if (param_3 < 1000000) {
          if (param_3 < 100000) {
            if (param_3 < 10000) {
              if (param_3 < 1000) {
                iVar1 = 1;
                if (9 < param_3) {
                  iVar1 = 2;
                }
                if (99 < param_3) {
                  iVar1 = 3;
                }
              }
              else {
                iVar1 = 5;
              }
            }
            else {
              iVar1 = 6;
            }
          }
          else {
            iVar1 = 7;
          }
        }
        else {
          iVar1 = 9;
        }
      }
      else {
        iVar1 = 10;
      }
    }
    else {
      iVar1 = 0xb;
    }
  }
  else {
    iVar1 = 0xd;
  }
  param_1 = param_1 + iVar1 * 10;
  RT_CALLRA(0x20454c, guest::uiDrawNumber10(param_1,param_2,param_3,param_4));
  return param_1;
}


RT_GUEST_FN(0x00204460, guest::uiDrawNumberAuto)

}  // namespace guest
