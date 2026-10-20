// guest:0x00203b78 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
// param_4 is ulong in pseudocode: param_4 << 0x38 needs the full 64-bit value.
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawTime(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint64_t param_4, ...) {
  rtguest::FrameGuard rt_fg(48);
uint32_t puVar1;
  int iVar2;
uint32_t auStack_30 = rtguest::allocFrame(16);
  
  if ((long)(*(uint32_t*)rtguest::G(0x335688)) != (param_4 & 0xffff)) {
    (*(uint32_t*)rtguest::G(0x335688)) = (int)(param_4 & 0xffff);
    if ((*(uint32_t*)rtguest::G(0x335684)) < 0xa00) {
      puVar1 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70);
      (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 1;
      (*(uint64_t*)rtguest::G((uint32_t)puVar1)) = param_4 << 0x38 | 0x13780001400000;
      (*(uint64_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
    }
  }
  if (param_3 == 0x7fffffff) {
    uiDrawSmallText(param_1,param_2,0x2e9b70);
  }
  else if (param_3 < 360000) {
    iVar2 = (int)param_3;
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_30)), (char *)rtguest::G((uint32_t)(0x2e9b90)), iVar2 / 0xe10, (iVar2 / 0x3c) % 0x3c, ((iVar2 % 0x3c) * 100) / 0x3c, iVar2 % 0x3c, 0x3c);
    uiDrawSmallText(param_1,param_2,auStack_30);
  }
  else {
    uiDrawSmallText(param_1,param_2,0x2e9b80);
  }
  return 0;
}


RT_GUEST_FN(0x00203b78, guest::uiDrawTime)

}  // namespace guest
