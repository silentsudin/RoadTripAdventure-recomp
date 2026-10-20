// guest:0x002660f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawStampNumberColumn(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(352);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x100) = (uint32_t)(int32_t)param_2;
  }
    uint32_t uStack_160 = rtguest::allocFrame(4);
    uint32_t uStack_15c = rtguest::allocFrame(4);
    uint32_t uStack_158 = rtguest::allocFrame(4);
    uint32_t uStack_154 = rtguest::allocFrame(4);
    uint32_t uStack_150 = rtguest::allocFrame(4);
    uint32_t uStack_14c = rtguest::allocFrame(4);
    uint32_t uStack_148 = rtguest::allocFrame(4);
    uint32_t uStack_144 = rtguest::allocFrame(4);
uint32_t aiStack_140 = rtguest::allocFrame(12);
    uint32_t uStack_134 = rtguest::allocFrame(4);
    uint32_t uStack_130 = rtguest::allocFrame(4);
    uint32_t uStack_12c = rtguest::allocFrame(4);
    uint32_t uStack_128 = rtguest::allocFrame(4);
    uint32_t uStack_124 = rtguest::allocFrame(4);
    uint32_t uStack_120 = rtguest::allocFrame(8);
    uint32_t uStack_118 = rtguest::allocFrame(8);
    uint32_t uStack_110 = rtguest::allocFrame(8);
    uint32_t uStack_108 = rtguest::allocFrame(8);
uint32_t auStack_100 = rtguest::allocFrame(112);
    uint32_t uStack_90 = rtguest::allocFrame(8);
    uint32_t uStack_88 = rtguest::allocFrame(8);
    uint32_t iStack_80 = rtguest::allocFrame(4);
    uint32_t iStack_7c = rtguest::allocFrame(4);
    uint32_t iStack_78 = rtguest::allocFrame(4);
    uint32_t uStack_74 = rtguest::allocFrame(4);
    uint32_t uStack_70 = rtguest::allocFrame(4);
    uint32_t uStack_6c = rtguest::allocFrame(4);
    uint32_t uStack_68 = rtguest::allocFrame(4);
    uint32_t uStack_64 = rtguest::allocFrame(4);
    uint32_t uStack_60 = rtguest::allocFrame(4);
    uint32_t iStack_5c = rtguest::allocFrame(4);
    uint32_t iStack_58 = rtguest::allocFrame(4);
  undefined4 uVar1;
uint32_t puVar2;
uint32_t puVar3;
  uint uVar4;
  long lVar5;
  uint uVar6;
uint32_t puVar7;
uint32_t puVar8;
  undefined8 uVar9;
  undefined8 uVar10;
  undefined8 uVar11;
