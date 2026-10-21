// guest:0x0026d020 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t HandleMenuScrollSelect(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::gS[1] = 0x000000000177ac50ull;  // walk const
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
  }
  ushort uVar1;
  
  RT_CALLRA(0x26d048, guest::uiFade(0xffffffff80006666));
  RT_CALLRA(0x26d050, guest::DrawWaypointDebugGrid(0x177ac50));
  uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30)));
  if ((uVar1 & 0x1000) != 0) {
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177aea8)) + (uint32_t)(*(int8_t*)rtguest::G(0x177ae9a))))) =
         (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177aea8)) + (uint32_t)(*(int8_t*)rtguest::G(0x177ae9a))))) + '\x01';
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30)));
  }
  if ((uVar1 & 0x4000) != 0) {
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177aea8)) + (uint32_t)(*(int8_t*)rtguest::G(0x177ae9a))))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177aea8)) + (uint32_t)(*(int8_t*)rtguest::G(0x177ae9a))))) + -1
    ;
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30)));
  }
  if ((uVar1 & 0x50) == 0) {
    if ((uVar1 & 0x100) != 0) {
      SaveAIDataFile((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x20))),(*(uint32_t*)rtguest::G(0x177aea8)));
      return 0;
    }
  }
  else {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = 0x26bc08;
  }
  return 0;
}


RT_GUEST_FN(0x0026d020, guest::HandleMenuScrollSelect)

}  // namespace guest
