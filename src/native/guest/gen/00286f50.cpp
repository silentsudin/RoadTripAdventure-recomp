// guest:0x00286f50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sifCmdGetFlag(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  return (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01776fc0 + param_1 * 4)));
}


RT_GUEST_FN(0x00286f50, guest::sifCmdGetFlag)

}  // namespace guest
