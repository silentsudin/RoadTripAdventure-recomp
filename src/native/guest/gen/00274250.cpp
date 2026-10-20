// guest:0x00274250 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadSetActAlign(uint64_t param_1, uint64_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(80);
uint32_t puVar1;
  long lVar2;
uint32_t puVar3;
  int iVar4;
  
  (*(uint32_t*)rtguest::G(0x1773500)) = 8;
  (*(uint32_t*)rtguest::G(0x1773504)) = (uint32_t)param_1;
  iVar4 = 0;
  (*(uint32_t*)rtguest::G(0x1773508)) = (uint32_t)param_2;
  do {
    puVar1 = (uint32_t)(param_3 + iVar4);
    puVar3 = (uint32_t)((int)0x0177350c + iVar4);
    iVar4 = iVar4 + 1;
    (*(uint8_t*)rtguest::G((uint32_t)puVar3)) = (*(uint8_t*)rtguest::G((uint32_t)puVar1));
  } while (iVar4 < 6);
  lVar2 = sceSifCallRpc(0x17731c0,1,0,0x1773500,0x80,0x1773500,0x80,0);
  if (lVar2 < 0) {
    iVar4 = 0;
  }
  else {
    iVar4 = (*(uint32_t*)rtguest::G(0x1773514));
    if ((*(uint32_t*)rtguest::G(0x1773514)) == 1) {
      scePadSetReqState(param_1,param_2,2);
      iVar4 = (*(uint32_t*)rtguest::G(0x1773514));
    }
  }
  return iVar4;
}


RT_GUEST_FN(0x00274250, guest::scePadSetActAlign)

}  // namespace guest
