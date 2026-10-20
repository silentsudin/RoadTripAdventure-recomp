// guest:0x00273bc8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadGetState(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  char cVar1;
  int iVar2;
  
  cVar1 = 'c';
  if ((*(uint32_t*)rtguest::G((uint32_t)0x1773220 + (uint32_t)((int)param_1 * 0x1c + param_2 * 7) * 4)) != 0) {
    iVar2 = scePadGetDmaStr(param_1);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x70)));
    if (cVar1 == '\x06') {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x71))) == '\x02') {
        cVar1 = '\x05';
      }
      else {
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x70)));
      }
    }
  }
  return cVar1;
}


RT_GUEST_FN(0x00273bc8, guest::scePadGetState)

}  // namespace guest
