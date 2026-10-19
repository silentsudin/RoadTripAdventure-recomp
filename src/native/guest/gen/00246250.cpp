// guest:0x00246250 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t shopMenuScene(uint32_t param_1, uint32_t param_2, ...) {
uint32_t auStack_f0 = rtguest::allocFrame(64);
    uint32_t uStack_b0 = rtguest::allocFrame(8);
    uint32_t uStack_a8 = rtguest::allocFrame(8);
    uint32_t uStack_a0 = rtguest::allocFrame(8);
    uint32_t uStack_98 = rtguest::allocFrame(8);
    uint32_t iStack_90 = rtguest::allocFrame(4);
    uint32_t iStack_8c = rtguest::allocFrame(4);
    uint32_t uStack_88 = rtguest::allocFrame(4);
    uint32_t iStack_84 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t iStack_7c = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t iStack_74 = rtguest::allocFrame(4);
    uint32_t uStack_70 = rtguest::allocFrame(8);
    uint32_t iStack_68 = rtguest::allocFrame(4);
    uint32_t iStack_64 = rtguest::allocFrame(4);
  char cVar1;
  uint uVar2;
  undefined1 uVar3;
  byte bVar4;
  short sVar5;
  undefined2 uVar6;
  int iVar7;
  int iVar8;
  long lVar9;
  ulong uVar10;
  undefined8 uVar11;
  byte bVar12;
  ushort uVar13;
uint32_t puVar14;
  uint uVar15;
  uint uVar16;
  uint uVar17;
  int iVar18;
  uint uVar19;
  ushort uVar20;
  uint uVar21;
  uint uVar22;
  int iVar23;
  undefined8 uVar24;
uint32_t puVar25;
uint32_t puVar26;
  int iVar27;
  int iVar28;
  int iVar29;
  int iVar30;
uint32_t puVar31;
uint32_t puVar32;
  int iVar33;
uint32_t puVar34;
uint32_t psVar35;
  int iVar36;
  float fVar37;
  float fVar38;
