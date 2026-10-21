// guest:0x002477a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawCarouselStatText(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
    rtguest::setS32(3, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(4, (uint32_t)param_2);  // walk: = param_2
  }
uint32_t puVar1;
  int iVar2;
  byte bVar3;
  undefined8 uVar4;
uint32_t ppuVar5;
  
  RT_CALLRA(0x2477dc, guest::uiDrawPanelCells(0xf,2,0x1d,9,0));
  bVar3 = (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int8_t)));
  if ((bVar3 & 1) != 0) {
    RT_CALLRA(0x247808, guest::uiDrawTextCell(0x10,3,0x334460,0));
    iVar2 = RT_CALLRA(0x247848, guest::uiDrawNumberFixed(0x212,0x54,(int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x002a8f30 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)param_1)) * 2))) % 1000,
                              0,3));
    RT_CALLRA(0x247864, guest::uiDrawText(iVar2 + -10,0x29,0x334468,0));
    RT_CALLRA(0x24789c, guest::uiDrawNumber16(iVar2 + -10,0x29,(int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x002a8f30 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)param_1)) * 2))) / 1000,0));
    RT_CALLRA(0x2478b4, guest::uiDrawTextCell(0x1a,3,0x334470,0));
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int8_t)));
  }
  if ((bVar3 & 2) != 0) {
    RT_CALLRA(0x2478d4, guest::uiDrawNumber16(0x212,0x35,(*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int8_t))),0));
    RT_CALLRA(0x2478ec, guest::uiDrawTextCell(0x1a,4,0x334478,0));
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 10))) == '\x01') {
    RT_CALLRA(0x247914, guest::uiDrawTextCell(0x10,5,0x334480,0));
    RT_CALLRA(0x247928, guest::uiDrawNumber16(0x212,0x41,(*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int8_t))),0));
    RT_CALLRA(0x247940, guest::uiDrawTextCell(0x1a,5,0x334488,0));
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int8_t)));
  }
  else {
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int8_t)));
  }
  if ((bVar3 & 8) != 0) {
    if ((*(int8_t*)rtguest::G(0x177fdd6)) == '\0') {
      bVar3 = (*(int8_t*)rtguest::G((uint32_t)param_1));
      goto LAB_0024799c;
    }
    if ((*(uint32_t*)rtguest::G(0x178321e)) == '\0') {
      bVar3 = (*(int8_t*)rtguest::G((uint32_t)param_1));
      goto LAB_0024799c;
    }
    uVar4 = 6;
    if (((*(uint64_t*)rtguest::G((uint32_t)param_2)) & 0x10) != 0) {
      uVar4 = 0;
    }
    RT_CALLRA(0x247990, guest::uiDrawTextCell(0x13,6,0x327688,uVar4));
  }
  bVar3 = (*(int8_t*)rtguest::G((uint32_t)param_1));
LAB_0024799c:
  if (bVar3 == 0x1a) {
    iVar2 = 0;
    ppuVar5 = 0x2a8f84u;
    puVar1 = (*(uint32_t*)rtguest::G(0x002a8f84));
    while( true ) {
      ppuVar5 = ppuVar5 + 1;
      if (puVar1 != (uint32_t)0x0) {
        RT_CALLRA(0x2479fc, guest::uiDrawTextCell(0x10,iVar2 + 4,puVar1,5));
      }
      iVar2 = iVar2 + 1;
      if (4 < iVar2) break;
      puVar1 = (*(uint32_t*)rtguest::G((uint32_t)ppuVar5));
    }
  }
  else if (bVar3 < 0x1b) {
    if (bVar3 == 0x13) {
      iVar2 = 0;
      ppuVar5 = 0x2a8f70u;
      puVar1 = (*(uint32_t*)rtguest::G(0x002a8f70));
      while( true ) {
        ppuVar5 = ppuVar5 + 1;
        if (puVar1 != (uint32_t)0x0) {
          RT_CALLRA(0x247a44, guest::uiDrawTextCell(0x10,iVar2 + 4,puVar1,5));
        }
        iVar2 = iVar2 + 1;
        if (4 < iVar2) break;
        puVar1 = (*(uint32_t*)rtguest::G((uint32_t)ppuVar5));
      }
    }
  }
  else if (bVar3 == 0x1d) {
    iVar2 = 0;
    ppuVar5 = 0x2a8f98u;
    puVar1 = (*(uint32_t*)rtguest::G(0x002a8f98));
    while( true ) {
      ppuVar5 = ppuVar5 + 1;
      if (puVar1 != (uint32_t)0x0) {
        RT_CALLRA(0x247a8c, guest::uiDrawTextCell(0x10,iVar2 + 4,puVar1,5));
      }
      iVar2 = iVar2 + 1;
      if (4 < iVar2) break;
      puVar1 = (*(uint32_t*)rtguest::G((uint32_t)ppuVar5));
    }
  }
  return 0;
}


RT_GUEST_FN(0x002477a8, guest::DrawCarouselStatText)

}  // namespace guest
