// guest:0x00279660 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sdrCBThread(uint32_t q0, ...) {
  undefined8 uVar1;
uint32_t auStack_80 = rtguest::allocFrame(32);
uint32_t auStack_60 = rtguest::allocFrame(80);
  
  sceSifInitRpc(0);
  uVar1 = GetThreadId();
  sceSifSetRpcQueue(auStack_80,uVar1);
  sceSifRegisterRpc(auStack_60,0xffffffff80000704,0x2796c8,0x1774b00,0,0,auStack_80);
  sceSifRpcLoop(auStack_80);
  return 0;
}


RT_GUEST_FN(0x00279660, guest::_sdrCBThread)

}  // namespace guest