uint32_t puStack_80;
uint32_t puStack_78;
  
  iVar30 = (int)param_1;
  (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x28))) = 0x01752780;
    puVar32 = 0x017527e0;
    gifBeginSpriteBatch(0x1752780,0x2003ecce21411efc);
    (*(uint64_t*)rtguest::G(0x1752790)) = 0x3001b;
    (*(uint32_t*)rtguest::G(0x17527d8)) = 0x42;
    (*(uint32_t*)rtguest::G(0x17527d0)) = 0x8000000044;
    gifSetTexRegs(0x17527e0,0x20045dc669329f2e,5);
    (*(int32_t*)rtguest::G((uint32_t)iStack_84)) = 0;
    puVar31 = 0x01752800;
    (*(int32_t*)rtguest::G((uint32_t)iStack_68)) = 0;
    (*(int32_t*)rtguest::G((uint32_t)iStack_64)) = 0;
    puVar34 = puVar32;
    do {
      gifTexQuadTag(puVar31);
      iVar28 = 3;
      puVar14 = puVar32 + (*(int32_t*)rtguest::G((uint32_t)iStack_68));
      puVar26 = (uint32_t)(puVar34 + (*(int32_t*)rtguest::G((uint32_t)iStack_64)) + 0x5c);
      puVar25 = 0x002a8a60;
      do {
        puVar14 = puVar14 + 0x30;
        vu0CopyVector(puVar14,puVar25);
        iVar28 = iVar28 + -1;
        (*(uint32_t*)rtguest::G((uint32_t)puVar26)) = 0xff0;
        puVar26 = puVar26 + 0x30;  // T* element step
        puVar25 = puVar25 + 0x10;
      } while (-1 < iVar28);
      puVar31 = puVar31 + 0xd0;
      puVar32 = puVar32 + 0x80;
      (*(int32_t*)rtguest::G((uint32_t)iStack_84)) = (*(int32_t*)rtguest::G((uint32_t)iStack_84)) + 1;
      (*(int32_t*)rtguest::G((uint32_t)iStack_68)) = (*(int32_t*)rtguest::G((uint32_t)iStack_68)) + 0x50;
      (*(int32_t*)rtguest::G((uint32_t)iStack_64)) = (*(int32_t*)rtguest::G((uint32_t)iStack_64)) + 0x50;
      puVar34 = puVar34 + 0x80;
    } while ((*(int32_t*)rtguest::G((uint32_t)iStack_84)) < 0x10);
    (*(uint32_t*)rtguest::G(0x1757b8c)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x1c))) >> 0x14 & 1;
    (*(uint32_t*)rtguest::G(0x1757b80)) = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x1c)));
    (*(uint32_t*)rtguest::G(0x1757b88)) = ClassifyObjectContext((*(int32_t*)rtguest::G((uint32_t)iStack_90)));
    uVar19 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x1d)));
    (*(uint32_t*)rtguest::G(0x1757b84)) = uVar19;
    if (((*(uint32_t*)rtguest::G(0x1757b88)) & 3) == 0) {
      if ((*(uint32_t*)rtguest::G(0x1757b88)) == 4) {
        (*(uint32_t*)rtguest::G(0x1757b94)) = 0xf;
        (*(uint32_t*)rtguest::G(0x1757b84)) = 0xff;
        (*(uint32_t*)rtguest::G(0x1757b90)) = 0;
        (*(uint32_t*)rtguest::G(0x1757b9c)) = uVar19;
      }
      else {
        (*(uint32_t*)rtguest::G(0x1757b90)) = (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x1e))) & 3;
        (*(uint32_t*)rtguest::G(0x1757b94)) = ~(1 << (*(uint32_t*)rtguest::G(0x1757b90))) & 7;
        (*(uint32_t*)rtguest::G(0x1757b9c)) = CountFlaggedSlots((*(uint32_t*)rtguest::G(0x1757b80)),uVar19,(*(uint32_t*)rtguest::G(0x1757b8c)),(*(uint32_t*)rtguest::G(0x1757b94)));
        (*(uint32_t*)rtguest::G(0x1757b90)) = (*(uint32_t*)rtguest::G(0x1757b90)) + (*(uint32_t*)rtguest::G(0x1757b8c)) * 3;
      }
      lVar9 = CountFlaggedSlots((*(uint32_t*)rtguest::G(0x1757b80)),0xffffffffffffffff,(*(uint32_t*)rtguest::G(0x1757b8c)),(*(uint32_t*)rtguest::G(0x1757b94)));
      (*(uint32_t*)rtguest::G(0x1757b98)) = (uint32_t)lVar9;
      if (lVar9 == 0) goto LAB_00247758;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27))) = (uint32_t)(*(uint32_t*)rtguest::G(0x1757b9c)) & 0xf;
      uVar21 = (*(uint32_t*)rtguest::G(0x1757b98));
      uVar15 = (*(uint32_t*)rtguest::G(0x1757b94));
      uVar17 = (*(uint32_t*)rtguest::G(0x1757b8c));
      uVar19 = (*(uint32_t*)rtguest::G(0x1757b80));
      if ((int)(*(uint32_t*)rtguest::G(0x1757b98)) < 0x11) {
        (*(uint32_t*)rtguest::G(0x1757b9c)) = 0;
      }
      uVar2 = (*(uint32_t*)rtguest::G(0x1757b9c));
      psVar35 = 0x01757ba0;
      puStack_80 = 0x01757ba0;
      if ((int)(*(uint32_t*)rtguest::G(0x1757b98)) < 0x10) {
        iVar28 = 0;
        uVar22 = (*(uint32_t*)rtguest::G(0x1757b9c));
        do {
          iVar28 = iVar28 + 1;
          sVar5 = FindFlaggedSlotIndex(uVar19,uVar17,uVar22,uVar15);
          (*(int16_t*)rtguest::G((uint32_t)psVar35)) = sVar5;
          psVar35 = psVar35 + 0x2;  // T* element step
          if (sVar5 == -1) goto LAB_00246708;
          uVar22 = iVar28 + uVar2;
        } while (iVar28 < 0x10);
      }
      for (uVar22 = uVar2; uVar22 != uVar2 + 8; uVar22 = uVar22 + 1) {
        iVar7 = ((int)uVar22 < 0) * uVar21 + uVar22;
        iVar28 = iVar7 - uVar21;
        if (iVar7 < (int)uVar21) {
          iVar28 = iVar7;
        }
        uVar6 = FindFlaggedSlotIndex(uVar19,uVar17,iVar28,uVar15);
        (*(uint16_t*)rtguest::G((uint32_t)puStack_80 + (uint32_t)(uVar22 & 0xf) * sizeof(uint16_t))) = uVar6;
      }
      uVar22 = uVar2 - 9;
      while (uVar2 = uVar2 - 1, uVar2 != uVar22) {
        iVar7 = ((int)uVar2 < 0) * uVar21 + uVar2;
        iVar28 = iVar7 - uVar21;
        if (iVar7 < (int)uVar21) {
          iVar28 = iVar7;
        }
        uVar6 = FindFlaggedSlotIndex(uVar19,uVar17,iVar28,uVar15);
        (*(uint16_t*)rtguest::G((uint32_t)puStack_80 + (uint32_t)(uVar2 & 0xf) * sizeof(uint16_t))) = uVar6;
      }
    }
    else {
      (*(uint32_t*)rtguest::G(0x1757b90)) = 0;
      lVar9 = countFlaggedSlotsAlt
                        ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_90)) + 0x23))),(*(uint32_t*)rtguest::G(0x1757b80)),0xffffffffffffffff);
      (*(uint32_t*)rtguest::G(0x1757b98)) = (uint32_t)lVar9;
      if (lVar9 == 0) {
LAB_00247758:
        gs_tasks_kill(param_1);
        iVar30 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 4))) = 0xffffffff;
        return 0;
      }
      (*(uint32_t*)rtguest::G(0x1757b9c)) = (*(uint32_t*)rtguest::G(0x1757b84));
      uVar19 = (*(uint32_t*)rtguest::G(0x1757b9c));
      (*(uint8_t*)rtguest::G((uint32_t)0x01757b9c + 0)) = (uint32_t)(*(uint32_t*)rtguest::G(0x1757b84));
      bVar12 = (uint32_t)(*(uint32_t*)rtguest::G(0x1757b9c)) & 0xf;
      (*(uint32_t*)rtguest::G(0x1757b9c)) = uVar19;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27))) = bVar12;
      uVar17 = (*(uint32_t*)rtguest::G(0x1757b98));
      uVar19 = (*(uint32_t*)rtguest::G(0x1757b80));
      if ((int)(*(uint32_t*)rtguest::G(0x1757b98)) < 0x11) {
        (*(uint32_t*)rtguest::G(0x1757b9c)) = 0;
      }
      uVar15 = (*(uint32_t*)rtguest::G(0x1757b9c));
      (*(uint32_t*)rtguest::G(0x1757b84)) = 0xff;
      psVar35 = 0x01757ba0;
      uVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_90)) + 0x23)));
      if ((int)(*(uint32_t*)rtguest::G(0x1757b98)) < 0x10) {
        iVar28 = 0;
        uVar21 = (*(uint32_t*)rtguest::G(0x1757b9c));
        do {
          iVar28 = iVar28 + 1;
          sVar5 = findFlaggedSlotIndexAlt(uVar3,uVar19,uVar21);
          (*(int16_t*)rtguest::G((uint32_t)psVar35)) = sVar5;
          psVar35 = psVar35 + 0x2;  // T* element step
          if (sVar5 == -1) goto LAB_00246708;
          uVar21 = iVar28 + uVar15;
        } while (iVar28 < 0x10);
      }
      for (uVar21 = uVar15; uVar21 != uVar15 + 8; uVar21 = uVar21 + 1) {
        iVar7 = ((int)uVar21 < 0) * uVar17 + uVar21;
        iVar28 = iVar7 - uVar17;
        if (iVar7 < (int)uVar17) {
          iVar28 = iVar7;
        }
        uVar6 = findFlaggedSlotIndexAlt(uVar3,uVar19,iVar28);
        (*(uint32_t*)rtguest::G((uint32_t)0x1757ba0 + (uint32_t)(uVar21 & 0xf) * 4)) = uVar6;
      }
      uVar21 = uVar15 - 9;
      while (uVar15 = uVar15 - 1, uVar15 != uVar21) {
        iVar7 = ((int)uVar15 < 0) * uVar17 + uVar15;
        iVar28 = iVar7 - uVar17;
        if (iVar7 < (int)uVar17) {
          iVar28 = iVar7;
        }
        uVar6 = findFlaggedSlotIndexAlt(uVar3,uVar19,iVar28);
        (*(uint32_t*)rtguest::G((uint32_t)0x1757ba0 + (uint32_t)(uVar15 & 0xf) * 4)) = uVar6;
      }
    }
