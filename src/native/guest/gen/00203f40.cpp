// guest:0x00203f40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiTextWidth(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  byte bVar1;
  uint uVar2;
uint32_t pbVar3;
  int iVar4;
  
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_1));
  iVar4 = 0;
  do {
    uVar2 = (uint32_t)bVar1;
    if (uVar2 == 0) {
      return iVar4;
    }
    pbVar3 = param_1 + 1;
    if (uVar2 == 0x7e) {
      uVar2 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar3));
LAB_00203f8c:
      pbVar3 = param_1 + 2;
LAB_00203fa0:
      iVar4 = iVar4 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(uVar2 + 0x29a860)));
    }
    else {
      if (uVar2 == 0x5e) {
        uVar2 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar3));
        goto LAB_00203f8c;
      }
      if (uVar2 == 0x2f) {
        uVar2 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar3));
        goto LAB_00203f8c;
      }
      if (uVar2 == 0x60) {
        uVar2 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar3));
        goto LAB_00203f8c;
      }
      if (-1 < (int)(uVar2 << 0x18)) goto LAB_00203fa0;
      iVar4 = iVar4 + 0x14;
      pbVar3 = param_1 + 2;
    }
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar3));
    param_1 = pbVar3;
  } while( true );
}


RT_GUEST_FN(0x00203f40, guest::uiTextWidth)

}  // namespace guest
