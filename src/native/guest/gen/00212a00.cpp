// guest:0x00212a00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitMenuElementLayout(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  rtguest::FrameGuard rt_fg(0);
  bool bVar1;
  uint uVar2;
uint32_t puVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  
  (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint32_t))) = 0x2a2;
  (*(uint32_t*)rtguest::G((uint32_t)param_2)) = (*(uint32_t*)rtguest::G((uint32_t)param_2)) | 1;
  uVar7 = 1;
  (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(uint32_t))) = 0xc;
  iVar6 = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(6) * sizeof(uint32_t))) = 2;
  (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(uint32_t))) = 1;
  uVar4 = (*(uint32_t*)rtguest::G((uint32_t)param_3));
  (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(8) * sizeof(uint32_t))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(7) * sizeof(uint32_t))) = uVar4;
  if (0 < param_4) {
    uVar5 = 0x50;
    uVar4 = 0x2e4;
    do {
      param_3 = param_3 + (uint32_t)(1) * sizeof(uint32_t);
      puVar3 = param_2 + (uint32_t)(9) * sizeof(uint32_t);
      bVar1 = param_5 == iVar6;
      (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(10) * sizeof(uint32_t))) = uVar4;
      (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(0xb) * sizeof(uint32_t))) = uVar5;
      (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(0xd) * sizeof(uint32_t))) = uVar7;
      uVar7 = uVar7 + 1;
      (*(uint32_t*)rtguest::G((uint32_t)puVar3)) = (*(uint32_t*)rtguest::G((uint32_t)puVar3)) | 1;
      iVar6 = iVar6 + 1;
      (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(0xc) * sizeof(uint32_t))) = 3;
      uVar2 = (uint32_t)bVar1;
      (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(0xe) * sizeof(uint32_t))) = uVar2;
      uVar5 = uVar5 + 0x38;
      (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(0xf) * sizeof(uint32_t))) = uVar2;
      uVar4 = uVar4 + 0x14;
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)param_3));
      (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(0x11) * sizeof(uint32_t))) = (uint32_t)bVar1;
      (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(0x10) * sizeof(uint32_t))) = uVar2;
      param_2 = puVar3;
    } while (iVar6 < param_4);
  }
  return 0;
}


RT_GUEST_FN(0x00212a00, guest::InitMenuElementLayout)

}  // namespace guest
