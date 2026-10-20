// guest:0x0025d028 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawAnimatedProps(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  long lVar2;
uint32_t puVar3;
uint32_t puVar4;
  uint uVar5;
uint32_t pfVar6;
  int iVar7;
  float fVar8;
  
  iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 8)));
  if (iVar7 == 1) {
    pfVar6 = (uint32_t)0x017608d0;
    uVar5 = 0;
    iVar7 = 0;
    do {
      puVar3 = 0x01760e20 + iVar7;
      iVar7 = iVar7 + 3;
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(puVar3)), 0, 3));
      if ((uVar5 & 1) != 0) {
        (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(puVar3)), 0x80, 3));
      }
      iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002c4b40 + (uVar5 & 0x1f) * 4))) * 0x1000 + uVar5;
      uVar5 = uVar5 + 1;
      fVar8 = ABS((float)(iVar1 % 0x6b));
      (*(float*)rtguest::G((uint32_t)pfVar6)) = -(fVar8 / (*(float*)rtguest::G(0x2c43b0)));
      (*(float*)rtguest::G((uint32_t)pfVar6 + (uint32_t)(1) * sizeof(float))) = -(fVar8 / (*(uint32_t*)rtguest::G(0x2c43b4)));
      (*(float*)rtguest::G((uint32_t)pfVar6 + (uint32_t)(2) * sizeof(float))) = -(fVar8 / (*(uint32_t*)rtguest::G(0x2c43b8)));
      pfVar6 = pfVar6 + 0xc;  // T* element step
    } while ((int)uVar5 < 0x6b);
    pfVar6 = (uint32_t)0x01760dd8;
    uVar5 = 0;
    iVar7 = 0;
    do {
      puVar3 = 0x01760f68 + iVar7;
      iVar7 = iVar7 + 3;
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(puVar3)), 0, 3));
      if ((uVar5 & 1) != 0) {
        (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(puVar3)), 0x80, 3));
      }
      iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002c4b40 + (uVar5 & 0x1f) * 4))) * 0x1000 + uVar5;
      uVar5 = uVar5 + 1;
      fVar8 = ABS((float)(iVar1 % 6));
      (*(float*)rtguest::G((uint32_t)pfVar6)) = -(fVar8 / (*(float*)rtguest::G(0x2c43b0)));
      (*(float*)rtguest::G((uint32_t)pfVar6 + (uint32_t)(1) * sizeof(float))) = -(fVar8 / (*(uint32_t*)rtguest::G(0x2c43b4)));
      (*(float*)rtguest::G((uint32_t)pfVar6 + (uint32_t)(2) * sizeof(float))) = -(fVar8 / (*(uint32_t*)rtguest::G(0x2c43b8)));
      pfVar6 = pfVar6 + 0xc;  // T* element step
    } while ((int)uVar5 < 6);
  }
  else if (1 < iVar7) {
    iVar7 = 0xd;
    puVar4 = 0x002c4ad0;
    do {
      lVar2 = tileSlotLoaded(param_2,(*(uint32_t*)rtguest::G((uint32_t)puVar4)));
      if (lVar2 != 0) {
        rtCall((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint32_t))))), param_1, param_2);
      }
      iVar7 = iVar7 + -1;
      puVar4 = puVar4 + 0x8;  // T* element step
    } while (-1 < iVar7);
  }
  return 0;
}


RT_GUEST_FN(0x0025d028, guest::drawAnimatedProps)

}  // namespace guest
