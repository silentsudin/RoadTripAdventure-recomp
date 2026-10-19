// guest:0x00252880 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawGolfHoleIntro(uint32_t param_1, uint32_t param_2, ...) {
    uint32_t uStack_100 = rtguest::allocFrame(8);
    uint32_t uStack_110 = rtguest::allocFrame(8);
    uint32_t uStack_118 = rtguest::allocFrame(8);
    uint32_t uStack_120 = rtguest::allocFrame(8);
    uint32_t uStack_128 = rtguest::allocFrame(8);
    uint32_t uStack_130 = rtguest::allocFrame(8);
    uint32_t uStack_d8 = rtguest::allocFrame(8);
    uint32_t uStack_e0 = rtguest::allocFrame(8);
    uint32_t uStack_e8 = rtguest::allocFrame(8);
    uint32_t uStack_f0 = rtguest::allocFrame(8);
    uint32_t uStack_f8 = rtguest::allocFrame(8);
  byte bVar1;
  int iVar2;
  int iVar3;
  short sVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
uint32_t auStack_d0 = rtguest::allocFrame(128);
  
  iVar8 = 0;
  iVar7 = (int)param_1;
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x1c)));
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8008)));
  uiArenaInit(iVar2,0x8000);
  uiArenaBeginBatch(iVar2,0,0);
  uiArenaSetTexture(iVar2,0x2ac330,1,0);
  uiArenaDataTag(iVar2,1);
  (*(uint64_t*)rtguest::G((uint32_t)uStack_130)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_128)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_120)) = 0x1c000000280;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_118)) = 0x4000000040;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0;
  uiArenaSpritesWrap(iVar2,uStack_130,1,0,0,1);
  uiDrawPanel9(0x154,0x2c,0x104,0x90,0);
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_d0)), (char *)rtguest::G((uint32_t)(0x3344f0)), iVar3 + 1);
  DrawTextHalfY(0x168,100,auStack_d0,0);
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_d0)), (char *)rtguest::G((uint32_t)(0x3344f8)), (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 * 4 + iVar2 + 0x8034))));
  DrawTextHalfY(0x168,0x7c,auStack_d0,0);
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_d0)), (char *)rtguest::G((uint32_t)(0x334500)), (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 * 4 + iVar2 + 0x800c))));
  DrawTextHalfY(0x168,0x94,auStack_d0,0);
  iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(0x2ac458u + (uint32_t)(iVar3) * 4)))));
  if (iVar5 != 0) {
    iVar6 = 0xc4;
    do {
      DrawTextHalfY(0x168,iVar6,iVar5,0);
      iVar8 = iVar8 + 1;
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(0x2ac458u + (uint32_t)(iVar3) * 4))) + iVar8 * 4)));
      iVar6 = iVar6 + 0x18;
    } while (iVar5 != 0);
  }
  uiArenaSetTexture(iVar2,0x2ac330,0,0);
  uiArenaDataTag(iVar2,1);
  (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x2200000032;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x2800000100;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  uiArenaSprites(iVar2,uStack_100,1,0,0,1);
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e0)) = (long)(iVar3 * 0x134 + 0x31ec) | 0x21310000U | (long)(iVar3 * 0x134 + 0x331c) << 0x25;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_d8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_e0)) | 0x2000000e40000000;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_e0)) | 0xe40000000;
  uiArenaSetTexture(iVar2,uStack_e0,0,0);
  uiArenaDataTag(iVar2,1);
  (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x500000004e;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x140000000c0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  uiArenaSprites(iVar2,uStack_100,1,0,0,1);
  uiArenaSubmit(iVar2);
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25)));
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return 0;
      }
      sVar4 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 2))) + 1;
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 2))) = sVar4;
      if (sVar4 < 0x11) {
      }
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 2))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
      gs_tasks_kill(param_1);
    }
    if (bVar1 != 0) {
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 1;
  }
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0x50) != 0) {
    PlayUiSound(0x1a);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 2;
  }
}


RT_GUEST_FN(0x00252880, guest::DrawGolfHoleIntro)

}  // namespace guest
