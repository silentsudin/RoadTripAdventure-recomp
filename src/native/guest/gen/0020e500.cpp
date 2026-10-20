// guest:0x0020e500 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t debugScreenSave(uint32_t param_1, uint64_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  undefined8 uVar2;
  long lVar3;
  long lVar4;
  
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ea6e0u)));
  uVar2 = debugScreenRead(param_2 ^ 1,param_3);
  lVar3 = sceOpen(param_1,0x202);
  if (lVar3 == -1) {
    fatal(0x2ea6f8,param_1);
  }
  lVar4 = sceWrite(lVar3,uVar2,0xd2014);
  if (lVar4 == -1) {
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ea710u)));
  }
  sceClose(lVar3);
  iVar1 = rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x333390u)));
  return iVar1;
}


RT_GUEST_FN(0x0020e500, guest::debugScreenSave)

}  // namespace guest
