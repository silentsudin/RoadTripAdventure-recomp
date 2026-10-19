// guest:0x00235f10 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawPlayerResults(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  byte bVar1;
  undefined8 uVar2;
uint32_t ppuVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  
  iVar5 = (int)param_1;
  bVar1 = (*(uint8_t*)rtguest::G((uint32_t)0x178700a + (uint32_t)(iVar5 * 4) * 1));
  uVar7 = (uint32_t)bVar1;
  if (0x19 < param_2) {
    param_2 = param_2 + -2;
  }
  uVar2 = GetSlotDataPtr(param_1,0);
  iVar4 = iVar5 * 0x18 + 0x41;
  uiDrawText(0x46,iVar4,uVar2,0);
  uiDrawTextCell(8,iVar5 * 2 + 6,0x333818,0);
  if (uVar7 == 0xff) {
    uiDrawText(0x104,iVar4,0x3337f0,0);
  }
  else {
    uVar2 = 6;
    if (5 < bVar1) {
      uVar2 = 0;
    }
    uiDrawText(0x118,iVar4,(*(uint32_t*)rtguest::G((uint32_t)(0x29fd30u + (uint32_t)(uVar7) * 4))),uVar2);
  }
  iVar4 = iVar5 * 0x18 + 0x4d;
  uiDrawText(0x10e,iVar4,0x333820,0);
  if ((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x1780750 + (uint32_t)(param_2 + iVar5 * 0x1a) * 4)) == 0xff) {
    ppuVar3 = 0x333828u;
  }
  else {
    ppuVar3 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(0x29fd30u + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x01780750u + (uint32_t)(param_2 + iVar5 * 0x1a) * 4))) * 4)));
  }
  uiDrawText(0x118,iVar4,ppuVar3,5);
  uiDrawText(0x140,iVar5 * 0x18 + 0x4d,0x333830,0);
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x017808d8 + (iVar5 * 0x1a + param_2) * 4)));
  if (uVar7 != 0xff) {
    if ((iVar4 == 0x7fffffff) ||
       ((uVar2 = 0, (int)(*(uint32_t*)rtguest::G((uint32_t)0x1786ff0 + (uint32_t)(iVar5) * 4)) < iVar4 && (uVar2 = 6, param_3 == 0)))) {
      uVar2 = 0;
    }
    uiDrawTime(0x168,iVar5 * 0x18 + 0x41,(*(uint32_t*)rtguest::G((uint32_t)0x1786ff0 + (uint32_t)(iVar5) * 4)),uVar2);
  }
  iVar6 = iVar5 * 0x18 + 0x4d;
  uiDrawText(0x15e,iVar6,0x333820,0);
  uiDrawTime(0x168,iVar6,iVar4,5);
  uiDrawText(0x1b8,iVar6,0x333830,0);
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x017807a0 + (iVar5 * 0x1a + param_2) * 4)));
  if (uVar7 != 0xff) {
    if ((iVar4 == 0x7fffffff) ||
       ((uVar2 = 0, (int)(*(uint32_t*)rtguest::G((uint32_t)0x1787074 + (uint32_t)(iVar5 * 4) * 4)) < iVar4 && (uVar2 = 6, param_3 == 0)))) {
      uVar2 = 0;
    }
    uiDrawTime(0x1e0,iVar5 * 0x18 + 0x41,(*(uint32_t*)rtguest::G((uint32_t)0x1787074 + (uint32_t)(iVar5 * 4) * 4)),uVar2);
  }
  iVar5 = iVar5 * 0x18 + 0x4d;
  uiDrawText(0x1d6,iVar5,0x333820,0);
  uiDrawTime(0x1e0,iVar5,iVar4,5);
  uiDrawText(0x230,iVar5,0x333830,0);
  return 0;
}


RT_GUEST_FN(0x00235f10, guest::DrawPlayerResults)

}  // namespace guest
