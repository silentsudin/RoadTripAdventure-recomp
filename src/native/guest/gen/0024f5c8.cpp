// guest:0x0024f5c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void uiArenaBeginBatch(uint32_t param_1, uint64_t param_2, uint32_t param_3) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  int iVar2;
  
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) + 0x60;
  iVar1 = (int)param_1 + (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) + 0x10;
  if ((*(int32_t*)rtguest::G((uint32_t)param_1)) < iVar2) {
    (*(int32_t*)rtguest::G((uint32_t)param_1)) = 0;
    iVar1 = 0;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) = iVar2;
  }
  if (iVar1 != 0) {
    if (param_2 != 0) {
      gifBeginSpriteBatch(iVar1,(*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_3 * 0x10 + (int)param_2 + 8))));
      return;
    }
    gifBeginSpriteBatch(iVar1,0);
    return;
  }
  return;
}


RT_GUEST_FN(0x0024f5c8, guest::uiArenaBeginBatch)

}  // namespace guest
