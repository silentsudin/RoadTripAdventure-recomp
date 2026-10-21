// guest:0x00252880 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawGolfHoleIntro(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(304);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0xe0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0xe8, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0xf8, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x100, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x110, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x118, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0xf0, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x108, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x120) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x124) = 0x00000000u;
    rtguest::setS32(3, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[4] = 0x0000000000000000ull;  // walk const
    rtguest::gS[6] = 0x00000000002b0000ull;  // walk const
    rtguest::setS32(7, (uint32_t)param_2);  // walk: = param_2
  }
    uint32_t uStack_130 = rtguest::allocFrame(8);
    uint32_t uStack_128 = rtguest::allocFrame(8);
    uint32_t uStack_120 = rtguest::allocFrame(8);
    uint32_t uStack_118 = rtguest::allocFrame(8);
    uint32_t uStack_110 = rtguest::allocFrame(8);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_100 = rtguest::allocFrame(8);
    uint32_t uStack_f8 = rtguest::allocFrame(8);
    uint32_t uStack_f0 = rtguest::allocFrame(8);
    uint32_t uStack_e8 = rtguest::allocFrame(8);
    uint32_t uStack_e0 = rtguest::allocFrame(8);
    uint32_t uStack_d8 = rtguest::allocFrame(8);
uint32_t auStack_d0 = rtguest::allocFrame(128);
  byte bVar1;
  int iVar2;
  int iVar3;
  short sVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  
  iVar8 = 0;
  iVar7 = (int)param_1;
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x1c)));
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8008)));
  RT_CALLRA(0x2528cc, guest::uiArenaInit(iVar2,0x8000));
  RT_CALLRA(0x2528dc, guest::uiArenaBeginBatch(iVar2,0,0));
  RT_CALLRA(0x2528f4, guest::uiArenaSetTexture(iVar2,0x2ac330,1,0));
  RT_CALLRA(0x252900, guest::uiArenaDataTag(iVar2,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_130)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_128)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_120)) = 0x1c000000280;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_118)) = 0x4000000040;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0;
  RT_CALLRA(0x252974, guest::uiArenaSpritesWrap(iVar2,uStack_130,1,0,0,1));
  RT_CALLRA(0x25298c, guest::uiDrawPanel9(0x154,0x2c,0x104,0x90,0));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_d0)), (char *)rtguest::G((uint32_t)(0x3344f0)), iVar3 + 1);
  RT_CALLRA(0x2529b8, guest::DrawTextHalfY(0x168,100,auStack_d0,0));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_d0)), (char *)rtguest::G((uint32_t)(0x3344f8)), (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 * 4 + iVar2 + 0x8034))));
  RT_CALLRA(0x2529ec, guest::DrawTextHalfY(0x168,0x7c,auStack_d0,0));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_d0)), (char *)rtguest::G((uint32_t)(0x334500)), (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 * 4 + iVar2 + 0x800c))));
  RT_CALLRA(0x252a20, guest::DrawTextHalfY(0x168,0x94,auStack_d0,0));
  iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(0x2ac458u + (uint32_t)(iVar3) * 4)))));
  if (iVar5 != 0) {
    iVar6 = 0xc4;
    do {
      RT_CALLRA(0x252a58, guest::DrawTextHalfY(0x168,iVar6,iVar5,0));
      iVar8 = iVar8 + 1;
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(0x2ac458u + (uint32_t)(iVar3) * 4))) + iVar8 * 4)));
      iVar6 = iVar6 + 0x18;
    } while (iVar5 != 0);
  }
  RT_CALLRA(0x252a88, guest::uiArenaSetTexture(iVar2,0x2ac330,0,0));
  RT_CALLRA(0x252a94, guest::uiArenaDataTag(iVar2,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x2200000032;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x2800000100;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  RT_CALLRA(0x252afc, guest::uiArenaSprites(iVar2,uStack_100,1,0,0,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e0)) = (long)(iVar3 * 0x134 + 0x31ec) | 0x21310000U | (long)(iVar3 * 0x134 + 0x331c) << 0x25;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_d8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_e0)) | 0x2000000e40000000;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_e0)) | 0xe40000000;
  RT_CALLRA(0x252b5c, guest::uiArenaSetTexture(iVar2,uStack_e0,0,0));
  RT_CALLRA(0x252b68, guest::uiArenaDataTag(iVar2,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x500000004e;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x140000000c0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  RT_CALLRA(0x252bcc, guest::uiArenaSprites(iVar2,uStack_100,1,0,0,1));
  RT_CALLRA(0x252bd4, guest::uiArenaSubmit(iVar2));
  bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25)));
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
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
      RT_CALLRA(0x252c78, guest::gs_tasks_kill(param_1));
      return 0;
    }
    if (bVar1 != 0) {
      return 0;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 1;
  }
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0x50) != 0) {
    RT_CALLRA(0x252c2c, guest::PlayUiSound(0x1a));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 2;
  }
  return 0;
}


RT_GUEST_FN(0x00252880, guest::DrawGolfHoleIntro)

}  // namespace guest
