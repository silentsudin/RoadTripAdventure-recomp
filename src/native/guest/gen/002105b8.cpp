// guest:0x002105b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"
typedef uint32_t rt_u32u __attribute__((aligned(1)));  // unaligned guest access: defined behavior, LE result unchanged

namespace guest {

uint32_t UpdateTownExplorationTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  char cVar1;
  undefined1 uVar2;
  short sVar3;
  undefined4 uVar4;
  uint uVar5;
  int iVar6;
  long lVar7;
  undefined8 uVar8;
  int iVar9;
  ulong uVar10;
uint32_t piVar11;
uint32_t puVar12;
  int iVar13;
  
  iVar13 = (int)param_1;
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x25)));
  puVar12 = (uint32_t)param_2;
  if (cVar1 == '\x05') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x24))) == '\0') {
      return 0;
    }
    iVar13 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 4))) = 1;
    RT_CALLRA(0x2105fc, guest::gs_tasks_kill(param_1)); // arg-drop fix: jal 0x204de8 @0x2105fc, $a0=param_1 ($s2=$a0, no writes; capstone)
    return 0;
  }
  if (cVar1 == '\x03') {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) | 0x400000;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x24))) == '\0') {
      return 0;
    }
    RT_CALLRA(0x210638, guest::DestroyTaskRecord());
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x27))) = 0;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))) != '\0') {
      lVar7 = RT_CALLRA(0x210650, guest::MatchSlotPairState((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x21)))
                                ));
      if (lVar7 == 0) goto LAB_00210ff0;
      (*(int32_t*)rtguest::G(0x177ea5c)) = (int)0x177f760 + (int)lVar7 * 0x1c;
      goto LAB_00211038;
    }
    uVar10 = (uint32_t)(*(uint32_t*)rtguest::G(0x177fdb2));
    goto LAB_00211070;
  }
  if (cVar1 == '\x02') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x24))) == '\0') {
      return 0;
    }
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 4))) != -1) {
      uVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 4)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x21))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))) = uVar2;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x25))) = 3;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) & 0xffffefff | 0x400000;
      RT_CALLRA(0x210704, guest::ClearFlagBitFc98(0x50));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x14))) =
           (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x14))) + ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 8))) & 3) * 9000 + 9000;
      switchD_002108bc_updateTownExplorationTaskReturn();
      return 0;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x25))) = 0;
    RT_CALLRA(0x2106b0, guest::ClearTaskListFlag());
    RT_CALLRA(0x2106b8, guest::SoundFadeOutChannels());
    RT_CALLRA(0x2106c0, guest::ApplySoundVolumes((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x14)))));
    RT_CALLRA(0x2106cc, guest::UpdateSceneLightsAndFog(0,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x22)))));
    return 0;
  }
  if (cVar1 == '\x06') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0xf))) != '\0') {
      return 0;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x25))) = 0;
    RT_CALLRA(0x210744, guest::ClearTaskPoolFlag());
