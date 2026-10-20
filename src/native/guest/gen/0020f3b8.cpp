// guest:0x0020f3b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateRaceGameTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  int iVar1;
  uint uVar2;
  char cVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
uint32_t piVar7;
  undefined8 uVar8;
  bool bVar9;
  int iVar10;
  
  iVar10 = (int)param_1;
  cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x25)));
  iVar6 = (int)param_2;
  if (cVar3 == '\x04') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) == '\0') {
      return 0;
    }
    RT_CALLRA(0x20f3ec, guest::PlayBgmSlot3());
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 10))) == '\x01') {
      RT_CALLRA(0x20f404, guest::UpdateRaceRecords(param_2));
    }
    RT_CALLRA(0x20f40c, guest::DestroyTaskRecord(param_1));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x20))) < 0x1c) {
      RT_CALLRA(0x20f430, guest::gs_tasks_spawn(param_1,0x237508,0));
    }
    else {
      RT_CALLRA(0x20f448, guest::gs_tasks_spawn(param_1,0x237388,0));
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x25))) = 3;
    return 0;
  }
  if (cVar3 == '\x03') {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x50))) = 0x80000000;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) == '\0') {
      return 0;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) | 0x400000;
    RT_CALLRA(0x20f48c, guest::GetOrSetPresidentRaceResult((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 4)))));
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x18)));
    goto LAB_0020f77c;
  }
  if (cVar3 == '\x02') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) == '\0') {
      return 0;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x25))) = 0;
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 4))) == -1) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) & 0xffff7fff;
      RT_CALLRA(0x20f4d0, guest::SoundFadeOutChannels());
      RT_CALLRA(0x20f4d8, guest::PlayBgmSlot4());
      return 0;
    }
    RT_CALLRA(0x20f4e8, guest::GetOrSetPresidentRaceResult(0));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 10))) == '\0') {
      RT_CALLRA(0x20f4fc, guest::AccumulateDistanceTotals());
      cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x20)));
    }
    else {
      cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x20)));
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = 0;
    if ((uint32_t)(cVar3 - 0x1cU) < 7) {
      RT_CALLRA(0x20f51c, guest::ResetRaceRanks());
      RT_CALLRA(0x20f524, guest::DestroyTaskRecord(param_1));
      RT_CALLRA(0x20f538, guest::gs_tasks_spawn(param_1,0x237388,0));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x25))) = 3;
      return 0;
    }
