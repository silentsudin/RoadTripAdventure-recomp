// guest:0x00234f68 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetTrackMeta4(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t __s;
  int iVar1;
  
  __s = (uint32_t)(param_1 + 0x82b4);
  iVar1 = 0;
  if (param_2 != 0) {
    iVar1 = 3;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)__s)) != iVar1) {
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x2c));
    (*(int32_t*)rtguest::G((uint32_t)__s)) = iVar1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82dc))) = 0x40000000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82b8))) = 0x140;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82bc))) = 0x70;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82c4))) = 3;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82c8))) = 0x0029fb08;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82cc))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82d8))) = 0x1e;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82c0))) = 1;
  }
  return 0;
}


RT_GUEST_FN(0x00234f68, guest::SetTrackMeta4)

}  // namespace guest
