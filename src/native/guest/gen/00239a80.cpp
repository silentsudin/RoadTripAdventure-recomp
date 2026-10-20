// guest:0x00239a80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawCourseList(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
  }
uint32_t pbVar1;
  byte bVar2;
  short sVar3;
  int iVar4;
uint32_t puVar5;
  undefined8 uVar6;
  int iVar7;
  char cVar8;
uint32_t pbVar9;
  int iVar10;
  long lVar11;
uint32_t pbVar12;
  
  iVar10 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 8))) == 0) {
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2))) = (short)(char)(*(uint32_t*)rtguest::G((uint32_t)0x2be000 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))) * 2) * 4));
    uVar6 = RT_CALLRA(0x239adc, guest::uiMenuCreate(param_1) /* $a0 = entry $a0 at the jal (MIPS: no $4 write/jal before 0x239adc); pseudocode dropped the live arg */);
    sVar3 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2)));
    lVar11 = (long)sVar3;
    if (lVar11 < (int)sVar3 + (int)(char)(*(uint32_t*)rtguest::G((uint32_t)0x2be001 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))) * 2) * 4))) {
      pbVar9 = 0x2bddd0 + sVar3 * 0x10;
      pbVar12 = 0x01780750 + sVar3;
      bVar2 = (*(int8_t*)rtguest::G((uint32_t)pbVar12));
      while( true ) {
        pbVar12 = pbVar12 + 1;
        puVar5 = 0x003338d8;
        if (bVar2 < 6) {
          puVar5 = 0x003338d0;
        }
        RT_CALLRA(0x239b5c, guest::uiMenuAddItem(uVar6,puVar5,0x10));
        RT_CALLRA(0x239b80, guest::uiMenuAddItem(uVar6,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be398 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)pbVar9))) * 4)),0x90));
        pbVar1 = pbVar9 + 3;
        pbVar9 = pbVar9 + 0x10;
        if ((long)(*(int8_t*)rtguest::G(0x177fdb1)) < (long)(uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar1))) {
          RT_CALLRA(0x239bb4, guest::uiMenuAddItem(uVar6,(*(uint32_t*)rtguest::G((uint32_t)(0x2a2b08u + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)pbVar1))) * 4))),0x99));
          cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23)));
        }
        else {
          RT_CALLRA(0x239bc8, guest::uiMenuAddItem(uVar6,(*(uint32_t*)rtguest::G((uint32_t)(0x2a2b08u + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)pbVar1))) * 4))),0x98));
          cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23)));
        }
        lVar11 = (long)((int)lVar11 + 1);
        if ((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2))) + (int)(char)(*(uint32_t*)rtguest::G((uint32_t)0x2be001 + (uint32_t)(cVar8 * 2) * 4)) <= lVar11) break;
        bVar2 = (*(int8_t*)rtguest::G((uint32_t)pbVar12));
      }
    }
    RT_CALLRA(0x239c0c, guest::uiMenuAddItem(uVar6,0x2ec5d0,0x10));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar6 + 0x5f))) = 2;
    RT_CALLRA(0x239c20, guest::CenterRectangle(uVar6,5));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar6 + 0xe))) = 1;
    return 0;
  }
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28)));
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) != '\0') {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xe))) & 0x7f;
  }
  lVar11 = RT_CALLRA(0x239c50, guest::uiMenuRun(iVar4));
  if (lVar11 == 0) {
    return 0;
  }
  if (lVar11 == -2) {
    gs_tasks_kill(param_1);
    return 0;
  }
  if (lVar11 == -1) {
LAB_00239c88:
    RT_CALLRA(0x239c88, guest::PlayUiSound(0x1a));
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x18)));
    bVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x1c))) + 1)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = (uint32_t)bVar2;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xe))) = 2;
    return 0;
  }
  cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xd)));
  if ((long)cVar8 == (long)(int)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xb))) - 1)) goto LAB_00239c88;
  iVar7 = ((int)cVar8 + (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2)))) * 0x10;
  if (((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x2bddd3 + (uint32_t)(iVar7) * 1)) < 4) &&
     ((long)(*(int8_t*)rtguest::G(0x177fdb1)) < (long)(uint32_t)(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x2bddd3 + (uint32_t)(iVar7) * 1)))) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xe))) | 0x80;
    if ((*(uint8_t*)rtguest::G((uint32_t)0x2bddd3 + (uint32_t)(((int)cVar8 + (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2)))) * 0x10) * 1)) == '\x01') {
      gs_tasks_spawn(param_1,0x23bb00,0x2ec5e8);
      return 0;
    }
    if ((*(uint8_t*)rtguest::G((uint32_t)0x2bddd3 + (uint32_t)(((int)cVar8 + (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2)))) * 0x10) * 1)) != '\x02') {
      gs_tasks_spawn(param_1,0x23bb00,0x2ec618);
      return 0;
    }
    gs_tasks_spawn(param_1,0x23bb00,0x2ec600);
    return 0;
  }
  if (((int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xd))) + (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2))) == 0x19) &&
     (lVar11 = TestStateBitFc78(100), lVar11 == 0)) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xe))) | 0x80;
    gs_tasks_spawn(param_1,0x23bb00,0x2ec638);
    return 0;
  }
  RT_CALLRA(0x239e78, guest::PlayUiSound(0x1a));
  iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x18)));
  bVar2 = (*(int8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x1c)))));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = (uint32_t)bVar2;
  cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xd))) + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 2)));
  (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x20))) = cVar8;
  if (cVar8 == '\x19') {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14))) - 54000U < 0x1a5e1) {
      bVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x20)));
      goto LAB_00239ee4;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x20))) = 0x1b;
  }
  bVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x20)));
LAB_00239ee4:
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10))) = 0x2bddd0 + (uint32_t)bVar2 * 0x10;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xe))) = 2;
  return 0;
}


RT_GUEST_FN(0x00239a80, guest::DrawCourseList)

}  // namespace guest
