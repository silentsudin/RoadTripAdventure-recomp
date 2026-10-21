// guest:0x0026d700 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawRaceHudNumbers(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x48, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x50) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x54) = 0x00000000u;
    rtguest::gS[0] = 0x0000000000335618ull;  // walk const
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  uint uVar1;
  undefined8 uVar2;
  uint uVar3;
  uint uVar4;
uint32_t auStack_60 = rtguest::allocFrame(64);
  
  uVar1 = RT_CALLRA(0x26d720, guest::ComputeGridCellIndex((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xa0))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xa8)))));
  uVar4 = uVar1;
  if ((int)uVar1 < 0) {
    uVar4 = uVar1 + 3;
  }
  uVar3 = uVar1;
  if ((int)uVar1 < 0) {
    uVar3 = uVar1 + 0xf;
  }
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_60)), (char *)rtguest::G((uint32_t)(0x32fb18)), (int)uVar3 >> 4, (int)uVar4 >> 2 & 3, (int)(uVar1 & 2) >> 1 | (uVar1 & 1) << 1);
  RT_CALLRA(0x26d780, guest::uiDebugText8(0x20,0x18,auStack_60,0xf));
  uVar2 = RT_CALLRA(0x26d788, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x90)))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_60)), (char *)rtguest::G((uint32_t)(0x335618)), uVar2);
  RT_CALLRA(0x26d7b0, guest::uiDebugText8(0x20,0x20,auStack_60,0xf));
  uVar2 = RT_CALLRA(0x26d7bc, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x94)))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_60)), (char *)rtguest::G((uint32_t)(0x335620)), uVar2);
  RT_CALLRA(0x26d7e4, guest::uiDebugText8(0x20,0x28,auStack_60,0xf));
  uVar2 = RT_CALLRA(0x26d7f0, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x98)))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_60)), (char *)rtguest::G((uint32_t)(0x335628)), uVar2);
  RT_CALLRA(0x26d814, guest::uiDebugText8(0x20,0x30,auStack_60,0xf));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_60)), (char *)rtguest::G((uint32_t)(0x32fb28)), (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x19c))));
  RT_CALLRA(0x26d83c, guest::uiDebugText8(0x20,0x38,auStack_60,0xf));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_60)), (char *)rtguest::G((uint32_t)(0x32fb38)), (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1f8))));
  RT_CALLRA(0x26d864, guest::uiDebugText8(0x20,0x40,auStack_60,0xf));
  return 0;
}


RT_GUEST_FN(0x0026d700, guest::DrawRaceHudNumbers)

}  // namespace guest
