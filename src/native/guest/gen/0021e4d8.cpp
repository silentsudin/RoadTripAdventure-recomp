// guest:0x0021e4d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitCameraTaskState(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  ushort uVar1;
  bool bVar2;
uint32_t pcVar3;
uint32_t puVar4;
  int iVar5;
  int iVar6;
  
  iVar5 = (int)param_1;
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x40) == 0) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) == 0) {
      gs_tasks_spawn(param_1,0x238b18,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) & 1);
      return 0;
    }
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) != 0x40) {
      return 0;
    }
    iVar6 = 0;
    puVar4 = 0x0177ade8;
    uVar1 = (*(uint16_t*)rtguest::G(0x177ade8));
    while (puVar4 = puVar4 + 0x270, (uVar1 & 0x27e) != ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) & 1) * 0x40 + 0x10)
    {
      iVar6 = iVar6 + 1;
      if (0x17 < iVar6) {
        bVar2 = false;
        goto LAB_0021e5d8;
      }
      uVar1 = (*(uint16_t*)rtguest::G((uint32_t)puVar4));
    }
    RT_CALLRA(0x21e508, guest::InitializeTask((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x10))),0x238b18,iVar6));
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x27))) = (char)iVar6;
    bVar2 = true;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 1;
LAB_0021e5d8:
    if (bVar2) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) = 0;
      pcVar3 = 0x0021ef98;
    }
    else {
      iVar6 = 0;
      puVar4 = 0x0177ade8;
      uVar1 = (*(uint16_t*)rtguest::G(0x177ade8));
      while (puVar4 = puVar4 + 0x270, (uVar1 & 0x27e) != ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) & 1) * 0x40 + 0x20
            ) {
        iVar6 = iVar6 + 1;
        if (0x17 < iVar6) {
          bVar2 = false;
          goto LAB_0021e668;
        }
        uVar1 = (*(uint16_t*)rtguest::G((uint32_t)puVar4));
      }
      RT_CALLRA(0x21e564, guest::InitializeTask((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x10))),0x238b18,iVar6));
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x27))) = (char)iVar6;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 2;
      bVar2 = true;
LAB_0021e668:
      if (!bVar2) {
        return 0;
      }
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) = 0;
      pcVar3 = 0x0021ee98;
    }
  }
  else {
    RT_CALLRA(0x21e5fc, guest::InitializeTask((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x10))),0x238b18,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) & 1));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x27))) = 0;
    pcVar3 = 0x0021e328;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xc))) = pcVar3;
  return 0;
}


RT_GUEST_FN(0x0021e4d8, guest::InitCameraTaskState)

}  // namespace guest
