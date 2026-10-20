// guest:0x0020a150 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateMenuOptionTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  char cVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = (int)param_1;
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8)));
  if (iVar2 == 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) = 8;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x400000;
    (*(uint32_t*)rtguest::G(0x177ea50)) = 0x177f760;
    gs_tasks_spawn(param_1,0x20f950,0x18);
    gs_music_play_bgm(0xa180201);
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8)));
  }
  else if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) == '\0') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) == '\0') {
      PlayBgmSlot1();
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8)));
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) + '\x01';
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x400000;
      uiFade(0x8000000);
      uiDrawTextCell(0xc,10,0x2ea398,0);
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8)));
    }
  }
  if (iVar2 == 0xe10) {
    PlayBgmSlot5();
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24)));
  }
  else if (iVar2 < 0xe11) {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24)));
  }
  else {
    uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 2))) << 0x19);
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 2))) + 1;
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24)));
  }
  if (cVar1 == '\0') {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) != 0xe50) {
      return 0;
    }
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
  }
  else {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 1;
  gs_tasks_kill(param_1);
  return 0;
}


RT_GUEST_FN(0x0020a150, guest::UpdateMenuOptionTask)

}  // namespace guest
