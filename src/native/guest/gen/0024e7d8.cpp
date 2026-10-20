// guest:0x0024e7d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CopyDialogueTextBounded(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
  }
  char cVar1;
  int iVar2;
uint32_t pcVar3;
  int iVar4;
uint32_t __dest;
  int iVar5;
  
  iVar5 = param_3;
  __dest = param_1;
  do {
    while( true ) {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)param_2));
      if (param_3 == 0) {
        if (cVar1 == '\0') {
          (*(int8_t*)rtguest::G((uint32_t)__dest)) = '\0';
          return param_1;
        }
      }
      else {
        iVar5 = iVar5 + -1;
        if (iVar5 < 0) {
          (*(int8_t*)rtguest::G((uint32_t)__dest)) = '\0';
          return param_1;
        }
      }
      if (-1 < cVar1) break;
      (*(int8_t*)rtguest::G((uint32_t)__dest)) = (*(int8_t*)rtguest::G((uint32_t)param_2));
      pcVar3 = param_2 + 1;
      param_2 = param_2 + 2;
      (*(int8_t*)rtguest::G((uint32_t)__dest + (uint32_t)(1) * sizeof(int8_t))) = (*(int8_t*)rtguest::G((uint32_t)pcVar3));
      __dest = __dest + 2;
    }
    if ((int)cVar1 - 0x30U < 10) {
      (uint32_t)(uintptr_t)(::strncpy((char *)rtguest::G((uint32_t)(__dest)), (char *)rtguest::G((uint32_t)(0x002a9b38)), 2));
      (*(int8_t*)rtguest::G((uint32_t)__dest + (uint32_t)(1) * sizeof(int8_t))) = cVar1 + (*(int8_t*)rtguest::G((uint32_t)__dest + (uint32_t)(1) * sizeof(int8_t))) + -0x30;
    }
    else {
      iVar4 = (int)cVar1;
      if ((int)cVar1 - 0x41U < 0x1a) {
        (uint32_t)(uintptr_t)(::strncpy((char *)rtguest::G((uint32_t)(__dest)), (char *)rtguest::G((uint32_t)(0x002a9b3a)), 2));
        (*(int8_t*)rtguest::G((uint32_t)__dest + (uint32_t)(1) * sizeof(int8_t))) = cVar1 + (*(int8_t*)rtguest::G((uint32_t)__dest + (uint32_t)(1) * sizeof(int8_t))) + -0x41;
      }
      else if (iVar4 - 0x61U < 0x1a) {
        (uint32_t)(uintptr_t)(::strncpy((char *)rtguest::G((uint32_t)(__dest)), (char *)rtguest::G((uint32_t)(0x002a9b3c)), 2));
        (*(int8_t*)rtguest::G((uint32_t)__dest + (uint32_t)(1) * sizeof(int8_t))) = cVar1 + (*(int8_t*)rtguest::G((uint32_t)__dest + (uint32_t)(1) * sizeof(int8_t))) + -0x61;
      }
      else {
        if (iVar4 - 0x20U < 0x10) {
          pcVar3 = (uint32_t)(cVar1 * 2 + 0x2a9afe);
        }
        else {
          if (iVar4 - 0x3aU < 7) {
            iVar4 = iVar4 << 1;
            iVar2 = 0x2a9aea;
          }
          else if (iVar4 - 0x5bU < 6) {
            iVar4 = iVar4 << 1;
            iVar2 = 0x2a9ab6;
          }
          else {
            if (3 < iVar4 - 0x7bU) {
              (uint32_t)(uintptr_t)(::strncpy((char *)rtguest::G((uint32_t)(__dest)), (char *)rtguest::G((uint32_t)(0x00334498)), 2));
              goto LAB_0024e9c8;
            }
            iVar4 = (int)cVar1 << 1;
            iVar2 = 0x2a9a82;
          }
          pcVar3 = (uint32_t)(iVar4 + iVar2);
        }
        (uint32_t)(uintptr_t)(::strncpy((char *)rtguest::G((uint32_t)(__dest)), (char *)rtguest::G((uint32_t)(pcVar3)), 2));
      }
    }
LAB_0024e9c8:
    param_2 = param_2 + 1;
    __dest = __dest + 2;
  } while( true );
}


RT_GUEST_FN(0x0024e7d8, guest::CopyDialogueTextBounded)

}  // namespace guest
