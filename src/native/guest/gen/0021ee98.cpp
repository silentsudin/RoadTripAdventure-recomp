// guest:0x0021ee98 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SwitchCameraPresetType10(uint32_t param_1, uint32_t param_2, ...) {
  ushort uVar1;
  bool bVar2;
uint32_t pcVar3;
uint32_t puVar4;
  int iVar5;
  
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x40) == 0) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) != 0xc0) {
      return 0;
    }
    iVar5 = 0;
    puVar4 = 0x0177ade8;
    uVar1 = (*(uint16_t*)rtguest::G(0x177ade8));
    while (puVar4 = puVar4 + 0x270, (uVar1 & 0x27e) != ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) & 1) * 0x40 + 0x10
          ) {
      iVar5 = iVar5 + 1;
      if (0x17 < iVar5) {
        bVar2 = false;
        goto LAB_0021ef70;
      }
      uVar1 = (*(uint16_t*)rtguest::G((uint32_t)puVar4));
    }
    InitializeTask((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))),0x238b18,iVar5);
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = (char)iVar5;
    bVar2 = true;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = 1;
LAB_0021ef70:
    if (!bVar2) {
      return 0;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = 0;
    pcVar3 = (*(uint8_t*)rtguest::G(0x21ef98));
  }
  else {
    InitializeTask((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))),0x238b18,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) & 1);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 0;
    pcVar3 = (*(uint8_t*)rtguest::G(0x21e328));
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = pcVar3;
  return 0;
}


RT_GUEST_FN(0x0021ee98, guest::SwitchCameraPresetType10)

}  // namespace guest
