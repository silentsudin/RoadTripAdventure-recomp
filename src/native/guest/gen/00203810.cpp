// guest:0x00203810 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiBatchFlush(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  int iVar2;
uint32_t puVar3;
  
  iVar1 = (int)(*(uint32_t*)rtguest::G(0x335680));
  if ((*(uint32_t*)rtguest::G(0x335684)) != 0) {
    if ((*(uint32_t*)rtguest::G(0x335684)) < 0xa00) {
      iVar2 = (*(uint32_t*)rtguest::G(0x335684)) * 0x10;
      (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 1;
      puVar3 = (uint32_t)((int)(*(uint32_t*)rtguest::G(0x335680)) + iVar2 + 0x70);
      (*(uint64_t*)rtguest::G((uint32_t)puVar3)) = 0x4000008080;
      (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t))) = 0x3b;
    }
    iVar2 = (*(uint32_t*)rtguest::G(0x335684)) + 7;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x60))) =
         (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x60))) & 0xffffffffffff8000 | (long)(*(uint32_t*)rtguest::G(0x335684)) & 0x7fffU;
    vifDirect(iVar1,iVar2);
  }
  iVar1 = (param_1 ^ 1) * 0xa070;
  (*(uint32_t*)rtguest::G(0x335688)) = 0xffffffff;
  (*(uint32_t*)rtguest::G(0x335684)) = 2;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x00336368 + iVar1))) = 0x3b;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x00336350 + iVar1))) = 0x8000000044;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x00336358 + iVar1))) = 0x42;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x00336360 + iVar1))) = 0x4000008080;
  (*(uint32_t*)rtguest::G(0x335680)) = 0x3362e0 + iVar1;
  return 0;
}


RT_GUEST_FN(0x00203810, guest::uiBatchFlush)

}  // namespace guest
