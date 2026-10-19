// guest:0x0026c460 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RequestLocationAssetLoad(uint32_t param_1, uint32_t param_2, ...) {
  char cVar1;
uint32_t puVar2;
  undefined8 uVar3;
  long lVar4;
  int iVar5;
uint32_t puVar6;
uint32_t ppuVar7;
  int iVar8;
uint32_t auStack_80 = rtguest::allocFrame(96);
  
  puVar6 = auStack_80;
  iVar5 = 0x15;
  ppuVar7 = 0x2be438;
  do {
    puVar2 = (*(uint32_t*)rtguest::G((uint32_t)ppuVar7));
    ppuVar7 = ppuVar7 + 2;
    iVar5 = iVar5 + -1;
    (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = puVar2;
    puVar6 = puVar6 + 1;
  } while (-1 < iVar5);
  iVar8 = (int)param_1;
  iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) == 0) {
    uVar3 = uiMenuCreate(param_1);
    LayoutTextItems(uVar3,auStack_80,0x16,0xb);
  }
  else {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) == '\0') {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25)));
    }
    else {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x50))) = 0x8033cc66;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25)));
    }
    if ((cVar1 == '\0') && (lVar4 = uiMenuWindow(iVar5,param_2,auStack_80), lVar4 != 0)) {
      if (lVar4 == -1) {
        iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 0xffffffff;
        gs_tasks_kill(param_1);
      }
      else {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x23))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xd)));
        gs_tasks_spawn(param_1,0x26be50,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c))));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 1;
      }
    }
  }
  return 0;
}


RT_GUEST_FN(0x0026c460, guest::RequestLocationAssetLoad)

}  // namespace guest