LAB_00246708:
    if ((int)(*(uint32_t*)rtguest::G(0x1757b98)) < 0x11) {
      iVar28 = (1 << ((*(uint32_t*)rtguest::G(0x1757b98)) & 0x1f)) + -1;
    }
    else {
      iVar28 = 0xffff;
    }
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x20))) = iVar28;
    uVar3 = CountSetBits();
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x26))) = uVar3;
    BuildCarouselSlotPackets(0x1752780,(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_90)) + 10))) >> 1 & 1);
  }
  iVar28 = (*(uint32_t*)rtguest::G(0x1757b8c)) * 2 + 0x30;
  uVar13 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_90)) + iVar28)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = (uint32_t)uVar13;
  uVar20 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_90)) + 4 + iVar28)));
  if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_90)) + 10))) & 2) == 0) {
LAB_002467bc:
    cVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x25)));
  }
  else {
    if ((*(uint32_t*)rtguest::G(0x1757b88)) == 4) {
      iVar28 = ((*(uint32_t*)rtguest::G(0x1757b8c)) ^ 1) * 2 + 0x30;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = (uint32_t)((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_90)) + iVar28))) | uVar13);
      uVar20 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_90)) + 4 + iVar28))) | uVar20;
      goto LAB_002467bc;
    }
    cVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x25)));
  }
  if (cVar1 != '\0') goto LAB_00246bd4;
  bVar12 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x26)));
  (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) = 0;
  if (bVar12 < 2) {
LAB_00246860:
    uVar13 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 2)));
    uVar10 = (uint32_t)uVar13;
