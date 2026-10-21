// guest:0x00205018 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DestroyTaskTree(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
  
  if ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x2c))) == (uint32_t)0x0) {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
  }
  else {
    // MIPS 0x205028/34: lw $v0,0x2c($s0); jalr $v0 — node+0x2c IS the handler
    // pointer ($a0 = node held from 0x205020). Ghidra's *(code**) double-deref
    // mistranslation made the kill call jump into code bytes (garbage handler).
    RT_CALLRA(0x00205034, rtCall((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x2c))), param_1));
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
  }
  (*(uint8_t*)rtguest::G((uint32_t)param_1)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = 0;
  if (iVar1 != 0) {
    RT_CALLRA(0x205050, guest::DestroyTaskTree(iVar1));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) = 0;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14))) != 0) {
    RT_CALLRA(0x205068, guest::DestroyTaskTree((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14)))));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14))) = 0;
  }
  return 0;
}


RT_GUEST_FN(0x00205018, guest::DestroyTaskTree)

}  // namespace guest
