// guest:0x00243b38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetupCarouselSlotTransforms(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
  }
  int iVar1;
uint32_t puVar2;
uint32_t puVar3;
  int iVar4;
  int iVar5;
  
  iVar5 = 0;
  do {
    puVar3 = (uint32_t)(param_1 + 0x5c);
    iVar4 = 3;
    iVar1 = ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2a8bf0u + iVar5))) * 0x18 -
            (int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2a8bf0u + iVar5)))) * 8;
    RT_CALLRA(0x243bd4, guest::gifSetTexRegs(param_1,(long)(iVar1 + 0x285e) | 0x21310000U | (long)(iVar1 + 0x2912) << 0x25 |
                          0x2000000600000000,5));
    RT_CALLRA(0x243bdc, guest::gifTexQuadTag(param_1 + 0x20));
    puVar2 = 0x002a8aa0;
    iVar1 = param_1;
    do {
      iVar1 = iVar1 + 0x30;
      iVar4 = iVar4 + -1;
      RT_CALLRA(0x243bf8, guest::vu0CopyVector(iVar1,puVar2));
      (*(uint32_t*)rtguest::G((uint32_t)puVar3)) = 0xff0;
      puVar3 = puVar3 + 0x30;  // T* element step
      puVar2 = puVar2 + 0x10;
    } while (-1 < iVar4);
    iVar5 = iVar5 + 1;
    param_1 = param_1 + 0xf0;
  } while (iVar5 < 0x10);
  return 0;
}


RT_GUEST_FN(0x00243b38, guest::SetupCarouselSlotTransforms)

}  // namespace guest
