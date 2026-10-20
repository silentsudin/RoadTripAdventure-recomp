// guest:0x0025b5a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawTextEntryGrid(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
  }
    uint32_t bStack_50 = rtguest::allocFrame(1);
    uint32_t bStack_4f = rtguest::allocFrame(1);
  byte bVar1;
  int iVar2;
  undefined8 uVar3;
  undefined4 uVar4;
  uint uVar5;
uint32_t puVar6;
  uint uVar7;
  
  uVar7 = 0x138;
  bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26)));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) == 2) {
    uVar7 = 0xa8;
  }
  RT_CALLRA(0x25b5f0, guest::uiDrawPanel9(0x29,uVar7 >> 1,0x22e,0x30,2));
  (*(int8_t*)rtguest::G((uint32_t)bStack_50)) = (*(uint32_t*)rtguest::G((uint32_t)0x2c3b1c + (uint32_t)((uint32_t)bVar1 * 0x10) * 4));
  (*(int8_t*)rtguest::G((uint32_t)bStack_4f)) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(0x2c3b1du + (uint32_t)bVar1 * 4)));
  if (((*(int8_t*)rtguest::G((uint32_t)bStack_50)) == 0) && ((*(int8_t*)rtguest::G((uint32_t)bStack_4f)) == 0)) {
    (*(int8_t*)rtguest::G((uint32_t)bStack_4f)) = bVar1 / 0x1a;
    (*(int8_t*)rtguest::G((uint32_t)bStack_50)) = bVar1 % 0x1a;
  }
  uVar5 = 0;
  uVar3 = RT_CALLRA(0x25b678, guest::uiTextWidth((*(uint32_t*)rtguest::G((uint32_t)(0x2c3b10u + (uint32_t)((uint32_t)bVar1 * 4) * 4)))));
  RT_CALLRA(0x25b6b8, guest::uiDrawBar3(((uint32_t)(*(int8_t*)rtguest::G((uint32_t)bStack_50)) * 0x12 - (int)uVar3 / 2) + 0x32,uVar7 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)bStack_4f)) * 0x18,uVar3));
  iVar2 = 0;
  do {
    puVar6 = (uint32_t)((int)0x2c3b10u + iVar2);
    (*(int8_t*)rtguest::G((uint32_t)bStack_50)) = (*(uint32_t*)rtguest::G((uint32_t)0x2c3b1c + (uint32_t)(iVar2) * 4));
    (*(int8_t*)rtguest::G((uint32_t)bStack_4f)) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2c3b1du + iVar2)));
    if ((*(int8_t*)rtguest::G((uint32_t)bStack_50)) == 0) {
      if ((*(int8_t*)rtguest::G((uint32_t)bStack_4f)) == 0) {
        (*(int8_t*)rtguest::G((uint32_t)bStack_50)) = (uint32_t)((int)uVar5 % 0x1a);
        (*(int8_t*)rtguest::G((uint32_t)bStack_4f)) = (uint32_t)((int)uVar5 / 0x1a);
        uVar4 = (*(uint32_t*)rtguest::G((uint32_t)puVar6));
      }
      else {
        uVar4 = (*(uint32_t*)rtguest::G((uint32_t)puVar6));
      }
    }
    else {
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)puVar6));
    }
    uVar5 = uVar5 + 1;
    iVar2 = RT_CALLRA(0x25b718, guest::uiTextWidth(uVar4));
    RT_CALLRA(0x25b760, guest::uiDrawText(((uint32_t)(*(int8_t*)rtguest::G((uint32_t)bStack_50)) * 0x12 - iVar2 / 2) + 0x32,uVar7 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)bStack_4f)) * 0x18 >> 1,
               (*(uint32_t*)rtguest::G((uint32_t)puVar6)),10));
    iVar2 = uVar5 * 0x10;
  } while (uVar5 < 0x5c);
  return 0;
}


RT_GUEST_FN(0x0025b5a0, guest::drawTextEntryGrid)

}  // namespace guest
