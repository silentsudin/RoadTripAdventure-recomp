// guest:0x0023e508 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SubmitAnimatedTextureLayout(uint32_t param_1, ...) {
uint32_t puVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  float fVar9;
  undefined4 uVar10;
  int iVar11;
uint32_t piVar12;
uint32_t puVar13;
  int iVar14;
  undefined4 uVar15;
  undefined4 uVar16;
  undefined4 uVar17;
  float fVar18;
  undefined4 uVar19;
  
  puVar13 = 0x002a34b0;
  piVar12 = 0x002a2c90;
  iVar14 = 2;
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
  do {
    iVar14 = iVar14 + -1;
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar12 + (uint32_t)(3) * sizeof(int32_t)));
    iVar11 = uVar2 * (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar12 + 0x1a)));
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)piVar12 + (uint32_t)(1) * sizeof(int32_t)));
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)piVar12 + (uint32_t)(2) * sizeof(int32_t)));
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)piVar12));
    puVar1 = (uint32_t)((int)piVar12 + 0x1e);
    if (iVar11 < 0) {
      iVar11 = iVar11 + 0xf;
    }
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)piVar12 + (uint32_t)(5) * sizeof(int32_t)));
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)piVar12 + (uint32_t)(4) * sizeof(int32_t)));
    piVar12 = piVar12 + 0x20;  // T* element step
    (*(uint64_t*)rtguest::G((uint32_t)puVar13 + (uint32_t)(2) * sizeof(uint64_t))) = ((long)(iVar11 >> 4) & (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar1))) << 0x10;
    (*(uint64_t*)rtguest::G((uint32_t)puVar13)) = (long)iVar6 | (long)iVar3 << 0x38 | (long)iVar4 << 0x20 | (long)iVar3 << 0x18 |
               (long)iVar5 << 0x10 | (long)iVar5 << 0x30;
    (*(uint64_t*)rtguest::G((uint32_t)puVar13 + (uint32_t)(4) * sizeof(uint64_t))) = (long)iVar8 | (long)iVar7 << 0x20;
    uVar16 = (*(uint32_t*)rtguest::G(0x332e5c));
    fVar9 = (*(float*)rtguest::G(0x332e54));
    puVar13 = puVar13 + 0x40;  // T* element step
  } while (-1 < iVar14);
  fVar18 = (float)(int)((uVar2 & 0xff) - 0x80) * (*(float*)rtguest::G(0x332e54));
  uVar15 = 0x440c0000;
  uVar17 = (*(uint32_t*)rtguest::G(0x332e58));
  uVar19 = (*(uint32_t*)rtguest::G(0x332e60));
  vu0UnitMatrix(0x177eb10);
  uVar10 = (*(uint32_t*)rtguest::G(0x332e64));
  vu0RotMatrixY(fVar18 * 0.0078125,0x177eb10,0x177eb10);
  iVar14 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
  (*(uint32_t*)rtguest::G(0x177eb48)) = uVar16;
  uVar16 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((((iVar14 >> 2) + 0xf) % 0x1e) * 4 + 0x2a36e8)));
  (*(uint32_t*)rtguest::G(0x177eb40)) = uVar15;
  (*(uint32_t*)rtguest::G(0x177eb44)) = uVar17;
  vu0UnitMatrix(0x177eb50);
  vu0RotMatrixY(((float)(iVar14 % 0x160 + -0xb0) * fVar9) / 176.0,0x177eb50,0x177eb50);
  iVar14 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
  (*(uint32_t*)rtguest::G(0x177eb88)) = uVar10;
  uVar17 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(((iVar14 >> 2) % 0x1e) * 4 + 0x2a36e8)));
  (*(uint32_t*)rtguest::G(0x177eb80)) = uVar19;
  (*(uint32_t*)rtguest::G(0x177eb84)) = uVar16;
  vu0UnitMatrix(0x177eb90);
  vu0RotMatrixY(((float)(iVar14 % 0x180 + -0xc0) * fVar9) / 192.0,0x177eb90,0x177eb90);
  (*(uint32_t*)rtguest::G(0x177ebc8)) = uVar10;
  (*(uint64_t*)rtguest::G(0x2a34a0)) = (*(uint64_t*)rtguest::G(0x2a34a0)) & 0xffffffffffff8000 | 0xc;
  (*(uint32_t*)rtguest::G(0x177ebc0)) = uVar19;
  (*(uint32_t*)rtguest::G(0x177ebc4)) = uVar17;
  gifChainEndTag(0x2a3490,0xd);
  sendTexturePackets(0x2a3490);
  return 0;
}


RT_GUEST_FN(0x0023e508, guest::SubmitAnimatedTextureLayout)

}  // namespace guest
