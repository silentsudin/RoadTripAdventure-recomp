// guest:0x0023eca0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SubmitEightTexturePackets(uint32_t param_1, ...) {
  ushort uVar1;
  int iVar2;
  int iVar3;
uint32_t piVar4;
uint32_t puVar5;
  int iVar6;
  int iVar7;
  
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) == 1) {
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0;
  }
  piVar4 = 0x002a2e10;
  puVar5 = 0x002a34b0;
  iVar7 = 0;
  do {
    if (iVar7 == 5) {
      iVar6 = (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) * (int)(*(int16_t*)rtguest::G(0x2a2eca));
      if (iVar6 < 0) {
        iVar6 = iVar6 + 0xf;
      }
      (*(uint32_t*)rtguest::G(0x2a35f0)) = (long)(*(uint32_t*)rtguest::G(0x2a2eb0)) | (long)(*(uint32_t*)rtguest::G(0x2a2ebc)) << 0x38 |
                     (long)(*(uint32_t*)rtguest::G(0x2a2eb4)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2a2ebc)) << 0x18 |
                     (long)(*(uint32_t*)rtguest::G(0x2a2eb8)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2a2eb8)) << 0x30;
      (*(uint32_t*)rtguest::G(0x2a3610)) = (long)(*(uint32_t*)rtguest::G(0x2a2ec0)) | (long)(*(uint32_t*)rtguest::G(0x2a2ec4)) << 0x20;
      (*(uint32_t*)rtguest::G(0x2a3600)) = ((long)(iVar6 >> 4) & (uint32_t)(uint32_t)(*(uint32_t*)rtguest::G(0x002a2ece))) << 0x10;
      AdvanceOscillatingValue(param_1 + 2);
    }
    else {
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) * (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar4 + 0x1a)));
      uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar4 + 0x1e)));
      if (iVar6 < 0) {
        iVar6 = iVar6 + 0xf;
      }
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(5) * sizeof(int32_t)));
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(4) * sizeof(int32_t)));
      (*(uint64_t*)rtguest::G((uint32_t)puVar5)) = (long)(*(int32_t*)rtguest::G((uint32_t)piVar4)) | (long)(*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(3) * sizeof(int32_t))) << 0x38 |
                (long)(*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(1) * sizeof(int32_t))) << 0x20 | (long)(*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(3) * sizeof(int32_t))) << 0x18 |
                (long)(*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(2) * sizeof(int32_t))) << 0x10 | (long)(*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(2) * sizeof(int32_t))) << 0x30;
      (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(2) * sizeof(uint64_t))) = ((long)(iVar6 >> 4) & (uint32_t)uVar1) << 0x10;
      (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(4) * sizeof(uint64_t))) = (long)iVar3 | (long)iVar2 << 0x20;
    }
    iVar7 = iVar7 + 1;
    puVar5 = puVar5 + 0x40;  // T* element step
    piVar4 = piVar4 + 0x20;  // T* element step
  } while (iVar7 < 8);
  (*(uint64_t*)rtguest::G(0x2a34a0)) = (*(uint64_t*)rtguest::G(0x2a34a0)) & 0xffffffffffff8000 | 0x20;
  gifChainEndTag(0x2a3490,0x21);
  sendTexturePackets(0x2a3490);
  return 0;
}


RT_GUEST_FN(0x0023eca0, guest::SubmitEightTexturePackets)

}  // namespace guest
