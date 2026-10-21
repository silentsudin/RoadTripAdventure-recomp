// guest:0x0026d628 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ApplyRacePlayerMode(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x24))) != '\0') {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x21))) = 9;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))) = 8;
    RT_CALLRA(0x26d650, guest::SpawnTownDialogueTasks());
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = 0x26d678;
  }
  return 0;
}


RT_GUEST_FN(0x0026d628, guest::ApplyRacePlayerMode)

}  // namespace guest
