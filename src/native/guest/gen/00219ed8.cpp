// guest:0x00219ed8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t QueryCarSurfaces(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x58, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x68) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x6c) = 0x00000000u;
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0x70);
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
  int iVar2;
  ulong uVar3;
  long lVar4;
  ulong uVar5;
  uint uVar6;
uint32_t piVar7;
uint32_t puVar8;
  int iVar9;
  float fVar10;
uint32_t auStack_80 = rtguest::allocFrame(64);
  
  iVar9 = (int)param_2;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x22))) == -1) {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0xa8)));
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0xa0)));
    if (iVar2 < 0) {
      iVar2 = iVar2 + 0x1ffffff;
    }
    if (iVar1 < 0) {
      iVar1 = iVar1 + 0xffffff;
    }
    uVar6 = iVar2 >> 0x19 & 1;
    iVar2 = ((iVar1 >> 0x18) - uVar6 & 2) + uVar6;
  }
  else {
    iVar2 = 2;
  }
  piVar7 = (uint32_t)(param_1 + iVar2 * 4 + 0x60);
  uVar3 = 0;
  if (-1 < (*(int32_t*)rtguest::G((uint32_t)piVar7))) {
    RT_CALLRA(0x219f7c, guest::vu0UnitMatrix(auStack_80));
    RT_CALLRA(0x219fb0, guest::vu0RotMatrixY(rtguest::f2bits((float)-(int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x1d4))) * (*(float*)rtguest::G(0x332b94)) * 3.0517578e-05f),auStack_80,
                  auStack_80));
    iVar2 = iVar2 * 4 + param_1;
    fVar10 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x94))) + 1.0;
    uVar3 = RT_CALLRA(0x219fe4, guest::TestQuadLateralMask(fVar10,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xe8))),param_2,auStack_80,
                                (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x70))) + 0x604)))));
    if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x22))) == -1) && ((*(int32_t*)rtguest::G((uint32_t)piVar7)) == 0xb)) {
      puVar8 = (uint32_t)(param_1 + 0xf8);
      piVar7 = (uint32_t)(param_1 + 0x80);
      iVar9 = 0;
      do {
        iVar2 = iVar9 + 1;
        lVar4 = RT_CALLRA(0x21a024, guest::TestProgressFlag(iVar9));
        if (lVar4 != 0) {
          uVar5 = RT_CALLRA(0x21a044, guest::TestQuadLateralMask(fVar10,(*(uint32_t*)rtguest::G((uint32_t)puVar8)),param_2,auStack_80,
                                      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)piVar7)) + 0x604)))));
          uVar3 = uVar3 | uVar5;
        }
        piVar7 = piVar7 + 0x4;  // T* element step
        puVar8 = puVar8 + 0x4;  // T* element step
        iVar9 = iVar2;
      } while (iVar2 < 0x1a);
    }
  }
  return uVar3;
}


RT_GUEST_FN(0x00219ed8, guest::QueryCarSurfaces)

}  // namespace guest
