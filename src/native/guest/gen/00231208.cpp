// guest:0x00231208 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceHudElementG(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
    uint32_t uStack_110 = rtguest::allocFrame(8);
    uint32_t uStack_108 = rtguest::allocFrame(8);
    uint32_t uStack_100 = rtguest::allocFrame(8);
    uint32_t uStack_f8 = rtguest::allocFrame(8);
    uint32_t uStack_f0 = rtguest::allocFrame(8);
    uint32_t iStack_e8 = rtguest::allocFrame(4);
    uint32_t iStack_e4 = rtguest::allocFrame(4);
    uint32_t uStack_e0 = rtguest::allocFrame(4);
    uint32_t uStack_dc = rtguest::allocFrame(4);
    uint32_t uStack_d8 = rtguest::allocFrame(4);
    uint32_t uStack_d4 = rtguest::allocFrame(4);
    uint32_t uStack_d0 = rtguest::allocFrame(8);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_c4 = rtguest::allocFrame(4);
    uint32_t uStack_c0 = rtguest::allocFrame(4);
    uint32_t uStack_bc = rtguest::allocFrame(4);
    uint32_t uStack_b8 = rtguest::allocFrame(4);
    uint32_t uStack_b4 = rtguest::allocFrame(4);
    uint32_t uStack_b0 = rtguest::allocFrame(4);
    uint32_t uStack_ac = rtguest::allocFrame(4);
    uint32_t uStack_a8 = rtguest::allocFrame(4);
    uint32_t uStack_a4 = rtguest::allocFrame(4);
    uint32_t uStack_a0 = rtguest::allocFrame(4);
    uint32_t uStack_9c = rtguest::allocFrame(4);
    uint32_t uStack_98 = rtguest::allocFrame(4);
    uint32_t uStack_94 = rtguest::allocFrame(4);
uint32_t auStack_90 = rtguest::allocFrame(16);
    uint32_t iStack_80 = rtguest::allocFrame(4);
    uint32_t iStack_7c = rtguest::allocFrame(4);
uint32_t puVar1;
uint32_t puVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
uint32_t puVar6;
uint32_t puVar7;
  undefined8 uVar8;
  uint uVar9;
  int iVar10;
  undefined8 uVar11;
  undefined8 uVar12;
  int iVar13;
  int iVar14;
