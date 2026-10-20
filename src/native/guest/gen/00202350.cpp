// guest:0x00202350 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawPanel9(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = (uint32_t)(int32_t)param_2;
  }
  short sVar1;
  int iVar2;
  int iVar3;
  short sVar4;
  undefined8 uVar5;
  undefined2 uVar6;
  undefined2 uVar7;
uint32_t puVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  ulong uVar12;
  int iVar13;
  long lVar14;
  int iVar15;
  ulong uVar16;
  
  iVar9 = (*(uint32_t*)rtguest::G(0x335680));
  lVar14 = 0x110;
  iVar10 = (*(uint32_t*)rtguest::G(0x335684));
  if (param_5 == 2) {
    if (((*(uint32_t*)rtguest::G(0x335688)) == 7) || ((*(uint32_t*)rtguest::G(0x335688)) = 7, 0x9ff < (*(uint32_t*)rtguest::G(0x335684)))) goto LAB_002024a8;
    uVar5 = 0x713780001400000;
    puVar8 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70);
  }
  else if (param_5 == 1) {
    if (((*(uint32_t*)rtguest::G(0x335688)) == 0xe) || ((*(uint32_t*)rtguest::G(0x335688)) = 0xe, 0x9ff < (*(uint32_t*)rtguest::G(0x335684)))) goto LAB_002024a8;
    uVar5 = 0xe13780001400000;
    puVar8 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70);
  }
  else {
    lVar14 = 0x138;
    if (((*(uint32_t*)rtguest::G(0x335688)) == 8) || ((*(uint32_t*)rtguest::G(0x335688)) = 8, 0x9ff < (*(uint32_t*)rtguest::G(0x335684)))) goto LAB_002024a8;
    puVar8 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70);
    uVar5 = 0x813780001400000;
  }
  iVar10 = (*(uint32_t*)rtguest::G(0x335684)) + 1;
  (*(uint32_t*)rtguest::G(0x335684)) = iVar10;
  (*(uint64_t*)rtguest::G((uint32_t)puVar8)) = uVar5;
  (*(uint64_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
LAB_002024a8:
  sVar1 = (short)param_1;
  sVar4 = sVar1 + -0xc;
  iVar15 = (int)lVar14 + -0xc;
  uVar16 = (uint32_t)iVar15;
  iVar11 = (param_2 + -6) * 0x10000 >> 0x10;
  if (iVar10 < 0x9fc) {
    uiSpriteWrite(iVar9 + iVar10 * 0x10 + 0x70,sVar4,(iVar11 << 0x11) >> 0x10,uVar16,0xdc,0xc,0xc);
    iVar10 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    (*(uint32_t*)rtguest::G(0x335684)) = iVar10;
    iVar9 = (*(uint32_t*)rtguest::G(0x335680));
  }
  iVar2 = (int)lVar14 + 8;
  iVar3 = (param_1 + param_3) * 0x10000 >> 0x10;
  if (iVar10 < 0x9fc) {
    uiSpriteWrite(iVar9 + iVar10 * 0x10 + 0x70,iVar3,(iVar11 << 0x11) >> 0x10,iVar2,0xdc,0xc,0xc);
    iVar10 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    (*(uint32_t*)rtguest::G(0x335684)) = iVar10;
    iVar9 = (*(uint32_t*)rtguest::G(0x335680));
  }
  uVar6 = (uint32_t)param_3;
  if (iVar10 < 0x9fc) {
    uiSpriteWrite(iVar9 + iVar10 * 0x10 + 0x70,sVar1,(param_2 * 2 + -0xc) * 0x10000 >> 0x10,lVar14,
                  0xdc,uVar6,0xc);
    iVar10 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    (*(uint32_t*)rtguest::G(0x335684)) = iVar10;
    iVar9 = (*(uint32_t*)rtguest::G(0x335680));
  }
  uVar12 = lVar14 << 0xe;
  iVar11 = iVar10 * 0x10 + iVar9;
  uVar7 = (uint32_t)(param_4 << 1);
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x20))) = uVar12 | 0x37007000073;
  iVar13 = (param_2 << 0x11) >> 0x10;
  if (iVar10 < 0x9fc) {
    uiSpriteWrite(iVar11 + 0x70,sVar1,iVar13,lVar14,0xe8,uVar6,uVar7);
    iVar10 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    (*(uint32_t*)rtguest::G(0x335684)) = iVar10;
    iVar9 = (*(uint32_t*)rtguest::G(0x335680));
  }
  iVar11 = iVar10 * 0x10 + iVar9;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x20))) = uVar12 | 0x3a00700007f;
  if (iVar10 < 0x9fc) {
    uiSpriteWrite(iVar11 + 0x70,sVar4,iVar13,uVar16,0xe8,0xc,uVar7);
    iVar10 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    (*(uint32_t*)rtguest::G(0x335684)) = iVar10;
    iVar9 = (*(uint32_t*)rtguest::G(0x335680));
  }
  iVar11 = iVar10 * 0x10 + iVar9;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x20))) = (uVar16 & 0xffff) << 0xe | 0x3a00700007c;
  if (iVar10 < 0x9fc) {
    uiSpriteWrite(iVar11 + 0x70,(short)(param_1 + param_3),iVar13,(long)iVar2,0xe8,0xc,uVar7);
    iVar10 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    (*(uint32_t*)rtguest::G(0x335684)) = iVar10;
    iVar9 = (*(uint32_t*)rtguest::G(0x335680));
  }
  param_2 = param_2 + ((param_4 << 1) >> 1);
  iVar11 = iVar10 * 0x10 + iVar9;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x20))) = (long)iVar2 << 0xe | 0x3a00700007c;
  if (iVar10 < 0x9fc) {
    uiSpriteWrite(iVar11 + 0x70,sVar1,param_2 * 0x20000 >> 0x10,lVar14,0xf0,uVar6,0xc);
    iVar10 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    (*(uint32_t*)rtguest::G(0x335684)) = iVar10;
    iVar9 = (*(uint32_t*)rtguest::G(0x335680));
  }
  iVar9 = iVar10 * 0x10 + iVar9;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) = uVar12 | 0x3c007000073;
  iVar11 = (int)(short)param_2;
  if (iVar10 < 0x9fc) {
    uiSpriteWrite(iVar9 + 0x70,sVar4,(iVar11 << 0x11) >> 0x10,iVar15,0xf0,0xc,0xc);
    iVar10 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    (*(uint32_t*)rtguest::G(0x335684)) = iVar10;
  }
  if (iVar10 < 0x9fc) {
    uiSpriteWrite((*(uint32_t*)rtguest::G(0x335680)) + iVar10 * 0x10 + 0x70,iVar3,(iVar11 << 0x11) >> 0x10,iVar2,0xf0,0xc,
                  0xc);
    (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
  }
  return 0;
}


RT_GUEST_FN(0x00202350, guest::uiDrawPanel9)

}  // namespace guest
