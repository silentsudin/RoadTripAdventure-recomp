// guest:0x00273e88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#define scePadInfoComb scePadInfoComb_decl
#include "guest_decls.inc"
#undef scePadInfoComb

namespace guest {

uint64_t scePadInfoComb(uint64_t param_1, uint32_t param_2, uint64_t param_3, uint64_t param_4, ...) {
  int iVar1;
  ulong uVar2;
  int iVar3;
  
  if ((*(uint32_t*)rtguest::G((uint32_t)0x1773220 + (uint32_t)((int)param_1 * 0x1c + param_2 * 7) * 4)) == 0) {
    uVar2 = 0;
  }
  else {
    iVar1 = scePadGetDmaStr(param_1);
    uVar2 = 0;
    if ((((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x72))) == '\x01') && (uVar2 = 0, 1 < (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 100))))) &&
       (uVar2 = (uint64_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x6b))), param_3 != -1)) {
      if (param_3 < (long)uVar2) {
        iVar3 = (int)param_3;
        if (param_4 == 0) {
          uVar2 = (uint64_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + iVar3 * 4 + 0x41)));
        }
        else if (param_4 < 1) {
          uVar2 = 0;
          if (param_4 == -1) {
            uVar2 = (uint64_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + iVar3 * 4 + 0x40)));
          }
        }
        else if (param_4 == 1) {
          uVar2 = (uint64_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + iVar3 * 4 + 0x42)));
        }
        else {
          uVar2 = 0;
          if (param_4 == 2) {
            uVar2 = (uint64_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + iVar3 * 4 + 0x43)));
          }
        }
      }
      else {
        uVar2 = 0;
      }
    }
  }
  return uVar2;
}


RT_GUEST_FN(0x00273e88, guest::scePadInfoComb)

}  // namespace guest
