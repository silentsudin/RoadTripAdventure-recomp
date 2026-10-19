// guest:0x00237388 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DispatchRecordsScreen(uint32_t param_1, uint32_t param_2, ...) {
  byte bVar1;
  byte bVar2;
  undefined8 uVar3;
  int iVar4;
uint32_t pbVar5;
  int iVar6;
  
  iVar6 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) = (uint32_t)(*(uint8_t*)rtguest::G(0x1782a70));
    uVar3 = getExtraPacketArea(0x1f0);
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = (int)uVar3;
    InitScreenLayout(uVar3);
    iVar4 = 0;
    pbVar5 = 0x0178700a;
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26)));
    while( true ) {
      bVar2 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
      pbVar5 = pbVar5 + 4;
      (*(uint32_t*)rtguest::G((uint32_t)0x1782a74 + (uint32_t)(iVar4 + (uint32_t)bVar1 * 0x18) * 4)) = (char)(0x1e - (uint32_t)bVar2);
      if ((int)((0x1e - (uint32_t)bVar2) * 0x1000000) >> 0x18 == 0x1f) {
        (*(uint32_t*)rtguest::G((uint32_t)0x1782a74 + (uint32_t)(iVar4 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) * 0x18) * 4)) = 0;
      }
      iVar4 = iVar4 + 1;
      if (0x17 < iVar4) break;
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26)));
    }
  }
  vifDirect((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))),0x1f);
  uiDrawText(0xd2,0x1b,0x2ec4e0,0);
  uiDrawText(0x1c2,0x1b,(*(uint32_t*)rtguest::G((uint32_t)(0x29fd90u + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26)))) * 4))),0);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) == 0) {
    DrawEntryRowsForState(param_1,param_2);
    return 0;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) < 3) {
    DrawRowsAndUpdateState(param_1,param_2);
    return 0;
  }
  UpdateRankingsAndAdvance(param_1,param_2);
  return 0;
}


RT_GUEST_FN(0x00237388, guest::DispatchRecordsScreen)

}  // namespace guest
