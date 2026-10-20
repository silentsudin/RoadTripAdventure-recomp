// guest:0x0023ad00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InterpretScriptCommands(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t pbVar1;
  int iVar2;
  byte bVar3;
  int iVar4;
  uint uVar5;
  ulong uVar6;
  long lVar7;
uint32_t pbVar8;
  
LAB_0023b13c:
  bVar3 = (*(int8_t*)rtguest::G((uint32_t)param_3));
  pbVar8 = param_3;
LAB_0023b140:
  if (0x1f < bVar3) {
    return pbVar8;
  }
  param_3 = pbVar8 + 1;
  iVar4 = (int)param_2;
  switch((*(int8_t*)rtguest::G((uint32_t)pbVar8))) {
  case 1:
    goto switchD_0023ad48_caseD_1;
  case 2:
    lVar7 = TestFlagBitFc98((*(int8_t*)rtguest::G((uint32_t)param_3)));
    if (lVar7 != 0) {
      param_3 = pbVar8 + 3;
      goto LAB_0023b13c;
    }
    uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar8 + (uint32_t)(2) * sizeof(int8_t)));
    goto LAB_0023afe0;
  case 3:
    lVar7 = GetFlaggedSlotCount((*(int8_t*)rtguest::G((uint32_t)param_3)),(*(int8_t*)rtguest::G((uint32_t)pbVar8 + (uint32_t)(2) * sizeof(int8_t))),0);
    goto LAB_0023afd4;
  case 4:
    lVar7 = TestStampFlag((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x23))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x21))));
    if (lVar7 == 0) {
      param_3 = pbVar8 + 2;
      goto LAB_0023b13c;
    }
    uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)param_3));
    goto LAB_0023afe0;
  case 5:
    lVar7 = TestFlagFc68((*(int8_t*)rtguest::G((uint32_t)param_3)));
    if (lVar7 == 0) {
      param_3 = pbVar8 + 3;
      goto LAB_0023b13c;
    }
    uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar8 + (uint32_t)(2) * sizeof(int8_t)));
    goto LAB_0023afe0;
  case 6:
    lVar7 = TestStateBitFc78((*(int8_t*)rtguest::G((uint32_t)param_3)));
    if (lVar7 == 0) {
      param_3 = pbVar8 + 3;
      goto LAB_0023b13c;
    }
    uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar8 + (uint32_t)(2) * sizeof(int8_t)));
    goto LAB_0023afe0;
  case 7:
    lVar7 = CheckSlotPresence();
    if (lVar7 == 0) {
      param_3 = pbVar8 + 2;
      goto LAB_0023b13c;
    }
    uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)param_3));
    goto LAB_0023afe0;
  case 8:
    lVar7 = MatchSlotPairState((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x23))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x21))));
    if (lVar7 != 0) {
      uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)param_3));
      goto LAB_0023afe0;
    }
    lVar7 = TestFlagFcb8((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x23))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x21))));
    if (lVar7 != 0) {
      param_3 = pbVar8 + 3;
      goto LAB_0023b13c;
    }
    uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar8 + (uint32_t)(2) * sizeof(int8_t)));
    goto LAB_0023afe0;
  default:
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ec718u)), (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar8)), (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x22))), (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x21))));
    if ((*(int8_t*)rtguest::G((uint32_t)param_3)) < 0x20) {
      for (param_3 = pbVar8 + 2; (*(int8_t*)rtguest::G((uint32_t)param_3)) < 0x20; param_3 = param_3 + 1) {
      }
    }
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)((uint32_t)0x333928u)), param_3);
    goto LAB_0023b13c;
  case 0xb:
    uVar6 = GetOrSetPresidentRaceResult(0xffffffffffffffff);
    if (uVar6 == (*(int8_t*)rtguest::G((uint32_t)param_3))) {
      uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar8 + (uint32_t)(2) * sizeof(int8_t)));
      goto LAB_0023afe0;
    }
    break;
  case 0xd:
    lVar7 = TestFlagFca8Clear((*(int8_t*)rtguest::G((uint32_t)param_3)));
    if (lVar7 == 0) {
      param_3 = pbVar8 + 3;
      goto LAB_0023b13c;
    }
    uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar8 + (uint32_t)(2) * sizeof(int8_t)));
    goto LAB_0023afe0;
  case 0xe:
    lVar7 = TestFlagBitFc98(0x50);
    if (lVar7 == 0) {
      param_3 = pbVar8 + 7;
    }
    else {
      iVar4 = FindCurrentTaskIndex();
      param_3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((uint32_t)(*(int8_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(iVar4) * sizeof(int8_t))) * 4 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))))));
    }
    goto LAB_0023b13c;
  case 0xf:
    pbVar8 = pbVar8 + 2;
    SetFlagBitFc98((*(int8_t*)rtguest::G((uint32_t)param_3)));
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)pbVar8));
    goto LAB_0023b140;
  case 0x10:
    pbVar8 = pbVar8 + 2;
    ClearFlagBitFc98((*(int8_t*)rtguest::G((uint32_t)param_3)));
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)pbVar8));
    goto LAB_0023b140;
  case 0x11:
    pbVar1 = pbVar8 + 2;
    pbVar8 = pbVar8 + 3;
    ClearFlaggedSlotBit((*(int8_t*)rtguest::G((uint32_t)param_3)),(*(int8_t*)rtguest::G((uint32_t)pbVar1)),0);
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)pbVar8));
    goto LAB_0023b140;
  case 0x12:
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = 10;
    goto LAB_0023b13c;
  case 0x13:
    pbVar8 = pbVar8 + 2;
    RecordTaskResult((*(int8_t*)rtguest::G((uint32_t)param_3)));
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)pbVar8));
    goto LAB_0023b140;
  case 0x14:
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 0xc;
    ReleaseStampFlag(param_2);
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x23))) = (*(int8_t*)rtguest::G((uint32_t)param_3));
    param_3 = pbVar8 + 3;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x21))) = (*(int8_t*)rtguest::G((uint32_t)pbVar8 + (uint32_t)(2) * sizeof(int8_t)));
    goto LAB_0023b13c;
  case 0x15:
    pbVar8 = pbVar8 + 2;
    SetStateBitFc88((*(int8_t*)rtguest::G((uint32_t)param_3)));
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)pbVar8));
    goto LAB_0023b140;
  case 0x16:
    lVar7 = TestProgressFlagForPair((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x23))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x21))));
    if (lVar7 == 0) {
      param_3 = pbVar8 + 2;
      goto LAB_0023b13c;
    }
    uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)param_3));
    goto LAB_0023afe0;
  case 0x17:
    goto switchD_0023ad48_caseD_17;
  case 0x18:
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x14))) - 54000U < 0x23281) {
      param_3 = pbVar8 + 2;
      goto LAB_0023b13c;
    }
    uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)param_3));
    goto LAB_0023afe0;
  case 0x19:
    lVar7 = TestProgressFlag((*(int8_t*)rtguest::G((uint32_t)param_3)));
    if (lVar7 == 0) {
      param_3 = pbVar8 + 3;
      goto LAB_0023b13c;
    }
    uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar8 + (uint32_t)(2) * sizeof(int8_t)));
    goto LAB_0023afe0;
  case 0x1a:
    iVar4 = CountBitsFca8Fcb0(0);
    if ((int)(uint32_t)(*(int8_t*)rtguest::G((uint32_t)param_3)) <= 100 - iVar4) {
      uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar8 + (uint32_t)(2) * sizeof(int8_t)));
      goto LAB_0023afe0;
    }
    break;
  case 0x1b:
    goto switchD_0023ad48_caseD_1b;
  case 0x1c:
    if ((long)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x23))) == (uint32_t)(*(int8_t*)rtguest::G((uint32_t)param_3))) {
      uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar8 + (uint32_t)(2) * sizeof(int8_t)));
      goto LAB_0023afe0;
    }
    break;
  case 0x1d:
    lVar7 = TestObjectAttribute(0x177f760,(*(int8_t*)rtguest::G((uint32_t)param_3)),(*(int8_t*)rtguest::G((uint32_t)pbVar8 + (uint32_t)(2) * sizeof(int8_t))));
