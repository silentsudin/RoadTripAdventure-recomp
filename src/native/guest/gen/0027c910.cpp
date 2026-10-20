// guest:0x0027c910 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __register_frame_info_table(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  undefined4 uVar1;
  
  (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(uint32_t))) = param_1;
  (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(uint32_t))) = param_1;
  (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
  uVar1 = (*(uint32_t*)rtguest::G(0x2e99ac));
  (*(uint32_t*)rtguest::G((uint32_t)param_2)) = 0;
  WaitSema(uVar1);
  uVar1 = (*(uint32_t*)rtguest::G(0x2e99ac));
  (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G(0x1776160));
  (*(uint32_t*)rtguest::G(0x1776160)) = param_2;
  SignalSema(uVar1);
  return 0;
}


RT_GUEST_FN(0x0027c910, guest::__register_frame_info_table)

}  // namespace guest
