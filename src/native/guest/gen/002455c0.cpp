// guest:0x002455c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t drawCarouselScene(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(160);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x88) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x8c) = 0x00000000u;
    *(uint32_t*)rtguest::G(rt_sv+0x0) = (uint32_t)(int32_t)param_2;
  }
    rtguest::allocFrame(16);  // guest frame gap (sp+0x0..0xf)
    uint32_t iStack_90 = rtguest::allocFrame(4);  // sp+0x10 (Ghidra entry-sp -0x90, frame 0xa0)
    rtguest::allocFrame(4);  // guest frame gap (sp+0x14..0x17)
    uint32_t iStack_88 = rtguest::allocFrame(4);  // sp+0x18 (Ghidra entry-sp -0x88, frame 0xa0)
  char cVar1;
  byte bVar2;
  ushort uVar3;
  ushort uVar4;
  undefined1 uVar5;
  byte bVar6;
  short sVar7;
  int iVar8;
  undefined4 uVar9;
  uint uVar10;
  undefined8 uVar11;
  int iVar12;
  uint uVar13;
  ushort uVar14;
  ulong uVar15;
  int iVar16;
  uint uVar17;
  int iVar18;
uint32_t puVar19;
uint32_t puVar20;
  int iVar21;
  int iVar22;
  int iVar23;
  int iVar24;
  int iVar25;
  int iVar26;
  float fVar27;
  float fVar28;
  float fVar29;
  
  iVar8 = RT_CALLRA(0x2455fc, guest::getExtraPacketArea(0x4c60));
  uVar3 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 6 * sizeof(uint64_t))));  // W13: fn/002455c0.c:43 `*(ushort *)(param_2 + 6)` with `undefined8 *param_2` = +0x30
  uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x34)));
  if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 10))) & 2) != 0) {
    uVar3 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x32))) | uVar3;
    uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x36))) | uVar4;
  }
  iVar22 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 8))) == 0) {
    iVar26 = 0;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x28))) = iVar8;
    iVar18 = iVar8 + 0x60;
    RT_CALLRA(0x245684, guest::gifBeginSpriteBatch(iVar8,0x2003ecce21411efc));
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x10))) = 0x3001b;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x58))) = 0x42;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x50))) = 0x8000000044;
    (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = 0;
    RT_CALLRA(0x2456dc, guest::gifSetTexRegs(iVar18,0x2003a84621411cc6,5));
    (*(int32_t*)rtguest::G((uint32_t)iStack_88)) = 0;
    iVar25 = iVar8 + 0x80;
    iVar24 = iVar18;
    do {
      RT_CALLRA(0x2456f8, guest::gifTexQuadTag(iVar25));
      puVar19 = (uint32_t)(iVar24 + iVar26 + 0x5c);
      iVar21 = 3;
      iVar12 = iVar18 + (*(int32_t*)rtguest::G((uint32_t)iStack_88));
      puVar20 = 0x002a8a20;
      do {
        iVar12 = iVar12 + 0x30;
        RT_CALLRA(0x245724, guest::vu0CopyVector(iVar12,puVar20));
        iVar21 = iVar21 + -1;
        (*(uint32_t*)rtguest::G((uint32_t)puVar19)) = 0xff0;
        puVar19 = puVar19 + 0x30;  // T* element step
        puVar20 = puVar20 + 0x10;
      } while (-1 < iVar21);
      iVar25 = iVar25 + 0xd0;
      iVar18 = iVar18 + 0x80;
      (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = (*(int32_t*)rtguest::G((uint32_t)iStack_90)) + 1;
      iVar26 = iVar26 + 0x50;
      (*(int32_t*)rtguest::G((uint32_t)iStack_88)) = (*(int32_t*)rtguest::G((uint32_t)iStack_88)) + 0x50;
      iVar24 = iVar24 + 0x80;
    } while ((*(int32_t*)rtguest::G((uint32_t)iStack_90)) < 0x10);
    RT_CALLRA(0x245778, guest::LayoutRaceRowQuads(iVar8 + 0xd80,param_2));
    iVar24 = 0x11;
    puVar19 = (uint32_t)(iVar8 + 0x1f60);
    do {
      iVar24 = iVar24 + -1;
      (*(uint32_t*)rtguest::G((uint32_t)puVar19)) = 0x3f800000;
      (*(uint32_t*)rtguest::G((uint32_t)puVar19 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar19 + (uint32_t)(2) * sizeof(uint32_t))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar19 + (uint32_t)(8) * sizeof(uint32_t))) = 0x43000000;
      (*(uint32_t*)rtguest::G((uint32_t)puVar19 + (uint32_t)(4) * sizeof(uint32_t))) = 0x43000000;
      (*(uint32_t*)rtguest::G((uint32_t)puVar19 + (uint32_t)(9) * sizeof(uint32_t))) = 0x437f0000;
      (*(uint32_t*)rtguest::G((uint32_t)puVar19 + (uint32_t)(5) * sizeof(uint32_t))) = 0x437f0000;
      (*(uint32_t*)rtguest::G((uint32_t)puVar19 + (uint32_t)(10) * sizeof(uint32_t))) = 0x3f000000;
      (*(uint32_t*)rtguest::G((uint32_t)puVar19 + (uint32_t)(6) * sizeof(uint32_t))) = 0x3f000000;
      (*(uint32_t*)rtguest::G((uint32_t)puVar19 + (uint32_t)(0xb) * sizeof(uint32_t))) = 0x44480000;
      (*(uint32_t*)rtguest::G((uint32_t)puVar19 + (uint32_t)(7) * sizeof(uint32_t))) = 0x44480000;
      puVar19 = puVar19 + 0x1e0;  // T* element step
    } while (-1 < iVar24);
    RT_CALLRA(0x2457f4, guest::SetupCarouselSlotTransforms(iVar8 + 0x3d60));
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 10)));
    if (cVar1 == '\x01') {
      uVar17 = 0x40ff;
LAB_00245864:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x20))) = uVar17;
    }
    else {
      if (cVar1 == '\x06') {
        uVar9 = 0xb3ff;
      }
      else if (cVar1 == '\x03') {
        uVar9 = 0xa3ff;
      }
      else {
        uVar9 = 0x43ff;
      }
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x20))) = uVar9;
      if ((*(int8_t*)rtguest::G(0x177fdd6)) == '\0') {
        uVar17 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x20)));
