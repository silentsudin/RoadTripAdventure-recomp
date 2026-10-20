// guest:0x0025df80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t balloonNpcUpdate(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
  }
  byte bVar1;
  byte bVar2;
uint32_t puVar3;
  bool bVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  int iVar7;
uint32_t puVar8;
uint32_t puVar9;
  float fVar10;
  float fVar11;
  undefined4 uVar12;
  float fVar13;
  float fVar14;
uint32_t auStack_50 = rtguest::allocFrame(16);
uint32_t auStack_40 = rtguest::allocFrame(4);
uint32_t auStack_3c = rtguest::allocFrame(12);
  
  fVar13 = (*(uint32_t*)rtguest::G(0x3330f0));
  puVar9 = (uint32_t)param_2;
  puVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
  bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 6)));
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar9 + 5))) & 0x8000) == 0) {
    bVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x19)));
    if (bVar2 == 1) {
      fVar11 = (float)(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t))) - (float)(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t)));
      fVar10 = (float)(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t))) + (*(uint32_t*)rtguest::G(0x3330ec));
      fVar14 = fVar11 - (*(uint32_t*)rtguest::G(0x3330e8));
      (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t))) = fVar11;
      (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t))) = fVar10;
      (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) = (float)(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) - fVar13;
      if (fVar14 <= 78.0) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x19))) = 2;
      }
    }
    else if (bVar2 < 2) {
      if ((bVar2 == 0) &&
         (bVar4 = (float)(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) - (*(float*)rtguest::G(0x3330e0)) < (*(uint32_t*)rtguest::G(0x3330e4)),
         (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) = (float)(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) - (*(float*)rtguest::G(0x3330e0)), bVar4)) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x19))) = 1;
      }
    }
    else if (bVar2 == 2) {
      rtCall((*(uint32_t*)rtguest::G(0x335710)), puVar3, auStack_40, auStack_3c, 0, param_2);
      fVar13 = (float)(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t))) + (*(uint32_t*)rtguest::G(0x3330f8));
      bVar4 = (float)(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) - (*(uint32_t*)rtguest::G(0x3330f4)) < (*(uint32_t*)rtguest::G(0x3330fc));
      (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) = (float)(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) - (*(uint32_t*)rtguest::G(0x3330f4));
      (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t))) = fVar13;
      if (bVar4) {
        iVar7 = RT_CALLRA(0x25e0c0, guest::__umoddi3((*(uint64_t*)rtguest::G((uint32_t)puVar9)),5));
        uVar6 = (*(uint32_t*)rtguest::G(0x333108));
        uVar5 = (*(uint32_t*)rtguest::G(0x333104));
        uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002c4bf0 + iVar7 * 4)));
        (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G(0x333100));
        (*(uint32_t*)rtguest::G((uint32_t)puVar3)) = uVar12;
        (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) = uVar5;
        (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t))) = uVar6;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x19))) = 0;
      }
    }
    RT_CALLRA(0x25e10c, guest::vu0SubVector(auStack_50,puVar3,0x177ace0));
    fVar13 = rtguest::fbits(RT_CALLRA(0x25e118, guest::vecDotProductVu0(auStack_50,auStack_50)));
    if ((*(uint32_t*)rtguest::G(0x33310c)) <= SQRT(fVar13)) {
      fVar13 = (float)(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t)));
      goto LAB_0025e178;
    }
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x19))) = 3;
  }
  fVar13 = (float)(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t)));
LAB_0025e178:
  if ((820.0 <= fVar13) || (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 1) != 0)) {
    (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(3) * sizeof(uint32_t))) = 0x3f800000;
  }
  puVar8 = 0x0177eb10 + (uint32_t)bVar1 * 0x40;
  RT_CALLRA(0x25e1c4, guest::vu0UnitMatrix(puVar8));
  if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x19))) != '\x03') && (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar9 + 5))) & 0x8000) == 0)) {
    RT_CALLRA(0x25e238, guest::vu0RotMatrixX(((float)(0x14 - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) % 0x28) * (*(float*)rtguest::G(0x333110))) / 20.0,puVar8,puVar8
                 ));
  }
  RT_CALLRA(0x25e24c, guest::vu0CopyVector(0x0177eb40 + (uint32_t)bVar1 * 0x10,puVar3));
  return 0;
}


RT_GUEST_FN(0x0025df80, guest::balloonNpcUpdate)

}  // namespace guest
