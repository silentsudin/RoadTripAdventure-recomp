// guest:0x0027e68c transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t memcpy(uint32_t __dest, uint32_t __src, uint32_t __n, ...) {
  rtguest::FrameGuard rt_fg(0);
  undefined1 uVar1;
  undefined8 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int iVar5;
uint32_t puVar6;
  
  puVar6 = __dest;
  if (0x1f < __n) {
    if ((((uint32_t)__src | (uint32_t)__dest) & 0xf) != 0) {
      iVar5 = (int)__n;
      goto joined_r0x0027e710;
    }
    do {
      uVar2 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)__src));
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)__src + 8)));
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)__src + 0xc)));
      __n = (size_t)((int)__n + -0x20);
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)puVar6)) = (int)uVar2;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 4))) = (int)((uint32_t)uVar2 >> 0x20);
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 8))) = uVar3;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0xc))) = uVar4;
      uVar2 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)__src + 0x10)));
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)__src + 0x18)));
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)__src + 0x1c)));
      __src = (uint32_t)((int)__src + 0x20);
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x10))) = (int)uVar2;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x14))) = (int)((uint32_t)uVar2 >> 0x20);
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x18))) = uVar3;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x1c))) = uVar4;
      puVar6 = puVar6 + 0x20;  // T* element step
    } while (0x1f < __n);
    for (; 7 < __n; __n = (size_t)((int)__n + -8)) {
      uVar2 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)__src));
      __src = (uint32_t)((int)__src + 8);
      (*(uint64_t*)rtguest::G((uint32_t)puVar6)) = uVar2;
      puVar6 = puVar6 + 0x8;  // T* element step
    }
  }
  iVar5 = (int)__n;
joined_r0x0027e710:
  while (iVar5 = iVar5 + -1, iVar5 != -1) {
    uVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)__src));
    __src = (uint32_t)((int)__src + 1);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)puVar6)) = uVar1;
    puVar6 = (uint32_t)((int)puVar6 + 1);
  }
  return __dest;
}


RT_GUEST_FN(0x0027e68c, guest::memcpy)

}  // namespace guest
