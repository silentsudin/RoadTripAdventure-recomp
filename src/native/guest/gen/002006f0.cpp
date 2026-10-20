// guest:0x002006f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t fatal(uint64_t param_1, uint64_t param_2, uint64_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t puVar1;
  char cVar2;
  undefined8 uVar3;
  long lVar4;
  
  (*(uint64_t*)rtguest::G(0x335f70)) = 0x3f80000080a01010;
  (*(uint64_t*)rtguest::G(0x335e00)) = 0x3f80000080a01010;
  FlushCache(0);
  textGridFreeAllSlots();
  uVar3 = textGridCreate(0x6d00,0x7a00,0x4b,8);
  do {
    textGridClear(uVar3);
    textGridPrintf(uVar3,param_1,param_2,param_3);
    textGridDraw(uVar3);
    sceGsSyncPath(0,0);
    lVar4 = sceGsSyncV(0);
    puVar1 = 0x00335ce0;
    if ((*(uint8_t*)rtguest::G(0x335908)) != '\0') {
      puVar1 = 0x00335e50;
    }
    uiEnvCenterPos(puVar1,0x800,0x800,lVar4 == 0);
    cVar2 = (*(uint8_t*)rtguest::G(0x335908));
    (*(uint8_t*)rtguest::G(0x335908)) = (*(uint8_t*)rtguest::G(0x335908)) + '\x01';
    sceGsSwapDBuffDc(0x335c80,cVar2);
  } while( true );
}


RT_GUEST_FN(0x002006f0, guest::fatal)

}  // namespace guest
