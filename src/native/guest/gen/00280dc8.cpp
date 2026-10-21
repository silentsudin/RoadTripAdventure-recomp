// guest:0x00280dc8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// pseudocode: cvt(ptr, long, int, ulong, ptr, ptr, long, int*)
uint32_t cvt(uint32_t param_1, uint64_t param_2, uint32_t param_3, uint64_t param_4, uint32_t param_5, uint32_t param_6, uint64_t param_7, uint32_t param_8, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x50, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x58) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x5c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_5);  // walk: = param_5
    rtguest::setS32(2, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(3, (uint32_t)param_3);  // walk: = param_3
    rtguest::gS[4] = 0x0000000000000003ull;  // walk const
    rtguest::setS32(5, (uint32_t)param_6);  // walk: = param_6
    rtguest::setS32(6, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(7, (uint32_t)param_4);  // walk: = param_4
  }
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
    param_2 = RT_CALLRA(0x280e54, guest::dpsub(0,param_2));
    (*(uint8_t*)rtguest::G((uint32_t)param_5)) = 0x2d;
  }
  else {
    (*(uint8_t*)rtguest::G((uint32_t)param_5)) = 0;
  }
  uVar6 = RT_CALLRA(0x280e80, guest::_dtoa_r(param_1,param_2,uVar6,param_3,param_6,auStack_60,apcStack_5c));
  pcVar5 = (uint32_t)uVar6;
  if (((param_7 != 0x67) && (param_7 != 0x47)) || ((param_4 & 1) != 0)) {
    pcVar4 = pcVar5 + param_3;
    if (param_7 == 0x66) {
      piVar7 = (uint32_t)param_6;
      if ((*(int8_t*)rtguest::G((uint32_t)pcVar5)) == '0') {
        lVar1 = RT_CALLRA(0x280ecc, guest::dpcmp(param_2,0));
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
    lVar1 = RT_CALLRA(0x280ef4, guest::dpcmp(param_2,0));
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
