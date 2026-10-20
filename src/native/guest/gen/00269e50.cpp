// guest:0x00269e50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t enterShopPartsSellMenu(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = 0x268e40;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) | 2;
  shopPartsSellMenuTask(param_1);  // jalr $3, $4 kept from entry (269e54 move $6,$4)
  return 0;
}


RT_GUEST_FN(0x00269e50, guest::enterShopPartsSellMenu)

}  // namespace guest
