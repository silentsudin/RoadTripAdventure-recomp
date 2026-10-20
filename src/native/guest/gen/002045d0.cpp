// guest:0x002045d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t uiDrawTextScaled(uint64_t param_1, uint64_t param_2, uint64_t param_3, uint32_t param_4, uint64_t param_5, uint64_t param_6, ...) {
  rtguest::FrameGuard rt_fg(48);
  char cVar1;
  int iVar2;
uint32_t puVar3;
uint32_t pcVar4;
  
  if ((long)(*(uint32_t*)rtguest::G(0x335688)) != (param_5 & 0xffff)) {
    (*(uint32_t*)rtguest::G(0x335688)) = (int)(param_5 & 0xffff);
    if ((*(uint32_t*)rtguest::G(0x335684)) < 0xa00) {
      iVar2 = (*(uint32_t*)rtguest::G(0x335684)) * 0x10;
      (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 1;
      puVar3 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + iVar2 + 0x70);
      (*(uint64_t*)rtguest::G((uint32_t)puVar3)) = param_5 << 0x38 | 0x13780001400000;
      (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
    }
  }
  while ((*(int8_t*)rtguest::G((uint32_t)param_4)) != '\0') {
    param_1 = RT_CALLRA(0x204670, guest::uiDrawGlyphScaled(param_1,param_2,param_3,0,param_4,param_6));
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)param_4));
    pcVar4 = param_4 + 2;
    param_4 = param_4 + 1;
    if (cVar1 < '\0') {
      param_4 = pcVar4;
    }
  }
  return param_1;
}


RT_GUEST_FN(0x002045d0, guest::uiDrawTextScaled)

}  // namespace guest
