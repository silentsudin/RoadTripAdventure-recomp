// guest:0x0020e5b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SaveAIDataFile(uint32_t param_1, uint32_t param_2, ...) {
  char cVar1;
  int iVar2;
uint32_t puVar3;
  long lVar4;
  undefined8 uVar5;
  long lVar6;
uint32_t pcVar7;
  int iVar8;
  int iVar9;
uint32_t auStack_f0 = rtguest::allocFrame(32);
uint32_t auStack_d0 = rtguest::allocFrame(128);
  
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_f0)), (char *)rtguest::G((uint32_t)(0x2ea720)), param_1);
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ea730u)), auStack_f0);
  lVar4 = sceOpen(auStack_f0,0x602);
  pcVar7 = param_2 + 0xff;
  if (lVar4 == -1) {
    iVar2 = rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ea740u)));
  }
  else {
    iVar2 = 0x100;
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar7));
    while (cVar1 == '\0') {
      pcVar7 = pcVar7 + -1;
      iVar2 = iVar2 + -1;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar7));
    }
    if (0 < iVar2) {
      puVar3 = param_2;
      iVar8 = 0;
      do {
        iVar9 = iVar8 + 1;
        uVar5 = ::sprintf((char *)rtguest::G((uint32_t)(auStack_d0)), (char *)rtguest::G((uint32_t)(0x2ea758)), (*(uint8_t*)rtguest::G((uint32_t)puVar3)), iVar8);
        lVar6 = sceWrite(lVar4,auStack_d0,uVar5);
        if (lVar6 == -1) {
          rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ea710u)));
        }
        puVar3 = param_2 + iVar9;
        iVar8 = iVar9;
      } while (iVar9 < iVar2);
    }
    sceClose(lVar4);
    iVar2 = rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x333390u)));
  }
  return iVar2;
}


RT_GUEST_FN(0x0020e5b8, guest::SaveAIDataFile)

}  // namespace guest
