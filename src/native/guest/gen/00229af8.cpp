// guest:0x00229af8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t MemcardSpaceCheckDialog(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
  undefined4 uVar2;
  byte bVar3;
  undefined8 uVar4;
  long lVar5;
  char cVar6;
  int iVar7;
  int iVar8;
uint32_t puVar9;
uint32_t puVar10;
uint32_t puVar11;
  int iVar12;
  
  iVar12 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 8))) == 0) {
    uVar4 = RT_CALLRA(0x229b20, guest::uiMenuCreate(param_1) /* $a0 = entry $a0 at the jal (MIPS: no $4 write/jal before 0x229b20); pseudocode dropped the live arg */);
    RT_CALLRA(0x229b54, guest::uiMenuAddItem(uVar4,(*(uint32_t*)rtguest::G((uint32_t)(0x29f110u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 0xd) * 4))),0));
    RT_CALLRA(0x229b80, guest::uiMenuAddItem(uVar4,(*(uint32_t*)rtguest::G((uint32_t)(0x29f114u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 0xd) * 4))),0));
    RT_CALLRA(0x229bac, guest::uiMenuAddItem(uVar4,(*(uint32_t*)rtguest::G((uint32_t)(0x29f118u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 0xd) * 4))),0));
    RT_CALLRA(0x229bd8, guest::uiMenuAddItem(uVar4,(*(uint32_t*)rtguest::G((uint32_t)(0x29f0fcu + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 0xd) * 4))),0));
    RT_CALLRA(0x229c04, guest::uiMenuAddItem(uVar4,(*(uint32_t*)rtguest::G((uint32_t)(0x29f100u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 0xd) * 4))),0));
    RT_CALLRA(0x229c30, guest::uiMenuAddItem(uVar4,(*(uint32_t*)rtguest::G((uint32_t)(0x29f104u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 0xd) * 4))),0));
    RT_CALLRA(0x229c5c, guest::uiMenuAddItem(uVar4,(*(uint32_t*)rtguest::G((uint32_t)(0x29f108u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 0xd) * 4))),0x10));
    RT_CALLRA(0x229c88, guest::uiMenuAddItem(uVar4,(*(uint32_t*)rtguest::G((uint32_t)(0x29f10cu + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 0xd) * 4))),0x10));
    RT_CALLRA(0x229c9c, guest::gs_tasks_spawn(param_1,0x22a320,0));
  }
  iVar7 = (*(uint32_t*)rtguest::G(0x333284));
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x28)));
  if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x24))) == '\0') {
LAB_00229eac:
    bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x25)));
  }
  else {
    bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x25)));
    if (bVar3 == 1) {
      iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 4)));
LAB_00229d64:
      if (iVar8 < 0) {
        cVar6 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x27)));
      }
      else {
        if (iVar8 < 2) goto LAB_00229ee4;
        if (iVar8 == 2) {
          uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 0x34 + 0x29f0ec)));
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x14))) =
               (*(uint32_t*)rtguest::G((uint32_t)(0x29f0e8u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 0xd) * 4)));
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x18))) = uVar2;
          if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x27))) == '\x01') {
            puVar9 = (*(uint32_t*)rtguest::G((uint32_t)(0x29f0f8u + (uint32_t)(iVar7 * 0xd) * 4)));
          }
          else {
            puVar9 = (*(uint32_t*)rtguest::G((uint32_t)(0x29f0f4u + (uint32_t)(iVar7 * 0xd) * 4)));
          }
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x1c))) = puVar9;
          RT_CALLRA(0x229d24, guest::gs_tasks_spawn(param_1,0x22a320,0));
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x25))) = 2;
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x27))) = 0;
          goto LAB_00229eac;
        }
        cVar6 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x27)));
      }
      if (cVar6 == '\0') {
        puVar9 = (*(uint32_t*)rtguest::G((uint32_t)(0x29f110u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 0xd) * 4)));
        puVar11 = (*(uint32_t*)rtguest::G((uint32_t)(0x29f114u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 0xd) * 4)));
        puVar10 = (*(uint32_t*)rtguest::G((uint32_t)(0x29f118u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 0xd) * 4)));
      }
      else {
        puVar9 = (*(uint32_t*)rtguest::G((uint32_t)(0x29f0e8u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 0xd) * 4)));
        puVar11 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 0x34 + 0x29f0ec)));
        puVar10 = (*(uint32_t*)rtguest::G((uint32_t)(0x29f0f0u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 0xd) * 4)));
      }
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x14))) = puVar9;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x18))) = puVar11;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x1c))) = puVar10;
      RT_CALLRA(0x229d48, guest::gs_tasks_spawn(param_1,0x22a320,0));
      cVar6 = '\x02';
LAB_00229ea8:
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x25))) = cVar6;
      goto LAB_00229eac;
    }
    if (bVar3 < 2) {
      if (bVar3 == 0) {
        iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 4)));
LAB_00229cf4:
        if (iVar7 < 0) {
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x27))) = 0;
        }
        else {
          if (iVar7 < 2) goto LAB_00229ee4;
          if (iVar7 == 2) {
            (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x27))) = 1;
            RT_CALLRA(0x229dec, guest::gs_tasks_spawn(param_1,0x22a320,1));
            cVar6 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x25))) + '\x01';
            goto LAB_00229ea8;
          }
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x27))) = 0;
        }
        RT_CALLRA(0x229e9c, guest::gs_tasks_spawn(param_1,0x22a320,1));
        cVar6 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x25))) + '\x01';
        goto LAB_00229ea8;
      }
      bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x25)));
    }
    else {
      if (bVar3 == 2) {
        iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 4)));
        goto LAB_00229cf4;
      }
      if (bVar3 == 3) {
        iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 4)));
        goto LAB_00229d64;
      }
      bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x25)));
    }
  }
  if (bVar3 < 2) {
    return 0;
  }
  lVar5 = (int32_t)RT_CALLRA(0x229ec0, guest::uiMenuRun(iVar1,param_2));
  if (lVar5 != 1) {
    return 0;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xd))) != '\0') {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x25))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x25))) & 1;
    return 0;
  }
LAB_00229ee4:
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 8))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0xc))) = 0x22a2f8;
  return 0;
}


RT_GUEST_FN(0x00229af8, guest::MemcardSpaceCheckDialog)

}  // namespace guest
