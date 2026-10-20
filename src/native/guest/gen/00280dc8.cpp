// guest:0x00280dc8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// pseudocode: cvt(ptr, long, int, ulong, ptr, ptr, long, int*)
uint32_t cvt(uint32_t param_1, uint64_t param_2, uint32_t param_3, uint64_t param_4, uint32_t param_5, uint32_t param_6, uint64_t param_7, uint32_t param_8, ...) {
  rtguest::FrameGuard rt_fg;
  long lVar1;
uint32_t pcVar2;
  int iVar3;
uint32_t pcVar4;
uint32_t pcVar5;
  undefined8 uVar6;
uint32_t piVar7;
uint32_t auStack_60 = rtguest::allocFrame(4);
uint32_t apcStack_5c = rtguest::allocFrame(12);
  
  uVar6 = 3;
  if ((param_7 != 0x66) && ((param_7 == 0x65 || (uVar6 = 2, param_7 == 0x45)))) {
    param_3 = param_3 + 1;
    uVar6 = 2;
  }
  if (param_2 >> 0x20 < 0) {
    param_2 = dpsub(0,param_2);
    (*(uint8_t*)rtguest::G((uint32_t)param_5)) = 0x2d;
  }
  else {
    (*(uint8_t*)rtguest::G((uint32_t)param_5)) = 0;
  }
  uVar6 = _dtoa_r(param_1,param_2,uVar6,param_3,param_6,auStack_60,apcStack_5c);
  pcVar5 = (uint32_t)uVar6;
  if (((param_7 != 0x67) && (param_7 != 0x47)) || ((param_4 & 1) != 0)) {
    pcVar4 = pcVar5 + param_3;
    if (param_7 == 0x66) {
      piVar7 = (uint32_t)param_6;
      if ((*(int8_t*)rtguest::G((uint32_t)pcVar5)) == '0') {
        lVar1 = dpcmp(param_2,0);
        if (lVar1 != 0) {
          (*(int32_t*)rtguest::G((uint32_t)piVar7)) = 1 - param_3;
        }
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
      }
      else {
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
      }
      pcVar4 = pcVar4 + iVar3;
    }
    lVar1 = dpcmp(param_2,0);
    if (lVar1 == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)apcStack_5c + (uint32_t)(0) * 4)) = pcVar4;
    }
    if ((*(uint32_t*)rtguest::G((uint32_t)apcStack_5c + (uint32_t)(0) * 4)) < pcVar4) {
      do {
        pcVar2 = (*(uint32_t*)rtguest::G((uint32_t)apcStack_5c + (uint32_t)(0) * 4)) + 1;
        (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)apcStack_5c + (uint32_t)(0) * 4)))) = '0';
        (*(uint32_t*)rtguest::G((uint32_t)apcStack_5c + (uint32_t)(0) * 4)) = pcVar2;
      } while (pcVar2 < pcVar4);
      iVar3 = (int)pcVar2 - (int)pcVar5;
      goto LAB_00280f50;
    }
  }
  iVar3 = (int)(*(uint32_t*)rtguest::G((uint32_t)apcStack_5c + (uint32_t)(0) * 4)) - (int)pcVar5;
LAB_00280f50:
  (*(int32_t*)rtguest::G((uint32_t)param_8)) = iVar3;
  return uVar6;
}


RT_GUEST_FN(0x00280dc8, guest::cvt)

}  // namespace guest
