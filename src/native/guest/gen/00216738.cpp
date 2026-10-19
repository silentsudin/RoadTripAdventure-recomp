// guest:0x00216738 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_options_settings_page(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
    uint32_t uStack_230 = rtguest::allocFrame(4);
    uint32_t uStack_22c = rtguest::allocFrame(4);
    uint32_t iStack_228 = rtguest::allocFrame(4);
    uint32_t iStack_224 = rtguest::allocFrame(4);
    uint32_t uStack_220 = rtguest::allocFrame(4);
    uint32_t uStack_21c = rtguest::allocFrame(4);
    uint32_t uStack_218 = rtguest::allocFrame(4);
    uint32_t uStack_214 = rtguest::allocFrame(4);
uint32_t auStack_210 = rtguest::allocFrame(48);
uint32_t auStack_1e0 = rtguest::allocFrame(16);
uint32_t auStack_1d0 = rtguest::allocFrame(16);
uint32_t auStack_1c0 = rtguest::allocFrame(16);
    uint32_t sStack_1b0 = rtguest::allocFrame(2);
    uint32_t uStack_1ae = rtguest::allocFrame(2);
    uint32_t uStack_1ac = rtguest::allocFrame(2);
    uint32_t uStack_1aa = rtguest::allocFrame(2);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t aiStack_1a0 = rtguest::allocFrame(16);
uint32_t auStack_190 = rtguest::allocFrame(160);
uint32_t auStack_f0 = rtguest::allocFrame(64);
uint32_t auStack_b0 = rtguest::allocFrame(48);
uint32_t auStack_80 = rtguest::allocFrame(16);
uint32_t aiStack_70 = rtguest::allocFrame(16);
uint32_t auStack_60 = rtguest::allocFrame(16);
uint32_t piVar1;
uint32_t piVar2;
  short sVar3;
uint32_t ppuVar4;
  long lVar5;
uint32_t piVar6;
uint32_t puVar7;
uint32_t psVar8;
  undefined8 uVar9;
uint32_t psVar10;
  undefined8 uVar11;
  undefined8 uVar12;
uint32_t piVar13;
  int iVar14;
  uint uVar15;
  int iVar16;
  int iVar17;
  int iVar18;
  int iVar19;
