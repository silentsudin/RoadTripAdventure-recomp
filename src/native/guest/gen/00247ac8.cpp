// guest:0x00247ac8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawCarouselStatBars(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  byte bVar1;
uint32_t pbVar2;
  int iVar3;
  undefined8 uVar4;
  int iVar5;
  
  uVar4 = 0xb;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) != '\x01') {
    uVar4 = 10;
  }
  RT_CALLRA(0x247b0c, guest::uiDrawTextCell(uVar4,0xc,0x327698,0));
  iVar5 = 0;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) == '\x01') {
    uiDrawNumber16(0x226,0x95,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x017802e0 + param_1 * 0xc))),0);
    return 0;
  }
  pbVar2 = 0x0177fdfe + param_1;
  iVar3 = 0x13;
  do {
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar2));
    pbVar2 = pbVar2 + 0x30;
    iVar3 = iVar3 + -1;
    iVar5 = iVar5 + (uint32_t)bVar1;
  } while (-1 < iVar3);
  uiDrawNumber16(0x96,0x95,iVar5,0);
  iVar3 = 0;
  pbVar2 = 0x01783246 + param_1;
  iVar5 = 0x13;
  do {
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar2));
    pbVar2 = pbVar2 + 0x30;
    iVar5 = iVar5 + -1;
    iVar3 = iVar3 + (uint32_t)bVar1;
  } while (-1 < iVar5);
  uiDrawNumber16(0x1fe,0x95,iVar3,0);
  return 0;
}


RT_GUEST_FN(0x00247ac8, guest::DrawCarouselStatBars)

}  // namespace guest
