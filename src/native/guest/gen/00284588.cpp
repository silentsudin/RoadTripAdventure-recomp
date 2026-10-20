// guest:0x00284588 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _ratio(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
    uint32_t iStack_30 = rtguest::allocFrame(4);
  int iVar1;
  ulong uVar2;
  ulong uVar3;
uint32_t aiStack_2c = rtguest::allocFrame(12);
  
  uVar2 = _b2d(param_1,iStack_30);
  uVar3 = _b2d(param_2,aiStack_2c);
  iVar1 = ((*(int32_t*)rtguest::G((uint32_t)iStack_30)) - (*(int32_t*)rtguest::G((uint32_t)aiStack_2c + (uint32_t)(0) * 4))) +
          ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x10))) - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x10)))) * 0x20;
  if (iVar1 < 1) {
    uVar3 = uVar3 & 0xffffffff | (long)((int)(uVar3 >> 0x20) + iVar1 * -0x100000) << 0x20;
  }
  else {
    uVar2 = uVar2 & 0xffffffff | (long)((int)(uVar2 >> 0x20) + iVar1 * 0x100000) << 0x20;
  }
  dpdiv(uVar2,uVar3);
  return 0;
}


RT_GUEST_FN(0x00284588, guest::_ratio)

}  // namespace guest
