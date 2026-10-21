// guest:0x0026d678 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FinishBootPhase(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x10))) == 0) {
    (*(uint32_t*)rtguest::G(0x177ea50)) = 0x177f760;
    (*(uint32_t*)rtguest::G(0x177ea58)) = 0x0177f798;
    (*(uint32_t*)rtguest::G(0x177ea54)) = 0x177f77c;
    RT_CALLRA(0x26d6c8, guest::gs_tasks_spawn(param_1,0x20dc50,0x70001));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x21))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xc))) = 0x211808;
  }
  return 0;
}


RT_GUEST_FN(0x0026d678, guest::FinishBootPhase)

}  // namespace guest
