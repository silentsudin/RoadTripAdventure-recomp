// guest:0x0026d628 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ApplyRacePlayerMode(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x24))) != '\0') {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x21))) = 9;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))) = 8;
    SpawnTownDialogueTasks();
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = 0x26d678;
  }
  return 0;
}


RT_GUEST_FN(0x0026d628, guest::ApplyRacePlayerMode)

}  // namespace guest
