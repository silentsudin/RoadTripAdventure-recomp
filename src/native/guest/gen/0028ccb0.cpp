// guest:0x0028ccb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifResetIop(uint32_t param_1, uint32_t param_2, ...) {
    uint32_t uStack_34 = rtguest::allocFrame(4);
    uint32_t uStack_38 = rtguest::allocFrame(4);
    uint32_t uStack_3c = rtguest::allocFrame(4);
  long lVar1;
  char cVar2;
  int iVar3;
uint32_t puStack_40;
  
  sceSifStopDma();
  (*(uint32_t*)rtguest::G((uint32_t)uStack_3c)) = sceSifGetReg(0xffffffff80000000);
  (*(uint32_t*)rtguest::G(0x177a050)) = 0;
  (*(uint32_t*)rtguest::G(0x177a054)) = param_2;
  if ((*(int8_t*)rtguest::G((uint32_t)param_1)) != '\0') {
    cVar2 = (*(int8_t*)rtguest::G((uint32_t)param_1));
    iVar3 = (*(uint32_t*)rtguest::G(0x177a050));
    while( true ) {
      (*(uint32_t*)rtguest::G(0x177a050)) = iVar3 + 1;
      (*(uint32_t*)rtguest::G((uint32_t)0x177a058 + (uint32_t)(iVar3) * 4)) = cVar2;
      if ((*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)((*(uint32_t*)rtguest::G(0x177a050))) * sizeof(int8_t))) == '\0') break;
      cVar2 = (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)((*(uint32_t*)rtguest::G(0x177a050))) * sizeof(int8_t)));
      iVar3 = (*(uint32_t*)rtguest::G(0x177a050));
    }
  }
  (*(uint32_t*)rtguest::G(0x177a048)) = 0x80000003;
  (*(uint32_t*)rtguest::G(0x177a040)) = 0x68;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_38)) = 0x68;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_34)) = 0x44;
  puStack_40 = 0x0177a040;
  sceSifWriteBackDCache(0x177a040,0x68);
  sceSifSetReg(4,0x40000);
  lVar1 = sceSifSetDma(rtguest::allocFrame(0x20),1);
  if (lVar1 != 0) {
    sceSifSetReg(4,0x10000);
    sceSifSetReg(4,0x20000);
    sceSifSetReg(0xffffffff80000002,0);
    sceSifSetReg(0xffffffff80000000,0);
  }
  return lVar1 != 0;
}


RT_GUEST_FN(0x0028ccb0, guest::sceSifResetIop)

}  // namespace guest