LAB_0024585c:
        uVar17 = uVar17 & 0xfffffdff;
        goto LAB_00245864;
      }
      if ((*(uint32_t*)rtguest::G(0x178321e)) == '\0') {
        uVar17 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x20)));
        goto LAB_0024585c;
      }
    }
    uVar5 = RT_CALLRA(0x245868, guest::CountSetBits((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x20)))));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x26))) = uVar5;
    uVar5 = RT_CALLRA(0x245878, guest::FindSetBitRank((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x20))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x1c)))));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x27))) = uVar5;
    RT_CALLRA(0x245888, guest::gs_music_play_bgm(0xa190201));
    RT_CALLRA(0x245890, guest::PlayBgmSlot1());
  }
  iVar26 = 0;
  iVar18 = iVar8 + 0x1aa0;
  iVar24 = (int)(*(uint64_t*)rtguest::G((uint32_t)param_2));
  RT_CALLRA(0x2458c0, guest::gifSetSpriteCount(iVar18,0xf));
  iVar25 = 0;
  do {
    iVar23 = 0;
    iVar21 = 4;
    iVar12 = iVar26 * 400 + iVar18;
    puVar19 = (uint32_t)(iVar12 + 0x54);
    iVar12 = iVar12 + 0x10;
    do {
      iVar21 = iVar21 + -1;
      iVar16 = iVar23 - iVar24 % 200;
      iVar23 = iVar23 + 200;
      RT_CALLRA(0x24594c, guest::uiSpriteWrite(iVar12,iVar16 * 0x10000 >> 0x10,(iVar25 << 0x11) >> 0x10,0,0,200,0x96));
      (*(uint32_t*)rtguest::G((uint32_t)puVar19)) = 1;
      puVar19 = puVar19 + 0x50;  // T* element step
      iVar12 = iVar12 + 0x50;
    } while (-1 < iVar21);
    iVar26 = iVar26 + 1;
    iVar25 = iVar25 + 0x4b;
    iVar24 = iVar24 + 0x40;
  } while (iVar26 < 3);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x25))) == '\0') {
    bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x26)));
    if (bVar2 < 2) {
      uVar14 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 2)));
LAB_00245a20:
      uVar15 = (uint32_t)uVar14;
