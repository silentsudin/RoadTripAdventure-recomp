// guest:0x002340c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetStageConfig(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(32);
  int iVar1;
  int iVar2;
  
  iVar1 = param_3 * 0x18 + param_2 * 0x408;
  iVar2 = iVar1 + 0x84c0;
  iVar1 = iVar1 + 0x84b0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 * 0x408 + param_1 + 0x84a0))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4 + iVar1))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + iVar2 + 8))) = param_4;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4 + iVar2))) = param_5;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + iVar1 + 0x10))) = param_6;
  return 0;
}


RT_GUEST_FN(0x002340c0, guest::SetStageConfig)

}  // namespace guest
