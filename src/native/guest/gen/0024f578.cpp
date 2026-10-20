// guest:0x0024f578 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiArenaSubmit(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4)));
  if (iVar1 < 0) {
    iVar1 = iVar1 + 0xf;
  }
  if (0 < iVar1 >> 4) {
    vifDirect(param_1 + 0x10,iVar1 >> 4);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0024f578, guest::uiArenaSubmit)

}  // namespace guest
