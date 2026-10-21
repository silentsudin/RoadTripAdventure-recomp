// guest:0x0024e7d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CopyDialogueTextBounded(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x40, 8);
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(2, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(3, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(4, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(5, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[6] = 0x00000000002a9afeull;  // walk const
    rtguest::gS[7] = 0x0000000000330000ull;  // walk const
    rtguest::gS[8] = 0x00000000002b0000ull;  // walk const
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
