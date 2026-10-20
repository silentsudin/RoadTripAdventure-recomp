// guest:0x00272658 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGifPkCloseGifTag(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t puVar1;
  ulong uVar2;
uint32_t puVar3;
  uint uVar4;
  ulong uVar5;
  
  puVar1 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t)));
  puVar3 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)param_1));
  uVar5 = (*(uint64_t*)rtguest::G((uint32_t)puVar1));
  uVar4 = ((int)puVar3 - (int)puVar1 >> 3) - 2;
  uVar2 = uVar5 >> 0x3a & 3;
  if (uVar2 != 1) {
    uVar4 = uVar4 >> 1;
  }
  if (uVar2 != 2) {
    uVar2 = uVar5 >> 0x3c;
    if (uVar5 >> 0x3c == 0) {
      uVar2 = 0x10;
    }
    if (uVar2 == 0) {
      trap(7);
    }
    uVar4 = (int)(uVar4 + (int)uVar2 + -1) / (int)uVar2;
  }
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t))) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)puVar1)) = uVar5 + uVar4;
  for (; ((uint32_t)puVar3 & 0xc) != 0; puVar3 = puVar3 + 1) {
    (*(uint32_t*)rtguest::G((uint32_t)puVar3)) = 0;
  }
  (*(uint32_t*)rtguest::G((uint32_t)param_1)) = puVar3;
  return 0;
}


RT_GUEST_FN(0x00272658, guest::sceGifPkCloseGifTag)

}  // namespace guest
