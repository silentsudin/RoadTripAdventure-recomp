// guest:0x00264498 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t emailBoxTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(928);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x380, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x350, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x358, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x360, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x368, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x370, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x378, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x388, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x390, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x398) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x39c) = 0x00000000u;
    *(uint32_t*)rtguest::G(rt_sv+0x340) = (uint32_t)(int32_t)param_2;
    rtguest::setS32(6, (uint32_t)param_1);  // walk: = param_1
  }
    uint32_t iStack_3a4 = rtguest::allocFrame(4);
uint32_t auStack_3a0 = rtguest::allocFrame(512);
uint32_t acStack_1a0 = rtguest::allocFrame(240);
uint32_t acStack_b0 = rtguest::allocFrame(32);
uint32_t acStack_90 = rtguest::allocFrame(48);
    uint32_t iStack_60 = rtguest::allocFrame(4);
    uint32_t iStack_5c = rtguest::allocFrame(4);
  byte bVar1;
  ushort uVar2;
uint32_t puVar3;
  char cVar4;
  short sVar5;
uint32_t __src;
  undefined8 uVar6;
  long lVar7;
  size_t sVar8;
  int iVar9;
  uint uVar10;
uint32_t pcVar11;
uint32_t ppuVar12;
  int iVar13;
uint32_t pcVar14;
  int iVar15;
  uint uVar16;
