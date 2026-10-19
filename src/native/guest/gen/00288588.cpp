// guest:0x00288588 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifRpcLoop(uint64_t param_1, ...) {
  long lVar1;
  
  do {
    while (lVar1 = sceSifGetNextRequest(param_1), lVar1 == 0) {
      SleepThread();
    }
    sceSifExecRequest(lVar1);
  } while( true );
}


RT_GUEST_FN(0x00288588, guest::sceSifRpcLoop)

}  // namespace guest
