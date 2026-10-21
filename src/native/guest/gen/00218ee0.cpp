// guest:0x00218ee0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AdvanceVehicleMotion(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, uint32_t param_8, ...) {
  rtguest::FrameGuard rt_fg(224);
    rtguest::allocFrame(4);  // guest frame gap (sp+0x0..0x3)
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x90, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x80, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x88, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x98, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0xa0, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0xa8, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0xb0, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0xb8, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0xc0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xc8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xcc) = 0x00000000u;
    rtguest::rtSpillF(rt_fg, 5, rt_sv, 0xd8);
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0xd0);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = (uint32_t)(int32_t)param_1;
    rtguest::setS32(2, (uint32_t)param_2);  // walk: = param_2
  }
    uint32_t iStack_dc = rtguest::allocFrame(4);  // sp+0x4 (Ghidra entry-sp -0xdc, frame 0xe0)
rtguest::allocFrame(8);  // guest frame gap (sp+0x8..0xf)
uint32_t auStack_d0 = rtguest::allocFrame(32);  // sp+0x10 (Ghidra entry-sp -0xd0, frame 0xe0)
    rtguest::allocFrame(4);  // guest frame gap (sp+0x30..0x33)
    uint32_t iStack_ac = rtguest::allocFrame(4);  // sp+0x34 (Ghidra entry-sp -0xac, frame 0xe0)
    uint32_t iStack_a8 = rtguest::allocFrame(4);  // sp+0x38 (Ghidra entry-sp -0xa8, frame 0xe0)
    uint32_t iStack_a4 = rtguest::allocFrame(4);  // sp+0x3c (Ghidra entry-sp -0xa4, frame 0xe0)
    uint32_t iStack_a0 = rtguest::allocFrame(4);  // sp+0x40 (Ghidra entry-sp -0xa0, frame 0xe0)
    uint32_t iStack_9c = rtguest::allocFrame(4);  // sp+0x44 (Ghidra entry-sp -0x9c, frame 0xe0)
    uint32_t iStack_98 = rtguest::allocFrame(4);  // sp+0x48 (Ghidra entry-sp -0x98, frame 0xe0)
  bool bVar1;
  char cVar2;
  short sVar3;
  short sVar4;
  int iVar5;
  short sVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  float fVar16;
uint32_t piStack_b0;
  
  iVar9 = 0;
  sVar6 = 0;
  sVar4 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d0)));
  if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d0))) < 0x251d) {
    if ((param_7 & 1) != 0) {
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23c))) - (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x240)));
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23c))) = iVar5;
      if (iVar5 < 0) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23c))) = 0;
        sVar6 = 3000;
      }
      else {
        sVar6 = 9000;
      }
      goto LAB_00218f7c;
    }
    cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1ff)));
  }
  else {
LAB_00218f7c:
    cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1ff)));
  }
  sVar3 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(cVar2 * 2 + param_2 + 0x22c)));
  iVar5 = param_5;
  if (param_8 == 0) {
    iVar5 = 0;
  }
  iVar12 = 0x280 / (int)sVar3;
  if (sVar3 == 0) {
    trap(7);
  }
  if (param_8 == 0) {
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x218)));
    iVar10 = 0;
  }
  else {
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x218)));
    iVar10 = param_5 * iVar8;
  }
  if (iVar12 < 0) {
    iVar12 = -iVar12;
  }
  iVar13 = 0;
  if (iVar5 != 0) {
    iVar13 = iVar8;
  }
  if (iVar12 == 0) {
    iVar12 = 1;
  }
  if (sVar4 < sVar6) {
    if (sVar3 == 0) {
      trap(7);
    }
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x214))) << 7;
LAB_0021902c:
    iVar5 = iVar5 / (int)sVar3;
  }
  else {
    if (sVar6 < sVar4) {
      if (sVar3 == 0) {
        trap(7);
      }
      iVar5 = -0x3e800;
      goto LAB_0021902c;
    }
    iVar5 = 0;
  }
  iVar11 = (int)sVar3;
  if (iVar10 == 0) {
    iVar14 = 0;
  }
  else {
    iVar9 = sVar4 * iVar11;
    if (iVar9 < 0) {
      iVar9 = iVar9 + 0x7f;
    }
    iVar9 = param_3 - (iVar9 >> 7);
    iVar14 = iVar9 * iVar13;
  }
  (*(int32_t*)rtguest::G((uint32_t)iStack_98)) = (iVar5 + iVar14) / iVar8;
  iVar15 = sVar4 * iVar11;
  if (iVar8 == 0) {
    trap(7);
  }
  if (iVar15 < 0) {
    iVar15 = iVar15 + 0x7f;
  }
  iVar15 = ((iVar15 >> 7) + (*(int32_t*)rtguest::G((uint32_t)iStack_98))) * iVar13;
  if (iVar15 < 0) {
    iVar7 = iVar15;
    if (-param_6 < iVar15) goto LAB_002190d4;
  }
  else {
    iVar7 = param_6;
    if (iVar15 < param_6) {
      iVar7 = iVar15;
    }
LAB_002190d4:
    param_6 = -iVar7;
  }
  iVar5 = iVar5 + param_6;
  iVar15 = iVar5 / iVar8;
  if (iVar8 == 0) {
    trap(7);
  }
  iVar14 = iVar5 - iVar14;
  iVar13 = -param_4 * iVar13;
  (*(int32_t*)rtguest::G((uint32_t)iStack_dc)) = iVar13;
  if (iVar10 < 0) {
LAB_00219210:
    (*(uint32_t*)rtguest::G((uint32_t)iStack_9c)) = 0;
  }
  else {
    if (iVar10 == 0) {
      (*(int32_t*)rtguest::G((uint32_t)iStack_dc)) = 0;
      goto LAB_00219210;
    }
    fVar16 = SQRT((float)iVar14 * (float)iVar14 + (float)iVar13 * (float)iVar13);
    if (fVar16 <= (float)iVar10) goto LAB_00219210;
    iVar8 = iVar10 << 1;
    if (iVar13 < 0) {
      if (iVar8 < -iVar13) {
        (*(int32_t*)rtguest::G((uint32_t)iStack_dc)) = iVar10 * -2;
      }
    }
    else {
      (*(int32_t*)rtguest::G((uint32_t)iStack_dc)) = iVar8;
      if (iVar13 <= iVar8) {
        (*(int32_t*)rtguest::G((uint32_t)iStack_dc)) = iVar13;
      }
    }
    (*(uint32_t*)rtguest::G((uint32_t)iStack_9c)) = (int)(rtguest::fdiv(fVar16, (float)iVar10));
  }
  iVar8 = 0;
  iVar13 = (*(int32_t*)rtguest::G((uint32_t)iStack_98));
  if (param_5 < 0) {
    if (sVar3 == 0) {
      trap(7);
    }
    if (-1 < (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d0))) + ((*(int32_t*)rtguest::G((uint32_t)iStack_98)) << 7) / iVar11) {
      iVar8 = 0 / iVar12;
      goto LAB_0021931c;
    }
    iVar15 = -param_3;
  }
  else {
    iVar7 = -iVar10;
    if (iVar10 < iVar14) {
      iVar14 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x218)));
      iVar8 = iVar5 - iVar10;
      iVar13 = iVar10 / iVar14;
      if (iVar14 == 0) {
        trap(7);
      }
      if (iVar9 < 0) {
        if (iVar12 == 0) {
          trap(7);
        }
        if (iVar9 <= iVar13 + iVar8 / iVar12) {
          iVar15 = iVar10 / iVar14;
          goto LAB_00219308;
        }
        iVar8 = 0;
        iVar13 = iVar9;
      }
      iVar15 = iVar10 / iVar14;
    }
    else {
      if (iVar7 <= iVar14) {
        iVar8 = 0 / iVar12;
        goto LAB_0021931c;
      }
      iVar14 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x218)));
      iVar8 = iVar5 + iVar10;
      iVar13 = iVar7 / iVar14;
      if (iVar14 == 0) {
        trap(7);
      }
      if (0 < iVar9) {
        if (iVar12 == 0) {
          trap(7);
        }
        if (iVar13 + iVar8 / iVar12 <= iVar9) {
          iVar15 = iVar7 / iVar14;
          goto LAB_00219308;
        }
        iVar8 = 0;
        iVar13 = iVar9;
      }
      iVar15 = iVar7 / iVar14;
    }
