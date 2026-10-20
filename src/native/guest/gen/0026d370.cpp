// guest:0x0026d370 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitDefaultSpawnPhase(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  undefined1 uVar1;
  
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14))) = 81000;
  gs_tasks_spawn(param_1,0x20dc50,1);
  gs_tasks_spawn(param_1,0x20dc50,0x000a0000 + (*(uint8_t*)rtguest::G((uint32_t)0x0177f778 + 2)));
  (*(uint32_t*)rtguest::G(0x177ea54)) = 0x177f77c;
  (*(uint32_t*)rtguest::G(0x177ea50)) = 0x177f760;
  (*(uint32_t*)rtguest::G(0x177ea58)) = 0x0177f798;
  gs_tasks_spawn(param_1,0x20dc50,0x70003);
  uVar1 = (*(uint8_t*)rtguest::G(0x1780a3c));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x21))) = 0;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))) = uVar1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xc))) = 0x00211808;  // &StartTownExplorationTask
  return 0;
}


RT_GUEST_FN(0x0026d370, guest::InitDefaultSpawnPhase)

}  // namespace guest
