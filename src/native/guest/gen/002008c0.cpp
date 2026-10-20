// guest:0x002008c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawSmallText(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(64);
  byte bVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  char cVar5;
  undefined4 uVar6;
  
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_3));
  do {
    uVar2 = (uint32_t)bVar1;
    if (uVar2 == 0) {
      return 0;
    }
    param_3 = param_3 + 1;
    if (((uVar2 == 0x27) || (uVar2 == 0x22)) || (uVar2 == 0x2d)) {
      cVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(uVar2 + 0x29a860)));
      iVar3 = ((int)(uVar2 - 0x21) % 0x1f) * 0x10 + (0x10 - cVar5) / 2;
      iVar4 = ((int)(uVar2 - 0x21) / 0x1f) * 0x18;
      if ((*(uint32_t*)rtguest::G(0x335684)) < 0x9fc) {
        uVar6 = 0x18;
LAB_00200a10:
        uiSpriteWrite((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70,param_1,((int)param_2 << 0x11) >> 0x10
                      ,iVar3,iVar4,cVar5,uVar6);
        (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
      }
    }
    else if (uVar2 != 0x20) {
      iVar3 = ((int)(uVar2 - 1) % 0x33) * 10;
      iVar4 = ((int)(uVar2 - 1) / 0x33) * 0x140000 + 0xb40000 >> 0x10;
      if ((*(uint32_t*)rtguest::G(0x335684)) < 0x9fc) {
        cVar5 = '\n';
        uVar6 = 0x14;
        goto LAB_00200a10;
      }
    }
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_3));
    param_1 = param_1 + 10;
  } while( true );
}


RT_GUEST_FN(0x002008c0, guest::uiDrawSmallText)

}  // namespace guest
