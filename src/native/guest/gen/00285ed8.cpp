// guest:0x00285ed8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t iWakeupThread(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
  }
  undefined4 uVar1;
  ulong in_v0;
  int iVar2;
  
  uVar1 = (*(uint32_t*)rtguest::G(0x1776570));
  in_v0 = syscall(0xffffffffffffffd1);  // v0 capture; real v0 = caller tid at W12 syscall model
  if (in_v0 == param_1) {
    if ((in_v0 < 0x100) && ((*(int32_t*)rtguest::G(0x2e98f0)) != 0)) {
      iVar2 = ((*(uint32_t*)rtguest::G(0x177657c)) & 0x1ff) * 2;
      (*(uint32_t*)rtguest::G(0x177657c)) = ((*(uint32_t*)rtguest::G(0x177657c)) & 0x1ff) + 1;
      (*(uint32_t*)rtguest::G((uint32_t)0x1776580 + (uint32_t)(iVar2) * 4)) = 0;
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1776581))) = (char)in_v0;
      RT_CALLRA(0x285f50, guest::iSignalSema(uVar1));
    }
  }
  else {
    RT_CALLRA(0x285ef8, guest::_iWakeupThread(param_1));  // W12: tail-call forwarded $4 (ledger bare-call fix)
  }
  return 0;
}


RT_GUEST_FN(0x00285ed8, guest::iWakeupThread)

}  // namespace guest
