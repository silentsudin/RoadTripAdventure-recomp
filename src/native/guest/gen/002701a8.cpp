// guest:0x002701a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDmaReset(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x20, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
uint32_t puVar1;
uint32_t ppuVar2;
  int iVar3;
uint32_t piVar4;
  uint uVar5;
uint32_t auStack_50 = rtguest::allocFrame(32);
  
  ppuVar2 = 0x2dcdc0u;
  piVar4 = 0x002dce00;
  iVar3 = 9;
  uVar5 = REG_DMAC_CTRL & 1;
  do {
    if ((*(int32_t*)rtguest::G((uint32_t)piVar4)) != 0) {
      puVar1 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)ppuVar2));
      (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(0x20) * sizeof(uint32_t))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar1)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(0xc) * sizeof(uint32_t))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(4) * sizeof(uint32_t))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(0x14) * sizeof(uint32_t))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(0x10) * sizeof(uint32_t))) = 0;
    }
    ppuVar2 = ppuVar2 + 1;
    iVar3 = iVar3 + -1;
    piVar4 = piVar4 + 0x4;  // T* element step
  } while (-1 < iVar3);
  REG_DMAC_STAT = 0;
  RT_CALLRA(0x27023c, guest::memclr(auStack_50,0x14));
  RT_CALLRA(0x270244, guest::sceDmaPutEnv(auStack_50));
  if (param_1 == 1) {
    REG_DMAC_CTRL = REG_DMAC_CTRL | 1;
  }
  return uVar5;
}


RT_GUEST_FN(0x002701a8, guest::sceDmaReset)

}  // namespace guest