uint32_t puVar20;
  
  iVar18 = (int)param_3;
  puVar20 = (uint32_t)param_4;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)puVar20)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)auStack_60 + (uint32_t)(0) * 4)) = 5;
    InitMenuElementLayout(param_3,iVar18 + 0x820c,auStack_60,0,0);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81d8))) = 5;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81d4))) = 0xfffff0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81cc))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81d0))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81c8))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81c8))) | 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81a4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81a4))) | 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81b0))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81a8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x81ac))) = 0;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0xf))) == '\0') {
    lVar5 = HandleControllerConfigMenuInput(param_1,param_2,param_3,param_4);
    if (lVar5 == 399) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 4))) = 0;
    }
    uVar15 = 0;
    uiDrawPanel9(0x28,0x2c,0x104,0x9c,2);
    uiDrawPanel9(0x154,0x8c,0x104,0x3c,0);
    piVar6 = 0x0029c618;
    piVar13 = puVar20 + 0x34;  // int*[0xd]: +0x34 bytes (addiu 216898)
    DrawOptionSlider((*(uint32_t*)rtguest::G((uint32_t)0x29c618 + (uint32_t)((*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(2) * sizeof(uint32_t))) * 0xf) * 4)) + 0x28,
                     (*(uint32_t*)rtguest::G((uint32_t)0x29c61c + (uint32_t)((*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(2) * sizeof(uint32_t))) * 0xf) * 4)) + 0x58,0x104,(*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(3) * sizeof(uint32_t))));
    iVar16 = (*(int32_t*)rtguest::G((uint32_t)piVar13));
    while( true ) {
      piVar13 = piVar13 + 0x4;  // T* element step
      iVar14 = (*(int32_t*)rtguest::G((uint32_t)piVar6));
      piVar1 = piVar6 + 4;  // int* +1 = +4B
      uVar9 = 0;
      if (iVar16 != 0) {
        uVar9 = 6;
      }
      piVar2 = piVar6 + 0x38;  // lw 2168bc 0x38
      piVar6 = piVar6 + 0x3c;  // T* element step
      uVar15 = uVar15 + 1;
      DrawTextHalfY(iVar14 + 0x3c,(*(int32_t*)rtguest::G((uint32_t)piVar1)) + 0x5a,(*(int32_t*)rtguest::G((uint32_t)piVar2)),uVar9);
      if (0xb < uVar15) break;
      iVar16 = (*(int32_t*)rtguest::G((uint32_t)piVar13));
    }
    piVar6 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(0x29c9b0u + (uint32_t)((*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(2) * sizeof(uint32_t))) * 2 + (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)((*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(2) * sizeof(uint32_t))) + 0xd) * sizeof(uint32_t)))) * 4)));
    if ((*(int32_t*)rtguest::G((uint32_t)piVar6)) != 0) {
      iVar16 = (*(int32_t*)rtguest::G((uint32_t)piVar6));
      iVar14 = 0x124;
      while( true ) {
        piVar13 = piVar6 + 4;  // int* +1 = +4B
        piVar6 = piVar6 + 0x8;  // T* element step
        DrawTextHalfY(0x168,iVar14,iVar16,(*(int32_t*)rtguest::G((uint32_t)piVar13)));
        if ((*(int32_t*)rtguest::G((uint32_t)piVar6)) == 0) break;
        iVar16 = (*(int32_t*)rtguest::G((uint32_t)piVar6));
        iVar14 = iVar14 + 0x18;
      }
    }
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_1d0)), 0, 0x10));
    UpdateSceneLightsAndFog(2,0);
    SetViewDistanceGlobals((*(uint32_t*)rtguest::G(0x332b3c)),(*(uint32_t*)rtguest::G(0x332b40)));
    vu0UnitMatrix(auStack_210);
    buildCamera(0x177e6d0,0x29ca10,auStack_210,auStack_210);
    updateLightDirection(0x177e850);
    uVar15 = (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(2) * sizeof(uint32_t)));
    if (uVar15 < 9) {
      iVar16 = (*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(uVar15 * 0xf + (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(uVar15 + 4) * sizeof(uint32_t)))) * 4)) * 0x14;
      (*(int16_t*)rtguest::G((uint32_t)sStack_1b0)) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x29c544)));
      (*(uint16_t*)rtguest::G((uint32_t)uStack_1ae)) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x29c546)));
      (*(uint16_t*)rtguest::G((uint32_t)uStack_1ac)) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar16 + 0x29c548)));
      (*(uint16_t*)rtguest::G((uint32_t)uStack_1aa)) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x0029c54a + iVar16)));
    }
    else {
      (*(int16_t*)rtguest::G((uint32_t)sStack_1b0)) = 0;
      (*(uint16_t*)rtguest::G((uint32_t)uStack_1ae)) = 0;
      (*(uint16_t*)rtguest::G((uint32_t)uStack_1ac)) = 0;
      (*(uint16_t*)rtguest::G((uint32_t)uStack_1aa)) = 0;
    }
    psVar8 = (uint32_t)(puVar20 + 0x19);
    iVar16 = 0;
    psVar10 = sStack_1b0;
    do {
      sVar3 = (*(int16_t*)rtguest::G((uint32_t)psVar10));
      psVar10 = psVar10 + 0x2;  // T* element step
      if ((*(int16_t*)rtguest::G((uint32_t)psVar8)) < sVar3) {
        (*(int16_t*)rtguest::G((uint32_t)psVar8)) = (*(int16_t*)rtguest::G((uint32_t)psVar8)) + 0x200;
      }
      else if (sVar3 < (*(int16_t*)rtguest::G((uint32_t)psVar8))) {
        (*(int16_t*)rtguest::G((uint32_t)psVar8)) = (*(int16_t*)rtguest::G((uint32_t)psVar8)) + -0x200;
      }
      iVar16 = iVar16 + 1;
      psVar8 = psVar8 + 0x2;  // T* element step
    } while (iVar16 < 3);
    piVar6 = aiStack_1a0;
    psVar8 = (uint32_t)(puVar20 + 0x19);
    iVar16 = 3;
    do {
      sVar3 = (*(int16_t*)rtguest::G((uint32_t)psVar8));
      psVar8 = psVar8 + 0x2;  // T* element step
      iVar16 = iVar16 + -1;
      (*(int32_t*)rtguest::G((uint32_t)piVar6)) = (int)sVar3;
      piVar6 = piVar6 + 0x4;  // T* element step
    } while (-1 < iVar16);
    vu0Itof4Vector(auStack_1c0,aiStack_1a0);
    iVar17 = iVar18 + 0x20;
    vu0ScaleVector((*(float*)rtguest::G(0x332b44)),auStack_1c0,auStack_1c0);
    vifUploadAt4(0x177e6d0);
    uVar15 = 0;
    ResetShowroomLighting();
    vu0UnitMatrix(auStack_210);
    vu0RotMatrix(auStack_210,auStack_210,auStack_1c0);
    vu0CopyVectorXYZ(auStack_1e0,auStack_1d0);
    iVar19 = 0;
    DrawShowroomCarModel(0x177e6d0,auStack_210,3);
    piVar6 = puVar20 + 4;
    SetViewDistanceGlobals(0x45000000,0x45000000);
    uiArenaInit(iVar17,0x8000);
    uiArenaBeginBatch(iVar17,0,0);
    DrawMenuGridBorder(param_3,iVar17,iVar18 + 0x81a4,1);
    DrawMenuGridBackground(param_3,iVar17,iVar18 + 0x81c8,1);
    DrawMenuCursorAndIcons(param_3,iVar17,iVar18 + 0x820c,0xb);
    uiArenaSetTexture(iVar17,0x29c030,4,0);
    uiArenaDataTag(iVar17,9);
    iVar16 = (*(int32_t*)rtguest::G((uint32_t)piVar6));
    iVar14 = 0x5a;
    while( true ) {
      piVar6 = piVar6 + 0x4;  // T* element step
      iVar16 = iVar19 + iVar16;
      iVar19 = iVar19 + 0xf;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_230)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_22c)) = 0;
      uVar15 = uVar15 + 1;
      (*(int32_t*)rtguest::G((uint32_t)iStack_224)) = ((int)(*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(iVar16) * 4)) % 6) * 0x14;
      (*(int32_t*)rtguest::G((uint32_t)iStack_228)) = ((int)(*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(iVar16) * 4)) / 6) * 0x28 + 0x80;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_220)) = 0x28;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_21c)) = 0x14;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_218)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_214)) = 0;
      uiArenaSprites(iVar17,uStack_230,1,0xf0,iVar14,0);
      if (8 < uVar15) break;
      iVar16 = (*(int32_t*)rtguest::G((uint32_t)piVar6));
      iVar14 = iVar14 + 0x18;
    }
    ppuVar4 = (uint32_t)0x002eacc0;
    puVar7 = auStack_190;
    do {
      uVar9 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(ppuVar4 + 2)));
      uVar11 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(ppuVar4 + 4)));
      uVar12 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(ppuVar4 + 6)));
      (*(uint64_t*)rtguest::G((uint32_t)puVar7)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)ppuVar4));
      (*(uint64_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(1) * sizeof(uint64_t))) = uVar9;
      (*(uint64_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(2) * sizeof(uint64_t))) = uVar11;
      (*(uint64_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(3) * sizeof(uint64_t))) = uVar12;
      ppuVar4 = ppuVar4 + 8;
      puVar7 = puVar7 + 0x20;  // T* element step
    } while (ppuVar4 != 0x2ead60);
    uVar15 = 0;
    vu0UnitMatrix(auStack_f0);
    (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = (int)((((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 8))) & 0x3f) - 0x20) * 0x4000000) >> 0x10;
    vu0Itof4Vector(auStack_80,aiStack_70);
    vu0ScaleVector((*(float*)rtguest::G(0x332b48)),auStack_80,auStack_80);
    vu0RotMatrixZ((*(uint32_t*)rtguest::G((uint32_t)auStack_80 + (uint32_t)(0) * 4)),auStack_f0,auStack_f0);
    puVar7 = auStack_190;
    do {
      vu0Itof0Vector(auStack_b0,puVar7);
      vu0ApplyMatrix(auStack_b0,auStack_f0,auStack_b0);
      uVar15 = uVar15 + 1;
      vu0Ftoi0Vector(puVar7,auStack_b0);
      puVar7 = puVar7 + 0x20;  // T* element step
    } while (uVar15 < 5);
    uVar15 = (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(2) * sizeof(uint32_t)));
    if (uVar15 < 9) {
      uiArenaFlatRects(iVar17,auStack_190,5,
                       (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(uVar15 * 0xf + (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(uVar15 + 4) * sizeof(uint32_t)))) * 4)) * 0x14 +
                               0x29c53c))) + 0x1d6,
                       (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0029c540 +
                               (*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(uVar15 * 0xf + (*(uint32_t*)rtguest::G((uint32_t)puVar20 + (uint32_t)(uVar15 + 4) * sizeof(uint32_t)))) * 4)) * 0x14))) + 0xa4,0)
      ;
    }
    uiArenaSubmit(iVar17);
  }
  return 0;
}


RT_GUEST_FN(0x00216738, guest::gs_options_settings_page)

}  // namespace guest