uint32_t puVar17;
  int iVar18;
  int iVar19;
  
  iVar18 = (int)param_1;
  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 8))) == 0) {
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(0x01760f80)), 0, 0x1480));
    ppuVar12 = 0x2db948u;
    iVar15 = 0;
    pcVar14 = (uint32_t)0x01760f80;
    do {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)pcVar14)) = (*(uint32_t*)rtguest::G(0x003352d8));
      iVar15 = iVar15 + 1;
      puVar3 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)ppuVar12));
      ppuVar12 = ppuVar12 + 1;
      (uint32_t)(uintptr_t)(::strcat((char *)rtguest::G((uint32_t)(pcVar14)), (char *)rtguest::G((uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar3))))));
      pcVar14 = pcVar14 + 0x29;
    } while (iVar15 < 0x80);
  }
  uVar16 = 0;
  if (0 < (int)(*(int32_t*)rtguest::G(0x177fd88))) {
    do {
      iVar15 = RT_CALLRA(0x264560, guest::GetQueuedByte(uVar16 & 0xff));
      iVar9 = (*(int32_t*)rtguest::G(0x177fd88)) - uVar16;
      uVar16 = uVar16 + 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iStack_3a4) + (uint32_t)(iVar9) * 4)) = (int)0x01760f80 + iVar15 * 0x29;
    } while ((int)uVar16 < (int)(*(int32_t*)rtguest::G(0x177fd88)));
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 8))) == 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x25))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x26))) = 0;
    uVar6 = RT_CALLRA(0x2645b0, guest::uiMenuCreate(param_1));
    RT_CALLRA(0x2645d0, guest::LayoutTextItems(uVar6,auStack_3a0,(*(int32_t*)rtguest::G(0x177fd88)),0xd));
    puVar17 = (uint32_t)uVar6;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar17 + 0x5f))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(puVar17 + 7))) = 1;
    (*(uint16_t*)rtguest::G((uint32_t)puVar17 + (uint32_t)(3) * sizeof(uint16_t))) = 0x9c;
    (*(uint16_t*)rtguest::G((uint32_t)puVar17)) = 100;
    (*(uint16_t*)rtguest::G((uint32_t)puVar17 + (uint32_t)(1) * sizeof(uint16_t))) = 0x30;
    (*(uint16_t*)rtguest::G((uint32_t)puVar17 + (uint32_t)(2) * sizeof(uint16_t))) = 0x1b8;
  }
  else {
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x25)));
    if (bVar1 == 1) {
      iVar15 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x28)));
      RT_CALLRA(0x264658, guest::uiDrawPanel9(0x50,0x18,0x1e0,0x18,0));
      RT_CALLRA(0x264670, guest::uiDrawText(0x78,0x19,0x3352f0,0));
      RT_CALLRA(0x264710, guest::uiDrawText(0x78,0x25,0x3352f8,0));
      (*(int32_t*)rtguest::G((uint32_t)iStack_5c)) = 0x3d;
      iVar9 = RT_CALLRA(0x26474c, guest::GetQueuedByte(((char)(*(int32_t*)rtguest::G(0x177fd88)) - (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0xd)))) + -1));
      iVar19 = 0;
      iVar9 = RT_CALLRA(0x2648c4, guest::uiTextWidth((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(0x2db948u + (uint32_t)(iVar9) * 4)))))));
      iVar13 = RT_CALLRA(0x2648ac, guest::GetQueuedByte(((char)(*(int32_t*)rtguest::G(0x177fd88)) - (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0xd)))) + -1));
      RT_CALLRA(0x26478c, guest::uiDrawText(0x208 - iVar9,0x19,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(0x2db948u + (uint32_t)(iVar13) * 4))))),0));
      iVar9 = RT_CALLRA(0x2648e0, guest::GetQueuedByte(((char)(*(int32_t*)rtguest::G(0x177fd88)) - (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0xd)))) + -1));
      iVar9 = RT_CALLRA(0x26492c, guest::uiTextWidth((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(0x2db948u + (uint32_t)(iVar9) * 4))) + 4)))));
      iVar13 = RT_CALLRA(0x264918, guest::GetQueuedByte(((char)(*(int32_t*)rtguest::G(0x177fd88)) - (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0xd)))) + -1));
      RT_CALLRA(0x2647a8, guest::uiDrawText(0x208 - iVar9,0x25,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(0x2db948u + (uint32_t)(iVar13) * 4))) + 4))),0));
      RT_CALLRA(0x264844, guest::uiDrawPanel9(0x50,0x3c,0x1e0,0x90,0));
      pcVar14 = acStack_1a0;
      for (iVar9 = 0; iVar9 < 0xb; iVar9 = iVar9 + 1) {
        iVar13 = RT_CALLRA(0x264948, guest::GetQueuedByte(((char)(*(int32_t*)rtguest::G(0x177fd88)) - (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0xd)))) + -1));
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(0x2db948u + (uint32_t)(iVar13) * 4))) + iVar9 * 4 + 8))) == 0) break;
        (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(pcVar14)), 0, 0x1a));
        iVar13 = RT_CALLRA(0x2649b0, guest::GetQueuedByte(((char)(*(int32_t*)rtguest::G(0x177fd88)) - (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0xd)))) + -1));
        (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)(acStack_1a0 + iVar19)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(0x2db948u + (uint32_t)(iVar13) * 4))) + iVar9 * 4 + 8)))))));
        pcVar11 = pcVar14;
        while ((*(int8_t*)rtguest::G((uint32_t)pcVar11)) != '\0') {
          if ((*(int8_t*)rtguest::G((uint32_t)pcVar11)) == '$') {
            if ((*(int8_t*)rtguest::G((uint32_t)pcVar11 + (uint32_t)(1) * sizeof(int8_t))) == 'N') {
              (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)(acStack_90)), (char *)rtguest::G((uint32_t)(pcVar11 + 2))));
              __src = (uint32_t)RT_CALLRA(0x264a2c, guest::GetSlotDataPtr(0,0));
              (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)(pcVar11)), (char *)rtguest::G((uint32_t)(__src))));
              (uint32_t)(uintptr_t)(::strcat((char *)rtguest::G((uint32_t)(pcVar14)), (char *)rtguest::G((uint32_t)(acStack_90))));
              break;
            }
            pcVar11 = pcVar11 + 1;
          }
          else {
            pcVar11 = pcVar11 + 1;
          }
        }
        pcVar11 = acStack_1a0 + iVar19;
        iVar19 = iVar19 + 0x15;
        pcVar14 = pcVar14 + 0x15;
        iVar13 = (*(int32_t*)rtguest::G((uint32_t)iStack_5c));
        (*(int32_t*)rtguest::G((uint32_t)iStack_5c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_5c)) + 0xc;
        RT_CALLRA(0x26485c, guest::uiDrawText(0x78,iVar13,pcVar11,0));
      }
      if (((*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x30))) & 0x5000000000) == 0) {
        cVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x26)));
        goto LAB_00264c5c;
      }
      RT_CALLRA(0x2647d8, guest::PlayUiSound(0x1b));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x25))) = 3;
    }
    else if (bVar1 < 2) {
      if (bVar1 != 0) {
        cVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x26)));
        goto LAB_00264c5c;
      }
      iVar15 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x28)));
      RT_CALLRA(0x264980, guest::uiDrawPanel9(100,0x18,0x1b8,0xc,0));
      RT_CALLRA(0x26487c, guest::uiDrawText(0x8c,0x19,0x32e230,0));
      cVar4 = '\0';
      uVar16 = 0;
      if (0 < (int)(*(int32_t*)rtguest::G(0x177fd88))) {
        uVar10 = 0;
        do {
          uVar16 = uVar16 + 1;
          lVar7 = RT_CALLRA(0x264698, guest::HasQueuedByteFlag(uVar10));
          if (lVar7 == 0) {
            cVar4 = cVar4 + '\x01';
          }
          uVar10 = uVar16 & 0xff;
        } while ((int)uVar16 < (int)(*(int32_t*)rtguest::G(0x177fd88)));
      }
      ::sprintf((char *)rtguest::G((uint32_t)(acStack_b0)), (char *)rtguest::G((uint32_t)(0x32e240)), cVar4, (*(int32_t*)rtguest::G(0x177fd88)));
      iVar19 = 0;
      sVar8 = ::strlen((char *)rtguest::G((uint32_t)(acStack_b0)));
      RT_CALLRA(0x264900, guest::uiDrawText((int)sVar8 * -10 + 0x1e0,0x19,acStack_b0,0));
      RT_CALLRA(0x264728, guest::uiMenuWindow(iVar15,(*(int32_t*)rtguest::G((uint32_t)iStack_60)),auStack_3a0));
      iVar13 = 0x31;
      iVar9 = 0x31;
      do {
        lVar7 = RT_CALLRA(0x264aa8, guest::GetQueuedByte(((((*(int32_t*)rtguest::G(0x177fd88)) & 0xff) - iVar19) - (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x10)))) -
                              1 & 0xff));
        if (lVar7 != -1) {
          lVar7 = RT_CALLRA(0x264770, guest::HasQueuedByteFlag(((((*(int32_t*)rtguest::G(0x177fd88)) & 0xff) - iVar19) -
                                    (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x10)))) - 1 & 0xff));
          if (lVar7 == 0) {
            RT_CALLRA(0x264968, guest::uiDrawText(0x78,iVar13,0x3352e8,0xd));
          }
          else {
            RT_CALLRA(0x264a6c, guest::uiDrawText(0x78,iVar9,0x3352e0,0xd));
          }
        }
        iVar13 = iVar13 + 0xc;
        iVar19 = iVar19 + 1;
        iVar9 = iVar9 + 0xc;
      } while (iVar19 < 0xd);
      uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x34)));
      if ((uVar2 & 0x40) != 0) {
        RT_CALLRA(0x264ae8, guest::PlayUiSound(0x1a));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x25))) = 2;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x1c))) = 0;
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 2))) = 0;
        RT_CALLRA(0x264808, guest::SetQueuedByteFlag(((char)(*(int32_t*)rtguest::G(0x177fd88)) - (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0xd)))) + -1));
        uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x34)));
      }
      if ((uVar2 & 0x10) != 0) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x26))) = 1;
      }
    }
    else {
      if (bVar1 == 2) {
        sVar5 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 2)));
        if (sVar5 < 0xf0) {
          sVar5 = sVar5 + 0x10;
          (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 2))) = sVar5;
        }
        iVar15 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x1c)));
        if (iVar15 < 0x90) {
          iVar15 = iVar15 + 0x10;
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x1c))) = iVar15;
        }
        if (0xef < sVar5) {
          sVar5 = 0xf0;
          (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 2))) = 0xf0;
        }
        if (0x8f < iVar15) {
          iVar15 = 0x90;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x1c))) = 0x90;
        }
        if ((sVar5 == 0xf0) && (iVar15 == 0x90)) {
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x25))) = 1;
        }
        RT_CALLRA(0x264bb8, guest::uiDrawPanel9(0x140 - sVar5,(0x108 - iVar15) / 2,(int)sVar5 << 1,iVar15,0));
        cVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x26)));
        goto LAB_00264c5c;
      }
      if (bVar1 != 3) {
        cVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x26)));
        goto LAB_00264c5c;
      }
      lVar7 = (long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 2)));
      if (0 < lVar7) {
        iVar15 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 2))) + -0x10;
        lVar7 = (long)iVar15;
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 2))) = (short)iVar15;
      }
      iVar15 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x1c)));
      uVar16 = (uint32_t)lVar7;
      if (0 < iVar15) {
        iVar15 = iVar15 + -0x10;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x1c))) = iVar15;
      }
      if ((int)(uVar16 << 0x10) < 1) {
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 2))) = 0;
        uVar16 = 0;
      }
      if (iVar15 < 1) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x1c))) = 0;
        iVar15 = 0;
      }
      if (((uVar16 & 0xffff) == 0) && (iVar15 == 0)) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x25))) = 0;
      }
      RT_CALLRA(0x264c50, guest::uiDrawPanel9(0x140 - (short)uVar16,(0x108 - iVar15) / 2,(int)(short)uVar16 << 1,iVar15,0));
    }
  }
  cVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x26)));
LAB_00264c5c:
  if (cVar4 != '\0') {
    iVar18 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 4))) = 0xffffffff;
    RT_CALLRA(0x264c74, guest::gs_tasks_kill(param_1));
  }
  return 0;
}


RT_GUEST_FN(0x00264498, guest::emailBoxTask)

}  // namespace guest
