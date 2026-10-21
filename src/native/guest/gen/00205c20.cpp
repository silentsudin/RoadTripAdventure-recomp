// guest:0x00205c20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiMenuCreate(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::gS[0] = 0x000000000034a3c0ull;  // walk const
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
uint32_t puVar1;
uint32_t puVar2;
  int iVar3;
  int iVar4;
uint32_t puVar5;
  
  puVar5 = 0x0034a3c0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = 0;
  iVar4 = 7;
  do {
    iVar3 = 0;
    puVar2 = 0x0177a0f8;
    puVar1 = (*(uint32_t*)rtguest::G(0x177a0f8));
    while (puVar2 = puVar2 + (uint32_t)(0xc) * sizeof(undefined4), puVar1 != puVar5) {
      iVar3 = iVar3 + 1;
      if (0x2f < iVar3) goto LAB_00205cac;
      puVar1 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar2));
    }
    iVar4 = iVar4 + -1;
    puVar5 = puVar5 + (uint32_t)(0x30) * sizeof(undefined2);
  } while (iVar4 != -1);
  RT_CALLRA(0x205ca0, guest::fatal(0x2ea058));
LAB_00205cac:
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = puVar5;
  (*(uint16_t*)rtguest::G((uint32_t)puVar5)) = 0x140;
  (*(uint16_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(1) * sizeof(uint16_t))) = 0x70;
  (*(uint16_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(2) * sizeof(uint16_t))) = 0;
  (*(uint16_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(3) * sizeof(uint16_t))) = 0;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0xb))) = 0;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + (uint32_t)(8) * sizeof(undefined2)))) = 0;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x11))) = 0;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + (uint32_t)(7) * sizeof(undefined2)))) = 0;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x13))) = 0;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0xf))) = 0;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + (uint32_t)(6) * sizeof(undefined2)))) = 0;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x5f))) = 0;
  return puVar5;
}


RT_GUEST_FN(0x00205c20, guest::uiMenuCreate)

}  // namespace guest
