// guest:0x0027cd18 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t find_fde(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[3] = 0x00000000002f0000ull;  // walk const
  }
  int iVar1;
uint32_t piVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
uint32_t puVar7;
uint32_t piVar8;
  
  RT_CALLRA(0x27cd3c, guest::WaitSema((*(uint32_t*)rtguest::G(0x2e99ac))));
  puVar7 = (*(uint32_t*)rtguest::G(0x1776160));
  while( true ) {
    while( true ) {
      if (puVar7 == (uint32_t)0x0) {
        RT_CALLRA(0x27cd98, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e99ac))));
        return 0;
      }
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)puVar7));
      if (uVar5 == 0) {
        RT_CALLRA(0x27cd64, guest::frame_init(puVar7));
        uVar5 = (*(uint32_t*)rtguest::G((uint32_t)puVar7));
      }
      if (uVar5 <= param_1) break;
      puVar7 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(5) * sizeof(uint32_t)));
    }
    if (param_1 < (*(uint32_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(1) * sizeof(uint32_t)))) break;
    puVar7 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(5) * sizeof(uint32_t)));
  }
  piVar8 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(3) * sizeof(uint32_t)));
  if ((piVar8 == (uint32_t)0x0) || (piVar2 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(2) * sizeof(uint32_t))), piVar8 == piVar2)) {
    RT_CALLRA(0x27cdbc, guest::frame_init(puVar7));
    piVar8 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(3) * sizeof(uint32_t)));
    piVar2 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(2) * sizeof(uint32_t)));
  }
  if (piVar8 != (uint32_t)0x0) {
    if (piVar8 != piVar2) {
      RT_CALLRA(0x27cddc, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e99ac))));
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(4) * sizeof(uint32_t)));
      uVar6 = 0;
      if (uVar5 == 0) {
        return 0;
      }
      uVar4 = uVar5;
      while( true ) {
        uVar4 = uVar4 >> 1;
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(uVar4 * 4 + (*(uint32_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(3) * sizeof(uint32_t))))));
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) <= param_1) &&
           (uVar6 = uVar4 + 1, uVar4 = uVar5, param_1 < (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xc)))
           )) break;
        uVar5 = uVar4;
        if (uVar5 <= uVar6) {
          return 0;
        }
        uVar4 = uVar6 + uVar5;
      }
      return iVar3;
    }
    if (piVar8 != (uint32_t)0x0) {
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar8));
      do {
        iVar1 = RT_CALLRA(0x27ce58, guest::search_fdes(iVar3,param_1));
        piVar8 = piVar8 + 0x4;  // T* element step
        if (iVar1 != 0) break;
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar8));
      } while (iVar3 != 0);
      goto LAB_0027ce8c;
    }
  }
  iVar1 = RT_CALLRA(0x27ce80, guest::search_fdes(piVar2,param_1));
LAB_0027ce8c:
  RT_CALLRA(0x27ce8c, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e99ac))));
  return iVar1;
}


RT_GUEST_FN(0x0027cd18, guest::find_fde)

}  // namespace guest
