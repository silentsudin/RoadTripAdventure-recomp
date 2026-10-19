// guest:0x002717b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textGridMoveRect(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, ...) {
  undefined8 uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  
  if ((param_5 <= param_3) && ((param_3 != param_5 || (param_4 <= param_2)))) {
    do {
      param_6 = param_6 - 1;
      uVar1 = textBufferGetPixel16(param_1,(int)param_4 + param_6,(int)param_5 + (param_7 - 1));
      textBufferSetPixel16(param_1,(int)param_2 + param_6,(int)param_3 + (param_7 - 1),uVar1);
    } while( true );
  }
  uVar4 = 0;
  if (param_7 != 0) {
    do {
      uVar3 = 0;
      if (param_6 != 0) {
        do {
          iVar2 = (int)param_2 + uVar3;
          uVar1 = textBufferGetPixel16(param_1,(int)param_4 + uVar3,(int)param_5 + uVar4);
          uVar3 = uVar3 + 1;
          textBufferSetPixel16(param_1,iVar2,(int)param_3 + uVar4,uVar1);
        } while (uVar3 < param_6);
      }
      uVar4 = uVar4 + 1;
    } while (uVar4 < param_7);
  }
  return 0;
}


RT_GUEST_FN(0x002717b0, guest::textGridMoveRect)

}  // namespace guest
