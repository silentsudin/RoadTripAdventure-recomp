// guest:0x00279660 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sdrCBThread(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x70) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x74) = 0x00000000u;
  }
  undefined8 uVar1;
uint32_t auStack_80 = rtguest::allocFrame(32);
uint32_t auStack_60 = rtguest::allocFrame(80);
  
  RT_CALLRA(0x279668, guest::sceSifInitRpc(0));
  uVar1 = RT_CALLRA(0x279670, guest::GetThreadId());
  RT_CALLRA(0x27967c, guest::sceSifSetRpcQueue(auStack_80,uVar1));
  RT_CALLRA(0x2796a8, guest::sceSifRegisterRpc(auStack_60,0xffffffff80000704,0x2796c8,0x1774b00,0,0,auStack_80));
  RT_CALLRA(0x2796b0, guest::sceSifRpcLoop(auStack_80));
  return 0;
}


RT_GUEST_FN(0x00279660, guest::_sdrCBThread)

}  // namespace guest
