// guest:0x0024e1a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
// UpdateCarControlState: boost gauge / tach / speedometer / transmission / gear HUD arena.
// Ghidra's aligned-store dance before each `auStack_68 = 0` zeroes the same 8-byte
// slot (aligned base == auStack_68, mask = 0); transliterated as the zero store.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateCarControlState(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x68, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = (uint32_t)(int32_t)param_6;
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x58, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x70, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x78) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x7c) = 0x00000000u;
    rtguest::gS[0] = 0x0000000000327f70ull;  // walk const
    rtguest::setS32(2, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(6, (uint32_t)param_1);  // walk: = param_1
  }
  uint32_t uVar2;
  uint32_t uVar3;
  uint32_t uVar4;
  int32_t lVar5;
  uint32_t uVar6;
  int32_t iVar7;
  int32_t iVar8;
  uint32_t puVar9;
  uint32_t uStack_80 = rtguest::allocFrame(8);
  uint32_t uStack_78 = rtguest::allocFrame(8);
  uint32_t uStack_70 = rtguest::allocFrame(8);
  uint32_t auStack_68 = rtguest::allocFrame(8);
  uint32_t iStack_60 = rtguest::allocFrame(4);

  iVar7 = (int32_t)((int32_t)((int32_t)param_4 + (int32_t)param_5) * 8);
  puVar9 = (uint32_t)0x177AC50 + (uint32_t)param_5 * 0x4e;  // gs_controls_car_object
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)((int32_t)iVar7 + (int32_t)0x2a9af0)));
  uVar3 = (*(uint32_t*)rtguest::G((uint32_t)((int32_t)iVar7 + (int32_t)0x2a9af4)));
  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = (int32_t)param_6;
  RT_CALLRA(0x24e224, guest::uiArenaInit(param_3, 0x1000));
  RT_CALLRA(0x24e23c, guest::uiArenaBeginBatch(param_3, 0, 0));
  RT_CALLRA(0x24e250, guest::uiArenaSetTexture(param_3, 0x2a9b08, 2, 0));
  RT_CALLRA(0x24e25c, guest::uiArenaDataTag(param_3, 1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x4c00000007;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1200000064;
  (*(uint64_t*)rtguest::G((uint32_t)auStack_68)) = 0;
  RT_CALLRA(0x24e2b8, guest::uiArenaSprites(param_3, uStack_80, 1, uVar2, uVar3, 1));
  lVar5 = (int32_t)RT_CALLRA(0x24e2c8, guest::CalculateBoostGaugeValue(puVar9, 0xf, 1));
  RT_CALLRA(0x24e2e4, guest::uiArenaSetTexture(param_3, 0x2a9b08, 1, 0xf - (int32_t)lVar5));
  RT_CALLRA(0x24e2f0, guest::uiArenaDataTag(param_3, 1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x4c00000007;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1200000064;
  (*(uint64_t*)rtguest::G((uint32_t)auStack_68)) = 0;
  RT_CALLRA(0x24e34c, guest::uiArenaSprites(param_3, uStack_80, 1, uVar2, uVar3, 1));
  if (lVar5 == 0) {
    RT_CALLRA(0x24e384, guest::uiArenaSetTexture(param_3, 0x2a9b08, 2,
                      ((*(int32_t*)rtguest::G((uint32_t)param_1 + 8)) >> 3) % 3 + 1));
    RT_CALLRA(0x24e390, guest::uiArenaDataTag(param_3, 1));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x520000001e;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x1800000000;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x80000003a;
    (*(uint64_t*)rtguest::G((uint32_t)auStack_68)) = 0;
    RT_CALLRA(0x24e3f4, guest::uiArenaSprites(param_3, uStack_80, 1, uVar2, uVar3, 1));
  }
  RT_CALLRA(0x24e408, guest::uiArenaSetTexture(param_3, 0x2a9b08, 0, 0));
  RT_CALLRA(0x24e414, guest::uiArenaDataTag(param_3, 1));
  ::memset(rtguest::G(uStack_80), 0, 0x20);
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x6000000084;
  RT_CALLRA(0x24e450, guest::uiArenaSprites(param_3, uStack_80, 1, uVar2, uVar3, 1));
  iVar7 = (int32_t)RT_CALLRA(0x24e460, guest::CalculateTachometerValue(puVar9, 0x10, 1));
  if (iVar7 != 0) {
    RT_CALLRA(0x24e47c, guest::uiArenaSetTexture(param_3, 0x2a9b08, 0, 0));
    RT_CALLRA(0x24e488, guest::uiArenaDataTag(param_3, (uint32_t)iVar7));
    if (0 < iVar7) {
      uVar4 = 0x22;
      iVar8 = 0;
      do {
        iVar7 = iVar7 + -1;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = (uint64_t)uVar4;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = (uint64_t)(iVar8 + 0x88);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x3c00000004;
        (*(uint64_t*)rtguest::G((uint32_t)auStack_68)) = 0;
        RT_CALLRA(0x24e4f0, guest::uiArenaSprites(param_3, uStack_80, 1, uVar2, uVar3, 1));
        uVar4 = (uint32_t)(iVar8 + 0x28);
        iVar8 = iVar8 + 6;
      } while (iVar7 != 0);
    }
  }
  iVar8 = 0;
  uVar4 = RT_CALLRA(0x24e504, guest::CalculateSpeedometerDigits(puVar9));
  iVar7 = 0x4e;
  do {
    RT_CALLRA(0x24e530, guest::uiArenaSetTexture(param_3, 0x2a9b08, 0, 0));
    RT_CALLRA(0x24e53c, guest::uiArenaDataTag(param_3, 1));
    uVar6 = uVar4 & 0xf;
    uVar4 = (uint32_t)((int32_t)uVar4 >> 4);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) =
         ((uint64_t)(uint32_t)0x31 << 32) | (uint32_t)iVar7;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) =
         ((uint64_t)(uint32_t)0x66 << 32) | (uint32_t)(uVar6 * 0x14);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1a00000014;
    (*(uint64_t*)rtguest::G((uint32_t)auStack_68)) = 0;
    RT_CALLRA(0x24e59c, guest::uiArenaSprites(param_3, uStack_80, 1, uVar2, uVar3, 1));
    if (uVar4 == 0) break;
    iVar8 = iVar8 + 1;
    iVar7 = iVar7 + -0x13;
  } while (iVar8 < 3);
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_60)) != 0) {
    lVar5 = (int32_t)RT_CALLRA(0x24e5cc, guest::GetVehicleTransmissionMode(puVar9, 0, 0));
    if (lVar5 == 1) {
      RT_CALLRA(0x24e60c, guest::uiArenaSetTexture(param_3, 0x2a9b08, 0, 0));
      RT_CALLRA(0x24e618, guest::uiArenaDataTag(param_3, 1));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0xc00000009;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0xf0;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1000000010;
      (*(uint64_t*)rtguest::G((uint32_t)auStack_68)) = 0;
      RT_CALLRA(0x24e67c, guest::uiArenaSprites(param_3, uStack_80, 1, uVar2, uVar3, 1));
    }
    else if ((1 < lVar5) || (lVar5 != 0)) {
      RT_CALLRA(0x24e69c, guest::uiArenaSetTexture(param_3, 0x2a9b08, 0, 0));
      RT_CALLRA(0x24e6a8, guest::uiArenaDataTag(param_3, 1));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x2400000009;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x18000000f0;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1000000010;
      (*(uint64_t*)rtguest::G((uint32_t)auStack_68)) = 0;
      RT_CALLRA(0x24e70c, guest::uiArenaSprites(param_3, uStack_80, 1, uVar2, uVar3, 1));
    }
  }
  lVar5 = (int32_t)RT_CALLRA(0x24e71c, guest::GetVehicleGear(puVar9, 0, 1));
  RT_CALLRA(0x24e73c, guest::uiArenaSetTexture(param_3, 0x2a9b08, 2, 4));
  RT_CALLRA(0x24e748, guest::uiArenaDataTag(param_3, 1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) =
       ((uint64_t)(uint32_t)0x20 << 32) |
       (uint32_t)(((uint32_t)(lVar5 != 0) + (uint32_t)lVar5) * 0x10);
  (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0xa00000010;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x3e0000000a;
  (*(uint64_t*)rtguest::G((uint32_t)auStack_68)) = 0;
  RT_CALLRA(0x24e798, guest::uiArenaSprites(param_3, uStack_80, 1, uVar2, uVar3, 1));
  RT_CALLRA(0x24e7a0, guest::uiArenaSubmit(param_3));
  return 0;
}


RT_GUEST_FN(0x0024e1a0, guest::UpdateCarControlState)

}  // namespace guest
