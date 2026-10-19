// guest:0x00256a78 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SlideInPaintEditorWindow(uint32_t param_1, ...) {
  short sVar1;
  
  sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) + 4;
  (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = sVar1;
  uiSpriteWrite(0x175b5e0,0x178,sVar1,0x120,0x140,0xd6,0x38);
  if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = (*(uint8_t*)rtguest::G(0x256640));
  }
  vifDirect(0x175b570,0xc);
  return 0;
}


RT_GUEST_FN(0x00256a78, guest::SlideInPaintEditorWindow)

}  // namespace guest
