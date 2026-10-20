// guest:0x0023fad0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SubmitFiveTexturePacketsC(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
uint32_t puVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
uint32_t piVar10;
uint32_t puVar11;
  int iVar12;
  
  piVar10 = 0x002a3250;
  puVar11 = 0x002a34b0;
  iVar12 = 4;
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
  do {
    iVar12 = iVar12 + -1;
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar10 + (uint32_t)(3) * sizeof(int32_t)));
    iVar9 = iVar2 * (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar10 + 0x1a)));
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)piVar10 + (uint32_t)(1) * sizeof(int32_t)));
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)piVar10 + (uint32_t)(2) * sizeof(int32_t)));
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)piVar10));
    puVar1 = (uint32_t)((int)piVar10 + 0x1e);
    if (iVar9 < 0) {
      iVar9 = iVar9 + 0xf;
    }
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)piVar10 + (uint32_t)(5) * sizeof(int32_t)));
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)piVar10 + (uint32_t)(4) * sizeof(int32_t)));
    piVar10 = piVar10 + 0x20;  // T* element step
    (*(uint64_t*)rtguest::G((uint32_t)puVar11 + (uint32_t)(2) * sizeof(uint64_t))) = ((long)(iVar9 >> 4) & (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar1))) << 0x10;
    (*(uint64_t*)rtguest::G((uint32_t)puVar11)) = (long)iVar6 | (long)iVar3 << 0x38 | (long)iVar4 << 0x20 | (long)iVar3 << 0x18 |
               (long)iVar5 << 0x10 | (long)iVar5 << 0x30;
    (*(uint64_t*)rtguest::G((uint32_t)puVar11 + (uint32_t)(4) * sizeof(uint64_t))) = (long)iVar8 | (long)iVar7 << 0x20;
    puVar11 = puVar11 + 0x40;  // T* element step
  } while (-1 < iVar12);
  (*(uint64_t*)rtguest::G(0x2a34a0)) = (*(uint64_t*)rtguest::G(0x2a34a0)) & 0xffffffffffff8000 | 0x14;
  gifChainEndTag(0x2a3490,0x15);
  sendTexturePackets(0x2a3490);
  return 0;
}


RT_GUEST_FN(0x0023fad0, guest::SubmitFiveTexturePacketsC)

}  // namespace guest
