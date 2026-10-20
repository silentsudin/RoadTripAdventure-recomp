// guest:0x00274198 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadSetActDirect(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(64);
  int iVar1;
uint32_t puVar2;
  undefined4 uVar3;
uint32_t puVar4;
  int iVar5;
  
  iVar1 = scePadGetDmaStr();
  uVar3 = 0;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x72))) == '\x01') {
    iVar5 = 0;
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x01773214 + (int)param_2 * 0x1c + (int)param_1 * 0x70)));
    do {
      puVar2 = (uint32_t)(param_3 + iVar5);
      puVar4 = (uint32_t)(iVar1 + 0xc + iVar5);
      iVar5 = iVar5 + 1;
      (*(uint8_t*)rtguest::G((uint32_t)puVar4)) = (*(uint8_t*)rtguest::G((uint32_t)puVar2));
    } while (iVar5 < 6);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 4))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) = 6;
    _send_to_iop(param_1,param_2);
    uVar3 = 1;
  }
  return uVar3;
}


RT_GUEST_FN(0x00274198, guest::scePadSetActDirect)

}  // namespace guest
