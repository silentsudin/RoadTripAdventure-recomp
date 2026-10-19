// guest:0x002086b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitializeMenuPanelTask(uint32_t param_1, uint32_t param_2, ...) {
  short sVar1;
  char cVar2;
uint32_t pcVar3;
  uint64_t uVar4;
  int iVar5;
  
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x50))) = 0x80000000;
  iVar5 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) == 0) {
    uVar4 = 0x320000;
    pcVar3 = 0x20dc50;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x400000;
  }
  else {
    cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25)));
    if ((cVar2 != '\0') || ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) != '\0')) goto LAB_00208738;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 1;
    pcVar3 = 0x208270;
    uVar4 = 1;
  }
  gs_tasks_spawn(param_1,pcVar3,uVar4);
  cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25)));
LAB_00208738:
  if (cVar2 == '\x01') {
    sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) + 1;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) = sVar1;
    if (sVar1 == 0xd4) {
      gs_tasks_spawn(param_1,0x2082d8,1);
    }
    sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2)));
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) < 0xf4) {
      gifBeginSpriteBatch(0x34a6c0,0x2003f9ce65319d8e);
      gifSetSpriteCount(0x34a720,1);
      uiSpriteWrite(0x34a730,0x80,0x20,0,0,0x180,0x180);
      vifDirect(0x34a6c0,0xc);
      sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2)));
    }
  }
  else {
    sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2)));
  }
  if (sVar1 == 0xf4) {
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x400000;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 0;
    gs_tasks_kill(param_1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x002086b0, guest::InitializeMenuPanelTask)

}  // namespace guest
