// guest:0x0021fb38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldTiles(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  long lVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  float fVar7;
  float fVar8;
  
  if ((-1 < (*(int8_t*)rtguest::G(0x3335b8))) && (iVar3 = (int)(*(int8_t*)rtguest::G(0x3335b8)), -1 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x60))))) {
    uVar6 = 0;
    vifSetBatchOffset(0x29d080 + iVar3 * 0x10);
    fVar7 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x29d080 + iVar3 * 0x10)));
    fVar8 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar3 * 0x10 + 0x29d088)));
    uVar5 = 0;
    while( true ) {
      lVar2 = cellVisible((float)(int)((uVar6 & 7) * 200 + (int)fVar7 + 100),
                          (float)(int)((uVar5 & 7) * 200 + (int)fVar8 + 100),param_1);
      iVar3 = uVar6 * 4;
      uVar6 = uVar6 + 1;
      if (lVar2 != 0) {
        vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + (*(uint32_t*)rtguest::G(0x177ab20))))));
      }
      if (0x3f < (int)uVar6) break;
      uVar5 = (int)uVar6 >> 3;
    }
  }
  if ((-1 < (*(uint32_t*)rtguest::G(0x3335b9))) && (iVar3 = (int)(*(uint32_t*)rtguest::G(0x3335b9)), -1 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 100))))) {
    uVar6 = 0;
    vifSetBatchOffset(0x29d080 + iVar3 * 0x10);
    fVar7 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x29d080 + iVar3 * 0x10)));
    fVar8 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar3 * 0x10 + 0x29d088)));
    uVar5 = 0;
    while( true ) {
      lVar2 = cellVisible((float)(int)((uVar6 & 7) * 200 + (int)fVar7 + 100),
                          (float)(int)((uVar5 & 7) * 200 + (int)fVar8 + 100),param_1);
      iVar3 = uVar6 * 4;
      uVar6 = uVar6 + 1;
      if (lVar2 != 0) {
        vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + (*(int32_t*)rtguest::G(0x177ab24))))));
      }
      if (0x3f < (int)uVar6) break;
      uVar5 = (int)uVar6 >> 3;
    }
  }
  if ((-1 < (*(uint32_t*)rtguest::G(0x3335ba))) && (iVar3 = (int)(*(uint32_t*)rtguest::G(0x3335ba)), -1 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x68))))) {
    uVar6 = 0;
    vifSetBatchOffset(0x29d080 + iVar3 * 0x10);
    fVar7 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x29d080 + iVar3 * 0x10)));
    fVar8 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar3 * 0x10 + 0x29d088)));
    uVar5 = 0;
    while( true ) {
      lVar2 = cellVisible((float)(int)((uVar6 & 7) * 200 + (int)fVar7 + 100),
                          (float)(int)((uVar5 & 7) * 200 + (int)fVar8 + 100),param_1);
      iVar3 = uVar6 * 4;
      uVar6 = uVar6 + 1;
      if (lVar2 != 0) {
        vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + (*(int32_t*)rtguest::G(0x177ab28))))));
      }
      if (0x3f < (int)uVar6) break;
      uVar5 = (int)uVar6 >> 3;
    }
  }
  if ((*(uint32_t*)rtguest::G(0x3335bb)) < '\0') {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x6c)));
  }
  else if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x6c))) < 0) {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x6c)));
  }
  else {
    iVar3 = (int)(*(uint32_t*)rtguest::G(0x3335bb));
    uVar6 = 0;
    vifSetBatchOffset(0x29d080 + iVar3 * 0x10);
    fVar7 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x29d080 + iVar3 * 0x10)));
    fVar8 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar3 * 0x10 + 0x29d088)));
    uVar5 = 0;
    while( true ) {
      lVar2 = cellVisible((float)(int)((uVar6 & 7) * 200 + (int)fVar7 + 100),
                          (float)(int)((uVar5 & 7) * 200 + (int)fVar8 + 100),param_1);
      iVar3 = uVar6 * 4;
      uVar6 = uVar6 + 1;
      if (lVar2 != 0) {
        vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + (*(uint32_t*)rtguest::G(0x177ab2c))))));
      }
      if (0x3f < (int)uVar6) break;
      uVar5 = (int)uVar6 >> 3;
    }
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x6c)));
  }
  if (iVar3 == 0xb) {
    fVar8 = (*(float*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x3335bb)) * 0x10 + 0x29d088)));
    fVar7 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x29d080 + (*(uint32_t*)rtguest::G(0x3335bb)) * 0x10)));
    if (-1 < (*(uint32_t*)rtguest::G(0x3335bb))) {
      uVar6 = 0;
      vifSetBatchOffset();
      uVar5 = 0;
      while( true ) {
        lVar2 = cellVisible((float)(int)((uVar6 & 7) * 200 + (int)fVar7 + 100),
                            (float)(int)((uVar5 & 7) * 200 + (int)fVar8 + 100),param_1);
        if (lVar2 != 0) {
          iVar3 = 0;
          do {
            iVar1 = TestProgressFlag(iVar3);
            iVar4 = iVar3 * 2;
            iVar3 = iVar3 + 1;
            vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(uVar6 * 4 + (*(uint32_t*)rtguest::G((uint32_t)0x177ab80 + (uint32_t)(iVar4 + iVar1) * 4))))));
          } while (iVar3 < 0x1a);
        }
        uVar6 = uVar6 + 1;
        if (0x3f < (int)uVar6) break;
        uVar5 = (int)uVar6 >> 3;
      }
    }
  }
  return 0;
}


RT_GUEST_FN(0x0021fb38, guest::drawFieldTiles)

}  // namespace guest
