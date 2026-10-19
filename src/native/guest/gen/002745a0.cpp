// guest:0x002745a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadSetVrefParam(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  int iVar1;
  long lVar2;
  
  (*(uint32_t*)rtguest::G(0x1773504)) = (uint32_t)param_1;
  (*(uint32_t*)rtguest::G(0x1773500)) = 0xb;
  (*(uint32_t*)rtguest::G(0x1773508)) = (uint32_t)param_2;
  (*(uint32_t*)rtguest::G(0x177350c)) = (*(uint64_t*)rtguest::G((uint32_t)param_3));
  (*(uint32_t*)rtguest::G(0x1773514)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 1)));
  lVar2 = sceSifCallRpc(0x17731c0,1,0,0x1773500,0x80,0x1773500,0x80,0);
  if (lVar2 < 0) {
    iVar1 = 0;
  }
  else {
    iVar1 = (*(uint32_t*)rtguest::G(0x177351c));
    if ((*(uint32_t*)rtguest::G(0x177351c)) == 1) {
      scePadSetReqState(param_1,param_2,2);
      iVar1 = (*(uint32_t*)rtguest::G(0x177351c));
    }
  }
  return iVar1;
}


RT_GUEST_FN(0x002745a0, guest::scePadSetVrefParam)

}  // namespace guest
