// guest:0x002780a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t cdvd_exit(uint32_t q0, ...) {
  if ((*(int32_t*)rtguest::G(0x2e6f14)) != 0) {
    (*(int32_t*)rtguest::G(0x2e6f54)) = 0xffffffff;
    SignalSema((*(uint32_t*)rtguest::G(0x2e6f20)));
  }
  DeleteSema((*(int32_t*)rtguest::G(0x2e6f28)));
  DeleteSema((*(int32_t*)rtguest::G(0x2e6f2c)));
  DeleteSema((*(uint32_t*)rtguest::G(0x2e6f20)));
  DIntr();
  sceSifRemoveCmdHandler(0xffffffff80000012);
  EIntr();
  return 0;
}


RT_GUEST_FN(0x002780a0, guest::cdvd_exit)

}  // namespace guest
