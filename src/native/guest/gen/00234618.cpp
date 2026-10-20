// guest:0x00234618 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetBannerConfig(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  int iVar1;
  undefined4 uVar2;
uint32_t __s;
  int iVar3;
  
  __s = (uint32_t)(param_1 + 0x80a4);
  iVar3 = 0x29fc48;
  if (param_2 != 0) {
    iVar3 = (int)param_2 * 0x20 + 0x29fc28;
  }
  uVar2 = 3;
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8004)));
  if (param_2 == 0) {
    uVar2 = 0;
  }
  (*(uint32_t*)rtguest::G((uint32_t)__s)) = uVar2;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x2c));
  (*(uint32_t*)rtguest::G((uint32_t)__s)) = uVar2;
  uVar2 = 0x70;
  if (iVar1 != 0) {
    uVar2 = 0xe0;
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80b8))) = iVar3;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80bc))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80cc))) = 0x40000000;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80a8))) = 0x140;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80ac))) = uVar2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80c8))) = 10;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80b0))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80b4))) = 0;
  return 0;
}


RT_GUEST_FN(0x00234618, guest::SetBannerConfig)

}  // namespace guest
