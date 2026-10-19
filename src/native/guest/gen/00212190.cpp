// guest:0x00212190
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void DrawMenuGridBackground(uint64_t param_1, uint64_t param_2, uint32_t param_3, uint64_t param_4) {
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint32_t uStack_60 = rtguest::allocFrame(0x20);

  iVar5 = (int)param_4 + -1;
  if (param_4 != 0) {
    do {
      if ((*(uint32_t*)rtguest::G(param_3) & 1) != 0) {
        switch (*(uint32_t*)rtguest::G(param_3 + 4 * sizeof(uint32_t))) {
        case 2:
          iVar4 = 0;
          uiArenaSetTexture((uint32_t)param_2, 0x29c030, 5, 0);
          iVar3 = 0;
          uiArenaDataTag((uint32_t)param_2, 0x46);
          *(uint64_t*)rtguest::G(uStack_60) = 0;
          *(uint64_t*)rtguest::G(uStack_60 + 8) = 0x8000000080;
          *(uint64_t*)rtguest::G(uStack_60 + 0x10) = 0x4000000040;
          *(uint64_t*)rtguest::G(uStack_60 + 0x18) = 0;
          do {
            iVar2 = 0;
            iVar1 = 9;
            do {
              uiArenaSprites((uint32_t)param_2, uStack_60, 1, *(uint32_t*)rtguest::G(param_3 + 4), *(uint32_t*)rtguest::G(param_3 + 8) + iVar3, *(uint32_t*)rtguest::G(param_3 + 0xc));
              iVar1 = iVar1 + -1;
              iVar2 = iVar2 + (int)*(uint64_t*)rtguest::G(uStack_60 + 0x10);
            } while (-1 < iVar1);
            iVar4 = iVar4 + 1;
            iVar3 = iVar3 + (int)(*(uint64_t*)rtguest::G(uStack_60 + 0x10) >> 32);
          } while (iVar4 < 7);
          break;
        case 3:
          iVar4 = 0;
          uiArenaSetTexture((uint32_t)param_2, 0x29c030, 3, 5);
          iVar3 = 0;
          uiArenaDataTag((uint32_t)param_2, 0x46);
          *(uint64_t*)rtguest::G(uStack_60) = 0;
          *(uint64_t*)rtguest::G(uStack_60 + 8) = 0x30000000c0;
          *(uint64_t*)rtguest::G(uStack_60 + 0x10) = 0x4200000040;
          *(uint64_t*)rtguest::G(uStack_60 + 0x18) = 0;
          do {
            iVar2 = 0;
            iVar1 = 9;
            do {
              uiArenaSprites((uint32_t)param_2, uStack_60, 1, *(uint32_t*)rtguest::G(param_3 + 4) + iVar2, *(uint32_t*)rtguest::G(param_3 + 8) + iVar3, *(uint32_t*)rtguest::G(param_3 + 0xc));
              iVar1 = iVar1 + -1;
              iVar2 = iVar2 + (int)*(uint64_t*)rtguest::G(uStack_60 + 0x10);
            } while (-1 < iVar1);
            iVar4 = iVar4 + 1;
            iVar3 = iVar3 + (int)(*(uint64_t*)rtguest::G(uStack_60 + 0x10) >> 32);
          } while (iVar4 < 7);
          break;
        case 4:
          iVar4 = 0;
          uiArenaSetTexture((uint32_t)param_2, 0x29c030, 3, 5);
          iVar3 = 0;
          uiArenaDataTag((uint32_t)param_2, 0x8c);
          *(uint64_t*)rtguest::G(uStack_60) = 0;
          *(uint64_t*)rtguest::G(uStack_60 + 8) = 0x3000000090;
          *(uint64_t*)rtguest::G(uStack_60 + 0x10) = 0x3000000030;
          *(uint64_t*)rtguest::G(uStack_60 + 0x18) = 0;
          do {
            iVar2 = 0;
            iVar1 = 0xd;
            do {
              uiArenaSprites((uint32_t)param_2, uStack_60, 1, *(uint32_t*)rtguest::G(param_3 + 4) + iVar2, *(uint32_t*)rtguest::G(param_3 + 8) + iVar3, *(uint32_t*)rtguest::G(param_3 + 0xc));
              iVar1 = iVar1 + -1;
              iVar2 = iVar2 + (int)*(uint64_t*)rtguest::G(uStack_60 + 0x10);
            } while (-1 < iVar1);
            iVar4 = iVar4 + 1;
            iVar3 = iVar3 + (int)(*(uint64_t*)rtguest::G(uStack_60 + 0x10) >> 32);
          } while (iVar4 < 10);
          break;
        case 5:
          iVar4 = 0;
          uiArenaSetTexture((uint32_t)param_2, 0x29c030, 4, 0);
          iVar3 = 0;
          uiArenaDataTag((uint32_t)param_2, 0x14);
          ::memset(rtguest::G(uStack_60), 0, 0x20);
          *(uint64_t*)rtguest::G(uStack_60 + 0x10) = 0x8000000080;
          do {
            iVar2 = 0;
            iVar1 = 4;
            do {
              uiArenaSprites((uint32_t)param_2, uStack_60, 1, *(uint32_t*)rtguest::G(param_3 + 4) + iVar2, *(uint32_t*)rtguest::G(param_3 + 8) + iVar3, *(uint32_t*)rtguest::G(param_3 + 0xc));
              iVar1 = iVar1 + -1;
              iVar2 = iVar2 + (int)*(uint64_t*)rtguest::G(uStack_60 + 0x10);
            } while (-1 < iVar1);
            iVar4 = iVar4 + 1;
            iVar3 = iVar3 + (int)(*(uint64_t*)rtguest::G(uStack_60 + 0x10) >> 32);
          } while (iVar4 < 4);
          break;
        case 0:
        case 1:
        default:
          iVar4 = 0;
          uiArenaSetTexture((uint32_t)param_2, 0x29c030, 2, 0xd);
          iVar3 = 0;
          uiArenaDataTag((uint32_t)param_2, 0xab);
          *(uint64_t*)rtguest::G(uStack_60) = 0;
          *(uint64_t*)rtguest::G(uStack_60 + 8) = 0x68;
          *(uint64_t*)rtguest::G(uStack_60 + 0x10) = 0x5000000050;
          *(uint64_t*)rtguest::G(uStack_60 + 0x18) = 0;
          do {
            iVar2 = 0;
            iVar1 = 9;
            do {
              iVar1 = iVar1 + -1;
              uiArenaSprites((uint32_t)param_2, uStack_60, 1, *(uint32_t*)rtguest::G(param_3 + 4) + iVar2, *(uint32_t*)rtguest::G(param_3 + 8) + iVar3, *(uint32_t*)rtguest::G(param_3 + 0xc));
              iVar2 = iVar2 + 0x50;
            } while (-1 < iVar1);
            iVar4 = iVar4 + 1;
            iVar3 = iVar3 + 0x50;
          } while (iVar4 < 9);
          iVar3 = 0x38;
          *(uint64_t*)rtguest::G(uStack_60) = 0;
          *(uint64_t*)rtguest::G(uStack_60 + 8) = 0x4400000000;
          *(uint64_t*)rtguest::G(uStack_60 + 0x10) = 0x1400000064;
          *(uint64_t*)rtguest::G(uStack_60 + 0x18) = 0;
          iVar4 = 0xe;
          do {
            iVar4 = iVar4 + -1;
            uiArenaSprites((uint32_t)param_2, uStack_60, 1, *(uint32_t*)rtguest::G(param_3 + 4), *(uint32_t*)rtguest::G(param_3 + 8) + iVar3, *(uint32_t*)rtguest::G(param_3 + 0xc));
            iVar3 = iVar3 + 0x28;
          } while (-1 < iVar4);
          iVar4 = 0;
          *(uint64_t*)rtguest::G(uStack_60) = 0;
          *(uint64_t*)rtguest::G(uStack_60 + 8) = 0x5000000068;
          *(uint64_t*)rtguest::G(uStack_60 + 0x10) = 0x70000004b;
          *(uint64_t*)rtguest::G(uStack_60 + 0x18) = 0;
          iVar3 = 0x55;
          do {
            iVar2 = 0x7d;
            iVar1 = 5;
            do {
              iVar1 = iVar1 + -1;
              uiArenaSprites((uint32_t)param_2, uStack_60, 1, *(uint32_t*)rtguest::G(param_3 + 4) + iVar2, *(uint32_t*)rtguest::G(param_3 + 8) + iVar3, *(uint32_t*)rtguest::G(param_3 + 0xc));
              iVar2 = iVar2 + 100;
            } while (-1 < iVar1);
            iVar4 = iVar4 + 1;
            iVar3 = iVar3 + 0x38;
          } while (iVar4 < 0xb);
          break;
        }
      }
      iVar5 = iVar5 + -1;
      param_3 = param_3 + 5 * sizeof(uint32_t);
    } while (iVar5 != -1);
  }
}

RT_GUEST_FN(0x00212190, guest::DrawMenuGridBackground)

}  // namespace guest
