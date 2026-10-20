// guest:0x0023bc90 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t dialogueCmdYesNoMenu(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  DisplayMenuMessage(param_1);  // guest jal keeps $4=task block (Ghidra elides the arg)
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) + 0xd))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = 0x239260;  // sw of func addr (code** store)
  return 0;
}


RT_GUEST_FN(0x0023bc90, guest::dialogueCmdYesNoMenu)

}  // namespace guest
