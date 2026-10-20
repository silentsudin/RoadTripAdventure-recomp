// guest:0x00279588 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSdRemoteCallbackInit(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(80);
uint32_t auStack_50 = rtguest::allocFrame(4);
rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_44 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_3c = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_30 = rtguest::allocFrame(4);
  undefined8 uVar1;
  long lVar2;
uint32_t pcStack_4c;
uint32_t puStack_48;
uint32_t puStack_40;
  
  (*(uint32_t*)rtguest::G(0x2e8984)) = 0;
  (*(uint32_t*)rtguest::G(0x2e8988)) = 0;
  (*(uint32_t*)rtguest::G(0x2e898c)) = 0;
  (*(uint32_t*)rtguest::G(0x2e8990)) = 0;
  (*(uint32_t*)rtguest::G(0x2e8994)) = 0;
  (*(uint32_t*)rtguest::G(0x2e8998)) = 0;
  (*(uint32_t*)rtguest::G(0x2e899c)) = 0;
  (*(uint32_t*)rtguest::G(0x2e89a0)) = 0;
  (*(uint32_t*)rtguest::G(0x2e89a4)) = 0;
  sceSdRemote(1,0xe620);
  pcStack_4c = 0x279660;
  puStack_48 = 0x01773a00;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_44)) = 0x1000;
  puStack_40 = 0x33aaf0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_3c)) = param_1;
  uVar1 = CreateThread(auStack_50);
  (*(uint32_t*)rtguest::G((uint32_t)uStack_30)) = (uint32_t)uVar1;
  lVar2 = StartThread(uVar1,0);
  if (lVar2 < 0) {
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x331da0u)));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_30)) = 0xffffffff;
  }
  return (*(uint32_t*)rtguest::G((uint32_t)uStack_30));;
}


RT_GUEST_FN(0x00279588, guest::sceSdRemoteCallbackInit)

}  // namespace guest
