// guest:0x0026fac8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGszbufaddr(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t puVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = (int)(int16_t)param_3;
  puVar1 = (uint32_t)sceGsGetEnv();
  iVar2 = (int16_t)param_2 + 0x3f;
  iVar3 = (int16_t)param_2 + 0x7e;
  if (-1 < iVar2) {
    iVar3 = iVar2;
  }
  if ((param_1 & 2) == 0) {
    iVar2 = iVar4 + 0x3e;
    if (-1 < iVar4 + 0x1f) {
      iVar2 = iVar4 + 0x1f;
    }
    iVar2 = iVar2 >> 5;
  }
  else {
    iVar2 = iVar4 + 0x7e;
    if (-1 < iVar4 + 0x3f) {
      iVar2 = iVar4 + 0x3f;
    }
    iVar2 = iVar2 >> 6;
  }
  iVar2 = (iVar3 >> 6) * iVar2;
  if (((*(uint64_t*)rtguest::G((uint32_t)puVar1)) & 0xffff0000ffff) == 1) {
    iVar2 = iVar2 * 0x10000;
  }
  else {
    iVar2 = iVar2 * 0x20000;
  }
  return iVar2 >> 0x10;
}


RT_GUEST_FN(0x0026fac8, guest::sceGszbufaddr)

}  // namespace guest
