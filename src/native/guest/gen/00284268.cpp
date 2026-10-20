// guest:0x00284268 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _b2d(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  long lVar2;
  ulong uVar3;
  ulong uVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
uint32_t puVar8;
  uint uVar9;
uint32_t puVar10;
  
  puVar10 = (uint32_t)(param_1 + 0x14);
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
  puVar8 = puVar10 + iVar1 + -1;
  uVar9 = (*(uint32_t*)rtguest::G((uint32_t)puVar8));
  lVar2 = _hi0bits((long)(int)uVar9);
  iVar6 = (int)lVar2;
  (*(int32_t*)rtguest::G((uint32_t)param_2)) = 0x20 - iVar6;
  if (lVar2 < 0xb) {
    uVar5 = 0;
    uVar3 = ((long)(int)(uVar9 >> (0xbU - iVar6 & 0x1f)) | 0x3ff00000U) << 0x20;
    if (puVar10 < puVar8) {
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(iVar1 + -2) * sizeof(uint32_t)));
    }
    uVar4 = (uint32_t)(uVar9 << (iVar6 + 0x15U & 0x1f) | uVar5 >> (0xbU - iVar6 & 0x1f));
  }
  else {
    if (puVar10 < puVar8) {
      puVar8 = puVar10 + iVar1 + -2;
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)puVar8));
    }
    else {
      uVar5 = 0;
    }
    uVar7 = iVar6 - 0xb;
    if (uVar7 == 0) {
      uVar3 = ((long)(int)uVar9 | 0x3ff00000U) << 0x20;
      uVar4 = (uint32_t)uVar5;
    }
    else {
      uVar3 = ((long)(int)(uVar9 << (uVar7 & 0x1f) | uVar5 >> (-uVar7 & 0x1f)) | 0x3ff00000U) <<
              0x20;
      uVar9 = 0;
      if (puVar10 < puVar8) {
        uVar9 = (*(uint32_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(-1) * sizeof(uint32_t)));
      }
      uVar4 = (uint32_t)(uVar5 << (uVar7 & 0x1f) | uVar9 >> (-uVar7 & 0x1f));
    }
  }
  return uVar3 | uVar4;
}


RT_GUEST_FN(0x00284268, guest::_b2d)

}  // namespace guest
