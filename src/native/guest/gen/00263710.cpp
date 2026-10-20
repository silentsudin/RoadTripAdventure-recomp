// guest:0x00263710 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t SeqIntroFadeTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  int iVar1;
uint32_t pvVar2;
  undefined8 uVar3;
  int iVar4;
  int iVar5;
uint32_t puVar6;
  
  iVar5 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) == 0) {
    uVar3 = RT_CALLRA(0x263734, guest::getExtraPacketArea(0x8cbc));
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))) = (int)uVar3;
    RT_CALLRA(0x263740, guest::InitObjectType22(uVar3));
    RT_CALLRA(0x26374c, guest::gs_music_play_bgm(0xa100201));
    RT_CALLRA(0x263760, guest::gs_tasks_spawn(param_1,0x2357c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)))));
    puVar6 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))) + 0x8cb0);
    pvVar2 = (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(puVar6)), 0, 0xc));
    (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = 0;
  }
  else {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
    puVar6 = (uint32_t)(iVar1 + 0x8cb0);
    iVar4 = iVar1;
    switch((*(uint32_t*)rtguest::G((uint32_t)puVar6))) {
    case 0:
      RT_CALLRA(0x2637dc, guest::StoreIndexedPosition((*(uint32_t*)rtguest::G(0x3331b4)),0x43960000,(*(uint32_t*)rtguest::G(0x3331b8)),0,0));
      (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = 1;
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
      break;
    case 2:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 8;
      (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = 3;
      break;
    case 3:
      RT_CALLRA(0x263814, guest::uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) << 0x18));
      if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) < 0x80) {
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) + 1;
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
      }
      else {
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
        (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = 0xffffffff;
      }
      break;
    case 0xffffffff:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 1;
    }
    RT_CALLRA(0x263858, guest::raceHudDraw(iVar4));
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) == 0) {
      pvVar2 = (uint32_t)0x0;
    }
    else {
      RT_CALLRA(0x26386c, guest::SoundStopAllChannels());
      RT_CALLRA(0x263874, guest::uiFade(0xffffffff80000000));
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 8;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 0xffffffff;
      pvVar2 = (uint32_t)RT_CALLRA(0x26389c, guest::gs_tasks_kill(param_1));
    }
  }
  return pvVar2;
}


RT_GUEST_FN(0x00263710, guest::SeqIntroFadeTask)

}  // namespace guest
