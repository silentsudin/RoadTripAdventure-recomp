// guest:0x002855f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sysSbrk(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::gS[2] = 0x00000000002f0000ull;  // walk const
  }
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
  puVar1 = (uint32_t)RT_CALLRA(0x285648, guest::EndOfHeap());
  if (puVar1 < puVar3) {
    puVar2 = (uint32_t)RT_CALLRA(0x28565c, guest::getReent());
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
