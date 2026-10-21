// guest:0x0026ad48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawAdjusterDebug(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x28, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
    rtguest::gS[0] = 0x0000000000335488ull;  // walk const
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  undefined8 uVar1;
uint32_t auStack_40 = rtguest::allocFrame(32);
  
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335470)), (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xb0))));
  RT_CALLRA(0x26ad84, guest::uiDebugText8(0x10,0x18,auStack_40,0xf));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335478)), (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xb2))));
  RT_CALLRA(0x26adac, guest::uiDebugText8(0x10,0x20,auStack_40,0xf));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335480)), (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xb4))));
  RT_CALLRA(0x26add4, guest::uiDebugText8(0x10,0x28,auStack_40,0xf));
  uVar1 = RT_CALLRA(0x26addc, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x90)))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335488)), uVar1);
  RT_CALLRA(0x26ae04, guest::uiDebugText8(0x10,0x30,auStack_40,0xf));
  uVar1 = RT_CALLRA(0x26ae10, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x94)))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335490)), uVar1);
  RT_CALLRA(0x26ae38, guest::uiDebugText8(0x10,0x38,auStack_40,0xf));
  uVar1 = RT_CALLRA(0x26ae44, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x98)))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335498)), uVar1);
  RT_CALLRA(0x26ae68, guest::uiDebugText8(0x10,0x40,auStack_40,0xf));
  return 0;
}


RT_GUEST_FN(0x0026ad48, guest::DrawAdjusterDebug)

}  // namespace guest
