// guest:0x00270d50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textGridCreate(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x50) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x54) = 0x00000000u;
    rtguest::gS[0] = 0x0000000000000000ull;  // walk const
    rtguest::setS32(1, (uint32_t)param_4);  // walk: = param_4
    rtguest::setS32(2, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(3, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(4, (uint32_t)param_2);  // walk: = param_2
  }
  long lVar1;
  uint uVar2;
uint32_t piVar3;
  
  if ((param_3 < 0x51) && (param_4 < 0x41)) {
    piVar3 = 0x01773040;
    if ((*(uint32_t*)rtguest::G(0x1773048)) != 0) {
      for (uVar2 = 1; piVar3 = (uint32_t)0x0, uVar2 < 4; uVar2 = uVar2 + 1) {
        if ((*(uint32_t*)rtguest::G((uint32_t)0x1773048 + (uint32_t)(uVar2 * 0x16) * 4)) == 0) {
          piVar3 = 0x01773040 + uVar2 * 0x16;
          break;
        }
      }
    }
    if (piVar3 == (uint32_t)0x0) {
      return (uint32_t)0x0;
    }
    lVar1 = RT_CALLRA(0x270de8, guest::heapAllocBlock((int)param_3 * (int)param_4 * 2));
    (*(int32_t*)rtguest::G((uint32_t)piVar3 + (uint32_t)(2) * sizeof(int32_t))) = (int)lVar1;
    if (lVar1 != 0) {
      (*(int32_t*)rtguest::G((uint32_t)piVar3)) = (int)param_3;
      (*(int32_t*)rtguest::G((uint32_t)piVar3 + (uint32_t)(1) * sizeof(int32_t))) = (int)param_4;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(piVar3 + 3))) = 7;
      RT_CALLRA(0x270e18, guest::textGridInitAttrs(piVar3 + 6,param_1,param_2));
      RT_CALLRA(0x270e20, guest::textGridClear(piVar3));
      return piVar3;
    }
  }
  return (uint32_t)0x0;
}


RT_GUEST_FN(0x00270d50, guest::textGridCreate)

}  // namespace guest
