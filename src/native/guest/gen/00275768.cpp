// guest:0x00275768 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t __kernel_cos(uint64_t param_1, uint64_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x38) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x3c) = 0x00000000u;
    rtguest::setS32(4, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(6, (uint32_t)param_2);  // walk: = param_2
  }
  long lVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  ulong uVar6;
  
  uVar6 = param_1 >> 0x20 & 0x7fffffff;
  if ((uVar6 < 0x3e400000) && (lVar1 = dptoli(), lVar1 == 0)) {
    uVar2 = 0x3ff0000000000000;
  }
  else {
    uVar2 = RT_CALLRA(0x2757e0, guest::dpmul(param_1,param_1));
    uVar3 = RT_CALLRA(0x2757f8, guest::dpmul(uVar2,0xbda8fae9be8838d4));
    uVar3 = RT_CALLRA(0x27580c, guest::dpadd(uVar3,0x3e21ee9ebdb4b1c4));
    uVar3 = RT_CALLRA(0x27581c, guest::dpmul(uVar2,uVar3));
    uVar3 = RT_CALLRA(0x275830, guest::dpadd(uVar3,0xbe927e4f809c52ad));
    uVar3 = RT_CALLRA(0x275840, guest::dpmul(uVar2,uVar3));
    uVar3 = RT_CALLRA(0x275854, guest::dpadd(uVar3,0x3efa01a019cb1590));
    uVar3 = RT_CALLRA(0x275864, guest::dpmul(uVar2,uVar3));
    uVar3 = RT_CALLRA(0x275878, guest::dpadd(uVar3,0xbf56c16c16c15177));
    uVar3 = RT_CALLRA(0x275888, guest::dpmul(uVar2,uVar3));
    uVar3 = RT_CALLRA(0x27589c, guest::dpadd(uVar3,0x3fa555555555554c));
    uVar3 = RT_CALLRA(0x2758ac, guest::dpmul(uVar2,uVar3));
    if (uVar6 < 0x3fd33333) {
      uVar4 = RT_CALLRA(0x2758d8, guest::dpmul(uVar2,0x3fe0000000000000));
      uVar2 = RT_CALLRA(0x2758ec, guest::dpmul(uVar2,uVar3));
      uVar3 = RT_CALLRA(0x275900, guest::dpmul(param_1,param_2));
      uVar2 = RT_CALLRA(0x275910, guest::dpsub(uVar2,uVar3));
      uVar2 = RT_CALLRA(0x275920, guest::dpsub(uVar4,uVar2));
      uVar2 = RT_CALLRA(0x275934, guest::dpsub(0x3ff0000000000000,uVar2));
    }
    else {
      lVar1 = 0x3fd2000000000000;
      if (uVar6 < 0x3fe90001) {
        lVar1 = (long)((int)uVar6 + -0x200000) << 0x20;
      }
      uVar4 = RT_CALLRA(0x27598c, guest::dpmul(uVar2,0x3fe0000000000000));
      uVar4 = RT_CALLRA(0x27599c, guest::dpsub(uVar4,lVar1));
      uVar5 = RT_CALLRA(0x2759b4, guest::dpsub(0x3ff0000000000000,lVar1));
      uVar2 = RT_CALLRA(0x2759c8, guest::dpmul(uVar2,uVar3));
      uVar3 = RT_CALLRA(0x2759dc, guest::dpmul(param_1,param_2));
      uVar2 = RT_CALLRA(0x2759ec, guest::dpsub(uVar2,uVar3));
      uVar2 = RT_CALLRA(0x2759fc, guest::dpsub(uVar4,uVar2));
      uVar2 = RT_CALLRA(0x275a0c, guest::dpsub(uVar5,uVar2));
    }
  }
  return uVar2;
}


RT_GUEST_FN(0x00275768, guest::__kernel_cos)

}  // namespace guest
