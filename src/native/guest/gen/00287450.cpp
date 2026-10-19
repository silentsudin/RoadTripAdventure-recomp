// guest:0x00287450 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sifSendRpcReply(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  _sceSifSendCmd(param_1,1,param_2,param_3,param_4,param_5,param_6);
  return 0;
}


RT_GUEST_FN(0x00287450, guest::sifSendRpcReply)

}  // namespace guest
