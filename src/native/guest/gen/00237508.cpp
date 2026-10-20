// guest:0x00237508 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t EnterLicenseScreen(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  byte bVar1;
  char cVar2;
  undefined4 uVar3;
  undefined8 uVar4;
uint32_t pbVar5;
  uint uVar6;
uint32_t pbVar7;
  int iVar8;
  int iVar9;
  
  iVar9 = (int)param_2;
  iVar8 = (int)param_1;
  if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 10))) != '\0') || ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) == '\x1a')) {
    RT_CALLRA(0x237618, guest::uiFade(0xffffffff80000000));
    if ((*(int8_t*)rtguest::G(0x178700a)) == 0) {
      iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 4))) = 2;
    }
    else {
      iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 4))) = 1;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))) | 0x400000;
    gs_tasks_kill(param_1);
    return 0;
  }
  uVar4 = RT_CALLRA(0x237538, guest::getExtraPacketArea(0x1f0));
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))) = (int)uVar4;
  RT_CALLRA(0x237544, guest::InitScreenLayout(uVar4));
  uVar3 = RT_CALLRA(0x23754c, guest::SumPlayerPoints((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x10)))));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))) = uVar3;
  if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) < 0x18) {
    pbVar5 = 0x01780750;
    pbVar7 = 0x002bddd3;
    uVar6 = 0;
    if ((long)(*(int8_t*)rtguest::G(0x177fdb1)) == 3) {
      cVar2 = '\0';
    }
    else {
      do {
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar7));
        pbVar7 = pbVar7 + 0x10;
        if ((uint32_t)bVar1 == (long)(*(int8_t*)rtguest::G(0x177fdb1))) {
          if (uVar6 == (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20)))) {
            if ((*(int8_t*)rtguest::G(0x178700a)) < 6) goto LAB_002375d8;
            bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
          }
          else {
            bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
          }
          cVar2 = '\0';
          if (5 < bVar1) goto LAB_002375e8;
        }
LAB_002375d8:
        uVar6 = uVar6 + 1;
        pbVar5 = pbVar5 + 1;
      } while ((int)uVar6 < 0x18);
      cVar2 = (*(int8_t*)rtguest::G(0x177fdb1)) + '\x01';
    }
LAB_002375e8:
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x27))) = cVar2;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xc))) = 0x2364b0;
  DrawLicenseResultsScreen(param_1,param_2);
  return 0;
}


RT_GUEST_FN(0x00237508, guest::EnterLicenseScreen)

}  // namespace guest
