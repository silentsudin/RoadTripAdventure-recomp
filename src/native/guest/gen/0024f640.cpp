// guest:0x0024f640 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiArenaSetTexture(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint64_t param_4, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
  }
uint32_t plVar1;
  int iVar2;
  int iVar3;
  
  plVar1 = (uint32_t)(param_3 * 0x10 + param_2);
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) + 0x20;
  iVar3 = (int)param_1 + (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) + 0x10;
  if ((*(int32_t*)rtguest::G((uint32_t)param_1)) < iVar2) {
    (*(int32_t*)rtguest::G((uint32_t)param_1)) = 0;
    iVar3 = 0;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) = iVar2;
  }
  if (iVar3 != 0) {
    RT_CALLRA(0x24f69c, guest::gifSetTexRegs(iVar3,(*(int64_t*)rtguest::G((uint32_t)plVar1 + (uint32_t)(1) * sizeof(int64_t))),(*(int64_t*)rtguest::G((uint32_t)plVar1)) + (param_4 << 0x38)));
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) & 0xffffffffffffff0f | 0x60;
  }
  return 0;
}


RT_GUEST_FN(0x0024f640, guest::uiArenaSetTexture)

}  // namespace guest
