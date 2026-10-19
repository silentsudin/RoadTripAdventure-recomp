// guest:0x00279290 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSdRemoteInit(uint32_t q0, ...) {
  int iVar1;
  long lVar2;
  
  sceSifInitRpc(0);
  do {
    lVar2 = sceSifBindRpc(0x17739c0,0xffffffff80000701,0);
    iVar1 = 10000;
    if (lVar2 < 0) {
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x331d78u)));
      return 0xffffffff;
    }
    do {
      iVar1 = iVar1 + -1;
    } while (iVar1 != -1);
  } while ((*(int32_t*)rtguest::G(0x17739e4)) == 0);
  FlushCache(0);
  return 0;
}


RT_GUEST_FN(0x00279290, guest::sceSdRemoteInit)

}  // namespace guest