LAB_00246868:
    if (uVar13 != 0) goto LAB_00246870;
  }
  else {
    uVar13 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 2)));
    uVar10 = (uint32_t)(short)uVar13;
    if (uVar10 == 0) {
      if ((uVar20 & 0x2000) != 0) {
        bVar4 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27))) + 1;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27))) = bVar4;
        if (bVar4 == bVar12) {
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27))) = 0;
        }
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 2))) = 8;
        (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) = 1;
LAB_00246858:
        PlayUiSound(0x24);
        goto LAB_00246860;
      }
      if ((uVar20 & 0x8000) != 0) {
        bVar4 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27)));
        if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27))) == 0) {
          bVar4 = bVar12;
        }
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27))) = bVar4 - 1;
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 2))) = 0xfff8;
        (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) = -1;
        goto LAB_00246858;
      }
      goto LAB_00246868;
    }
LAB_00246870:
    sVar5 = (short)uVar10;
    iVar28 = (int)uVar10 << 0x10;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 8))) = 0;
    if (iVar28 < 0) {
      iVar28 = (int)uVar10 + 1;
      sVar5 = (short)iVar28;
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 2))) = sVar5;
      iVar28 = iVar28 * 0x10000;
    }
    if (0 < iVar28) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 2))) = sVar5 + -1;
    }
  }
  if ((int)(*(uint32_t*)rtguest::G(0x1757b98)) < 0x11) {
    sVar5 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 2)));
  }
  else if ((*(int32_t*)rtguest::G((uint32_t)iStack_7c)) == 0) {
    sVar5 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 2)));
  }
  else {
    (*(uint32_t*)rtguest::G(0x1757b9c)) = (*(uint32_t*)rtguest::G(0x1757b9c)) + (*(int32_t*)rtguest::G((uint32_t)iStack_7c));
    if ((int)(*(uint32_t*)rtguest::G(0x1757b98)) <= (int)(*(uint32_t*)rtguest::G(0x1757b9c))) {
      (*(uint32_t*)rtguest::G(0x1757b9c)) = (*(uint32_t*)rtguest::G(0x1757b9c)) - (*(uint32_t*)rtguest::G(0x1757b98));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27))) - (char)(*(uint32_t*)rtguest::G(0x1757b98)) & 0xf;
    }
    if ((int)(*(uint32_t*)rtguest::G(0x1757b9c)) < 0) {
      (*(uint32_t*)rtguest::G(0x1757b9c)) = (*(uint32_t*)rtguest::G(0x1757b9c)) + (*(uint32_t*)rtguest::G(0x1757b98));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27))) + (char)(*(uint32_t*)rtguest::G(0x1757b98)) & 0xf;
    }
    uVar10 = ClassifyObjectContext((*(int32_t*)rtguest::G((uint32_t)iStack_90)));
    uVar2 = (*(uint32_t*)rtguest::G(0x1757b9c));
    uVar21 = (*(uint32_t*)rtguest::G(0x1757b98));
    uVar15 = (*(uint32_t*)rtguest::G(0x1757b94));
    uVar17 = (*(uint32_t*)rtguest::G(0x1757b8c));
    uVar19 = (*(uint32_t*)rtguest::G(0x1757b80));
    psVar35 = 0x01757ba0;
    if ((uVar10 & 3) == 0) {
      psVar35 = 0x01757ba0;
      puStack_78 = 0x01757ba0;
      if ((int)(*(uint32_t*)rtguest::G(0x1757b98)) < 0x10) {
        iVar28 = 0;
        uVar22 = (*(uint32_t*)rtguest::G(0x1757b9c));
        do {
          iVar28 = iVar28 + 1;
          sVar5 = FindFlaggedSlotIndex(uVar19,uVar17,uVar22,uVar15);
          (*(int16_t*)rtguest::G((uint32_t)psVar35)) = sVar5;
          psVar35 = psVar35 + 0x2;  // T* element step
          if (sVar5 == -1) goto LAB_00246b4c;
          uVar22 = iVar28 + uVar2;
        } while (iVar28 < 0x10);
      }
      for (uVar22 = uVar2; uVar22 != uVar2 + 8; uVar22 = uVar22 + 1) {
        iVar7 = ((int)uVar22 < 0) * uVar21 + uVar22;
        iVar28 = iVar7 - uVar21;
        if (iVar7 < (int)uVar21) {
          iVar28 = iVar7;
        }
        uVar6 = FindFlaggedSlotIndex(uVar19,uVar17,iVar28,uVar15);
        (*(uint16_t*)rtguest::G((uint32_t)puStack_78 + (uint32_t)(uVar22 & 0xf) * sizeof(uint16_t))) = uVar6;
      }
      uVar22 = uVar2 - 1;
      if (uVar22 == uVar2 - 9) {
        sVar5 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 2)));
      }
      else {
        do {
          iVar7 = ((int)uVar22 < 0) * uVar21 + uVar22;
          iVar28 = iVar7 - uVar21;
          if (iVar7 < (int)uVar21) {
            iVar28 = iVar7;
          }
          uVar6 = FindFlaggedSlotIndex(uVar19,uVar17,iVar28,uVar15);
          uVar16 = uVar22 & 0xf;
          uVar22 = uVar22 - 1;
          (*(uint16_t*)rtguest::G((uint32_t)puStack_78 + (uint32_t)(uVar16) * sizeof(uint16_t))) = uVar6;
        } while (uVar22 != uVar2 - 9);
LAB_00246b4c:
        sVar5 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 2)));
      }
    }
    else {
      uVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_90)) + 0x23)));
      if ((int)(*(uint32_t*)rtguest::G(0x1757b98)) < 0x10) {
        iVar28 = 0;
        uVar17 = (*(uint32_t*)rtguest::G(0x1757b9c));
        do {
          iVar28 = iVar28 + 1;
          sVar5 = findFlaggedSlotIndexAlt(uVar3,uVar19,uVar17);
          (*(int16_t*)rtguest::G((uint32_t)psVar35)) = sVar5;
          psVar35 = psVar35 + 0x2;  // T* element step
          if (sVar5 == -1) goto LAB_00246b4c;
          uVar17 = iVar28 + uVar2;
        } while (iVar28 < 0x10);
      }
      for (uVar17 = uVar2; uVar17 != uVar2 + 8; uVar17 = uVar17 + 1) {
        iVar7 = ((int)uVar17 < 0) * uVar21 + uVar17;
        iVar28 = iVar7 - uVar21;
        if (iVar7 < (int)uVar21) {
          iVar28 = iVar7;
        }
        uVar6 = findFlaggedSlotIndexAlt(uVar3,uVar19,iVar28);
        (*(uint32_t*)rtguest::G((uint32_t)0x1757ba0 + (uint32_t)(uVar17 & 0xf) * 4)) = uVar6;
      }
      uVar17 = uVar2 - 1;
      if (uVar17 == uVar2 - 9) {
        sVar5 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 2)));
      }
      else {
        do {
          iVar7 = ((int)uVar17 < 0) * uVar21 + uVar17;
          iVar28 = iVar7 - uVar21;
          if (iVar7 < (int)uVar21) {
            iVar28 = iVar7;
          }
          uVar6 = findFlaggedSlotIndexAlt(uVar3,uVar19,iVar28);
          uVar15 = uVar17 & 0xf;
          uVar17 = uVar17 - 1;
          (*(uint32_t*)rtguest::G((uint32_t)0x1757ba0 + (uint32_t)(uVar15) * 4)) = uVar6;
        } while (uVar17 != uVar2 - 9);
        sVar5 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 2)));
      }
    }
  }
  if (((sVar5 == 0) && ((*(int32_t*)rtguest::G((uint32_t)iStack_7c)) == 0)) && ((*(uint32_t*)rtguest::G(0x1757b80)) == 0)) {
    uVar10 = (uint32_t)(short)(*(uint32_t*)rtguest::G((uint32_t)0x1757ba0 + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27)))) * 4));
    if ((((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x335720 + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27))) & 7) * 1)) != uVar10) &&
        (uVar10 != (long)(int)(*(uint32_t*)rtguest::G(0x1757b84)))) && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_90)) + 0xf))) == '\0')) {
      gs_tasks_spawn(param_1,0x20dc50,
                     uVar10 | (long)(int)((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27))) << 8) | 0x3b0000);
    }
  }
