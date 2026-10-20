// guest:0x002028b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawDialogueFrameA(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  int iVar1;
uint32_t puVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  iVar1 = (*(uint32_t*)rtguest::G(0x335680));
  iVar4 = (*(uint32_t*)rtguest::G(0x335684));
  if (((*(uint32_t*)rtguest::G(0x335688)) != 7) && ((*(uint32_t*)rtguest::G(0x335688)) = 7, (*(uint32_t*)rtguest::G(0x335684)) < 0xa00)) {
    iVar4 = (*(uint32_t*)rtguest::G(0x335684)) + 1;
    puVar2 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70);
    (*(uint32_t*)rtguest::G(0x335684)) = iVar4;
    (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = 0x713780001400000;
    (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
  }
  iVar6 = (param_1 + -0x10) * 0x10000 >> 0x10;
  if (iVar4 < 0x9fc) {
    RT_CALLRA(0x20296c, guest::uiSpriteWrite(iVar1 + iVar4 * 0x10 + 0x70,0x20,iVar6,0,0x100,0x28,0x38));
    iVar4 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    iVar1 = (*(uint32_t*)rtguest::G(0x335680));
    (*(uint32_t*)rtguest::G(0x335684)) = iVar4;
  }
  iVar5 = (param_1 + 0x28) * 0x10000 >> 0x10;
  if (iVar4 < 0x9fc) {
    RT_CALLRA(0x2029b8, guest::uiSpriteWrite(iVar1 + iVar4 * 0x10 + 0x70,0x20,iVar5,0x78,0x100,0x28,0x30));
    iVar4 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    iVar1 = (*(uint32_t*)rtguest::G(0x335680));
    (*(uint32_t*)rtguest::G(0x335684)) = iVar4;
  }
  if (iVar4 < 0x9fc) {
    RT_CALLRA(0x2029fc, guest::uiSpriteWrite(iVar1 + iVar4 * 0x10 + 0x70,0x48,(param_1 + -0x10) * 0x10000 >> 0x10,0x50,0x100,
                  0x1f0,0x38));
    iVar4 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    iVar1 = (*(uint32_t*)rtguest::G(0x335680));
    (*(uint32_t*)rtguest::G(0x335684)) = iVar4;
  }
  iVar3 = iVar4 * 0x10 + iVar1;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x20))) = 0x4dd0014007b;
  if (iVar4 < 0x9fc) {
    RT_CALLRA(0x202a58, guest::uiSpriteWrite(iVar3 + 0x70,0x48,(param_1 + 0x28) * 0x10000 >> 0x10,200,0x100,0x1f0,0x30));
    iVar4 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    iVar1 = (*(uint32_t*)rtguest::G(0x335680));
    (*(uint32_t*)rtguest::G(0x335684)) = iVar4;
  }
  iVar1 = iVar4 * 0x10 + iVar1;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x20))) = 0x4bd0032007b;
  if (iVar4 < 0x9fc) {
    RT_CALLRA(0x202ab0, guest::uiSpriteWrite(iVar1 + 0x70,0x238,iVar6,0x28,0x100,0x28,0x38));
    iVar4 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    (*(uint32_t*)rtguest::G(0x335684)) = iVar4;
  }
  if (iVar4 < 0x9fc) {
    RT_CALLRA(0x202af4, guest::uiSpriteWrite((*(uint32_t*)rtguest::G(0x335680)) + iVar4 * 0x10 + 0x70,0x238,iVar5,0xa0,0x100,0x28,0x30));
    iVar4 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    (*(uint32_t*)rtguest::G(0x335684)) = iVar4;
  }
  if (iVar4 < 0x9fc) {
    RT_CALLRA(0x202b3c, guest::uiSpriteWrite((*(uint32_t*)rtguest::G(0x335680)) + iVar4 * 0x10 + 0x70,0x130,(param_1 + 0x48) * 0x10000 >> 0x10,0x58,
                  0x100,0x20,0x18));
    (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
  }
  return 0;
}


RT_GUEST_FN(0x002028b0, guest::uiDrawDialogueFrameA)

}  // namespace guest
