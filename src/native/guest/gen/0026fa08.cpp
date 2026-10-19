// guest:0x0026fa08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsPutDispEnv(uint32_t param_1, ...) {
  int iVar1;
  
  iVar1 = sceGsGetEnv();
  if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 6))) == 1) {
    (*(uint8_t*)rtguest::G(0x12000000)) = (*(uint64_t*)rtguest::G((uint32_t)param_1));
    (*(uint8_t*)rtguest::G(0x12000070)) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t)));
    (*(uint8_t*)rtguest::G(0x12000080)) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint64_t)));
    (*(uint8_t*)rtguest::G(0x120000c0)) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint64_t)));
  }
  else {
    (*(uint8_t*)rtguest::G(0x12000000)) = (*(uint64_t*)rtguest::G((uint32_t)param_1));
    (*(uint8_t*)rtguest::G(0x12000020)) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t)));
    (*(uint8_t*)rtguest::G(0x12000090)) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t)));
    (*(uint8_t*)rtguest::G(0x120000a0)) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint64_t)));
    (*(uint8_t*)rtguest::G(0x120000e0)) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint64_t)));
  }
  return 0;
}


RT_GUEST_FN(0x0026fa08, guest::sceGsPutDispEnv)

}  // namespace guest
