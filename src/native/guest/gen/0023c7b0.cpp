// guest:0x0023c7b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ReleaseStampFlag(uint32_t param_1, ...) {
  char cVar1;
  uint uVar2;
  
  if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x21))) < 0x20) {
    (*(uint32_t*)rtguest::G((uint32_t)0x1780338 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x23)))) * 4)) =
         (*(uint32_t*)rtguest::G((uint32_t)0x1780338 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x23)))) * 4)) & ~(1 << ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x21))) & 0x1f));
    if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x21))) == '\0') {
      uVar2 = (uint32_t)(*(uint16_t*)rtguest::G(0x1780a3a)) & ~(1 << ((int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x23))) & 0x1fU));
      (*(uint16_t*)rtguest::G(0x1780a3a)) = (uint32_t)uVar2;
      if ((uVar2 & 0x25e) == 0) {
        SetProgressFlagAndQueue(0x19);
      }
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x23)));
    }
    else {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x23)));
    }
    if ((cVar1 != '\0') && (cVar1 < '\n')) {
      uVar2 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2a2b20 + (uint32_t)(cVar1) * 4)) + 0xff;
      if ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2a2b20 + (uint32_t)(cVar1) * 4)) != 0) {
        (*(uint64_t*)rtguest::G((uint32_t)0x177fc88 + (uint32_t)((uVar2 & 0xff) >> 6) * 8)) =
             (*(uint64_t*)rtguest::G((uint32_t)0x177fc88 + (uint32_t)((uVar2 & 0xff) >> 6) * 8)) | 1L << ((long)(int)uVar2 & 0x3fU);
      }
    }
  }
  return 0;
}


RT_GUEST_FN(0x0023c7b0, guest::ReleaseStampFlag)

}  // namespace guest
