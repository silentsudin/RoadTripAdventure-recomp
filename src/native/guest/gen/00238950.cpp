// guest:0x00238950 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AwardRaceResult(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  char cVar1;
  undefined4 uVar2;
  
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20)));
  if (cVar1 == '&') {
    if ((*(int8_t*)rtguest::G(0x177ae97)) == '\0') {
      uVar2 = 2;
    }
    else {
      uVar2 = 1;
    }
    GetOrSetPresidentRaceResult(uVar2);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20)));
  }
  if (cVar1 == '(') {
    if (((*(float*)rtguest::G(0x177ace4)) < 280.0) && ((*(float*)rtguest::G(0x177af54)) < 280.0)) {
      GetOrSetPresidentRaceResult(3);
    }
    if ((280.0 <= (*(float*)rtguest::G(0x177ace4))) && (280.0 <= (*(float*)rtguest::G(0x177af54)))) {
      if ((*(int8_t*)rtguest::G(0x177ae97)) != '\0') {
        GetOrSetPresidentRaceResult(1);
        return 0;
      }
      GetOrSetPresidentRaceResult(2);
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x00238950, guest::AwardRaceResult)

}  // namespace guest
