// guest:0x00267060 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceSummaryDrawGolfRow(uint32_t param_1, uint32_t param_2, ...) {
    uint32_t uStack_60 = rtguest::allocFrame(2);
  bool bVar1;
  int iVar2;
  size_t sVar3;
  uint uVar4;
  uint uVar5;
uint32_t puStack_90;
uint32_t acStack_8c = rtguest::allocFrame(44);
uint32_t puStack_5c;
  
  iVar2 = param_2 + 0x120;
  if (((*(uint32_t*)rtguest::G(0x178034c)) & 8) == 0) {
    uiDrawText(param_1,param_2 / 2,(*(uint32_t*)rtguest::G(0x002be3f4)),3);
    (*(uint16_t*)rtguest::G((uint32_t)uStack_60)) = (*(uint32_t*)rtguest::G(0x335348));
    puStack_5c = (*(uint32_t*)rtguest::G(0x00335350));
    bVar1 = true;
    uVar5 = 0;
    do {
      iVar2 = param_2;
      param_2 = iVar2 + 0x18;
      if (bVar1) {
        ::sprintf((char *)rtguest::G((uint32_t)(rtguest::allocFrame(0x20))), (char *)rtguest::G((uint32_t)(0x32f280)), uVar5 + 1);
      }
      else {
        puStack_90 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x335338u + 0));
        (*(int8_t*)rtguest::G((uint32_t)acStack_8c + (uint32_t)(0) * 1)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x335338u + (uint32_t)(4) * 4));
        (*(int8_t*)rtguest::G((uint32_t)acStack_8c + (uint32_t)(1) * 1)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x335338u + (uint32_t)(5) * 4));
      }
      uVar4 = uVar5 + 1;
      uiDrawText((int)param_1 + 0x14,param_2 / 2,rtguest::allocFrame(0x20),3);
      if ((*(uint8_t*)rtguest::G((uint32_t)0x1780a30 + (uint32_t)(uVar5) * 1)) == '\x7f') {
        puStack_90 = puStack_5c;
      }
      else if ((*(uint8_t*)rtguest::G((uint32_t)0x1780a30 + (uint32_t)(uVar5) * 1)) == '\0') {
        puStack_90 = (uint32_t)CONCAT22((*(uint16_t*)rtguest::G((uint32_t)rtguest::allocFrame(0x20) + 2)),(*(uint16_t*)rtguest::G((uint32_t)uStack_60)));
      }
      else {
        ::sprintf((char *)rtguest::G((uint32_t)(rtguest::allocFrame(0x20))), (char *)rtguest::G((uint32_t)(0x335340)));
      }
      sVar3 = ::strlen((char *)rtguest::G((uint32_t)((uint32_t)rtguest::allocFrame(0x20))));
      uiDrawText((int)param_1 + (int)sVar3 * -0x10 + 0xf0,param_2 / 2,rtguest::allocFrame(0x20),3);
      bVar1 = uVar4 < 9;
      uVar5 = uVar4;
    } while (uVar4 < 10);
    iVar2 = iVar2 + 0x48;
  }
  else {
    uiDrawText(param_1,param_2 / 2,0x32f250,3);
  }
}


RT_GUEST_FN(0x00267060, guest::raceSummaryDrawGolfRow)

}  // namespace guest
