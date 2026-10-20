// guest:0x00247e38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawTwoPlayerResults(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
  }
  byte bVar1;
  char cVar2;
  undefined8 uVar3;
uint32_t pbVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  
  RT_CALLRA(0x247e74, guest::uiDrawPanelCells(1,10,0x1d,0x11,0));
  uVar3 = RT_CALLRA(0x247e80, guest::GetSlotDataPtr(0,0));
  RT_CALLRA(0x247e94, guest::uiDrawTextCell(2,0xb,uVar3,5));
  uVar3 = RT_CALLRA(0x247ea0, guest::GetSlotDataPtr(0,1));
  RT_CALLRA(0x247eb4, guest::uiDrawTextCell(0x14,0xb,uVar3,5));
  RT_CALLRA(0x247ec0, guest::DrawCarouselStatBars((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))),param_2));
  iVar7 = (int)param_2;
  if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 3))) & 4) == 0) {
    cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 2)));
  }
  else {
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    RT_CALLRA(0x247ee8, guest::uiDrawTextCell(0xb,0xd,0x3276b0,0));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 10))) == '\x01') {
      RT_CALLRA(0x247f20, guest::uiDrawTime(0x1d6,0xa1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017802dc + iVar5 * 0xc))),0));
      cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 2)));
    }
    else {
      iVar5 = iVar5 * 0x20;
      RT_CALLRA(0x247f50, guest::uiDrawTime(0x5a,0xa1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801b4 + iVar5))),0));
      RT_CALLRA(0x247f64, guest::uiDrawTime(0x1c2,0xa1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835fc + iVar5))),0));
      cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 2)));
    }
  }
  if ('\x01' < cVar2) {
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    RT_CALLRA(0x247fa0, guest::uiDrawTextCell(0xb,0xe,0x3276c8,0));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 10))) == '\x01') {
      RT_CALLRA(0x247fd8, guest::uiDrawTime(0x1d6,0xad,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017802d8 + iVar5 * 0xc))),0));
    }
    else {
      iVar5 = iVar5 * 0x20;
      RT_CALLRA(0x248004, guest::uiDrawTime(0x5a,0xad,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017801b0 + iVar5))),0));
      RT_CALLRA(0x248018, guest::uiDrawTime(0x1c2,0xad,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017835f8 + iVar5))),0));
    }
  }
  RT_CALLRA(0x248034, guest::uiDrawTextCell(0xb,0xf,0x3276f0,0));
  RT_CALLRA(0x24804c, guest::uiDrawTextCell(0xb,0x10,0x327700,0));
  uiDrawNumber16(0x96,0xb9,
                 (*(uint8_t*)rtguest::G((uint32_t)0x177fdf4 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x1c))) * 0x30) * 1)),0);
  uiDrawNumber16(0x1fe,0xb9,
                 (*(uint32_t*)rtguest::G((uint32_t)0x178323c + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x1e))) * 0x30) * 4)),0);
  iVar6 = 0;
  iVar5 = 9;
  pbVar4 = 0x0177fdf4 + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x1c))) * 0x30;
  do {
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar4));
    pbVar4 = pbVar4 + 1;
    iVar5 = iVar5 + -1;
    iVar6 = iVar6 + (uint32_t)bVar1;
  } while (-1 < iVar5);
  uiDrawNumber16(0x96,0xc5,iVar6,0);
  iVar6 = 0;
  iVar5 = 9;
  pbVar4 = 0x0178323c + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x1e))) * 0x30;
  do {
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar4));
    pbVar4 = pbVar4 + 1;
    iVar5 = iVar5 + -1;
    iVar6 = iVar6 + (uint32_t)bVar1;
  } while (-1 < iVar5);
  uiDrawNumber16(0x1fe,0xc5,iVar6,0);
  return 0;
}


RT_GUEST_FN(0x00247e38, guest::DrawTwoPlayerResults)

}  // namespace guest
