// guest:0x002359e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawStandingsEntry(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
  }
  byte bVar1;
  undefined8 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar3 = (int)param_1;
  iVar5 = (int)param_3;
  bVar1 = (*(uint8_t*)rtguest::G((uint32_t)0x178700a + (uint32_t)((iVar5 + iVar3 * 2) * 4) * 1));
  uVar2 = RT_CALLRA(0x235a28, guest::GetSlotDataPtr(param_1,param_3));
  iVar4 = (iVar3 + iVar5 * 3) * 0xc + 0x41;
  RT_CALLRA(0x235a54, guest::uiDrawText(0x46,iVar4,uVar2,0));
  if (bVar1 == 0xff) {
    RT_CALLRA(0x235a7c, guest::uiDrawText(0x104,iVar4,0x3337f0,0));
  }
  else {
    RT_CALLRA(0x235a9c, guest::uiDrawText(0x118,iVar4,(*(uint32_t*)rtguest::G((uint32_t)(0x29fd30u + (uint32_t)(bVar1) * 4))),0));
    RT_CALLRA(0x235ab4, guest::uiDrawNumber16(0x172,iVar4,6 - (uint32_t)bVar1,0));
  }
  if (bVar1 != 0xff) {
    iVar3 = iVar3 + iVar5 * 3;
    iVar5 = iVar3 * 0xc + 0x41;
    uiDrawTime(0x186,iVar5,(*(uint32_t*)rtguest::G((uint32_t)0x1786ff0 + (uint32_t)(iVar3) * 4)),0);
    uiDrawTime(0x1e0,iVar5,(*(uint32_t*)rtguest::G((uint32_t)0x1787074 + (uint32_t)(iVar3 * 4) * 4)),0);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x002359e8, guest::DrawStandingsEntry)

}  // namespace guest
