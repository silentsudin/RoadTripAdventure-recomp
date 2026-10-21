// guest:0x0026dcf8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PrintXyzRow(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0x68);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x58, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x60) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x64) = 0x00000000u;
    rtguest::rtSpillF(rt_fg, 6, rt_sv, 0x78);
    rtguest::rtSpillF(rt_fg, 5, rt_sv, 0x70);
    rtguest::gS[0] = 0x000000000032fba8ull;  // walk const
  }
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
uint32_t auStack_80 = rtguest::allocFrame(64);
  
  uVar1 = RT_CALLRA(0x26dd2c, guest::fptodp());
  uVar2 = RT_CALLRA(0x26dd38, guest::fptodp(param_2));
  uVar3 = RT_CALLRA(0x26dd44, guest::fptodp(param_3));
  uVar4 = RT_CALLRA(0x26dd50, guest::fptodp(param_4));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_80)), (char *)rtguest::G((uint32_t)(0x32fba8)), uVar1, uVar2, uVar3, uVar4);
  RT_CALLRA(0x26dd80, guest::uiDebugText8(0x10,0x50,auStack_80,0xf));
  return 0;
}


RT_GUEST_FN(0x0026dcf8, guest::PrintXyzRow)

}  // namespace guest