LAB_00245a24:
      if (uVar14 == 0) goto LAB_00245b08;
    }
    else {
      uVar14 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 2)));
      uVar15 = (uint32_t)(short)uVar14;
      if (uVar15 == 0) {
        if ((uVar4 & 0x2000) != 0) {
          bVar6 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x27))) + 1;
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x27))) = bVar6;
          if (bVar6 == bVar2) {
            (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x27))) = 0;
          }
          (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 2))) = 8;
LAB_00245a14:
          PlayUiSound(0x24);
          uVar14 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 2)));
          goto LAB_00245a20;
        }
        if ((uVar4 & 0x8000) != 0) {
          bVar6 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x27)));
          if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x27))) == 0) {
            bVar6 = bVar2;
          }
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x27))) = bVar6 - 1;
          (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 2))) = 0xfff8;
          goto LAB_00245a14;
        }
        goto LAB_00245a24;
      }
    }
    sVar7 = (short)uVar15;
    iVar24 = (int)uVar15 << 0x10;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 8))) = 0;
    if (iVar24 < 0) {
      iVar24 = (int)uVar15 + 1;
      sVar7 = (short)iVar24;
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 2))) = sVar7;
      iVar24 = iVar24 * 0x10000;
    }
    if (0 < iVar24) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 2))) = sVar7 + -1;
    }
  }
  else if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x24))) != '\0') {
    iVar24 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 4)));
    if (iVar24 == -1) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 8))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x25))) = 2;
    }
    else if (iVar24 == 1) {
      uVar17 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x20)));
      iVar24 = 0;
      uVar13 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x27)));
      while (uVar17 != 0) {
        uVar10 = uVar17 & 1;
        uVar17 = uVar17 >> 1;
        if (uVar10 != 0) {
          if (uVar13 == 0) goto LAB_00245ad0;
          uVar13 = uVar13 - 1;
        }
        iVar24 = iVar24 + 1;
      }
      iVar24 = -1;
LAB_00245ad0:
      iVar25 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar25 + 0x24))) = 1;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar25 + 4))) = iVar24;
    }
    else if (iVar24 == 2) {
      RT_CALLRA(0x245af0, guest::PlayBgmSlot5());
      iVar24 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar24 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar24 + 4))) = 0xffffffff;
    }
  }
LAB_00245b08:
  RT_CALLRA(0x245b08, guest::UpdateSceneLightsAndFog(1,0));
  uVar17 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x26)));
  iVar24 = 0;
  if (uVar17 != 0) {
    fVar28 = 64.0;
    uVar13 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x20)));
    do {
      iVar26 = 0;
      iVar25 = iVar24;
      while (uVar13 != 0) {
        uVar10 = uVar13 & 1;
        uVar13 = uVar13 >> 1;
        if (uVar10 != 0) {
          if (iVar25 == 0) goto LAB_00245b78;
          iVar25 = iVar25 + -1;
        }
        iVar26 = iVar26 + 1;
      }
      iVar26 = -1;
LAB_00245b78:
      if (uVar17 == 0) {
        trap(7);
      }
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x25)));
      fVar29 = (float)(int)(short)(((((int)((iVar24 + uVar17) - (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x27)))) %
                                     (int)uVar17) * 8 + (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 2)))) * 0x2000) /
                                  (int)uVar17) * (*(float*)rtguest::G(0x332f3c)) * 3.0517578e-05f;
      if (cVar1 == '\0') {
        fVar27 = rtguest::fbits(RT_CALLRA(0x245bdc, guest::cosf(rtguest::f2bits(fVar29))));
        iVar23 = (int)(fVar27 * fVar28) + 0xc0;
        fVar27 = rtguest::fbits(RT_CALLRA(0x245bf4, guest::sinf(rtguest::f2bits(fVar29))));
        iVar25 = (int)(fVar27 * 192.0) + 0x140;
        fVar27 = rtguest::fbits(RT_CALLRA(0x245c14, guest::cosf(rtguest::f2bits(fVar29))));
        iVar12 = (int)(fVar27 * fVar28) + 0x80;
        RT_CALLRA(0x245c48, guest::BuildCarouselQuadA(fVar29,iVar8 + 0x60,iVar26,iVar25,iVar12,iVar23));
        RT_CALLRA(0x245c68, guest::BuildCarouselQuadD(fVar29,iVar8 + 0xd80,iVar26,iVar25,iVar12,iVar23));
        if (iVar26 < 10) {
          if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 10))) == '\x01') {
            iVar21 = iVar26 * 0x10;
            uVar17 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x2be158 + (uint32_t)iVar21));
          }
          else {
            iVar21 = iVar26 * 0x10;
            uVar17 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x2be1d8 + (uint32_t)iVar21));
          }
        }
        else {
          uVar17 = 0x19;
          if (iVar26 != 0xc) {
            if (iVar26 == 0xd) {
              uVar17 = 0x1b;
            }
            else {
              uVar17 = iVar26 + 0x10;
            }
          }
          iVar21 = iVar26 << 4;
        }
        if ((int)uVar17 < 0x12) {
          iVar21 = iVar21 - iVar26;
          uVar9 = 0;
LAB_00245ea8:
          RT_CALLRA(0x245ea8, guest::BuildCarouselDrawPacket(fVar29,iVar8 + iVar21 * 0x20 + 0x1f60,iVar25,iVar12,iVar23,uVar9));
          uVar17 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x26)));
        }
        else {
          RT_CALLRA(0x245d28, guest::BuildCarouselQuadB(fVar29,iVar8 + (iVar21 - iVar26) * 0x10 + 0x3d60,iVar25,iVar12,iVar23,
                             (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 8)))));
          bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x26)));
