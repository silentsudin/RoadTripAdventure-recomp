// guest:0x0024ea00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ExtractDelimitedToken(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg;
  char cVar1;
  long lVar2;
uint32_t pcVar3;
uint32_t pcVar4;
uint32_t pcVar5;
  char cVar6;
  int iVar7;
  
  iVar7 = 0;
  pcVar5 = param_3;
  pcVar3 = param_1;
  if (param_4 + 1 < 1) {
    cVar6 = (*(int8_t*)rtguest::G((uint32_t)param_2));
  }
  else {
    cVar6 = (*(int8_t*)rtguest::G((uint32_t)param_2));
    pcVar4 = param_3;
    do {
      do {
        while( true ) {
          lVar2 = (long)(int)cVar6;
          if (lVar2 == 0) {
            return (uint32_t)0x0;
          }
          cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar4));
          pcVar4 = pcVar4 + 1;
          if ((long)cVar1 != 0) break;
          param_2 = param_2 + 1;
          cVar6 = (*(int8_t*)rtguest::G((uint32_t)param_2));
          pcVar4 = param_3;
        }
      } while (lVar2 != cVar1);
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)param_3));
      pcVar4 = param_3;
      while ((long)cVar1 != 0) {
        if (lVar2 == cVar1) {
          param_2 = param_2 + 1;
          cVar6 = (*(int8_t*)rtguest::G((uint32_t)param_2));
          pcVar4 = param_3;
        }
        else {
          pcVar4 = pcVar4 + 1;
        }
        lVar2 = (long)(int)cVar6;
        if (lVar2 == 0) {
          return (uint32_t)0x0;
        }
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar4));
      }
      iVar7 = iVar7 + 1;
      pcVar4 = param_3;
    } while (iVar7 < param_4 + 1);
  }
  do {
    while( true ) {
      if ((long)(int)cVar6 == 0) {
        (*(int8_t*)rtguest::G((uint32_t)pcVar3)) = '\0';
        return param_1;
      }
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar5));
      pcVar5 = pcVar5 + 1;
      if ((long)cVar1 != 0) break;
      (*(int8_t*)rtguest::G((uint32_t)pcVar3)) = cVar6;
      param_2 = param_2 + 1;
      cVar6 = (*(int8_t*)rtguest::G((uint32_t)param_2));
      pcVar5 = param_3;
      pcVar3 = pcVar3 + 1;
    }
  } while ((long)(int)cVar6 != (long)cVar1);
  (*(int8_t*)rtguest::G((uint32_t)pcVar3)) = '\0';
  return param_1;
}


RT_GUEST_FN(0x0024ea00, guest::ExtractDelimitedToken)

}  // namespace guest