uint32_t puVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  
  (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = param_2;
  uiArenaSetTexture(param_1,0x2dbb50,3,0);
  (*(uint32_t*)rtguest::G((uint32_t)uStack_15c)) = 0x2a;
  uVar4 = (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) & 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_150)) = 0xffffff4c;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_160)) = 0x1a4;
  if (uVar4 == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_150)) = 0xb4;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_160)) = 0x28;
  }
  iVar15 = (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) + 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_14c)) = 0x68;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_158)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_154)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_148)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_144)) = 0;
  uiArenaDataTag(param_1,1);
  uiArenaSprites(param_1,uStack_160,1,0,0,2);
  iVar16 = 0x214;
  if (uVar4 == 0) {
    iVar16 = 0x58;
  }
  uVar4 = IntToPackedBCD(iVar15);
  iVar15 = CountDecimalDigits(iVar15);
  uiArenaDataTag(param_1,iVar15);
  if (0 < iVar15) {
    iVar16 = iVar16 + iVar15 * 10 + -10;
    do {
      uVar6 = uVar4 & 0xf;
      uVar4 = (int)uVar4 >> 4;
      iVar15 = iVar15 + -1;
      (*(int32_t*)rtguest::G((uint32_t)aiStack_140 + (uint32_t)(2) * 4)) = uVar6 * 0x14;
      (*(int32_t*)rtguest::G((uint32_t)aiStack_140 + (uint32_t)(1) * 4)) = 0x4e;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_134)) = 0x6c;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_130)) = 0x14;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_12c)) = 0x14;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_128)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_124)) = 0;
      (*(int32_t*)rtguest::G((uint32_t)aiStack_140 + (uint32_t)(0) * 4)) = iVar16;
      uiArenaSprites(param_1,aiStack_140,1,0,0,2);
      iVar16 = iVar16 + -0x14;
    } while (iVar15 != 0);
  }
  iVar16 = 4;
  (*(int32_t*)rtguest::G((uint32_t)iStack_5c)) = ((int)(*(uint32_t*)rtguest::G((uint32_t)uStack_60)) % 2) * 5;
  uiArenaDataTag(param_1,5);
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dbd34 + (*(int32_t*)rtguest::G((uint32_t)iStack_5c)) * 0xc)));
  puVar12 = (uint32_t)(0x002dbd38 + (*(int32_t*)rtguest::G((uint32_t)iStack_5c)) * 0xc);
  while( true ) {
    iVar16 = iVar16 + -1;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_120)) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_118)) = 0xb4;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0x420000005e;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_108)) = 0;
    uiArenaSprites(param_1,uStack_120,1,uVar1,(*(uint32_t*)rtguest::G((uint32_t)puVar12)),2);
    if (iVar16 < 0) break;
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar12 + (uint32_t)(2) * sizeof(uint32_t)));
    puVar12 = puVar12 + 0xc;  // T* element step
  }
  iVar15 = 0;
  (*(int32_t*)rtguest::G((uint32_t)iStack_58)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) << 2;
  iVar16 = (*(int32_t*)rtguest::G((uint32_t)iStack_5c)) * 0xc;
  do {
    iVar17 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dbd34 + iVar16)));
    iVar13 = (*(int32_t*)rtguest::G((uint32_t)iStack_58)) + (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) + iVar15 + 1;
    iVar14 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dbd38 + iVar16)));
    uVar4 = IntToPackedBCD(iVar13);
    iVar13 = CountDecimalDigits(iVar13);
    uiArenaDataTag(param_1,iVar13);
    if (0 < iVar13) {
      iVar17 = iVar17 + iVar13 * 10 + 0x1b;
      do {
        uVar6 = uVar4 & 0xf;
        uVar4 = (int)uVar4 >> 4;
        iVar13 = iVar13 + -1;
        (*(int32_t*)rtguest::G((uint32_t)iStack_78)) = uVar6 * 0x14;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) = 0x6c;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = 0x14;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = 0x14;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = 0;
        (*(int32_t*)rtguest::G((uint32_t)iStack_80)) = iVar17;
        (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) = iVar14 + 0x46;
        uiArenaSprites(param_1,iStack_80,1,0,0,2);
        iVar17 = iVar17 + -0x14;
      } while (iVar13 != 0);
    }
    iVar15 = iVar15 + 1;
    iVar16 = iVar16 + 0xc;
  } while (iVar15 < 5);
  iVar16 = (*(int32_t*)rtguest::G((uint32_t)iStack_58)) + (*(uint32_t*)rtguest::G((uint32_t)uStack_60));
  iVar15 = (*(int32_t*)rtguest::G((uint32_t)iStack_5c)) * 0xc;
  iVar17 = 0;
  do {
    iVar14 = iVar16 + iVar17;
    lVar5 = TestFlagFc68(iVar14 + 1);
    iVar14 = iVar14 * 8;
    if (lVar5 != 0) {
      (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = (long)(iVar14 + 0x203b) | 0x19408000U | (long)(iVar14 + 0x2042) << 0x25;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_88)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) | 0x2000000d80000000;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) | 0xd80000000;
      puVar2 = auStack_100;
      puVar3 = 0x0032f1c0;
      do {
        puVar8 = puVar3;
        puVar7 = puVar2;
        uVar9 = (*(uint64_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(1) * sizeof(uint64_t)));
        uVar10 = (*(uint64_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(2) * sizeof(uint64_t)));
        uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(3) * sizeof(uint64_t)));
        (*(uint64_t*)rtguest::G((uint32_t)puVar7)) = (*(uint64_t*)rtguest::G((uint32_t)puVar8));
        (*(uint64_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(1) * sizeof(uint64_t))) = uVar9;
        (*(uint64_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(2) * sizeof(uint64_t))) = uVar10;
        (*(uint64_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(3) * sizeof(uint64_t))) = uVar11;
        puVar2 = puVar7 + 4;
        puVar3 = puVar8 + 4;
      } while (puVar8 + 4 != (uint32_t)0x0032f220);
      uVar9 = (*(uint64_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(5) * sizeof(uint64_t)));
      (*(uint64_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(4) * sizeof(uint64_t))) = 0;
      (*(uint64_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(5) * sizeof(uint64_t))) = uVar9;
      uiArenaSetTexture(param_1,uStack_90,0,0);
      uiArenaRotQuads(param_1,auStack_100,1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dbd34 + iVar15))),
                      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002dbd38 + iVar15))),2);
    }
    iVar17 = iVar17 + 1;
    iVar15 = iVar15 + 0xc;
  } while (iVar17 < 5);
  return 0;
}


RT_GUEST_FN(0x002660f8, guest::drawStampNumberColumn)

}  // namespace guest
