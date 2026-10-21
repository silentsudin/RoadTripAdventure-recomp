// guest:0x00283890 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _s2b(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(1, (uint32_t)param_5);  // walk: = param_5
    rtguest::setS32(2, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(3, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(4, (uint32_t)param_4);  // walk: = param_4
  }
  char cVar1;
  undefined8 uVar2;
  int iVar3;
  int iVar4;
uint32_t pcVar5;
  int iVar6;
  
  iVar4 = 1;
  iVar6 = (param_4 + 8) / 9;
  iVar3 = 0;
  if (1 < iVar6) {
    do {
      iVar4 = iVar4 << 1;
      iVar3 = iVar3 + 1;
    } while (iVar4 < iVar6);
  }
  uVar2 = RT_CALLRA(0x283904, guest::_Balloc(param_1,iVar3));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar2 + 0x14))) = param_5;
  iVar6 = 9;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar2 + 0x10))) = 1;
  if (param_3 < 10) {
    pcVar5 = (uint32_t)(param_2 + 10);
  }
  else {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 9)));
    pcVar5 = (uint32_t)(param_2 + 9);
    while( true ) {
      iVar6 = iVar6 + 1;
      uVar2 = RT_CALLRA(0x283940, guest::_multadd(param_1,uVar2,10,cVar1 + -0x30));
      if (param_3 <= iVar6) break;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar5 + (uint32_t)(1) * sizeof(int8_t)));
      pcVar5 = pcVar5 + 1;
    }
    pcVar5 = pcVar5 + 2;
  }
  if (iVar6 < param_4) {
    param_4 = param_4 - iVar6;
    do {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar5));
      pcVar5 = pcVar5 + 1;
      param_4 = param_4 + -1;
      uVar2 = RT_CALLRA(0x28398c, guest::_multadd(param_1,uVar2,10,cVar1 + -0x30));
    } while (param_4 != 0);
  }
  return uVar2;
}


RT_GUEST_FN(0x00283890, guest::_s2b)

}  // namespace guest