LAB_0020f778:
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x18)));
LAB_0020f77c:
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 0;
    RT_CALLRA(0x20f788, guest::gs_tasks_kill(param_1));
    return 0;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xf))) != '\0') {
    return 0;
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) & 0x10) != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) & 0xffffffef | 0x20;
    RT_CALLRA(0x20f56c, guest::SaveControlStateForReplay(param_2));
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x22))) == '\x11') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xb))) != '\0') {
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x2c))) & 1) != 0) {
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x14)));
        iVar5 = iVar1 + 0x168;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x14))) = iVar5;
        if (0x34bc0 < iVar5) {
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x14))) = iVar1 + -0x34a58;
        }
      }
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x2c))) & 4) != 0) {
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x14)));
        iVar5 = iVar1 + -0x168;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x14))) = iVar5;
        if (iVar5 < 0) {
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x14))) = iVar1 + 0x34a58;
        }
      }
    }
    piVar7 = (uint32_t)(iVar6 + 0x14);
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
    (*(int32_t*)rtguest::G((uint32_t)piVar7)) = iVar1 + 1;
    if (0x34bc0 < iVar1) {
      (*(int32_t*)rtguest::G((uint32_t)piVar7)) = iVar1 + -0x34bbf;
    }
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) & 0x1000) != 0) {
      (*(int32_t*)rtguest::G(0x335730)) = (*(int32_t*)rtguest::G(0x335730)) + 1;
    }
    RT_CALLRA(0x20f640, guest::updateTimeOfDayLight((*(uint32_t*)rtguest::G(0x177ace4)),param_2,(*(int32_t*)rtguest::G((uint32_t)piVar7))));
    uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
  }
  else {
    RT_CALLRA(0x20f654, guest::updateTimeOfDayLight(0,param_2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x14)))));
    RT_CALLRA(0x20f660, guest::setFixedLight(param_2,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x22)))));
    uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
  }
  if ((uVar2 & 8) == 0) {
    if ((uVar2 & 0x100) == 0) {
      cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24)));
    }
    else {
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x30))) & 0x800) != 0) {
LAB_0020f820:
        if ((uVar2 & 0x8000) != 0) {
          return 0;
        }
        RT_CALLRA(0x20f698, guest::SoundStopAllChannels());
        RT_CALLRA(0x20f830, guest::PlayUiSound(0x1f));
        RT_CALLRA(0x20f6a0, guest::PlayBgmSlot3());
        bVar9 = false;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) | 0x8000;
        if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 10))) & 2) != 0) {
          bVar9 = ((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x32))) & 0x800) != 0;
        }
        uVar4 = RT_CALLRA(0x20f870, guest::gs_tasks_spawn(param_1,0x26bc08,bVar9));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x25))) = 2;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = uVar4;
        return 0;
      }
      if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 10))) & 2) == 0) {
        cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24)));
      }
      else {
        if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x32))) & 0x800) != 0) goto LAB_0020f820;
        cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24)));
      }
    }
    if ((cVar3 == '\0') && ((uVar2 & 0x200) == 0)) {
      return 0;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x50))) = 0x80000000;
    RT_CALLRA(0x20f768, guest::uiFade(0xffffffff80000000));
    RT_CALLRA(0x20f6b8, guest::SoundStopAllChannels());
    RT_CALLRA(0x20f838, guest::PlayBgmSlot3());
    RT_CALLRA(0x20f8bc, guest::GetOrSetPresidentRaceResult(0));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 10))) == '\0') {
      RT_CALLRA(0x20f690, guest::AccumulateDistanceTotals());
      cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x20)));
    }
    else {
      cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x20)));
    }
    if (6 < (uint32_t)(cVar3 - 0x1cU)) {
      iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x18)));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 4))) = 0;
      RT_CALLRA(0x20f930, guest::gs_tasks_kill(param_1));
      return 0;
    }
    RT_CALLRA(0x20f8f0, guest::ResetRaceRanks());
  }
  else {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) == '\0') {
      return 0;
    }
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 10))) == '\0') {
      RT_CALLRA(0x20f8d0, guest::AccumulateDistanceTotals());
    }
    RT_CALLRA(0x20f828, guest::SoundStopAllChannels());
    RT_CALLRA(0x20f8b4, guest::PlayBgmSlot3());
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = 0;
    if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 10))) & 2) != 0) {
      RT_CALLRA(0x20f8ac, guest::SoundStopAllChannels());
      RT_CALLRA(0x20f6c0, guest::DestroyTaskRecord(param_1));
      uVar8 = 0x235b58;
      goto LAB_0020f908;
    }
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x20))) < 0x1c) {
      if ((((*(uint8_t*)rtguest::G(0x178700a)) < 6) && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x22))) != '\x0f')) &&
         ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x22))) != '\x10')) {
LAB_0020f7ac:
        RT_CALLRA(0x20f7ac, guest::gs_music_play_bgm(0xa180201));
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = 0x10044;
        RT_CALLRA(0x20f7c0, guest::RestoreControlStateFromReplay(param_2));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x25))) = 4;
        RT_CALLRA(0x20f7d4, guest::sceGsSyncPath(0,0));
        return 0;
      }
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 10))) == 1) {
        RT_CALLRA(0x20f724, guest::UpdateRaceRecords(param_2));
        RT_CALLRA(0x20f72c, guest::DestroyTaskRecord(param_1));
        uVar8 = 0x237508;
      }
      else {
        RT_CALLRA(0x20f740, guest::DestroyTaskRecord(param_1));
        uVar8 = 0x237508;
      }
      goto LAB_0020f908;
    }
    if (0x22 < (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x20)))) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = 0x400000;
      RT_CALLRA(0x20f8a4, guest::uiFade(0xffffffff80000000));
      RT_CALLRA(0x20f770, guest::AwardRaceResult(param_2));
      goto LAB_0020f778;
    }
    if ((*(uint8_t*)rtguest::G(0x178700a)) < 6) goto LAB_0020f7ac;
  }
  RT_CALLRA(0x20f8f8, guest::DestroyTaskRecord(param_1));
  uVar8 = 0x237388;
LAB_0020f908:
  RT_CALLRA(0x20f90c, guest::gs_tasks_spawn(param_1,uVar8,0));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x25))) = 3;
  return 0;
}


RT_GUEST_FN(0x0020f3b8, guest::UpdateRaceGameTask)

}  // namespace guest
