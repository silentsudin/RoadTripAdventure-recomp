// guest:0x0026dc98 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PrintUiSlotN(uint32_t param_1, ...) {
  undefined8 uVar1;
uint32_t auStack_40 = rtguest::allocFrame(32);
  
  uVar1 = fptodp();
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335670)), param_1, uVar1);
  uiDebugText8(0xa0,(int)param_1 << 3,auStack_40,0xf);
  return 0;
}


RT_GUEST_FN(0x0026dc98, guest::PrintUiSlotN)

}  // namespace guest