LAB_00246bd4:
  UpdateSceneLightsAndFog(3,0);
  uVar19 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x26)));
  (*(int32_t*)rtguest::G((uint32_t)iStack_8c)) = 0;
  if (uVar19 != 0) {
    do {
      uVar17 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x20)));
      iVar7 = 0;
      iVar28 = (*(int32_t*)rtguest::G((uint32_t)iStack_8c));
      while (uVar17 != 0) {
        uVar15 = uVar17 & 1;
        uVar17 = uVar17 >> 1;
        if (uVar15 != 0) {
          if (iVar28 == 0) goto LAB_00246c28;
          iVar28 = iVar28 + -1;
        }
        iVar7 = iVar7 + 1;
      }
      iVar7 = -1;
LAB_00246c28:
      if (uVar19 == 0) {
        trap(7);
      }
      fVar38 = (float)(int)(short)(((((int)(((*(int32_t*)rtguest::G((uint32_t)iStack_8c)) + uVar19) - (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27)))) %
                                     (int)uVar19) * 8 + (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 2)))) * 0x2000) /
                                  (int)uVar19) * (*(uint32_t*)rtguest::G(0x332f40)) * 3.0517578e-05;
      if (fVar38 == 0.0) {
        bVar12 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x25)));
        if (bVar12 == 0) goto LAB_00246d18;
        if (bVar12 < 0x30) {
          bVar12 = bVar12 + 1;
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x25))) = bVar12;
        }
        if (bVar12 == 0x30) {
          ClearTaskListFlag(param_1);
          bVar12 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x25)));
          uVar19 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x26)));
        }
        if (0x40 < bVar12) {
          bVar12 = bVar12 - 1;
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x25))) = bVar12;
        }
        if (bVar12 == 0x40) {
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x25))) = 0;
        }
      }
      else {
LAB_00246d18:
        fVar37 = rtguest::fbits(cosf(rtguest::f2bits(fVar38)));
        iVar28 = (int)(fVar37 * ((float)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x26))) + 32.0)) -
                 (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x26)));
        fVar37 = rtguest::fbits(sinf(rtguest::f2bits(fVar38)));
        (*(int32_t*)rtguest::G((uint32_t)iStack_74)) = (int)(fVar37 * 112.0);
        iVar29 = iVar28 + 0xa0;
        fVar37 = rtguest::fbits(cosf(rtguest::f2bits(fVar38)));
        iVar36 = (*(int32_t*)rtguest::G((uint32_t)iStack_74)) + 0xf0;
        iVar33 = ((int)(fVar37 * ((float)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x26))) + 24.0)) -
                 (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x26)))) + 0xf0;
        if (0.0 < fVar38) {
          iVar29 = iVar28 + 0xa1;
        }
        BuildCarouselQuadA(fVar38,0x17527e0,iVar7,iVar36,iVar33,iVar29);
        iVar27 = iVar7 * 8;
        uVar11 = DispatchByIndex16((*(uint32_t*)rtguest::G(0x1757b80)),(*(uint32_t*)rtguest::G((uint32_t)0x1757ba0 + (uint32_t)(iVar7) * 4)));
        iVar8 = uiTextWidth(uVar11);
        iVar28 = iVar29 * 0xe;
        iVar23 = iVar29 * 0x28;
        iVar8 = iVar8 * 5;
        iVar18 = iVar8 + 3;
        if (-1 < iVar8) {
          iVar18 = iVar8;
        }
        iVar8 = ((iVar18 >> 2) - (iVar18 >> 0x1f) >> 1) * iVar29;
        if (iVar28 < 0) {
          iVar28 = iVar28 + 0xff;
        }
        if (iVar23 < 0) {
          iVar23 = iVar23 + 0xff;
        }
        iVar28 = iVar33 + (iVar28 >> 8);
        if (iVar8 < 0) {
          iVar8 = iVar8 + 0xff;
        }
        iVar18 = iVar36 - (iVar8 >> 8);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = (long)iVar18 & 0xffffU | ((long)(iVar36 + (iVar8 >> 8)) & 0xffffU) << 0x10 |
                    ((long)iVar28 & 0xffffU) << 0x20 | (long)(iVar33 + (iVar23 >> 8)) << 0x30;
        iVar23 = iVar29 * 5;
        if (iVar23 < 0) {
          iVar23 = iVar23 + 3;
        }
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar27 + 0x1757b04))) = (short)iVar29;
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar27 + 0x1757b06))) = (short)(iVar23 >> 2);
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar27 + 0x1757b00))) = (short)iVar18;
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar27 + 0x1757b02))) = (short)iVar28;
        uVar19 = (*(uint32_t*)rtguest::G(0x1757b80));
        vu0UnitMatrix(auStack_f0);
        iVar28 = uVar19 * 0x20;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_98)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar28 + 0x2a8d08)));
        (*(uint64_t*)rtguest::G((uint32_t)uStack_b0)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar28 + 0x2a8cf0)));
        (*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar28 + 0x2a8d00)));
        (*(uint64_t*)rtguest::G((uint32_t)uStack_a8)) = CONCAT44((int)((uint32_t)(*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x002a8cf8 + iVar28))) >> 0x20),
                             ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a8cf8 + iVar28))) * 256.0) / (float)iVar29);
        if ((*(uint32_t*)rtguest::G(0x333288)) == 0) {
          SetViewDistanceGlobals
                    ((float)((*(int32_t*)rtguest::G((uint32_t)iStack_74)) + 0x7b0),(float)((iVar33 + iVar29 / 2) / 2 + 0x790));
        }
        else {
          SetViewDistanceGlobals
                    ((float)((iVar36 * 4) / 5 + 0x700),(float)((iVar33 + iVar29 / 2) / 2 + 0x780));
        }
        buildCamera(iVar7 * 400 + 0x1756200,uStack_b0,auStack_f0,auStack_f0);
        updateLightDirection(iVar7 * 400 + 0x1756380);
        BuildCarouselQuadC(fVar38,iVar7 * 0xf0 + 0x1755300,iVar36,iVar33,iVar29,
                           (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 8))));
        uVar19 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x26)));
      }
      (*(int32_t*)rtguest::G((uint32_t)iStack_8c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_8c)) + 1;
    } while ((*(int32_t*)rtguest::G((uint32_t)iStack_8c)) < (int)uVar19);
  }
  uVar11 = sceDmaGetChan(1);
  sceDmaSync(uVar11,0,0);
  vifBeginPacket(1);
  vifDirect(0x1752780,6);
  vifDirect(0x2a8b20,3);
  bVar12 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x26)));
  uVar19 = (uint32_t)(bVar12 >> 1);
  bVar4 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27)));
  do {
    if (bVar12 == 0) {
      trap(7);
    }
    uVar17 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x20)));
    iVar29 = 0;
    iVar28 = (int)(((uint32_t)bVar12 + (uint32_t)bVar4) - uVar19) % (int)(uint32_t)bVar12;
    iVar7 = iVar28;
    while (uVar17 != 0) {
      uVar15 = uVar17 & 1;
      uVar17 = uVar17 >> 1;
      if (uVar15 != 0) {
        if (iVar7 == 0) goto LAB_00247180;
        iVar7 = iVar7 + -1;
      }
      iVar29 = iVar29 + 1;
    }
    iVar29 = -1;
