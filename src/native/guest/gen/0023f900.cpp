// guest:0x0023f900 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SubmitFiveTexturePacketsB(uint32_t param_1, ...) {
  short sVar1;
  short sVar2;
  ushort uVar3;
  int iVar4;
  int iVar5;
uint32_t piVar6;
uint32_t puVar7;
  int iVar8;
  int iVar9;
  
  piVar6 = 0x002a31b0;
  puVar7 = 0x002a34b0;
  sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2)));
  sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2)));
  iVar9 = 0;
  do {
    if (iVar9 == 4) {
      iVar8 = (int)sVar1 * (int)(*(int16_t*)rtguest::G(0x2a324a));
      if (iVar8 < 0) {
        iVar8 = iVar8 + 0xf;
      }
      (*(uint32_t*)rtguest::G(0x2a35b0)) = (long)(*(uint32_t*)rtguest::G(0x2a3230)) | (long)(*(uint32_t*)rtguest::G(0x2a323c)) << 0x38 |
                     (long)(*(uint32_t*)rtguest::G(0x2a3234)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2a323c)) << 0x18 |
                     (long)(*(uint32_t*)rtguest::G(0x2a3238)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2a3238)) << 0x30;
      (*(uint32_t*)rtguest::G(0x2a35d0)) = (long)(*(uint32_t*)rtguest::G(0x2a3240)) | (long)(*(uint32_t*)rtguest::G(0x2a3244)) << 0x20;
      (*(uint32_t*)rtguest::G(0x2a35c0)) = ((long)(iVar8 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2a324e))) << 0x10;
    }
    else {
      iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) * (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar6 + 0x1a)));
      uVar3 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar6 + 0x1e)));
      if (iVar8 < 0) {
        iVar8 = iVar8 + 0xf;
      }
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(5) * sizeof(int32_t)));
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(4) * sizeof(int32_t)));
      (*(uint64_t*)rtguest::G((uint32_t)puVar7)) = (long)(*(int32_t*)rtguest::G((uint32_t)piVar6)) | (long)(*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(3) * sizeof(int32_t))) << 0x38 |
                (long)(*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(1) * sizeof(int32_t))) << 0x20 | (long)(*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(3) * sizeof(int32_t))) << 0x18 |
                (long)(*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(2) * sizeof(int32_t))) << 0x10 | (long)(*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(2) * sizeof(int32_t))) << 0x30;
      (*(uint64_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(2) * sizeof(uint64_t))) = ((long)(iVar8 >> 4) & (uint32_t)uVar3) << 0x10;
      (*(uint64_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(4) * sizeof(uint64_t))) = (long)iVar5 | (long)iVar4 << 0x20;
    }
    iVar9 = iVar9 + 1;
    puVar7 = puVar7 + 0x40;  // T* element step
    piVar6 = piVar6 + 0x20;  // T* element step
  } while (iVar9 < 5);
  (*(uint64_t*)rtguest::G(0x2a34a0)) = (*(uint64_t*)rtguest::G(0x2a34a0)) & 0xffffffffffff8000 | 0x14;
  (*(uint32_t*)rtguest::G(0x2a2be0)) = (float)(int)sVar2 * (*(uint32_t*)rtguest::G(0x332e7c)) + (*(uint32_t*)rtguest::G(0x332e80));
  gifChainEndTag(0x2a3490,0x15);
  sendTexturePackets(0x2a3490);
  return 0;
}


RT_GUEST_FN(0x0023f900, guest::SubmitFiveTexturePacketsB)

}  // namespace guest
