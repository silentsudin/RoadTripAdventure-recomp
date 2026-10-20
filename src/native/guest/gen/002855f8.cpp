// guest:0x002855f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sysSbrk(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t puVar1;
uint32_t puVar2;
uint32_t puVar3;
  uint uVar4;
  
  uVar4 = Status & 0x10000;
  while ((Status & 0x10000) != 0) {
    DI();
    SYNC(0x10);
  }
  puVar3 = (*(uint32_t*)rtguest::G(0x002e9140)) + param_1;
  puVar1 = (uint32_t)EndOfHeap();
  if (puVar1 < puVar3) {
    puVar2 = (uint32_t)getReent();
    (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0xc;
    if (uVar4 != 0) {
      EI();
    }
    puVar1 = (uint32_t)0xffffffff;
    puVar3 = (*(uint32_t*)rtguest::G(0x002e9140));
  }
  else {
    puVar1 = (*(uint32_t*)rtguest::G(0x002e9140));
    if (uVar4 != 0) {
      EI();
    }
  }
  (*(uint32_t*)rtguest::G(0x002e9140)) = puVar3;
  return puVar1;
}


RT_GUEST_FN(0x002855f8, guest::sysSbrk)

}  // namespace guest