LAB_00247180:
    drawCarouselCar((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x28))),iVar29,(*(int32_t*)rtguest::G((uint32_t)iStack_90)));
    iVar7 = (int)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27))) + uVar19) % (int)(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x26)));
    if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x26))) == 0) {
      trap(7);
    }
    if (iVar28 != iVar7) {
      uVar17 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x20)));
      iVar28 = 0;
      while (uVar17 != 0) {
        uVar15 = uVar17 & 1;
        uVar17 = uVar17 >> 1;
        if (uVar15 != 0) {
          if (iVar7 == 0) goto LAB_002471f0;
          iVar7 = iVar7 + -1;
        }
        iVar28 = iVar28 + 1;
      }
      iVar28 = -1;
LAB_002471f0:
      drawCarouselCar((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x28))),iVar28,(*(int32_t*)rtguest::G((uint32_t)iStack_90)));
    }
    uVar19 = uVar19 - 1;
    if ((int)uVar19 < 0) break;
    bVar12 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x26)));
    bVar4 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27)));
  } while( true );
  uVar11 = vifEndPacket(1);
  vifKick(uVar11);
  if (((*(uint32_t*)rtguest::G(0x1757b88)) & 3) == 0) {
    if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_90)) + 10))) & 2) == 0) {
      uVar19 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27)));
      if ((*(uint32_t*)rtguest::G(0x1757b88)) == 4) {
        iVar28 = DispatchByIndex15((*(uint32_t*)rtguest::G(0x1757b80)),(*(uint32_t*)rtguest::G((uint32_t)0x1757ba0 + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27)))) * 4)));
        uiDrawPanelCells(0x11,10,0x1e,0xc,2);
        uiDrawTextCell(0x13,0xb,0x334390,0);
        uiDrawNumber16(0x226,0x89,iVar28 / 2,0);
        uiDrawTextCell(0x1b,0xb,0x0177fd9f + (*(uint32_t*)rtguest::G(0x1757b8c)) * 0xd12,0);
        DrawAltScoreAndRemaining(-(iVar28 / 2),(*(int32_t*)rtguest::G((uint32_t)iStack_90)));
        uVar19 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27)));
      }
    }
    else {
      uVar19 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27)));
      if ((*(uint32_t*)rtguest::G(0x1757b88)) == 4) {
        lVar9 = GetFlaggedSlotCount((*(uint32_t*)rtguest::G(0x1757b80)),(*(uint32_t*)rtguest::G((uint32_t)0x1757ba0 + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27)))) * 4)),
                                    (*(uint32_t*)rtguest::G(0x1757b8c)) ^ 1);
        uiDrawPanelCells(0x11,8,0x1e,0xc,2);
        uiDrawTextCell(0x12,9,0x0177fd8c + ((*(uint32_t*)rtguest::G(0x1757b8c)) ^ 1) * 0xd12,0);
        if (lVar9 == 0) {
          uiDrawTextCell(0x12,10,0x3275b8,0);
        }
        else {
          iVar28 = uiDrawText(0x186,0x7d,0x3275a8,0);
          uiDrawNumber16(iVar28 + 0x10,0x7d,lVar9,0);
        }
        uiDrawTextCell(0x13,0xb,0x334388,0);
        uVar11 = ComputeScaledPartValue
                           ((*(uint32_t*)rtguest::G(0x1757b80)),(*(uint32_t*)rtguest::G((uint32_t)0x1757ba0 + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27)))) * 4)),(*(uint32_t*)rtguest::G(0x1757b8c)) ^ 1,
                            0);
        iVar28 = ComputeScaledPartValue
                           ((*(uint32_t*)rtguest::G(0x1757b80)),(*(uint32_t*)rtguest::G((uint32_t)0x1757ba0 + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27)))) * 4)),(*(uint32_t*)rtguest::G(0x1757b8c)) ^ 1,
                            0);
        uVar24 = 6;
        if (iVar28 <= (*(uint32_t*)rtguest::G((uint32_t)0x177fdb4 + (uint32_t)(((*(uint32_t*)rtguest::G(0x1757b8c)) ^ 1) * 0xd12) * 4))) {
          uVar24 = 0;
        }
        uiDrawNumber16(0x226,0x89,uVar11,uVar24);
        uiDrawTextCell(0x1b,0xb,0x0177fd9f + ((*(uint32_t*)rtguest::G(0x1757b8c)) ^ 1) * 0xd12,0);
        uiDrawPanelCells(0x11,5,0x1e,8,2);
        uiDrawTextCell(0x12,6,0x0177fd8c + (*(uint32_t*)rtguest::G(0x1757b8c)) * 0xd12,0);
        uiDrawTextCell(0x13,7,0x334390,0);
        uVar11 = ComputeScaledPartValue
                           ((*(uint32_t*)rtguest::G(0x1757b80)),(*(uint32_t*)rtguest::G((uint32_t)0x1757ba0 + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27)))) * 4)),(*(uint32_t*)rtguest::G(0x1757b8c)),1);
        uiDrawNumber16(0x226,0x59,uVar11,0);
        uiDrawTextCell(0x1b,7,0x0177fd9f + (*(uint32_t*)rtguest::G(0x1757b8c)) * 0xd12,0);
        uVar19 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27)));
      }
    }
  }
  else {
    lVar9 = GetFlaggedSlotCount((*(uint32_t*)rtguest::G(0x1757b80)),(*(uint32_t*)rtguest::G((uint32_t)0x1757ba0 + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27)))) * 4)),(*(uint32_t*)rtguest::G(0x1757b8c)))
    ;
    iVar28 = DispatchByIndex15((*(uint32_t*)rtguest::G(0x1757b80)),(*(uint32_t*)rtguest::G((uint32_t)0x1757ba0 + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27)))) * 4)));
    uiDrawPanelCells(0x11,9,0x1e,0xc,2);
    if (lVar9 == 0) {
      uiDrawTextCell(0x12,10,0x3275b8,0);
    }
    else if ((*(uint32_t*)rtguest::G(0x1757b88)) == 2) {
      uiDrawTextCell(0x12,10,0x327598,0);
    }
    else {
      iVar7 = uiDrawText(0x186,0x7d,0x3275a8,0);
      uiDrawNumber16(iVar7 + 0x10,0x7d,lVar9,0);
    }
    uiDrawTextCell(0x13,0xb,0x334380,0);
    uVar11 = 6;
    if (iVar28 <= (*(uint32_t*)rtguest::G(0x177fdb4))) {
      uVar11 = 0;
    }
    uiDrawNumber16(0x226,0x89,iVar28,uVar11);
    uiDrawTextCell(0x1b,0xb,0x177fd9f,0);
    DrawAltScoreAndRemaining(iVar28,(*(int32_t*)rtguest::G((uint32_t)iStack_90)));
    uVar19 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27)));
  }
  DrawTextListRows((*(uint32_t*)rtguest::G(0x1757b80)),(*(uint32_t*)rtguest::G((uint32_t)0x1757ba0 + (uint32_t)(uVar19) * 4)));  // pseudocode arity 2; ABI 3rd arg unused by callee
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 8))) == 0) {
    return 0;
  }
  iVar28 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x18))) + 0x28)));
  if (iVar28 != 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar28 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar28 + 0xe))) & 0x7f;
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)uStack_88)) & 0x40) != 0) {
    bVar12 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x27)));
    if ((long)(int)(*(uint32_t*)rtguest::G(0x1757b84)) == (long)(short)(*(uint32_t*)rtguest::G((uint32_t)0x1757ba0 + (uint32_t)(bVar12) * 4))) {
      return 0;
    }
    if (((*(uint32_t*)rtguest::G(0x1757b80)) == 0) && ((*(uint32_t*)rtguest::G(0x1757b88)) == 0)) {
      if ((uint32_t)(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x335720 + (uint32_t)(bVar12 & 7) * 1)) != (long)(short)(*(uint32_t*)rtguest::G((uint32_t)0x1757ba0 + (uint32_t)(bVar12) * 4))) {
        return 0;
      }
      iVar30 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 4))) = bVar12 & 7;
      PlayUiSound(0x1a);
      return 0;
    }
    iVar30 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x18)));
    sVar5 = (*(uint32_t*)rtguest::G((uint32_t)0x1757ba0 + (uint32_t)(bVar12) * 4));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 0x24))) = 1;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar30 + 4))) = (int)sVar5;
    if ((*(uint32_t*)rtguest::G(0x1757b80)) == 0xd) {
      return 0;
    }
    PlayUiSound(0x1a);
    return 0;
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)uStack_88)) & 0x10) == 0) {
    return 0;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_90)) + 0xf))) != '\0') {
    return 0;
  }
  PlayUiSound(0x1b);
  goto LAB_00247758;
}


RT_GUEST_FN(0x00246250, guest::shopMenuScene)

}  // namespace guest
