// guest:0x002797a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceMcInit(uint32_t q0, ...) {
    uint32_t uStack_8c = rtguest::allocFrame(4);
    uint32_t uStack_98 = rtguest::allocFrame(4);
    uint32_t uStack_9c = rtguest::allocFrame(4);
  int iVar1;
  long lVar2;
uint32_t auStack_a0 = rtguest::allocFrame(4);
  
  if ((*(int32_t*)rtguest::G(0x2e89bc)) < 0) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_98)) = 1;
    (*(int32_t*)rtguest::G(0x2e89bc)) = CreateSema(auStack_a0);
  }
  sceMcSync(0,0,0);
  WaitSema((*(int32_t*)rtguest::G(0x2e89bc)));
  sceSifInitRpc(0);
  while( true ) {
    lVar2 = sceSifBindRpc(0x1774b40,0xffffffff80000400,0);
    if (lVar2 < 0) {
      ::printf((char *)rtguest::G((uint32_t)(0x331e08u)));
      do {
                    /* WARNING: Do nothing block with infinite loop */
      } while( true );
    }
    if ((*(uint32_t*)rtguest::G(0x1774b64)) != 0) break;
    iVar1 = 0x100000;
    do {
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
  }
  lVar2 = sceSifCallRpc(0x1774b40,0xfe,0,0x1774bc0,0x30,0x1776100,0xc,0);
  SignalSema((*(int32_t*)rtguest::G(0x2e89bc)));
  if (lVar2 < 0) {
    (*(uint32_t*)rtguest::G(0x1774b64)) = 0;
    iVar1 = (int)lVar2 + -100;
  }
  else if ((*(uint32_t*)rtguest::G(0x1776104)) < 0x20a) {
    ::printf((char *)rtguest::G((uint32_t)(0x331e20u)));
    (*(uint32_t*)rtguest::G(0x1774b64)) = 0;
    iVar1 = -0x78;
  }
  else {
    iVar1 = (*(uint32_t*)rtguest::G(0x1776100));
    if ((*(uint32_t*)rtguest::G(0x1776108)) < 0x20e) {
      ::printf((char *)rtguest::G((uint32_t)(0x331e48u)));
      (*(uint32_t*)rtguest::G(0x1774b64)) = 0;
      iVar1 = -0x79;
    }
  }
  return iVar1;
}


RT_GUEST_FN(0x002797a8, guest::sceMcInit)

}  // namespace guest
