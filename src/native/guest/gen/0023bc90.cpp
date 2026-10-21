// guest:0x0023bc90 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DisplayMenuMessage(uint32_t param_1, uint32_t param_2, ...);

uint32_t dialogueCmdYesNoMenu(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
  }
  RT_CALLRA(0x23bc9c, guest::DisplayMenuMessage(param_1,param_2));  // guest jal keeps $4=task block (Ghidra elides the arg)
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) + 0xd))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = 0x239260;  // sw of func addr (code** store)
  return 0;
}

uint32_t dialogueCmdYesNoMenu(uint32_t param_1, ...) {
  return dialogueCmdYesNoMenu(param_1, 0);
}


RT_GUEST_FN(0x0023bc90, *static_cast<uint32_t (*)(uint32_t, uint32_t, ...)>(&guest::dialogueCmdYesNoMenu))

}  // namespace guest
