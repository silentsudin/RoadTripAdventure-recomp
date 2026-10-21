// guest:0x0026e458 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsSyncV(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  ulong uVar1;
uint32_t psVar2;
  uint uVar3;
  long lVar4;
  
  psVar2 = (uint32_t)RT_CALLRA(0x26e464, guest::sceGsGetEnv());
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(psVar2 + 4))) == 0) {
    RT_CALLRA(0x26e47c, guest::gsSyncVClearIntc());
    uVar3 = 1;
    if ((*(int16_t*)rtguest::G((uint32_t)psVar2)) == 1) {
      uVar1 = (*(uint8_t*)rtguest::G(0x12001000));
      uVar3 = (uint32_t)(uVar1 >> 0xd) & 1;
    }
  }
  else {
    lVar4 = RT_CALLRA(0x26e4b8, guest::gsSyncVSetFlagIntc());
    uVar3 = 1;
    if ((*(int16_t*)rtguest::G((uint32_t)psVar2)) == 1) {
      uVar3 = (uint32_t)(lVar4 >> 0xd) & 1;
    }
  }
  return uVar3;
}


RT_GUEST_FN(0x0026e458, guest::sceGsSyncV)

}  // namespace guest
