// guest:0x002729e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vu0CopyMatrix(uint32_t param_1, uint32_t param_2, ...) {
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  undefined4 uVar13;
  undefined4 uVar14;
  undefined4 uVar15;
  
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint32_t)));
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(uint32_t)));
  uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(uint32_t)));
  uVar4 = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(uint32_t)));
  uVar5 = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(uint32_t)));
  uVar6 = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(6) * sizeof(uint32_t)));
  uVar7 = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(7) * sizeof(uint32_t)));
  uVar8 = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(8) * sizeof(uint32_t)));
  uVar9 = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(9) * sizeof(uint32_t)));
  uVar10 = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(10) * sizeof(uint32_t)));
  uVar11 = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(0xb) * sizeof(uint32_t)));
  uVar12 = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(0xc) * sizeof(uint32_t)));
  uVar13 = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(0xd) * sizeof(uint32_t)));
  uVar14 = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(0xe) * sizeof(uint32_t)));
  uVar15 = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(0xf) * sizeof(uint32_t)));
  (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_2));
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))) = uVar1;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = uVar2;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t))) = uVar3;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t))) = uVar4;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(uint32_t))) = uVar5;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(6) * sizeof(uint32_t))) = uVar6;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * sizeof(uint32_t))) = uVar7;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(8) * sizeof(uint32_t))) = uVar8;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(9) * sizeof(uint32_t))) = uVar9;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(10) * sizeof(uint32_t))) = uVar10;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xb) * sizeof(uint32_t))) = uVar11;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xc) * sizeof(uint32_t))) = uVar12;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xd) * sizeof(uint32_t))) = uVar13;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xe) * sizeof(uint32_t))) = uVar14;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xf) * sizeof(uint32_t))) = uVar15;
  return 0;
}


RT_GUEST_FN(0x002729e0, guest::vu0CopyMatrix)

}  // namespace guest