LAB_0023afd4:
    if (lVar7 == 0) {
      param_3 = pbVar8 + 4;
      goto LAB_0023b13c;
    }
    uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar8 + (uint32_t)(3) * sizeof(int8_t)));
    goto LAB_0023afe0;
  }
  param_3 = pbVar8 + 3;
  goto LAB_0023b13c;
switchD_0023ad48_caseD_1b:
  if ((*(int8_t*)rtguest::G(0x177fdb1)) != '\x03') {
    param_3 = pbVar8 + 2;
    goto LAB_0023b13c;
  }
  lVar7 = TestStateBitFc78(100);
  if (lVar7 != 0) {
    uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)param_3));
    goto LAB_0023afe0;
  }
  lVar7 = TestFlagBitFc98(0x57);
  if (lVar7 != 0) {
    param_3 = pbVar8 + 2;
    goto LAB_0023b13c;
  }
  uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)param_3));
  goto LAB_0023afe0;
switchD_0023ad48_caseD_17:
  pbVar8 = pbVar8 + 2;
  SetProgressFlagAndQueue((*(int8_t*)rtguest::G((uint32_t)param_3)));
  bVar3 = (*(int8_t*)rtguest::G((uint32_t)pbVar8));
  goto LAB_0023b140;
switchD_0023ad48_caseD_1:
  lVar7 = TestFlagBitFc98((*(int8_t*)rtguest::G((uint32_t)param_3)));
  if (lVar7 == 0) {
    param_3 = pbVar8 + 3;
  }
  else {
    uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar8 + (uint32_t)(2) * sizeof(int8_t)));
LAB_0023afe0:
    param_3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(uVar5 * 4 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))))));
  }
  goto LAB_0023b13c;
}


RT_GUEST_FN(0x0023ad00, guest::InterpretScriptCommands)

}  // namespace guest
