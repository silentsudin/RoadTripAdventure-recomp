// guest:0x0025b0c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceResultPanelTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(272);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0xe0, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0xe8, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0xf0, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0xf8, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x100, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0xd0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0xd8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x108) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x10c) = 0x00000000u;
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(6, (uint32_t)param_2);  // walk: = param_2
  }
    uint32_t uStack_110 = rtguest::allocFrame(8);
    uint32_t uStack_108 = rtguest::allocFrame(8);
    uint32_t uStack_100 = rtguest::allocFrame(8);
    uint32_t uStack_f8 = rtguest::allocFrame(8);
    uint32_t uStack_f0 = rtguest::allocFrame(8);
    uint32_t uStack_e8 = rtguest::allocFrame(8);
    uint32_t uStack_e0 = rtguest::allocFrame(8);
    uint32_t uStack_d8 = rtguest::allocFrame(8);
    uint32_t uStack_d0 = rtguest::allocFrame(8);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_c0 = rtguest::allocFrame(128);
  byte bVar1;
  int iVar2;
  int iVar3;
  short sVar4;
  undefined8 uVar5;
uint32_t piVar6;
  int iVar7;
  int iVar8;
  
  iVar7 = (int)param_1;
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x1c)));
  iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x80a8)));
  RT_CALLRA(0x25b100, guest::uiArenaInit(iVar2,0x8000));
  iVar8 = iVar8 * 4;
  RT_CALLRA(0x25b120, guest::uiArenaBeginBatch(iVar2,0,0));
  RT_CALLRA(0x25b134, guest::uiArenaSetTexture(iVar2,0x2c3a48,1,0));
  RT_CALLRA(0x25b140, guest::uiArenaDataTag(iVar2,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e0)) = 0x1c000000280;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_d8)) = 0x4000000040;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_d0)) = 0;
  RT_CALLRA(0x25b1b8, guest::uiArenaSpritesWrap(iVar2,uStack_f0,1,0,0,1));
  RT_CALLRA(0x25b1cc, guest::uiArenaSetTexture(iVar2,0x2c3a48,0,0));
  RT_CALLRA(0x25b1d8, guest::uiArenaDataTag(iVar2,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0x2200000032;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_108)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x2800000100;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0;
  RT_CALLRA(0x25b23c, guest::uiArenaSprites(iVar2,uStack_110,1,0,0,1));
  RT_CALLRA(0x25b244, guest::uiArenaSubmit(iVar2));
  RT_CALLRA(0x25b25c, guest::uiDrawPanel9(100,0x38,0x1b8,0x54,0));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_c0)), (char *)rtguest::G((uint32_t)(0x334b90)), (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x80a8))) + 1);
  RT_CALLRA(0x25b294, guest::DrawTextHalfY(0x78,0x70,auStack_c0,0));
  piVar6 = (uint32_t)(iVar2 + 0xc + iVar8 + 0x80a0);
  if ((*(int32_t*)rtguest::G((uint32_t)piVar6)) == 0) {
    RT_CALLRA(0x25b2c4, guest::DrawTextHalfY(0x78,0xa0,0x329068,6));
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25)));
  }
  else {
    RT_CALLRA(0x25b2e4, guest::DrawTextHalfY(0x78,0xa0,0x334b98,0));
    RT_CALLRA(0x25b2f8, guest::uiDrawTime(300,0x50,(*(int32_t*)rtguest::G((uint32_t)piVar6)),0));
    RT_CALLRA(0x25b310, guest::DrawTextHalfY(0x78,0xb8,0x329078,0));
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + iVar2 + 0x80c0)));
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_c0)), (char *)rtguest::G((uint32_t)(0x329088)), iVar3, iVar3 * 5);
    RT_CALLRA(0x25b350, guest::DrawTextHalfY(300,0xb8,auStack_c0,0));
    RT_CALLRA(0x25b368, guest::DrawTextHalfY(0x78,0xd0,0x3290a0,0));
    piVar6 = (uint32_t)(iVar8 + iVar2 + 33000);
    uVar5 = 0;
    if (((*(int32_t*)rtguest::G((uint32_t)piVar6)) != 0) && (uVar5 = 6, ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) & 0x20) == 0)) {
      uVar5 = 0;
    }
    RT_CALLRA(0x25b3ac, guest::uiDrawTime(300,0x68,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + iVar2 + 0x80d4))),uVar5));
    RT_CALLRA(0x25b3c4, guest::DrawTextHalfY(0x78,0xe8,0x3290b0,0));
    uVar5 = 0;
    if (((*(int32_t*)rtguest::G((uint32_t)piVar6)) != 0) && (uVar5 = 6, ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) & 0x20) == 0)) {
      uVar5 = 0;
    }
    RT_CALLRA(0x25b3fc, guest::uiDrawTime(300,0x74,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + iVar2 + 0xc + 0x80f0))),uVar5));
    RT_CALLRA(0x25b414, guest::DrawTextHalfY(0x78,0x100,0x3290c0,0));
    RT_CALLRA(0x25b434, guest::uiDrawTime(300,0x80,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + iVar2 + 0x8110))),0));
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25)));
  }
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return 0;
      }
      sVar4 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 2))) + 1;
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 2))) = sVar4;
      if (sVar4 < 0x11) {
        return 0;
      }
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 2))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 0xffffffff;
      RT_CALLRA(0x25b4d0, guest::gs_tasks_kill(param_1));
      return 0;
    }
    if (bVar1 != 0) {
      return 0;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 1;
  }
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0x50) != 0) {
    RT_CALLRA(0x25b48c, guest::PlayUiSound(0x1a));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 2;
  }
  return 0;
}


RT_GUEST_FN(0x0025b0c0, guest::raceResultPanelTask)

}  // namespace guest
