// guest:0x0022a168 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t MemoryCardOpenWriteTask(uint32_t param_1, ...) {
  long lVar1;
  int iVar2;
  
  iVar2 = (int)param_1;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x26))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1c)));
  (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x27))) = (char)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1c))) >> 8);
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb7b8u)));
  lVar1 = sceMcOpen((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x26))),0,0x2eb5b0,2);
  if (lVar1 != 0) {
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb6e8u)), lVar1);
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 0xffffffff;
    gs_tasks_kill(param_1);
    return 0;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x228708;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) = 7;
  return 0;
}


RT_GUEST_FN(0x0022a168, guest::MemoryCardOpenWriteTask)

}  // namespace guest