LAB_00219308:
    if (iVar14 == 0) {
      trap(7);
    }
  }
  iVar8 = iVar8 / iVar12;
LAB_0021931c:
  if (iVar12 == 0) {
    trap(7);
  }
  (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int32_t))) = param_3 + iVar15;
  (*(int32_t*)rtguest::G((uint32_t)param_1)) = param_4 + (*(int32_t*)rtguest::G((uint32_t)iStack_dc)) / (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x218)));
  bVar1 = (*(int32_t*)rtguest::G(0x3356d4)) != 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1f8))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1f8))) | 2;
  piStack_b0 = param_1;
  (*(int32_t*)rtguest::G((uint32_t)iStack_ac)) = param_3;
  (*(int32_t*)rtguest::G((uint32_t)iStack_a8)) = param_4;
  (*(int32_t*)rtguest::G((uint32_t)iStack_a4)) = param_5;
  (*(int32_t*)rtguest::G((uint32_t)iStack_a0)) = param_8;
  if (bVar1) {
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_d0)), (char *)rtguest::G((uint32_t)(0x2eadb0)), iVar5);
    RT_CALLRA(0x2193a0, guest::uiDebugText8(300,0x78,auStack_d0,0xf));
  }
  iVar9 = (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d0))) + ((iVar13 + iVar8) * 0x80) / iVar11;
  (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d0))) = (short)iVar9;
  if (10000 < iVar9 * 0x10000 >> 0x10) {
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d0))) = 10000;
  }
  if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d0))) < 0) {
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d0))) = 0;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_a0)) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1bc))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)iStack_9c)) = 0;
    (*(int32_t*)rtguest::G((uint32_t)piStack_b0)) = 0;
    (*(int32_t*)rtguest::G((uint32_t)piStack_b0 + (uint32_t)(2) * sizeof(int32_t))) = 0;
  }
  else {
    iVar11 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d0))) * iVar11;
    if (iVar11 < 0) {
      iVar11 = iVar11 + 0x7f;
    }
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1bc))) = iVar11 >> 7;
  }
  if ((*(int32_t*)rtguest::G(0x3356d4)) != 0) {
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_d0)), (char *)rtguest::G((uint32_t)(0x2eadc0)), (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1bc))), (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d0))));
    RT_CALLRA(0x219448, guest::uiDebugText8(300,0x88,auStack_d0,0xf));
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_d0)), (char *)rtguest::G((uint32_t)(0x2eadd0)), (*(uint32_t*)rtguest::G((uint32_t)iStack_9c)));
    RT_CALLRA(0x219470, guest::uiDebugText8(300,0x98,auStack_d0,0xf));
  }
  return (*(uint32_t*)rtguest::G((uint32_t)iStack_9c));;
}


RT_GUEST_FN(0x00218ee0, guest::AdvanceVehicleMotion)

}  // namespace guest
