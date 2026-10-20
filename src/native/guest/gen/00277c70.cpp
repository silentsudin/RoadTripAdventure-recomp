// guest:0x00277c70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdDelayThread(uint16_t param_1, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000001u;
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
  }
  undefined8 uVar1;
  uint32_t auStack_50 = rtguest::allocFrame(0x18);
  uint32_t uStack_4c = auStack_50 + 4;
  uint32_t uStack_48 = auStack_50 + 8;
  uint32_t uStack_3c = auStack_50 + 0x14;
  
  (*(uint32_t*)rtguest::G((uint32_t)uStack_4c)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_48)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_3c)) = 0;
  uVar1 = RT_CALLRA(0x277c94, guest::CreateSema(auStack_50));
  RT_CALLRA(0x277cac, guest::SetAlarm(param_1,0x277c48,uVar1));
  RT_CALLRA(0x277cb4, guest::WaitSema(uVar1));
  RT_CALLRA(0x277cbc, guest::DeleteSema(uVar1));
  return 0;
}


RT_GUEST_FN(0x00277c70, guest::sceCdDelayThread)

}  // namespace guest
