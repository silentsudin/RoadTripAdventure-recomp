// guest:0x00202d10 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawDialogTextLine(uint32_t param_1, uint32_t param_2, uint64_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(64);
  byte bVar1;
  undefined2 uVar2;
uint32_t puVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  
  iVar5 = (*(uint32_t*)rtguest::G(0x335680));
  iVar6 = (*(uint32_t*)rtguest::G(0x335684));
  if (((*(uint32_t*)rtguest::G(0x335688)) != 9) && ((*(uint32_t*)rtguest::G(0x335688)) = 9, (*(uint32_t*)rtguest::G(0x335684)) < 0xa00)) {
    iVar6 = (*(uint32_t*)rtguest::G(0x335684)) + 1;
    puVar3 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70);
    (*(uint32_t*)rtguest::G(0x335684)) = iVar6;
    (*(uint64_t*)rtguest::G((uint32_t)puVar3)) = 0x913780001400000;
    (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
  }
  uVar2 = (uint32_t)param_2;
  if (iVar6 < 0x9fc) {
    RT_CALLRA(0x202de0, guest::uiSpriteWrite(iVar5 + iVar6 * 0x10 + 0x70,(param_1 + -5) * 0x10000 >> 0x10,uVar2,0x1ab,0xdd,5,
                  0x20));
    iVar6 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    iVar5 = (*(uint32_t*)rtguest::G(0x335680));
    (*(uint32_t*)rtguest::G(0x335684)) = iVar6;
  }
  if (iVar6 < 0x9fc) {
    RT_CALLRA(0x202e2c, guest::uiSpriteWrite(iVar5 + iVar6 * 0x10 + 0x70,(short)param_1,uVar2,0x1b0,0xdd,0x60,0x20));
    iVar6 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    iVar5 = (*(uint32_t*)rtguest::G(0x335680));
    (*(uint32_t*)rtguest::G(0x335684)) = iVar6;
  }
  iVar7 = param_1 + 0x60;
  iVar4 = iVar6 * 0x10 + iVar5;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x20))) = 0x374076c0073;
  if (iVar6 < 0x9fc) {
    RT_CALLRA(0x202e8c, guest::uiSpriteWrite(iVar4 + 0x70,iVar7 * 0x10000 >> 0x10,uVar2,0x1c0,0xdd,0x40,0x20));
    iVar6 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    iVar5 = (*(uint32_t*)rtguest::G(0x335680));
    (*(uint32_t*)rtguest::G(0x335684)) = iVar6;
  }
  iVar5 = iVar6 * 0x10 + iVar5;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20))) = 0x37407700073;
  if (iVar6 < 0x9fc) {
    RT_CALLRA(0x202eec, guest::uiSpriteWrite(iVar5 + 0x70,(param_1 + 0xa0) * 0x10000 >> 0x10,uVar2,0x1c8,0xdd,5,0x20));
    (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
  }
  if (param_3 < 0) {
    iVar5 = RT_CALLRA(0x202f10, guest::uiDrawNumber16Y2(iVar7,param_2 + 8,-(int)param_3,6));
    if ((*(uint32_t*)rtguest::G(0x335684)) < 0x9fc) {
      RT_CALLRA(0x202f58, guest::uiSpriteWrite((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70,(iVar5 + -10) * 0x10000 >> 0x10,
                    (param_2 + 4) * 0x10000 >> 0x10,0x1b8,0xb4,10,0x14));
      (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    }
  }
  else {
    RT_CALLRA(0x202f78, guest::uiDrawNumber16Y2(iVar7,param_2 + 8,param_3,0));
  }
  param_1 = param_1 + 0x62;
  if (((*(uint32_t*)rtguest::G(0x335688)) != 0) && ((*(uint32_t*)rtguest::G(0x335688)) = 0, (*(uint32_t*)rtguest::G(0x335684)) < 0xa00)) {
    iVar5 = (*(uint32_t*)rtguest::G(0x335684)) * 0x10;
    (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 1;
    puVar3 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + iVar5 + 0x70);
    (*(uint64_t*)rtguest::G((uint32_t)puVar3)) = 0x13780001400000;
    (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
  }
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_4));
  while (bVar1 != 0) {
    param_1 = RT_CALLRA(0x203018, guest::uiDrawGlyph(param_1,(param_2 / 2 + 3) * 2,0,param_4));
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_4));
    if (bVar1 == 0x2f) {
      param_4 = param_4 + 2;
    }
    else if (bVar1 == 0x60) {
      param_4 = param_4 + 2;
    }
    else if (bVar1 == 0x5e) {
      param_4 = param_4 + 2;
    }
    else if (bVar1 == 0x7e) {
      param_4 = param_4 + 2;
    }
    else if ((int)((uint32_t)bVar1 << 0x18) < 0) {
      param_4 = param_4 + 2;
    }
    else {
      param_4 = param_4 + 1;
    }
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_4));
  }
  return 0;
}


RT_GUEST_FN(0x00202d10, guest::DrawDialogTextLine)

}  // namespace guest