LAB_002107f4:
    RT_CALLRA(0x2107f4, guest::gs_tasks_spawn(param_1,0x208270,0));
    return 0;
  }
  if (cVar1 == '\n') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x24))) != '\0') {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) & 0xfffafbff;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x25))) = 0;
      RT_CALLRA(0x210788, guest::SoundFadeOutChannels());
      return 0;
    }
    return 0;
  }
  if (cVar1 == '\a') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x24))) == '\0') {
      return 0;
    }
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x27))) != '\x02') {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x25))) = 6;
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x27))) == '\0') {
        RT_CALLRA(0x21081c, guest::SpawnTownEntities((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x21))),
                          param_1,param_2));
      }
      RT_CALLRA(0x21082c, guest::StartTownModeTask(param_1,param_2,0));
      return 0;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x25))) = 0;
    RT_CALLRA(0x2107b8, guest::ClearTaskListFlag());
    RT_CALLRA(0x2107c0, guest::SoundFadeOutChannels());
    RT_CALLRA(0x2107c8, guest::ApplySoundVolumes((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x14)))));
    RT_CALLRA(0x2107d4, guest::UpdateSceneLightsAndFog(0,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x22)))));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) | 0x404;
    goto LAB_002107f4;
  }
  if (cVar1 == '\x04') {
    piVar11 = (uint32_t)((int)puVar12 + 0x14);
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)piVar11));
    (*(int32_t*)rtguest::G((uint32_t)piVar11)) = iVar6 + 1;
    if (0x34bc0 < iVar6) {
      (*(int32_t*)rtguest::G((uint32_t)piVar11)) = iVar6 + -0x34bbf;
    }
    if (((*(rt_u32u*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) & 0x1000) != 0) {
      (*(int32_t*)rtguest::G(0x335730)) = (*(int32_t*)rtguest::G(0x335730)) + 1;
    }
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x24))) == '\0') {
      return 0;
    }
    switch((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 4)))) {
    case 4:
      RT_CALLRA(0x210970, guest::DestroyTaskRecord(param_1));
      RT_CALLRA(0x210904, guest::gs_tasks_spawn(param_1,0x20f950,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 4)))));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x25))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x20))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x14)));
      switchD_002108bc_updateTownExplorationTaskReturn();
      return 0;
    case 5:
      RT_CALLRA(0x2108f0, guest::AwardHouseStampIfUnset((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23)))));
      RT_CALLRA(0x2109f0, guest::gs_tasks_spawn(param_1,0x26d008,0));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x25))) = 7;
      lVar7 = RT_CALLRA(0x210914, guest::TestFlagBitFc98(0x50));
      if (lVar7 != 0) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) | 0x1000;
        return 0;
      }
      return 0;
    case 6:
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x25))) = 5;
      switchD_002108bc_updateTownExplorationTaskReturn();
      return 0;
    case 7:
    case 8:
    case 9:
    case 0xd:
    case 0xe:
      goto code_r0x00211154;
    case 10:
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x27))) == '\0') {
        RT_CALLRA(0x2108e0, guest::SpawnTownEntities((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x21))),
                          param_1,param_2));
        return 0;
      }
      return 0;
    case 0xb:
      RT_CALLRA(0x210a60, guest::DestroyTaskRecord(param_1));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x27))) = 0;
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))) == '\0') {
        RT_CALLRA(0x210a04, guest::gs_tasks_spawn(param_1,0x20dc50,(uint32_t)(*(uint32_t*)rtguest::G(0x177fdb2)) | 0x30000));
        RT_CALLRA(0x210a2c, guest::gs_tasks_spawn(param_1,0x22d1e0,0));
        uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5)));
      }
      else {
        lVar7 = RT_CALLRA(0x210988, guest::MatchSlotPairState((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))),
                                   (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x21)))));
        if (lVar7 == 0) {
          (*(int32_t*)rtguest::G(0x177ea5c)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))) * 4))) +
                         (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x21))) * 0x10;
        }
        else {
          (*(int32_t*)rtguest::G(0x177ea5c)) = (int)0x177f760 + (int)lVar7 * 0x1c;
        }
        RT_CALLRA(0x210a40, guest::gs_tasks_spawn(param_1,0x20dc50,0x30000));
        RT_CALLRA(0x210a74, guest::gs_tasks_spawn(param_1,0x22c518,0));
        uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5)));
      }
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) = uVar5 | 0x400000;
      switchD_002108bc_updateTownExplorationTaskReturn();
      return 0;
    case 0xc:
      RT_CALLRA(0x210938, guest::UiDestroyRemovedNodes(param_1));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x27))) = 0;
      RT_CALLRA(0x210950, guest::SpawnTownEntities((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x21))),
                        param_1,param_2));
      return 0;
    case 0xf:
      RT_CALLRA(0x210960, guest::UiDestroyRemovedNodes(param_1));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x27))) = 0;
      switchD_002108bc_updateTownExplorationTaskReturn();
      return 0;
    default:
      return 0;
    }
  }
  if (cVar1 == '\t') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x24))) == '\0') {
      return 0;
    }
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 2))) = (short)((*(uint32_t*)rtguest::G(0x177fdb8)) / 6000);
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))) == '\0') {
      RT_CALLRA(0x210b48, guest::gs_tasks_spawn(param_1,0x20dc50,(uint32_t)(*(uint32_t*)rtguest::G(0x177fdb2)) | 0x30000));
      RT_CALLRA(0x210b5c, guest::gs_tasks_spawn(param_1,0x22d1e0,0));
    }
    else {
      lVar7 = RT_CALLRA(0x210ae4, guest::MatchSlotPairState((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x21)))
                                ));
      if (lVar7 == 0) {
        (*(int32_t*)rtguest::G(0x177ea5c)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))) * 4))) +
                       (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x21))) * 0x10;
      }
      else {
        (*(int32_t*)rtguest::G(0x177ea5c)) = (int)0x177f760 + (int)lVar7 * 0x1c;
      }
      RT_CALLRA(0x210b7c, guest::gs_tasks_spawn(param_1,0x20dc50,0x30000));
      RT_CALLRA(0x210b90, guest::gs_tasks_spawn(param_1,0x22c518,0));
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x27))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x25))) = 4;
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a88ec + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x22))) * 8))) == 0) {
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x14))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x20))) + 9000;
      switchD_002108bc_updateTownExplorationTaskReturn();
      return 0;
    }
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x14))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a88ec + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x22))) * 8)));
    switchD_002108bc_updateTownExplorationTaskReturn();
    return 0;
  }
  if (cVar1 == '\b') {
    piVar11 = (uint32_t)((int)puVar12 + 0x14);
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)piVar11));
    (*(int32_t*)rtguest::G((uint32_t)piVar11)) = iVar6 + 1;
    if (0x34bc0 < iVar6) {
      (*(int32_t*)rtguest::G((uint32_t)piVar11)) = iVar6 + -0x34bbf;
    }
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) & 0x1000) != 0) {
      (*(int32_t*)rtguest::G(0x335730)) = (*(int32_t*)rtguest::G(0x335730)) + 1;
    }
    RT_CALLRA(0x210c30, guest::updateTimeOfDayLight((*(uint32_t*)rtguest::G(0x177ace4)),param_2,(*(int32_t*)rtguest::G((uint32_t)piVar11))));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x24))) == '\0') {
      return 0;
    }
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 4))) != 0xb) {
      RT_CALLRA(0x210cc8, guest::gs_tasks_spawn(param_1,0x26d008,1));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x25))) = 10;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) & 0xfffffbff;
      switchD_002108bc_updateTownExplorationTaskReturn();
      return 0;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) = 0x400000;
    RT_CALLRA(0x210c54, guest::AccumulateDistanceSingle());
    RT_CALLRA(0x210c5c, guest::SoundStopAllChannels());
    RT_CALLRA(0x210c64, guest::SoundRpcCmd35());
    RT_CALLRA(0x210c6c, guest::SetTaskListFlag(param_1));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))) == '\0') {
      uVar10 = (uint32_t)(*(uint32_t*)rtguest::G(0x177fdb2));
      goto LAB_00211070;
    }
    lVar7 = RT_CALLRA(0x210c80, guest::MatchSlotPairState((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x21)))));
    if (lVar7 == 0) goto LAB_00210ff0;
    (*(int32_t*)rtguest::G(0x177ea5c)) = (int)0x177f760 + (int)lVar7 * 0x1c;
  }
  else {
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) & 0x8000) != 0) {
      return 0;
    }
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0xb))) != '\0') {
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x2c))) & 1) != 0) {
        iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x14)));
        iVar9 = iVar6 + 0x168;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x14))) = iVar9;
        if (0x34bc0 < iVar9) {
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x14))) = iVar6 + -0x34a58;
        }
      }
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x2c))) & 4) != 0) {
        iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x14)));
        iVar9 = iVar6 + -0x168;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x14))) = iVar9;
        if (iVar9 < 0) {
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x14))) = iVar6 + 0x34a58;
        }
      }
    }
    piVar11 = (uint32_t)((int)puVar12 + 0x14);
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)piVar11));
    (*(int32_t*)rtguest::G((uint32_t)piVar11)) = iVar6 + 1;
    if (0x34bc0 < iVar6) {
      (*(int32_t*)rtguest::G((uint32_t)piVar11)) = iVar6 + -0x34bbf;
    }
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) & 0x1000) != 0) {
      (*(int32_t*)rtguest::G(0x335730)) = (*(int32_t*)rtguest::G(0x335730)) + 1;
    }
    RT_CALLRA(0x210db4, guest::updateTimeOfDayLight((*(uint32_t*)rtguest::G(0x177ace4)),param_2,(*(int32_t*)rtguest::G((uint32_t)piVar11))));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x22))) == -1) {
      uVar4 = RT_CALLRA(0x210dd4, guest::DecomposeWorldPosition(puVar12 + 0xb,(int)puVar12 + 0x5c,0x177acf0));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x54))) = uVar4;
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5)));
    }
    else {
      if ((*(uint32_t*)rtguest::G(0x177ace4)) < 200.0) {
        RT_CALLRA(0x210e04, guest::AccumulateDistanceSingle());
        RT_CALLRA(0x210e0c, guest::SoundStopAllChannels());
        RT_CALLRA(0x210e14, guest::DestroyTaskRecord(param_1));
        RT_CALLRA(0x210e1c, guest::SoundRpcCmd35());
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))) = 7;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) | 0x400000;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x21))) = 0x11;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x27))) = 0;
        if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))) != '\0') {
          lVar7 = RT_CALLRA(0x210e54, guest::MatchSlotPairState((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))),
                                     (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x21)))));
          if (lVar7 == 0) goto LAB_00210ff0;
          (*(int32_t*)rtguest::G(0x177ea5c)) = (int)0x177f760 + (int)lVar7 * 0x1c;
          goto LAB_00211038;
        }
        uVar10 = (uint32_t)(*(uint32_t*)rtguest::G(0x177fdb2));
        goto LAB_00211070;
      }
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5)));
    }
    if ((uVar5 & 0x400) != 0) {
      return 0;
    }
    iVar6 = (*(int32_t*)rtguest::G(0x177ae54));
    if ((*(int32_t*)rtguest::G(0x177ae54)) < 0) {
      iVar6 = (*(int32_t*)rtguest::G(0x177ae54)) + 0x7fff;
    }
    if ((long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 2))) < (long)(((*(uint32_t*)rtguest::G(0x177fdb8)) + (iVar6 >> 0xf)) / 6000)) {
      lVar7 = RT_CALLRA(0x210ef4, guest::TestStateBitFc78(0x28));
      if (lVar7 == 0) {
        sVar3 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 2)));
      }
      else {
        uVar8 = RT_CALLRA(0x210f10, guest::SubmitMessage2B((int)(*(uint64_t*)rtguest::G((uint32_t)puVar12))));
        RT_CALLRA(0x210f1c, guest::PlayUiSound(0x31e));
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ea790u)), uVar8);
        sVar3 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 2)));
      }
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 2))) = sVar3 + 1;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x24)));
    }
    else {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x24)));
    }
    if (cVar1 == '\0') {
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 6))) & 0x800) != 0) {
        RT_CALLRA(0x210f78, guest::AccumulateDistanceSingle());
        RT_CALLRA(0x210f80, guest::SoundStopAllChannels());
        RT_CALLRA(0x211120, guest::PlayUiSound(0x1f));
        RT_CALLRA(0x210f88, guest::SoundRpcCmd35());
        RT_CALLRA(0x210fc8, guest::SetTaskListFlag(param_1));
        RT_CALLRA(0x211044, guest::gs_tasks_spawn(param_1,0x217148,0));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x25))) = 2;