LAB_00245ee4:
          uVar17 = (uint32_t)bVar2;
        }
      }
      else if (fVar29 == 0.0) {
        iVar25 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 8)));
        if (iVar25 < 0x10) {
          if (cVar1 == '\x01') {
            iVar12 = iVar25 * -10 + 0xc0;
            iVar25 = iVar25 * -10 + 0x140;
          }
          else {
            iVar12 = iVar25 * 10 + 0x20;
            iVar25 = iVar25 * 10 + 0xa0;
          }
        }
        else if (cVar1 == '\x01') {
          iVar25 = 0xa0;
          iVar12 = 0x20;
        }
        else {
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x25))) = 0;
          iVar25 = 0x140;
          iVar12 = 0xc0;
        }
        RT_CALLRA(0x245dd8, guest::BuildCarouselQuadA(fVar29,iVar8 + 0x60,iVar26,iVar25,iVar12,0x100));
        RT_CALLRA(0x245df8, guest::BuildCarouselQuadD(fVar29,iVar8 + 0xd80,iVar26,iVar25,iVar12,0x100));
        if (iVar26 < 10) {
          if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 10))) == '\x01') {
            iVar21 = iVar26 * 0x10;
            uVar17 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x2be158 + (uint32_t)iVar21));
          }
          else {
            iVar21 = iVar26 * 0x10;
            uVar17 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x2be1d8 + (uint32_t)iVar21));
          }
        }
        else {
          uVar17 = 0x19;
          if (iVar26 != 0xc) {
            if (iVar26 == 0xd) {
              uVar17 = 0x1b;
            }
            else {
              uVar17 = iVar26 + 0x10;
            }
          }
          iVar21 = iVar26 << 4;
        }
        if (0x11 < (int)uVar17) {
          RT_CALLRA(0x245ed8, guest::BuildCarouselQuadB(fVar29,iVar8 + (iVar21 - iVar26) * 0x10 + 0x3d60,iVar25,iVar12,0x100,
                             (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 8)))));
          bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x26)));
          goto LAB_00245ee4;
        }
        iVar21 = iVar21 - iVar26;
        uVar9 = 0;
        if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x25))) != '\x02') {
          uVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 8)));
        }
        iVar23 = 0x100;
        goto LAB_00245ea8;
      }
      iVar24 = iVar24 + 1;
      if ((int)uVar17 <= iVar24) break;
      uVar13 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x20)));
    } while( true );
  }
  RT_CALLRA(0x245ef4, guest::vifBeginPacket(1));
  RT_CALLRA(0x245f00, guest::vifDirect(iVar8,6));
  RT_CALLRA(0x245f0c, guest::vifDirect(iVar18,0x4c));
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x25))) == '\0') {
    RT_CALLRA(0x245f24, guest::vifDirect(0x2a8b20,3));
    bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x26)));
    uVar17 = (uint32_t)(bVar2 >> 1);
    bVar6 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x27)));
    do {
      if (bVar2 == 0) {
        trap(7);
      }
      uVar13 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x20)));
      iVar18 = 0;
      iVar8 = (int)(((uint32_t)bVar2 + (uint32_t)bVar6) - uVar17) % (int)(uint32_t)bVar2;
      iVar24 = iVar8;
      while (uVar13 != 0) {
        uVar10 = uVar13 & 1;
        uVar13 = uVar13 >> 1;
        if (uVar10 != 0) {
          if (iVar24 == 0) goto LAB_00245fa8;
          iVar24 = iVar24 + -1;
        }
        iVar18 = iVar18 + 1;
      }
      iVar18 = -1;