uint32_t piVar15;
  undefined4 uVar16;
  
  (*(int32_t*)rtguest::G((uint32_t)iStack_80)) = (int)param_3 + -1;
  if (param_3 != 0) {
    (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) = param_2 + 4;
    do {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_7c)) + -4))) == 1) {
        (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0x1800000020;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_108)) = 0xd800000000;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x2000000070;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0;
        uiArenaSetTexture(param_1,0x29f5f8,1,2);
        uiArenaDataTag(param_1,1);
        uiArenaSprites(param_1,uStack_110,1,0,0,0);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0x4100000062;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_108)) = 0xc2000000a0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x1400000020;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0;
        uiArenaSetTexture(param_1,0x29f5f8,1,1);
        uiArenaDataTag(param_1,1);
        uiArenaSprites(param_1,uStack_110,1,0,0,0);
        uVar16 = (*(uint32_t*)rtguest::G(0x1780a2c));
        uVar3 = IntToPackedBCD((*(uint32_t*)rtguest::G(0x1780a2c)));
        iVar4 = CountDecimalDigits(uVar16);
        uiArenaSetTexture(param_1,0x29f5f8,1,1);
        uiArenaDataTag(param_1,iVar4);
        if (0 < iVar4) {
          iVar13 = 0x52;
          do {
            uVar5 = uVar3 & 0xf;
            uVar3 = (int)uVar3 >> 4;
            iVar4 = iVar4 + -1;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = CONCAT44(0x41,iVar13);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_108)) = CONCAT44(0xc2,uVar5 << 4);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x1400000010;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0;
            uiArenaSprites(param_1,uStack_110,1,0,0,0);
            iVar13 = iVar13 + -0xf;
          } while (iVar4 != 0);
        }
        uVar3 = 0;
        iVar4 = 0;
        piVar15 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_7c)) + 0x10);
        do {
          iVar13 = (*(int32_t*)rtguest::G((uint32_t)piVar15));
          uVar5 = 0xaaa;
          if (iVar13 == -1) {
            iVar13 = 3;
          }
          else {
            uVar5 = IntToPackedBCD(iVar13);
            iVar13 = CountDecimalDigits(iVar13);
          }
          uiArenaSetTexture(param_1,0x29f5f8,1,3);
          if (0 < iVar13) {
            uVar16 = 0x42100000;
            iVar14 = 0;
            do {
              uVar9 = uVar5 & 0xf;
              uVar5 = (int)uVar5 >> 4;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_f0 + 0)) = 0;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_f0 + 4)) = 0;
              iVar13 = iVar13 + -1;
              (*(int32_t*)rtguest::G((uint32_t)iStack_e8)) = (uVar9 % 6) * 0x28;
              (*(int32_t*)rtguest::G((uint32_t)iStack_e4)) = (uVar9 / 6) * 0x30 + 0x38;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_dc)) = 0x30;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = 0x28;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = 0;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_d4)) = 0;
              (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_d0)), 0, 0x10));
              (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0x3f800000;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = 0x41f00000;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = 0;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = 0x41f00000;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_98)) = 0;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_94)) = 0x3f800000;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = uVar16;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = uVar16;
              (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_90)), 0, 0x10));
              iVar10 = 0x221 - iVar14;
              iVar14 = iVar14 + 0x1e;
              uiArenaRotQuads(param_1,uStack_f0,1,iVar10,iVar4 + 0x20,0);
            } while (iVar13 != 0);
          }
          uVar3 = uVar3 + 1;
          iVar4 = iVar4 + 0x24;
          piVar15 = piVar15 + 0x4;  // T* element step
        } while (uVar3 < 4);
        puVar1 = uStack_f0;
        puVar2 = 0x002ebf60;
        do {
          puVar6 = puVar2;
          puVar7 = puVar1;
          uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(1) * sizeof(uint64_t)));
          uVar12 = (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(2) * sizeof(uint64_t)));
          uVar8 = (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(3) * sizeof(uint64_t)));
          (*(uint64_t*)rtguest::G((uint32_t)puVar7)) = (*(uint64_t*)rtguest::G((uint32_t)puVar6));
          (*(uint64_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(1) * sizeof(uint64_t))) = uVar11;
          (*(uint64_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(2) * sizeof(uint64_t))) = uVar12;
          (*(uint64_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(3) * sizeof(uint64_t))) = uVar8;
          puVar1 = puVar7 + 4;
          puVar2 = puVar6 + 4;
        } while (puVar6 + 4 != (uint32_t)0x002ebfc0);
        uVar8 = (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(5) * sizeof(uint64_t)));
        (*(uint64_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(4) * sizeof(uint64_t))) = 0;
        (*(uint64_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(5) * sizeof(uint64_t))) = uVar8;
        uiArenaSetTexture(param_1,0x29f5f8,1,3);
        uiArenaRotQuads(param_1,uStack_f0,1,0x23f,0x8c,0);
        uiArenaSetTexture(param_1,0x29f5f8,1,0xb);
        uiArenaDataTag(param_1,3);
        iVar14 = 0x26;
        iVar13 = 1;
        iVar4 = 0x70;
        while( true ) {
          (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = CONCAT44(iVar14,0x1ca);
          iVar14 = iVar14 + 0x24;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_108)) = CONCAT44(0xd8,iVar4);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x1800000018;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0;
          uiArenaSprites(param_1,uStack_110,1,0,0,0);
          if (iVar13 < 0) break;
          iVar13 = iVar13 + -1;
          iVar4 = iVar4 + 0x18;
        }
      }
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_7c)) + 0xc))) == 1) {
        uVar16 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_7c)) + 8))) * 4 + (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) + 0x10)));
        uVar3 = IntToPackedBCD(uVar16);
        iVar4 = CountDecimalDigits(uVar16);
        uiArenaSetTexture(param_1,0x29f5f8,1,3);
        if (0 < iVar4) {
          uVar16 = 0x42c00000;
          iVar13 = iVar4 * 0x28 + 0xf0;
          do {
            uVar5 = uVar3 & 0xf;
            uVar3 = (int)uVar3 >> 4;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_f0 + 0)) = 0;
            iVar4 = iVar4 + -1;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_f0 + 4)) = 0;
            (*(int32_t*)rtguest::G((uint32_t)iStack_e8)) = (uVar5 % 6) * 0x28;
            (*(int32_t*)rtguest::G((uint32_t)iStack_e4)) = (uVar5 / 6) * 0x30 + 0x38;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_dc)) = 0x30;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = 0x28;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = 0;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_d4)) = 0;
            (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_d0)), 0, 0x10));
            (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0x3f800000;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = 0x42a00000;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = 0;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = 0x42a00000;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_98)) = 0;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_94)) = 0x3f800000;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = uVar16;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = uVar16;
            (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_90)), 0, 0x10));
            uiArenaRotQuads(param_1,uStack_f0,1,iVar13,0xa0,0);
            iVar13 = iVar13 + -0x50;
          } while (iVar4 != 0);
        }
      }
      (*(int32_t*)rtguest::G((uint32_t)iStack_80)) = (*(int32_t*)rtguest::G((uint32_t)iStack_80)) + -1;
      (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) + 0x24;
    } while ((*(int32_t*)rtguest::G((uint32_t)iStack_80)) != -1);
  }
  return 0;
}


RT_GUEST_FN(0x00231208, guest::raceHudElementG)

}  // namespace guest
