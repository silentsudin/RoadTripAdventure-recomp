// guest:0x002006f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t fatal(uint64_t param_1, uint64_t param_2, uint64_t param_3, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x38, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    rtguest::setS32(2, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(3, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(4, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[5] = 0x0000000000330000ull;  // walk const
    rtguest::gS[6] = 0x0000000000330000ull;  // walk const
    rtguest::gS[7] = 0x0000000000330000ull;  // walk const
  }
uint32_t puVar1;
  char cVar2;
  undefined8 uVar3;
  long lVar4;
  
  (*(uint64_t*)rtguest::G(0x335f70)) = 0x3f80000080a01010;
  (*(uint64_t*)rtguest::G(0x335e00)) = 0x3f80000080a01010;
  FlushCache(0);
  RT_CALLRA(0x20075c, guest::textGridFreeAllSlots());
  uVar3 = RT_CALLRA(0x200770, guest::textGridCreate(0x6d00,0x7a00,0x4b,8));
  do {
    RT_CALLRA(0x200788, guest::textGridClear(uVar3));
    RT_CALLRA(0x20079c, guest::textGridPrintf(uVar3,param_1,param_2,param_3));
    RT_CALLRA(0x2007a4, guest::textGridDraw(uVar3));
    sceGsSyncPath(0,0);
    lVar4 = RT_CALLRA(0x2007b8, guest::sceGsSyncV(0));
    puVar1 = 0x00335ce0;
    if ((*(uint8_t*)rtguest::G(0x335908)) != '\0') {
      puVar1 = 0x00335e50;
    }
    RT_CALLRA(0x2007d8, guest::uiEnvCenterPos(puVar1,0x800,0x800,lVar4 == 0));
    cVar2 = (*(uint8_t*)rtguest::G(0x335908));
    (*(uint8_t*)rtguest::G(0x335908)) = (*(uint8_t*)rtguest::G(0x335908)) + '\x01';
    RT_CALLRA(0x2007f0, guest::sceGsSwapDBuffDc(0x335c80,cVar2));
  } while( true );
}


RT_GUEST_FN(0x002006f0, guest::fatal)

}  // namespace guest
