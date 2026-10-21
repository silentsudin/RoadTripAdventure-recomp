// guest:0x00205da0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t LayoutTextItems(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(4, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(5, (uint32_t)param_2);  // walk: = param_2
  }
  char cVar1;
  short sVar2;
  undefined4 uVar3;
  int iVar4;
uint32_t psVar5;
uint32_t puVar6;
  int iVar7;
  
  if (param_3 < param_4) {
    param_4 = param_3;
  }
  iVar4 = 0;
  if (0 < param_4) {
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_2));
    puVar6 = param_2;
    iVar7 = param_4;
    while( true ) {
      puVar6 = puVar6 + (uint32_t)(1) * sizeof(uint32_t);
      iVar7 = iVar7 + -1;
      RT_CALLRA(0x205dfc, guest::uiMenuAddItem(param_1,uVar3,0x10));
      iVar4 = param_4;
      if (iVar7 == 0) break;
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)puVar6));
    }
  }
  psVar5 = (uint32_t)param_1;
  if (iVar4 < param_3) {
    param_3 = param_3 - iVar4;
    param_2 = param_2 + iVar4 * (uint32_t)sizeof(uint32_t);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar5 + 0x11)));
    while( true ) {
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_2));
      param_2 = param_2 + (uint32_t)(1) * sizeof(uint32_t);
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar5 + 0x11))) = cVar1 + '\x01';
      iVar7 = RT_CALLRA(0x205e3c, guest::uiTextWidth(uVar3));
      sVar2 = (*(int16_t*)rtguest::G((uint32_t)psVar5 + (uint32_t)(2) * sizeof(int16_t)));
      iVar7 = iVar7 + 0x14;
      iVar4 = (int)sVar2;
      if ((long)sVar2 < (long)iVar7) {
        sVar2 = (short)((iVar7 - sVar2) / 2);
        (*(int16_t*)rtguest::G((uint32_t)psVar5 + (uint32_t)(2) * sizeof(int16_t))) = (short)iVar7;
        (*(int16_t*)rtguest::G((uint32_t)psVar5)) = (*(int16_t*)rtguest::G((uint32_t)psVar5)) - sVar2;
        (*(int16_t*)rtguest::G((uint32_t)psVar5 + (uint32_t)(4) * sizeof(int16_t))) = (*(int16_t*)rtguest::G((uint32_t)psVar5 + (uint32_t)(4) * sizeof(int16_t))) + sVar2;
        iVar4 = iVar7;
      }
      iVar4 = ((short)iVar4 - iVar7) / 2;
      if ((long)iVar4 < (long)(*(int16_t*)rtguest::G((uint32_t)psVar5 + (uint32_t)(4) * sizeof(int16_t)))) {
        (*(int16_t*)rtguest::G((uint32_t)psVar5 + (uint32_t)(4) * sizeof(int16_t))) = (short)iVar4;
      }
      param_3 = param_3 + -1;
      if (param_3 == 0) break;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar5 + 0x11)));
    }
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(psVar5 + (uint32_t)(6) * sizeof(int16_t)))) = 4;
  return 0;
}


RT_GUEST_FN(0x00205da0, guest::LayoutTextItems)

}  // namespace guest
