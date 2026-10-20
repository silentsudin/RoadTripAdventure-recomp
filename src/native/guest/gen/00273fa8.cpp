// guest:0x00273fa8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadInfoMode(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(48);
  int iVar1;
  
  if ((*(uint32_t*)rtguest::G((uint32_t)0x1773220 + (uint32_t)((int)param_1 * 0x1c + param_2 * 7) * 4)) != 0) {
    iVar1 = scePadGetDmaStr(param_1);
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x72))) != '\x01') {
      return 0;
    }
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x71))) == '\x02') {
      return 0;
    }
    if (param_3 == 2) {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 100))) == '\x01') {
        return 0;
      }
      param_4 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x69)));
LAB_002740bc:
      return (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + param_4 * 2 + 0x50)));
    }
    if (param_3 < 3) {
      if (param_3 != 1) {
        return 0;
      }
      if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x65))) != 0xf3) {
        return (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x65))) >> 4);
      }
    }
    else {
      if (param_3 == 3) {
        if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 100))) == '\x01') {
          return 0;
        }
        return (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x69)));
      }
      if (param_3 != 4) {
        return 0;
      }
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 100))) != '\x01') {
        if (param_4 == 0xffffffff) {
          return (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x68)));
        }
        if ((int)param_4 < (int)(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x68)))) goto LAB_002740bc;
      }
    }
  }
  return 0;
}


RT_GUEST_FN(0x00273fa8, guest::scePadInfoMode)

}  // namespace guest
