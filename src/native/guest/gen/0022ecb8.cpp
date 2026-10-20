// guest:0x0022ecb8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitRankingDisplay(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  byte bVar1;
  int iVar2;
  uint uVar3;
uint32_t pbVar4;
  int iVar5;
  
  RT_CALLRA(0x22eccc, guest::PlayBgmSlot1());
  iVar5 = (int)param_1;
  RT_CALLRA(0x22ecd4, guest::ResetStateBlock((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)))));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 0;
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x10)));
  if ((((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 3))) & 8) == 0) ||
     ((uVar3 = 2, (*(int8_t*)rtguest::G(0x177fdd6)) != '\0' && ((*(uint32_t*)rtguest::G(0x178321e)) != '\0')))) {
    uVar3 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 1)));
  }
  if (uVar3 == 0) {
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25)));
  }
  else {
    pbVar4 = 0x0178700b;
    bVar1 = (*(uint32_t*)rtguest::G(0x178700b));
    while( true ) {
      pbVar4 = pbVar4 + 4;
      if ((bVar1 & 0x73) != 0) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) =
             (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) |
             (uint32_t)(1 << ((int)(bVar1 & 0x30) >> 4) + ((int)(bVar1 & 0x40) >> 6) * 3);
      }
      uVar3 = uVar3 - 1;
      if (uVar3 == 0) break;
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar4));
    }
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25)));
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xc))) = 0x22eb78;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = bVar1 ^ 0x3f;
  UpdateZoneCheckFrame(param_1,param_2);
  return 0;
}


RT_GUEST_FN(0x0022ecb8, guest::InitRankingDisplay)

}  // namespace guest
