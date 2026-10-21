// guest:0x0026d370 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitDefaultSpawnPhase(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::gS[0] = 0x000000000177f760ull;  // walk const
    rtguest::gS[1] = 0x000000000020dc50ull;  // walk const
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(3, (uint32_t)param_2);  // walk: = param_2
  }
  undefined1 uVar1;
  
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14))) = 81000;
  RT_CALLRA(0x26d3b0, guest::gs_tasks_spawn(param_1,0x20dc50,1));
  RT_CALLRA(0x26d3c8, guest::gs_tasks_spawn(param_1,0x20dc50,0x000a0000 + (*(uint8_t*)rtguest::G((uint32_t)0x0177f778 + 2))));
  (*(uint32_t*)rtguest::G(0x177ea54)) = 0x177f77c;
  (*(uint32_t*)rtguest::G(0x177ea50)) = 0x177f760;
  (*(uint32_t*)rtguest::G(0x177ea58)) = 0x0177f798;
  RT_CALLRA(0x26d3f8, guest::gs_tasks_spawn(param_1,0x20dc50,0x70003));
  uVar1 = (*(uint8_t*)rtguest::G(0x1780a3c));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x21))) = 0;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))) = uVar1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xc))) = 0x00211808;  // &StartTownExplorationTask
  return 0;
}


RT_GUEST_FN(0x0026d370, guest::InitDefaultSpawnPhase)

}  // namespace guest
