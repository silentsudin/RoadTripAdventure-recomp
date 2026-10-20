// guest:0x00207a28 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiSetQuadXY(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar1 = (int)param_2 << 4;
  iVar2 = (int)param_3 << 4;
  iVar3 = ((int)param_2 + (int)param_4) * 0x10;
  iVar4 = ((int)param_3 + (int)param_5) * 0x10;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x84))) = iVar4;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x44))) = iVar2;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x60))) = iVar1;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80))) = iVar3;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) = iVar1;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x24))) = iVar2;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x40))) = iVar3;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 100))) = iVar4;
  return 0;
}


RT_GUEST_FN(0x00207a28, guest::uiSetQuadXY)

}  // namespace guest
