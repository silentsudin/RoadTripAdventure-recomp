// guest:0x00286758 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t fpFormatFloatSci(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
  }
  long lVar1;
  undefined8 uVar2;
  int iVar3;
  
  iVar3 = 0;
  lVar1 = dpcmp(param_1,0);
  if (lVar1 < 0) {
    param_1 = RT_CALLRA(0x286788, guest::dpsub(0,param_1));
    rtCall((*(uint32_t*)rtguest::G(0x002e98f8)), 0x2d);  // PTR_ global holds the sink fn address
  }
  lVar1 = dpcmp(param_1,0x3fb999999999999a);
  if (lVar1 < 0) {
    while (lVar1 = dpcmp(param_1,0x3fb999999999999a), lVar1 < 0) {
      iVar3 = iVar3 + -1;
      param_1 = RT_CALLRA(0x2867d0, guest::dpmul(param_1,0x4024000000000000));
    }
  }
  else {
    lVar1 = dpcmp(param_1,0x3ff0000000000000);
    if (-1 < lVar1) {
      while (lVar1 = dpcmp(param_1,0x3ff0000000000000), -1 < lVar1) {
        iVar3 = iVar3 + 1;
        param_1 = RT_CALLRA(0x286828, guest::dpdiv(param_1,0x4024000000000000));
      }
    }
  }
  uVar2 = RT_CALLRA(0x286854, guest::dpmul(param_1,0x412e848000000000));
  uVar2 = RT_CALLRA(0x28685c, guest::__fixunsdfdi(uVar2));
  uVar2 = RT_CALLRA(0x286864, guest::fpDoubleToInt(uVar2));
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x3325e0)), uVar2);
  if (iVar3 < 0) {
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x3325f0)), iVar3);
    return 0;
  }
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x3325e8)), iVar3);
  return 0;
}


RT_GUEST_FN(0x00286758, guest::fpFormatFloatSci)

}  // namespace guest
