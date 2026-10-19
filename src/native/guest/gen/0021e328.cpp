// guest:0x0021e328 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CycleCameraPresetState(uint32_t param_1, uint32_t param_2, ...) {
  uint uVar1;
  byte bVar2;
  ushort uVar3;
  bool bVar4;
uint32_t puVar5;
  uint uVar6;
  int iVar7;
  
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0x10) == 0) {
    if ((*(uint16_t*)rtguest::G((uint32_t)0x177ade8 + (uint32_t)(((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) * 0x28 - (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)))) * 8) * 2)) != 0) {
      return 0;
    }
    bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25)));
  }
  else {
    bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25)));
  }
  uVar1 = (bVar2 + 1) % 3;
  puVar5 = 0x0177ade8;
  iVar7 = 0;
  uVar6 = uVar1 * 0x10 + ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) & 1) * 0x40;
  uVar3 = (*(uint16_t*)rtguest::G(0x177ade8));
  if (uVar1 == 0) {
    uVar6 = uVar6 | 2;
  }
  do {
    puVar5 = puVar5 + 0x270;  // T* element step
    if ((uVar3 & 0x27e) == uVar6) {
      InitializeTask((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))),0x238b18,iVar7);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = (byte)iVar7;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = (byte)uVar1;
      bVar4 = true;
LAB_0021e440:
      if (!bVar4) {
        uVar1 = ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) + 2) % 3;
        puVar5 = 0x0177ade8;
        iVar7 = 0;
        uVar6 = uVar1 * 0x10 + ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) & 1) * 0x40;
        uVar3 = (*(uint16_t*)rtguest::G(0x177ade8));
        if (uVar1 == 0) {
          uVar6 = uVar6 | 2;
        }
        while (puVar5 = puVar5 + 0x270, (uVar3 & 0x27e) != uVar6) {
          iVar7 = iVar7 + 1;
          if (0x17 < iVar7) {
            return 0;
          }
          uVar3 = (*(uint16_t*)rtguest::G((uint32_t)puVar5));
        }
        InitializeTask((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))),0x238b18,iVar7);
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = (byte)iVar7;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = (byte)uVar1;
      }
      return 0;
    }
    iVar7 = iVar7 + 1;
    if (0x17 < iVar7) {
      bVar4 = false;
      goto LAB_0021e440;
    }
    uVar3 = (*(uint16_t*)rtguest::G((uint32_t)puVar5));
  } while( true );
}


RT_GUEST_FN(0x0021e328, guest::CycleCameraPresetState)

}  // namespace guest
