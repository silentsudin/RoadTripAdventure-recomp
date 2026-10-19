// guest:0x0026d020 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t HandleMenuScrollSelect(uint32_t param_1, uint32_t param_2, ...) {
  ushort uVar1;
  
  uiFade(0xffffffff80006666);
  DrawWaypointDebugGrid(0x177ac50);
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
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = (*(uint8_t*)rtguest::G(0x26bc08));
  }
}


RT_GUEST_FN(0x0026d020, guest::HandleMenuScrollSelect)

}  // namespace guest
