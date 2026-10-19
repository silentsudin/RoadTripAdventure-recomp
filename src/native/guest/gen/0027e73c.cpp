// guest:0x0027e73c transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t memmove(uint32_t __dest, uint32_t __src, uint32_t __n, ...) {
  undefined1 uVar1;
uint32_t puVar2;
  undefined8 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iVar6;
uint32_t puVar7;
uint32_t puVar8;
  
  iVar6 = (int)__n;
  if ((__src < __dest) && (puVar7 = (uint32_t)((int)__src + iVar6), __dest < puVar7)) {
    puVar2 = (uint32_t)((int)__dest + iVar6);
    iVar6 = iVar6 + -1;
    if (iVar6 != -1) {
      do {
        puVar7 = puVar7 + -1;
        puVar2 = puVar2 + -1;
        iVar6 = iVar6 + -1;
        (*(uint8_t*)rtguest::G((uint32_t)puVar2)) = (*(uint8_t*)rtguest::G((uint32_t)puVar7));
      } while (iVar6 != -1);
    }
  }
  else {
    puVar8 = __dest;
    if (__n < 0x20) {
      iVar6 = iVar6 + -1;
    }
    else if ((((uint32_t)__src | (uint32_t)__dest) & 0xf) == 0) {
      do {
        uVar3 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)__src));
        uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)__src + 8)));
        uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)__src + 0xc)));
        iVar6 = (int)__n + -0x20;
        __n = (size_t)iVar6;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)puVar8)) = (int)uVar3;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar8 + 4))) = (int)((uint32_t)uVar3 >> 0x20);
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar8 + 1))) = uVar4;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar8 + 0xc))) = uVar5;
        uVar3 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)__src + 0x10)));
        uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)__src + 0x18)));
        uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)__src + 0x1c)));
        __src = (uint32_t)((int)__src + 0x20);
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar8 + 2))) = (int)uVar3;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar8 + 0x14))) = (int)((uint32_t)uVar3 >> 0x20);
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar8 + 3))) = uVar4;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar8 + 0x1c))) = uVar5;
        puVar8 = puVar8 + 4;
      } while (0x1f < __n);
      while (7 < __n) {
        uVar3 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)__src));
        iVar6 = (int)__n + -8;
        __src = (uint32_t)((int)__src + 8);
        (*(uint64_t*)rtguest::G((uint32_t)puVar8)) = uVar3;
        puVar8 = puVar8 + 1;
        __n = (size_t)iVar6;
      }
      iVar6 = iVar6 + -1;
    }
    else {
      iVar6 = iVar6 + -1;
    }
    for (; iVar6 != -1; iVar6 = iVar6 + -1) {
      uVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)__src));
      __src = (uint32_t)((int)__src + 1);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)puVar8)) = uVar1;
      puVar8 = (uint32_t)((int)puVar8 + 1);
    }
  }
}


RT_GUEST_FN(0x0027e73c, guest::memmove)

}  // namespace guest
