// guest:0x0023f0a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateRotatingEffect(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x38) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x3c) = 0x00000000u;
  }
  uint uVar1;
  uint uVar2;
  uint uVar3;
  float fVar4;
  long lVar5;
uint32_t puVar6;
  int iVar7;
  float fVar8;
  
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) == 1) {
    lVar5 = RT_CALLRA(0x23f0d8, guest::TestFlagBitFc98(0x5a));
    if (lVar5 == 0) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0;
    }
    else {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0x168;
    }
  }
  lVar5 = RT_CALLRA(0x23f0f4, guest::TestFlagBitFc98(0x5a));
  fVar4 = (*(float*)rtguest::G(0x332e74));
  fVar8 = (*(float*)rtguest::G(0x332e70));
  if (lVar5 == 0) {
    puVar6 = 0x0177acf0;
    iVar7 = 0xb;
    do {
      if (fVar8 <= (float)(*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(-4) * sizeof(uint32_t)))) {
        uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(0x14) * sizeof(uint32_t)));
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x4c))) = 0;
        uVar2 = (*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(0x15) * sizeof(uint32_t)));
        uVar3 = (*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(0x16) * sizeof(uint32_t)));
        (*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(0x14) * sizeof(uint32_t))) = (int)((float)(int)uVar1 * 0.75);
        (*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(0x15) * sizeof(uint32_t))) = (int)((float)(int)uVar2 * 0.75);
        (*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(0x16) * sizeof(uint32_t))) = (int)((float)(int)uVar3 * 0.75);
        (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = (*(uint32_t*)rtguest::G((uint32_t)puVar6)) + ((int)(uVar1 << 5) / 0x19) * -2 & 0xfffffff;
        (*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(1) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(1) * sizeof(uint32_t))) + ((int)(uVar2 << 5) / 0x19) * -2;
        (*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(2) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(2) * sizeof(uint32_t))) + ((int)(uVar3 << 5) / 0x19) * -2 & 0xfffffff;
      }
      iVar7 = iVar7 + -1;
      if (fVar4 <= (float)(*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(-4) * sizeof(uint32_t)))) {
        (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = (*(uint32_t*)rtguest::G((uint32_t)puVar6)) - 16000 & 0xfffffff;
      }
      puVar6 = puVar6 + 0x270;  // T* element step
    } while (-1 < iVar7);
  }
  lVar5 = RT_CALLRA(0x23f23c, guest::TestFlagBitFc98(0x5a));
  if ((lVar5 == 0) || (0x167 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))))) {
    (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) & 0xf7ff;
  }
  else {
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) + 1;
    (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
  }
  fVar8 = (*(float*)rtguest::G(0x332e78));
  RT_CALLRA(0x23f2a0, guest::vu0UnitMatrix(0x177eb10));
  RT_CALLRA(0x23f2f8, guest::vu0RotMatrixY(((float)((0x2d0 - (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2)))) % 0x5a0 + -0x2d0) * fVar8) / 720.0,
                0x177eb10,0x177eb10));
  RT_CALLRA(0x23f304, guest::vu0CopyVector(0x177eb40,0x2a37e0));
  RT_CALLRA(0x23f30c, guest::vu0UnitMatrix(0x177eb50));
  RT_CALLRA(0x23f358, guest::vu0RotMatrixY(((float)(((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) + 0x2d0) % 0x5a0 + -0x2d0) * fVar8) / 720.0,
                0x177eb50,0x177eb50));
  RT_CALLRA(0x23f364, guest::vu0CopyVector(0x177eb80,0x2a37f0));
  return 0;
}


RT_GUEST_FN(0x0023f0a0, guest::UpdateRotatingEffect)

}  // namespace guest
