// guest:0x00239508 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ShowResidentMessage(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  byte bVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  long lVar4;
  uint uVar5;
uint32_t piVar6;
  int iVar7;
  int iVar8;
  
  iVar8 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) == 0) {
    uVar2 = RT_CALLRA(0x23952c, guest::uiMenuCreate(param_1) /* $a0 = entry $a0 at the jal (MIPS: no $4 write/jal before 0x23952c); pseudocode dropped the live arg */);
    RT_CALLRA(0x239544, guest::uiMenuAddItem(uVar2,0x2ec598,0));
    uVar3 = RT_CALLRA(0x239550, guest::GetSlotDataPtr((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))),0));
    RT_CALLRA(0x239560, guest::uiMenuAddItem(uVar2,uVar3,0x80));
    RT_CALLRA(0x239574, guest::uiMenuAddItem(uVar2,0x2ec5b0,0));
    RT_CALLRA(0x239588, guest::uiMenuAddItem(uVar2,0x3338a0,0x10));
    RT_CALLRA(0x23959c, guest::uiMenuAddItem(uVar2,0x333898,0x10));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar2 + 0x5f))) = 2;
  }
  else {
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    lVar4 = (int32_t)RT_CALLRA(0x2395b4, guest::uiMenuRun(iVar7,param_2));
    if (lVar4 != 0) {
      if ((lVar4 != -1) && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xd))) == '\0')) {
        for (iVar7 = 1; iVar7 < 8; iVar7 = iVar7 + 1) {
          RT_CALLRA(0x239644, guest::ClearFlaggedSlotBit(iVar7,(*(uint8_t*)rtguest::G((uint32_t)0x177f76c + (uint32_t)(iVar7 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) * 0x1c) * 1)),0))
          ;
        }
        uVar5 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
        piVar6 = 0x0177ea60;
        iVar7 = 10;
        do {
          iVar7 = iVar7 + -1;
          if ((*(int32_t*)rtguest::G((uint32_t)piVar6)) == (int)0x177f760 + uVar5 * 0x1c) {
            (*(int32_t*)rtguest::G((uint32_t)piVar6)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 +
                              (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177fdd0 + uVar5 * 2 + 2))) * 4))) +
                      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177fdd0 + uVar5 * 2 + 3))) * 0x10;
            uVar5 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
          }
          piVar6 = piVar6 + 0x4;  // T* element step
        } while (-1 < iVar7);
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177fdd0 + uVar5 * 2 + 2))) = 0;
        iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c)))));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = (uint32_t)bVar1;
        RT_CALLRA(0x239708, guest::gs_tasks_kill(param_1));
        PlayUiSound(0x1a);
        return 0;
      }
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xc))) = 0x239748;
      ShowSequencedMessages(param_1,param_2);
      PlayUiSound(0x1b);
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x00239508, guest::ShowResidentMessage)

}  // namespace guest
