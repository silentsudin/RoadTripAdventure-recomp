// guest:0x00223d58 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateCarEffectSlots(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x38, 8);
    rtguest::gS[2] = 0x0000000000000000ull;  // walk const
    rtguest::setS32(3, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(4, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(6, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[7] = 0x0000000001730000ull;  // walk const
  }
  int iVar1;
  uint uVar2;
uint32_t puVar3;
  int iVar4;
  float fVar5;
  float fVar6;
  
  iVar4 = 0;
  iVar1 = 0;
  do {
    puVar3 = 0x0172ff70 + iVar1;
    fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)((int)0x0172ff7c + iVar1)));
    if (0.0 < fVar5) {
      if (7 < (int)(char)(*(uint8_t*)rtguest::G((uint32_t)0x1730770 + (uint32_t)(iVar4))) - 1U) goto switchD_00223df4_caseD_4;
      uVar2 = param_1 >> 2;
      switch((*(uint8_t*)rtguest::G((uint32_t)0x1730770 + (uint32_t)(iVar4)))) {
      case 1:
        RT_CALLRA(0x223e18, guest::drawCarEffectB((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0172ff8c + iVar1))),fVar5,uVar2 & 3,param_3,puVar3,1));
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x8000) == 0) {
          fVar5 = 0.015625;
          fVar6 = (*(float*)rtguest::G((uint32_t)(uint32_t)((int)0x0172ff7c + iVar1)));
LAB_00223ee4:
          (*(float*)rtguest::G((uint32_t)(uint32_t)((int)0x0172ff7c + iVar1))) = fVar6 - fVar5;
          if (fVar6 - fVar5 < 0.0) {
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0172ff7c + iVar1))) = 0;
LAB_00223f04:
            (*(uint8_t*)rtguest::G((uint32_t)0x1730770 + (uint32_t)(iVar4))) = 0;
          }
LAB_00223f0c:
          (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff8c + iVar1))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff8c + iVar1))) + 0.0625;
          goto switchD_00223df4_caseD_4;
        }
        break;
      case 2:
        RT_CALLRA(0x223e58, guest::drawCarEffectB((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0172ff8c + iVar1))),fVar5,uVar2 & 3,param_3,puVar3,2));
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x8000) == 0) {
          fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)((int)0x0172ff7c + iVar1))) - 0.0078125;
          (*(float*)rtguest::G((uint32_t)(uint32_t)((int)0x0172ff7c + iVar1))) = fVar5;
          if (fVar5 <= 0.125) {
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0172ff7c + iVar1))) = 0;
            goto LAB_00223f04;
          }
          goto LAB_00223f0c;
        }
        break;
      case 3:
        RT_CALLRA(0x223ec0, guest::drawCarEffectB((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0172ff8c + iVar1))),fVar5,uVar2 & 3,param_3,puVar3,3));
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x8000) == 0) {
          fVar6 = (*(float*)rtguest::G((uint32_t)(uint32_t)((int)0x0172ff7c + iVar1)));
          fVar5 = 0.0078125;
          goto LAB_00223ee4;
        }
        break;
      default:
        goto switchD_00223df4_caseD_4;
      case 7:
        RT_CALLRA(0x223f6c, guest::drawCarEffectB(fVar5,0x3f000000,0,param_3,puVar3,7));
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x8000) == 0) {
          fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)((int)0x0172ff7c + iVar1))) + 0.0078125;
          (*(float*)rtguest::G((uint32_t)(uint32_t)((int)0x0172ff7c + iVar1))) = fVar5;
          if (fVar5 < 1.0) {
            fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff74 + iVar1)));
            if ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff8c + iVar1))) <= fVar5) {
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0172ff7c + iVar1))) = 0;
              goto LAB_00224004;
            }
          }
          else {
            fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff74 + iVar1)));
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0172ff7c + iVar1))) = 0;
LAB_00224004:
            (*(uint8_t*)rtguest::G((uint32_t)0x1730770 + (uint32_t)(iVar4))) = 0;
          }
          (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff74 + iVar1))) = fVar5 + 0.0625;
          (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff84 + iVar1))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff84 + iVar1))) + 0.0625;
          goto switchD_00223df4_caseD_4;
        }
        break;
      case 8:
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x8000) == 0) {
          RT_CALLRA(0x223f38, guest::AdvanceCarEffectSlot(iVar4));
          fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)((int)0x0172ff7c + iVar1)));
          if ((*(uint8_t*)rtguest::G((uint32_t)0x17307f4 + (uint32_t)(iVar4 * 8))) != '\0') {
            (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x017307f0 + iVar4 * 8))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x017307f0 + iVar4 * 8))) + 1;
          }
        }
        RT_CALLRA(0x223f98, guest::drawCarEffectB((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0172ff8c + iVar1))),fVar5,uVar2 & 3,param_3,puVar3,3));
      }
    }
    else {
switchD_00223df4_caseD_4: ;
    }
    iVar4 = iVar4 + 2;
    if (0x7f < iVar4) {
      return 0;
    }
    iVar1 = iVar4 * 0x10;
  } while( true );
}


RT_GUEST_FN(0x00223d58, guest::UpdateCarEffectSlots)

}  // namespace guest
