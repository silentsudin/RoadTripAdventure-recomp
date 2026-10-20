// guest:0x00203098 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawBar3(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t puVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar2 = (*(uint32_t*)rtguest::G(0x335680));
  iVar3 = (*(uint32_t*)rtguest::G(0x335684));
  if (((*(uint32_t*)rtguest::G(0x335688)) != 0xb) && ((*(uint32_t*)rtguest::G(0x335688)) = 0xb, (*(uint32_t*)rtguest::G(0x335684)) < 0xa00)) {
    iVar3 = (*(uint32_t*)rtguest::G(0x335684)) + 1;
    puVar1 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70);
    (*(uint32_t*)rtguest::G(0x335684)) = iVar3;
    (*(uint64_t*)rtguest::G((uint32_t)puVar1)) = 0xb13780001400000;
    (*(uint64_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
  }
  iVar4 = (param_2 + -1) * 0x10000 >> 0x10;
  if (iVar3 < 0x9fc) {
    uiSpriteWrite(iVar2 + iVar3 * 0x10 + 0x70,(param_1 + -8) * 0x10000 >> 0x10,iVar4,0x180,0xe0,8,
                  0x1c);
    iVar3 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    iVar2 = (*(uint32_t*)rtguest::G(0x335680));
    (*(uint32_t*)rtguest::G(0x335684)) = iVar3;
  }
  if (iVar3 < 0x9fc) {
    uiSpriteWrite(iVar2 + iVar3 * 0x10 + 0x70,(short)param_1,(param_2 + -1) * 0x10000 >> 0x10,0x188,
                  0xe0,(short)param_3,0x1c);
    iVar3 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    iVar2 = (*(uint32_t*)rtguest::G(0x335680));
    (*(uint32_t*)rtguest::G(0x335684)) = iVar3;
  }
  iVar2 = iVar3 * 0x10 + iVar2;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x20))) = 0x38007620073;
  if (iVar3 < 0x9fc) {
    uiSpriteWrite(iVar2 + 0x70,(param_1 + param_3) * 0x10000 >> 0x10,iVar4,400,0xe0,8,0x1c);
    (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
  }
  return 0;
}


RT_GUEST_FN(0x00203098, guest::uiDrawBar3)

}  // namespace guest