LAB_00245fa8:
      RT_CALLRA(0x245fac, guest::drawCarouselCells((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x28))),iVar18,param_2));
      iVar24 = (int)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x27))) + uVar17) % (int)(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x26)));
      if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x26))) == 0) {
        trap(7);
      }
      if (iVar8 != iVar24) {
        uVar13 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x20)));
        iVar8 = 0;
        while (uVar13 != 0) {
          uVar10 = uVar13 & 1;
          uVar13 = uVar13 >> 1;
          if (uVar10 != 0) {
            if (iVar24 == 0) goto LAB_00246018;
            iVar24 = iVar24 + -1;
          }
          iVar8 = iVar8 + 1;
        }
        iVar8 = -1;
LAB_00246018:
        RT_CALLRA(0x24601c, guest::drawCarouselCells((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x28))),iVar8,param_2));
      }
      uVar17 = uVar17 - 1;
      if ((int)uVar17 < 0) goto LAB_002460ac;
      bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x26)));
      bVar6 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x27)));
    } while( true );
  }
  RT_CALLRA(0x24603c, guest::vifDirect(0x2a8b20,3));
  bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x26)));
  iVar8 = 0;
  if (bVar2 == 0) {
    trap(7);
  }
  uVar13 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x20)));
  uVar17 = ((uint32_t)bVar2 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x27)))) % (uint32_t)bVar2;
  iVar24 = -1;
  if (uVar13 != 0) {
    do {
      uVar10 = uVar13 & 1;
      uVar13 = uVar13 >> 1;
      if (uVar10 != 0) {
        iVar24 = iVar8;
        if (uVar17 == 0) goto LAB_002460a0;
        uVar17 = uVar17 - 1;
      }
      iVar8 = iVar8 + 1;
    } while (uVar13 != 0);
    iVar24 = -1;
  }
LAB_002460a0:
  RT_CALLRA(0x2460a4, guest::drawCarouselCells((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x28))),iVar24,param_2));
LAB_002460ac:
  uVar11 = RT_CALLRA(0x2460ac, guest::vifEndPacket(1));
  RT_CALLRA(0x2460b4, guest::vifKick(uVar11));
  if (((((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x25))) != '\0') || ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 8))) == 0)) ||
      ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 2))) != 0)) || (iVar8 = 0, (uVar3 & 0x40) == 0)) {
    return 0;
  }
  uVar17 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x20)));
  uVar13 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x27)));
  while (uVar17 != 0) {
    uVar10 = uVar17 & 1;
    uVar17 = uVar17 >> 1;
    if (uVar10 != 0) {
      if (uVar13 == 0) goto LAB_00246130;
      uVar13 = uVar13 - 1;
    }
    iVar8 = iVar8 + 1;
  }
  iVar8 = -1;
LAB_00246130:
  if (iVar8 == 0xd) {
    uVar11 = 0x248398;
  }
  else if (iVar8 < 0xe) {
    if (iVar8 != 0xc) {
LAB_002461c4:
      RT_CALLRA(0x2461ac, guest::gs_tasks_spawn(param_1,0x248180));
      goto LAB_002461d0;
    }
    uVar11 = 0x248580;
  }
  else if (iVar8 == 0xe) {
    uVar11 = 0x248a70;
  }
  else {
    if (iVar8 != 0xf) goto LAB_002461c4;
    uVar11 = 0x248bf0;
  }
  RT_CALLRA(0x2461c4, guest::gs_tasks_spawn(param_1,uVar11,0));
LAB_002461d0:
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 8))) = 0;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar22 + 0x25))) = 1;
  PlayUiSound(0x1a);
  return 0;
}


RT_GUEST_FN(0x002455c0, guest::drawCarouselScene)

}  // namespace guest
