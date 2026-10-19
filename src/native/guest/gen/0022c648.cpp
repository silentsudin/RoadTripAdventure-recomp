// guest:0x0022c648 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t stampBookBuild(uint32_t q0, ...) {
uint32_t puVar1;
uint32_t puVar2;
uint32_t puVar3;
uint32_t puVar4;
uint32_t puVar5;
  char cVar6;
  undefined2 uVar7;
  int iVar8;
  long lVar9;
  int iVar10;
  int iVar11;
uint32_t puVar12;
  int iVar13;
uint32_t pcVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  int iVar18;
  
  iVar16 = 0;
  iVar8 = getShopPacketArea(0xc50);
  gifBeginSpriteBatch(iVar8,0x200435ce29329f2e);
  gifSetSpriteCount(iVar8 + 0x60,0x26);
  iVar18 = iVar8;
  iVar17 = iVar8;
  do {
    iVar15 = 0x80;
    iVar13 = 3;
    iVar10 = 0x800000;
    iVar11 = iVar17 + 0x70;
    do {
      iVar15 = iVar15 + 0x20;
      iVar13 = iVar13 + -1;
      uiSpriteWriteZ(iVar11,iVar10 >> 0x10,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)0x29f508u + iVar16 * 2))),0xba
                     ,0x4f,0x1e,0x20,0x100000);
      iVar10 = iVar15 * 0x10000;
      iVar11 = iVar11 + 0x50;
    } while (-1 < iVar13);
    iVar15 = 0x186;
    iVar13 = 3;
    iVar10 = 0x1860000;
    iVar11 = iVar18 + 0x1b0;
    do {
      iVar15 = iVar15 + 0x20;
      iVar13 = iVar13 + -1;
      uiSpriteWriteZ(iVar11,iVar10 >> 0x10,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)0x29f508u + iVar16 * 2))),0xba
                     ,0x4f,0x1e,0x20,0x100000);
      iVar10 = iVar15 * 0x10000;
      iVar11 = iVar11 + 0x50;
    } while (-1 < iVar13);
    iVar16 = iVar16 + 1;
    iVar18 = iVar18 + 0x280;
    iVar17 = iVar17 + 0x280;
  } while (iVar16 < 3);
  iVar18 = iVar8 + 0x70;
  iVar17 = 0x17;
  pcVar14 = 0x01780750;
  do {
    cVar6 = (*(int8_t*)rtguest::G((uint32_t)pcVar14));
    pcVar14 = pcVar14 + 1;
    if (cVar6 != '\0') {
      uiHideSprite(iVar18);
    }
    iVar17 = iVar17 + -1;
    iVar18 = iVar18 + 0x50;
  } while (-1 < iVar17);
  uiSpriteWrite(iVar8 + 0x7f0,0xf2,0x16,0,0,0x9c,0x5c);
  lVar9 = TestFlagBitFc98(0x57);
  if (lVar9 == 0) {
    uiHideSprite(iVar8 + 0x7f0);
  }
  iVar17 = 0;
  iVar18 = iVar8 + 0x840;
  puVar12 = 0x0029f472;
  do {
    puVar1 = puVar12 + -5;
    puVar2 = puVar12 + -4;
    puVar3 = puVar12 + -3;
    puVar4 = puVar12 + -2;
    puVar5 = puVar12 + -1;
    uVar7 = (*(uint16_t*)rtguest::G((uint32_t)puVar12));
    puVar12 = puVar12 + 6;
    uiSpriteWrite(iVar18,(*(uint16_t*)rtguest::G((uint32_t)puVar1)),(*(uint16_t*)rtguest::G((uint32_t)puVar2)),(*(uint16_t*)rtguest::G((uint32_t)puVar3)),(*(uint16_t*)rtguest::G((uint32_t)puVar4)),(*(uint16_t*)rtguest::G((uint32_t)puVar5)),uVar7);
    iVar16 = iVar17 + 10;
    iVar17 = iVar17 + 1;
    lVar9 = GetFlaggedSlotCount(0xf,iVar16,0);
    if (lVar9 == 0) {
      uiHideSprite(iVar18);
    }
    iVar18 = iVar18 + 0x50;
  } while (iVar17 < 0xd);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xab4))) = 0x100000;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xbf4))) = 0x200000;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xa14))) = 0x100000;
}


RT_GUEST_FN(0x0022c648, guest::stampBookBuild)

}  // namespace guest
