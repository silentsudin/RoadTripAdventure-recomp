// guest:0x0026e458 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsSyncV(uint32_t q0, ...) {
  ulong uVar1;
uint32_t psVar2;
  uint uVar3;
  long lVar4;
  
  psVar2 = (uint32_t)sceGsGetEnv();
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(psVar2 + 4))) == 0) {
    gsSyncVClearIntc();
    uVar3 = 1;
    if ((*(int16_t*)rtguest::G((uint32_t)psVar2)) == 1) {
      uVar1 = (*(uint8_t*)rtguest::G(0x12001000));
      uVar3 = (uint32_t)(uVar1 >> 0xd) & 1;
    }
  }
  else {
    lVar4 = gsSyncVSetFlagIntc();
    uVar3 = 1;
    if ((*(int16_t*)rtguest::G((uint32_t)psVar2)) == 1) {
      uVar3 = (uint32_t)(lVar4 >> 0xd) & 1;
    }
  }
}


RT_GUEST_FN(0x0026e458, guest::sceGsSyncV)

}  // namespace guest