code_r0x00211154:
        return 0;
      }
      return 0;
    }
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 4))) != 1) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x25))) = 8;
      RT_CALLRA(0x211058, guest::gs_tasks_spawn(param_1,0x23b168,
                     (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x21))) * 4 +
                      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a7ed0 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))) * 4))))))));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) | 0x50000;
      RT_CALLRA(0x2110ec, guest::SoundStopAllChannels());
      return 0;
    }
    if ((((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) & 0x1000) != 0) && (lVar7 = IsSlotActive(param_2), lVar7 == 0)) {
      return 0;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar12 + 5))) = 0x400000;
    RT_CALLRA(0x211110, guest::AccumulateDistanceSingle());
    RT_CALLRA(0x211118, guest::SoundStopAllChannels());
    RT_CALLRA(0x211128, guest::SoundRpcCmd35());
    if (((*(uint64_t*)rtguest::G((uint32_t)puVar12 + (uint32_t)(4) * sizeof(uint64_t))) & 0xff00ff00) == 0x5000000) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x27))) = 0;
      RT_CALLRA(0x210fb4, guest::DestroyTaskRecord(param_1));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23)));
    }
    else {
      RT_CALLRA(0x211130, guest::SetTaskListFlag(param_1));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23)));
    }
    if (cVar1 == '\0') {
      uVar10 = (uint32_t)(*(uint32_t*)rtguest::G(0x177fdb2));
LAB_00211070:
      RT_CALLRA(0x21107c, guest::gs_tasks_spawn(param_1,0x20dc50,uVar10 | 0x30000));
      RT_CALLRA(0x211090, guest::gs_tasks_spawn(param_1,0x22d1e0,0));
      goto LAB_0021109c;
    }
    lVar7 = RT_CALLRA(0x210fdc, guest::MatchSlotPairState(cVar1,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x21)))));
    if (lVar7 == 0) {
LAB_00210ff0:
      (*(int32_t*)rtguest::G(0x177ea5c)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x23))) * 4))) +
                     (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar12 + 0x21))) * 0x10;
    }
    else {
      (*(int32_t*)rtguest::G(0x177ea5c)) = (int)0x177f760 + (int)lVar7 * 0x1c;
    }
  }
LAB_00211038:
  RT_CALLRA(0x2110d8, guest::gs_tasks_spawn(param_1,0x20dc50,0x30000));
  RT_CALLRA(0x211144, guest::gs_tasks_spawn(param_1,0x22c518,0));
LAB_0021109c:
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x25))) = 4;
  switchD_002108bc_updateTownExplorationTaskReturn();
  return 0;
}


RT_GUEST_FN(0x002105b8, guest::UpdateTownExplorationTask)

}  // namespace guest
