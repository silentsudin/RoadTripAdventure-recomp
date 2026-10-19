// guest:0x0023dd30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RecordTaskResult(uint32_t param_1, ...) {
  if ((param_1 != 0) && ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017870c4 + param_1 * 4))) = (*(uint32_t*)rtguest::G(0x335730)), param_1 == 6))
  {
    (*(uint32_t*)rtguest::G(0x17870e0)) = (*(uint32_t*)rtguest::G(0x17870c8)) + (*(uint32_t*)rtguest::G(0x17870dc)) + (*(uint32_t*)rtguest::G(0x17870d4)) + (*(uint32_t*)rtguest::G(0x17870d0)) +
                   (*(uint32_t*)rtguest::G(0x17870cc)) + (*(uint32_t*)rtguest::G(0x17870d8));
    if ((*(uint32_t*)rtguest::G(0x17870e0)) < (*(uint32_t*)rtguest::G(0x1780a10))) {
      (*(uint32_t*)rtguest::G(0x1780a10)) = (*(uint32_t*)rtguest::G(0x17870e0));
    }
  }
  (*(uint32_t*)rtguest::G(0x335730)) = 0;
  (*(int32_t*)rtguest::G(0x335734)) = (int)(char)(*(uint32_t*)rtguest::G((uint32_t)0x2a2b30 + (uint32_t)(param_1) * 4));
  return 0;
}


RT_GUEST_FN(0x0023dd30, guest::RecordTaskResult)

}  // namespace guest
