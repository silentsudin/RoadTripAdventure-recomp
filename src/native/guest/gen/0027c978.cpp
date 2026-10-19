// guest:0x0027c978 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __register_frame_table(uint32_t param_1, ...) {
uint32_t pvVar1;
  
  pvVar1 = malloc(0x18);
  __register_frame_info_table(param_1,pvVar1);
  return 0;
}


RT_GUEST_FN(0x0027c978, guest::__register_frame_table)

}  // namespace guest
