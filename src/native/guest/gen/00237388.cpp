// guest:0x00237388 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DispatchRecordsScreen(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  byte bVar1;
  byte bVar2;
  undefined8 uVar3;
  int iVar4;
uint32_t pbVar5;
  int iVar6;
  
  iVar6 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) = (uint32_t)(*(uint8_t*)rtguest::G(0x1782a70));
    uVar3 = RT_CALLRA(0x2373c0, guest::getExtraPacketArea(0x1f0));
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = (int)uVar3;
    RT_CALLRA(0x2373cc, guest::InitScreenLayout(uVar3));
    iVar4 = 0;
    pbVar5 = 0x0178700a;
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26)));
    while( true ) {
      bVar2 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
      pbVar5 = pbVar5 + 4;
      (*(uint32_t*)rtguest::G((uint32_t)0x1782a74 + (uint32_t)(iVar4 + (uint32_t)bVar1 * 0x18) * 4)) = (char)(0x1e - (uint32_t)bVar2);
      if ((int)((0x1e - (uint32_t)bVar2) * 0x1000000) >> 0x18 == 0x1f) {
        (*(uint32_t*)rtguest::G((uint32_t)0x1782a74 + (uint32_t)(iVar4 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) * 0x18) * 4)) = 0;
      }
      iVar4 = iVar4 + 1;
      if (0x17 < iVar4) break;
      bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26)));
    }
  }
  RT_CALLRA(0x237450, guest::vifDirect((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))),0x1f));
  RT_CALLRA(0x237468, guest::uiDrawText(0xd2,0x1b,0x2ec4e0,0));
  RT_CALLRA(0x23748c, guest::uiDrawText(0x1c2,0x1b,(*(uint32_t*)rtguest::G((uint32_t)(0x29fd90u + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26)))) * 4))),0));
  if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) == 0) {
    DrawEntryRowsForState(param_1,param_2);
    return 0;
  }
  if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) < 3) {
    DrawRowsAndUpdateState(param_1,param_2);
    return 0;
  }
  UpdateRankingsAndAdvance(param_1,param_2);
  return 0;
}


RT_GUEST_FN(0x00237388, guest::DispatchRecordsScreen)

}  // namespace guest
