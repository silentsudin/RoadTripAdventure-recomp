// guest:0x0020e500 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t debugScreenSave(uint32_t param_1, uint64_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  int iVar1;
  undefined8 uVar2;
  long lVar3;
  long lVar4;
  
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ea6e0u)));
  uVar2 = RT_CALLRA(0x20e530, guest::debugScreenRead(param_2 ^ 1,param_3));
  lVar3 = RT_CALLRA(0x20e540, guest::sceOpen(param_1,0x202));
  if (lVar3 == -1) {
    RT_CALLRA(0x20e560, guest::fatal(0x2ea6f8,param_1));
  }
  lVar4 = RT_CALLRA(0x20e574, guest::sceWrite(lVar3,uVar2,0xd2014));
  if (lVar4 == -1) {
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ea710u)));
  }
  RT_CALLRA(0x20e590, guest::sceClose(lVar3));
  iVar1 = rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x333390u)));
  return iVar1;
}


RT_GUEST_FN(0x0020e500, guest::debugScreenSave)

}  // namespace guest
