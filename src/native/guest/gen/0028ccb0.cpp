// guest:0x0028ccb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifResetIop(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
    uint32_t uStack_3c = rtguest::allocFrame(4);
    uint32_t uStack_38 = rtguest::allocFrame(4);
    uint32_t uStack_34 = rtguest::allocFrame(4);
  long lVar1;
  char cVar2;
  int iVar3;
uint32_t puStack_40;
  
  RT_CALLRA(0x28ccc4, guest::sceSifStopDma());
  (*(uint32_t*)rtguest::G((uint32_t)uStack_3c)) = RT_CALLRA(0x28ccd0, guest::sceSifGetReg(0xffffffff80000000));
  (*(uint32_t*)rtguest::G(0x177a050)) = 0;
  (*(uint32_t*)rtguest::G(0x177a054)) = param_2;
  if ((*(int8_t*)rtguest::G((uint32_t)param_1)) != '\0') {
    cVar2 = (*(int8_t*)rtguest::G((uint32_t)param_1));
    iVar3 = (*(uint32_t*)rtguest::G(0x177a050));
    while( true ) {
      (*(uint32_t*)rtguest::G(0x177a050)) = iVar3 + 1;
      (*(uint8_t*)rtguest::G(0x177a058 + (uint32_t)iVar3)) = cVar2;
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
  RT_CALLRA(0x28cd80, guest::sceSifSetReg(4,0x40000));
  uint32_t dma = rtguest::allocFrame(0x10);
  (*(uint32_t*)rtguest::G(dma)) = puStack_40;
  (*(uint32_t*)rtguest::G(dma + 4)) = (*(uint32_t*)rtguest::G(uStack_3c));
  (*(uint32_t*)rtguest::G(dma + 8)) = (*(uint32_t*)rtguest::G(uStack_38));
  (*(uint32_t*)rtguest::G(dma + 0xc)) = (*(uint32_t*)rtguest::G(uStack_34));
  lVar1 = RT_CALLRA(0x28cd8c, guest::sceSifSetDma(dma,1));
  if (lVar1 != 0) {
    RT_CALLRA(0x28cd9c, guest::sceSifSetReg(4,0x10000));
    RT_CALLRA(0x28cda8, guest::sceSifSetReg(4,0x20000));
    RT_CALLRA(0x28cdb8, guest::sceSifSetReg(0xffffffff80000002,0));
    RT_CALLRA(0x28cdc4, guest::sceSifSetReg(0xffffffff80000000,0));
  }
  return lVar1 != 0;
}


RT_GUEST_FN(0x0028ccb0, guest::sceSifResetIop)

}  // namespace guest
