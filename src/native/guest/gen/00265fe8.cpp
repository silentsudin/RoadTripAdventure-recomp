// guest:0x00265fe8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t navStampDigitByDpad(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  ushort uVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = param_2 % 10;
  uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x34)));
  if ((uVar1 & 0x1000) == 0) {
    if ((uVar1 & 0x4000) == 0) {
      if ((uVar1 & 0x8000) == 0) {
        if ((uVar1 & 0x2000) == 0) {
          return param_2;
        }
        iVar3 = param_2 + (char)(*(uint32_t*)rtguest::G((uint32_t)0x2dbd33 + (uint32_t)(iVar3 * 0xc) * 4));
      }
      else {
        iVar3 = param_2 + (char)(*(uint32_t*)rtguest::G((uint32_t)0x2dbd32 + (uint32_t)(iVar3 * 0xc) * 4));
      }
    }
    else {
      iVar3 = param_2 + (char)(*(uint32_t*)rtguest::G((uint32_t)0x2dbd31 + (uint32_t)(iVar3 * 0xc) * 4));
    }
  }
  else {
    iVar3 = param_2 + (char)(*(uint8_t*)rtguest::G((uint32_t)0x2dbd30 + (uint32_t)(iVar3 * 0xc) * 1));
  }
  iVar2 = iVar3;
  if ((iVar3 != param_2) && (iVar2 = param_2, iVar3 == (iVar3 + 100) % 100)) {
    PlayUiSound(0x1d);
    iVar2 = iVar3;
  }
  return iVar2;
}


RT_GUEST_FN(0x00265fe8, guest::navStampDigitByDpad)

}  // namespace guest
