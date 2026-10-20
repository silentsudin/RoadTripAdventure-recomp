// guest:0x00273d68 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadInfoAct(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(48);
  int iVar1;
  int iVar2;
  
  if ((*(uint32_t*)rtguest::G((uint32_t)0x1773220 + (uint32_t)((int)param_1 * 0x1c + param_2 * 7) * 4)) != 0) {
    iVar1 = scePadGetDmaStr(param_1);
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x72))) != '\x01') {
      return 0;
    }
    if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 100))) < 2) {
      return 0;
    }
    if (param_3 < (long)(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x6a)))) {
      if (param_3 == -1) {
        return (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x6a)));
      }
      iVar2 = (int)param_3;
      if (param_4 == 2) {
        return (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + iVar2 * 4 + 0x31)));
      }
      if (param_4 < 3) {
        if (param_4 != 1) {
          return 0;
        }
        return (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + iVar2 * 4 + 0x30)));
      }
      if (param_4 == 3) {
        return (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + iVar2 * 4 + 0x32)));
      }
      if (param_4 != 4) {
        return 0;
      }
      return (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + iVar2 * 4 + 0x33)));
    }
  }
  return 0;
}


RT_GUEST_FN(0x00273d68, guest::scePadInfoAct)

}  // namespace guest
