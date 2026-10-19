// guest:0x0028e670 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t int64ToDouble(int64_t param_1, ...) {
  undefined8 uVar1;
  undefined8 uVar2;
  
  uVar1 = litodp(param_1 >> 0x20);
  uVar1 = dpmul(uVar1,0x40f0000000000000);
  uVar1 = dpmul(uVar1,0x40f0000000000000);
  uVar2 = litodp((int)param_1);
  if ((int)param_1 < 0) {
    uVar2 = dpadd(uVar2,0x41f0000000000000);
  }
  // epilogue is jr $ra after the last dpadd, so $2 (dpadd's double bits) is the return
  return dpadd(uVar1,uVar2);
}


RT_GUEST_FN(0x0028e670, guest::int64ToDouble)

}  // namespace guest
