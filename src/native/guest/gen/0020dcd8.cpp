// guest:0x0020dcd8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t debugScreenRead(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  int iVar2;
uint32_t puVar3;
uint32_t auStack_90 = rtguest::allocFrame(112);
  
  (*(uint32_t*)rtguest::G(0x100014)) = 0x000d200c;
  (*(int32_t*)rtguest::G(0x100018)) = (*(uint32_t*)rtguest::G(0x333378));
  iVar2 = 0;
  (*(uint32_t*)rtguest::G(0x10000c)) = 0x10;
  (*(uint32_t*)rtguest::G(0x100010)) = 3;
  iVar1 = 0;
  puVar3 = 0x00100020 + param_2 * 0xf0;
  do {
    iVar2 = iVar2 + 1;
    sceGsSetDefStoreImage(auStack_90,param_1 * 0x8c00000 >> 0x10,10,1,0,iVar1 >> 0x10,0x280,1);
    FlushCache(0);
    sceGsExecStoreImage(auStack_90,puVar3);
    sceGsSyncPath(0,0);
    iVar1 = iVar2 * 0x10000;
    puVar3 = puVar3 + 0x1e0;
  } while (iVar2 < 0xe0);
  return 0x0010000c;
}


RT_GUEST_FN(0x0020dcd8, guest::debugScreenRead)

}  // namespace guest
