// guest:0x00276590 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t __kernel_sin(uint64_t param_1, uint64_t param_2, uint64_t param_3, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(3, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(5, (uint32_t)param_2);  // walk: = param_2
  }
  long lVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  
  if ((0x3e3fffff < (param_1 >> 0x20 & 0x7fffffffU)) || (lVar1 = dptoli(), lVar1 != 0)) {
    uVar2 = RT_CALLRA(0x276608, guest::dpmul(param_1,param_1));
    uVar3 = RT_CALLRA(0x27661c, guest::dpmul(uVar2,param_1));
    uVar4 = RT_CALLRA(0x276634, guest::dpmul(uVar2,0x3de5d93a5acfd57c));
    uVar4 = RT_CALLRA(0x276648, guest::dpadd(uVar4,0xbe5ae5e68a2b9ceb));
    uVar4 = RT_CALLRA(0x276658, guest::dpmul(uVar2,uVar4));
    uVar4 = RT_CALLRA(0x27666c, guest::dpadd(uVar4,0x3ec71de357b1fe7d));
    uVar4 = RT_CALLRA(0x27667c, guest::dpmul(uVar2,uVar4));
    uVar4 = RT_CALLRA(0x276690, guest::dpadd(uVar4,0xbf2a01a019c161d5));
    uVar4 = RT_CALLRA(0x2766a0, guest::dpmul(uVar2,uVar4));
    uVar4 = RT_CALLRA(0x2766b4, guest::dpadd(uVar4,0x3f8111111110f8a6));
    if (param_3 == 0) {
      uVar2 = RT_CALLRA(0x2766dc, guest::dpmul(uVar2,uVar4));
      uVar2 = RT_CALLRA(0x2766f0, guest::dpadd(uVar2,0xbfc5555555555549));
      uVar2 = RT_CALLRA(0x276700, guest::dpmul(uVar3,uVar2));
      param_1 = RT_CALLRA(0x276710, guest::dpadd(param_1,uVar2));
    }
    else {
      uVar5 = RT_CALLRA(0x276720, guest::dpmul(param_2,0x3fe0000000000000));
      uVar4 = RT_CALLRA(0x276734, guest::dpmul(uVar3,uVar4));
      uVar4 = RT_CALLRA(0x276744, guest::dpsub(uVar5,uVar4));
      uVar2 = RT_CALLRA(0x276754, guest::dpmul(uVar2,uVar4));
      uVar2 = RT_CALLRA(0x276764, guest::dpsub(uVar2,param_2));
      uVar3 = RT_CALLRA(0x27677c, guest::dpmul(uVar3,0xbfc5555555555549));
      uVar2 = RT_CALLRA(0x27678c, guest::dpsub(uVar2,uVar3));
      param_1 = RT_CALLRA(0x27679c, guest::dpsub(param_1,uVar2));
    }
  }
  return param_1;
}


RT_GUEST_FN(0x00276590, guest::__kernel_sin)

}  // namespace guest
