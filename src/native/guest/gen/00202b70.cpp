// guest:0x00202b70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawDialogueFrameB(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  int iVar1;
uint32_t puVar2;
  int iVar3;
  int iVar4;
  
  iVar1 = (*(uint32_t*)rtguest::G(0x335680));
  iVar3 = (*(uint32_t*)rtguest::G(0x335684));
  if (((*(uint32_t*)rtguest::G(0x335688)) != 7) && ((*(uint32_t*)rtguest::G(0x335688)) = 7, (*(uint32_t*)rtguest::G(0x335684)) < 0xa00)) {
    iVar3 = (*(uint32_t*)rtguest::G(0x335684)) + 1;
    puVar2 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70);
    (*(uint32_t*)rtguest::G(0x335684)) = iVar3;
    (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = 0x713780001400000;
    (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
  }
  iVar4 = (param_1 + -0x1c) * 0x10000 >> 0x10;
  if (iVar3 < 0x9fc) {
    uiSpriteWrite(iVar1 + iVar3 * 0x10 + 0x70,(0x232 - param_2) * 0x10000 >> 0x10,iVar4,0x58,0x118,8
                  ,0x1c);
    iVar3 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    iVar1 = (*(uint32_t*)rtguest::G(0x335680));
    (*(uint32_t*)rtguest::G(0x335684)) = iVar3;
  }
  if (iVar3 < 0x9fc) {
    uiSpriteWrite(iVar1 + iVar3 * 0x10 + 0x70,(0x23a - param_2) * 0x10000 >> 0x10,
                  (param_1 + -0x1c) * 0x10000 >> 0x10,0x60,0x118,(param_2 + 4) * 0x10000 >> 0x10,
                  0x1c);
    iVar3 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    iVar1 = (*(uint32_t*)rtguest::G(0x335680));
    (*(uint32_t*)rtguest::G(0x335684)) = iVar3;
  }
  iVar1 = iVar3 * 0x10 + iVar1;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x20))) = 0x46007180073;
  if (iVar3 < 0x9fc) {
    uiSpriteWrite(iVar1 + 0x70,0x23e,iVar4,0xd0,0x100,0x18,0x1c);
    (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
  }
  return 0;
}


RT_GUEST_FN(0x00202b70, guest::uiDrawDialogueFrameB)

}  // namespace guest
