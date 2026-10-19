// guest:0x002737f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadPortOpen(uint64_t param_1, uint64_t param_2, uint32_t param_3, ...) {
  undefined4 uVar1;
  long lVar2;
uint32_t puVar3;
uint32_t __s;
  int iVar4;
  int iVar5;
  int iVar6;
  
  if (((uint32_t)param_3 & 0x3f) == 0) {
    iVar6 = (int)param_1;
    iVar5 = (int)param_2;
    if ((*(uint32_t*)rtguest::G((uint32_t)0x1773220 + (uint32_t)(iVar6 * 0x1c + iVar5 * 7) * 4)) == 1) {
      if ((*(int32_t*)rtguest::G(0x2e6ec4)) != 0) {
        ::printf((char *)rtguest::G((uint32_t)(0x331268u)), param_1, param_2);
        return 0;
      }
    }
    else {
      iVar4 = 1;
      __s = param_3;
      do {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)__s + 0x58))) = 0;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)__s + 0x70))) = 5;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)__s + 0x71))) = 2;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)__s + 0x67))) = 0;
        iVar4 = iVar4 + -1;
        (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0xff, 0x20));
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)__s + 0x60))) = 0;
        __s = (uint32_t)((int)__s + 0x80);
      } while (-1 < iVar4);
      (*(uint32_t*)rtguest::G(0x1773500)) = 1;
      (*(uint32_t*)rtguest::G(0x1773504)) = iVar6;
      (*(uint32_t*)rtguest::G(0x1773508)) = iVar5;
      (*(uint32_t*)rtguest::G(0x1773510)) = param_3;
      lVar2 = sceSifCallRpc(0x17731c0,1,0,0x1773500,0x80,0x1773500,0x80,0);
      uVar1 = (*(uint32_t*)rtguest::G(0x1773514));
      if (-1 < lVar2) {
        iVar4 = iVar5 * 0x1c + iVar6 * 0x70;
        puVar3 = (uint32_t)(iVar6 * 0x100 + iVar5 * 0x40 + 0x1773300);
        (*(uint32_t*)rtguest::G((uint32_t)0x1773220 + (uint32_t)(iVar6 * 0x1c + iVar5 * 7) * 4)) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)puVar3)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177321c + iVar4))) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01773218 + iVar4))) = uVar1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01773210 + iVar4))) = param_3;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01773214 + iVar4))) = puVar3;
        return (*(uint32_t*)rtguest::G(0x177350c));
      }
    }
  }
  else if ((*(int32_t*)rtguest::G(0x2e6ec4)) != 0) {
    ::printf((char *)rtguest::G((uint32_t)(0x331238u)), param_3);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x002737f8, guest::scePadPortOpen)

}  // namespace guest
