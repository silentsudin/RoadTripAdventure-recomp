// guest:0x002642b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t SeqLongFadeTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  short sVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
uint32_t __s;
  int iVar5;
uint32_t psVar6;
  int iVar7;
  undefined4 uVar8;
  
  uVar4 = (*(uint32_t*)rtguest::G(0x3331ec));
  iVar7 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) == 0) {
    __s = (uint32_t)RT_CALLRA(0x2642e4, guest::getExtraPacketArea(0x8cc0));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) = __s;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x8cc0));
    RT_CALLRA(0x264304, guest::InitMode1C((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x1c)))));
    RT_CALLRA(0x264310, guest::gs_music_play_bgm(0xa100201));
    RT_CALLRA(0x264324, guest::gs_tasks_spawn(param_1,0x2357c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28)))));
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28)));
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x8cb6))) = 0;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x8cb4))) = 0;
    (*(uint8_t*)rtguest::G(0x177b05a)) = 1;
  }
  else {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28)));
    psVar6 = (uint32_t)(iVar2 + 0x8cb0);
    iVar5 = iVar2;
    switch((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4)))) {
    case 0:
      uVar8 = (*(uint32_t*)rtguest::G(0x3331e8));
      RT_CALLRA(0x26439c, guest::StoreIndexedPosition(0x44060000,(*(uint32_t*)rtguest::G(0x3331e8)),(*(uint32_t*)rtguest::G(0x3331ec)),0,0));
      RT_CALLRA(0x2643b4, guest::StoreIndexedPosition((*(uint32_t*)rtguest::G(0x3331f0)),uVar8,uVar4,1,0));
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4))) = 1;
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28)));
      break;
    case 2:
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4))) = 3;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = uVar3 | 8;
      break;
    case 3:
      sVar1 = (*(int16_t*)rtguest::G((uint32_t)psVar6));
      if (0x40 < (*(int16_t*)rtguest::G((uint32_t)psVar6))) {
        RT_CALLRA(0x2643fc, guest::uiFade(((*(int16_t*)rtguest::G((uint32_t)psVar6)) + -0x40) * 0x1000000));
        iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28)));
        sVar1 = (*(int16_t*)rtguest::G((uint32_t)psVar6));
      }
      if (sVar1 < 0xc0) {
        (*(int16_t*)rtguest::G((uint32_t)psVar6)) = sVar1 + 2;
      }
      else {
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4))) = 4;
      }
    }
    RT_CALLRA(0x264434, guest::raceHudDraw(iVar5));
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb8))) != 0) {
      RT_CALLRA(0x264448, guest::uiFade(0xffffffff80000000));
      iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 8;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = 0xffffffff;
      RT_CALLRA(0x264470, guest::gs_tasks_kill(param_1));
    }
  }
  return 0;
}


RT_GUEST_FN(0x002642b8, guest::SeqLongFadeTask)

}  // namespace guest
