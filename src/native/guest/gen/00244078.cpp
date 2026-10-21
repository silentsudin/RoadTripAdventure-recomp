// guest:0x00244078 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t LayoutRaceRowQuads(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x30, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[1] = 0x0000000000000000ull;  // walk const
    rtguest::setS32(3, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[4] = 0x0000000000000000ull;  // walk const
    rtguest::setS32(7, (uint32_t)param_2);  // walk: = param_2
  }
uint32_t puVar1;
uint32_t puVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
uint32_t pbVar9;
uint32_t pbVar10;
  
  iVar6 = 0;
  iVar8 = 0;
  iVar7 = param_1 + 0x20;
  RT_CALLRA(0x2440d0, guest::gifSetTexRegs(param_1,0x2003df66a1411d43,5));
  pbVar10 = 0x2be1d8;
  pbVar9 = 0x2be158;
  do {
    if (iVar6 < 10) {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) == '\x01') {
        uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar9));
      }
      else {
        uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar10));
      }
    }
    else {
      uVar5 = 0x19;
      if (iVar6 != 0xc) {
        uVar5 = 0x1b;
        if (iVar6 != 0xd) {
          uVar5 = iVar6 + 0x10;
        }
      }
    }
    RT_CALLRA(0x244128, guest::gifTexQuadTag(iVar7));
    uVar3 = 0;
    puVar1 = (uint32_t)(param_1 + iVar8);
    do {
      puVar2 = puVar1 + 0xc * sizeof(uint32_t);  // undefined4* +0xc = 48B stride (fn/00244078.c:48; disasm 0x244148 addiu $a1,$a1,0x30)
      if ((uVar3 & 1) == 0) {
        (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0;
      }
      else {
        (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0x3f400000;
      }
      (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(0xe) * sizeof(uint32_t))) = 0x3f800000;
      uVar4 = uVar3 + 1;
      (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(0x17) * sizeof(uint32_t))) = 0xff0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(0xd) * sizeof(uint32_t))) = rtguest::f2bits((float)(int)((uVar5 + (int)uVar3 / 2) * 0x1c) * 0.0009765625f);  // float store: swc1 bits, not value-convert (disasm 0x2441a4 swc1 after mul.s f0,f0,f2; fn:58)
      uVar3 = uVar4;
      puVar1 = puVar2;
    } while ((int)uVar4 < 4);
    iVar6 = iVar6 + 1;
    iVar7 = iVar7 + 0xd0;
    iVar8 = iVar8 + 0x50;
    param_1 = param_1 + 0x80;
    pbVar10 = pbVar10 + 0x10;
    pbVar9 = pbVar9 + 0x10;
  } while (iVar6 < 0x10);
  return 0;
}


RT_GUEST_FN(0x00244078, guest::LayoutRaceRowQuads)

}  // namespace guest
