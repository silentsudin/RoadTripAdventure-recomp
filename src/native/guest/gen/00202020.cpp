// guest:0x00202020 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GsDrawDigitsCounter(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  int iVar1;
  ulong in_v0;
uint32_t puVar2;
  ulong in_v1;
  undefined4 uVar3;
  int unaff_s0_lo;
  int unaff_s1_lo;
  
  if (param_1 != 0) {
    iVar1 = param_5 * 0x10;
    param_5 = param_5 + 1;
    puVar2 = (uint32_t)(param_3 + iVar1 + 0x70);
    (*(uint32_t*)rtguest::G(0x335684)) = param_5;
    (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = in_v0 | in_v1;
    (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
  }
  do {
    unaff_s1_lo = unaff_s1_lo + -0x10;
    iVar1 = unaff_s0_lo % 10;
    if (iVar1 < 4) {
      iVar1 = iVar1 * 0x100000 + 0x1b80000 >> 0x10;
      if (param_5 < 0x9fc) {
        uVar3 = 200;
LAB_00202104:
        uiSpriteWrite((*(uint32_t*)rtguest::G(0x335680)) + param_5 * 0x10 + 0x70,unaff_s1_lo * 0x10000 >> 0x10,
                      (param_2 + -2) * 0x10000 >> 0x10,iVar1,uVar3,0x10,0x14);
        (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
      }
    }
    else {
      iVar1 = iVar1 * 0x10 + -0x40;
      if (param_5 < 0x9fc) {
        uVar3 = 0xdc;
        goto LAB_00202104;
      }
    }
    unaff_s0_lo = unaff_s0_lo / 10;
    param_5 = (*(uint32_t*)rtguest::G(0x335684));
    if (unaff_s0_lo == 0) {
    }
  } while( true );
}


RT_GUEST_FN(0x00202020, guest::GsDrawDigitsCounter)

}  // namespace guest
