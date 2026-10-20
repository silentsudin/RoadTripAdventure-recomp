// guest:0x002872b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifRemoveCmdHandler(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(128);
  int iVar1;
  
  iVar1 = (*(uint32_t*)rtguest::G(0x1776eac));
  if ((int32_t)param_1 < 0) {
    iVar1 = (*(int32_t*)rtguest::G(0x1776ea4));
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 * 8 + iVar1))) = 0;
  return 0;
}


RT_GUEST_FN(0x002872b0, guest::sceSifRemoveCmdHandler)

}  // namespace guest
