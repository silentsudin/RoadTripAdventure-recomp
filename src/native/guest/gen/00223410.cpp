// guest:0x00223410 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitCarRenderRegs(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
uint32_t puVar5;
  
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(0x0177eb10)), 0, 0x800));
  puVar5 = 0x0177ea30;
  iVar3 = 0;
  iVar4 = 3;
  do {
    iVar1 = iVar3 + 0x2b26;
    iVar2 = iVar3 + 0x2b66;
    iVar4 = iVar4 + -1;
    iVar3 = iVar3 + 0x44;
    (*(uint64_t*)rtguest::G((uint32_t)puVar5)) = (long)iVar1 | 0x1d308000U | (long)iVar2 << 0x25 | 0x20000005c0000000;
    puVar5 = puVar5 + (uint32_t)(1) * sizeof(uint64_t);
  } while (-1 < iVar4);
  return 0;
}


RT_GUEST_FN(0x00223410, guest::InitCarRenderRegs)

}  // namespace guest
