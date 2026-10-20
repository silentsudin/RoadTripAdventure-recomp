// guest:0x00262458 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t auditRouletteOutcome(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  if ((*(int32_t*)rtguest::G(0x177fdb4)) < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x800c)))) {
    GetOrSetPresidentRaceResult(1);
    return 0;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x800c))) < (*(int32_t*)rtguest::G(0x177fdb4))) {
    GetOrSetPresidentRaceResult(2);
    return 0;
  }
  GetOrSetPresidentRaceResult(3);
  return 0;
}


RT_GUEST_FN(0x00262458, guest::auditRouletteOutcome)

}  // namespace guest
