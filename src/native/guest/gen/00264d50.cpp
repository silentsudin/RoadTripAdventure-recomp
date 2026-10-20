// guest:0x00264d50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawRotatedQuadPair(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(160);
    uint32_t iStack_a0 = rtguest::allocFrame(4);
    uint32_t iStack_9c = rtguest::allocFrame(4);
    rtguest::allocFrame(24);  // guest frame gap
uint32_t afStack_80 = rtguest::allocFrame(80);
uint32_t puVar1;
uint32_t piVar2;
uint32_t puVar3;
uint32_t pfVar4;
uint32_t puVar5;
  int iVar6;
  undefined8 uVar7;
  undefined8 uVar8;
  undefined8 uVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  
  puVar1 = 0x0032efc0;
  piVar2 = iStack_a0;
  do {
    puVar5 = (uint32_t)piVar2;
    puVar3 = puVar1;
    uVar7 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t)));
    uVar8 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint64_t)));
    uVar9 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(3) * sizeof(uint64_t)));
    (*(uint64_t*)rtguest::G((uint32_t)puVar5)) = (*(uint64_t*)rtguest::G((uint32_t)puVar3));
    (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(1) * sizeof(uint64_t))) = uVar7;
    (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(2) * sizeof(uint64_t))) = uVar8;
    (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(3) * sizeof(uint64_t))) = uVar9;
    puVar1 = puVar3 + 4;
    piVar2 = (uint32_t)(puVar5 + 4);
  } while (puVar3 + 4 != (uint32_t)0x0032f020);
  uVar7 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(5) * sizeof(uint64_t)));
  (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(4) * sizeof(uint64_t))) = 0;
  fVar10 = (*(float*)rtguest::G(0x3331f4));
  (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(5) * sizeof(uint64_t))) = uVar7;
  fVar12 = (float)(int)param_2 * fVar10 * 3.0517578e-05;
  fVar10 = rtguest::fbits(cosf(rtguest::f2bits(fVar12)));
  fVar12 = rtguest::fbits(sinf(rtguest::f2bits(fVar12)));
  pfVar4 = afStack_80 + 1;
  iVar6 = 3;
  do {
    iVar6 = iVar6 + -1;
    fVar11 = (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(-1) * sizeof(float))) + ((float)(*(int32_t*)rtguest::G((uint32_t)iStack_a0)) - 320.0);
    (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(-1) * sizeof(float))) = fVar11 * fVar10 - ((float)(*(int32_t*)rtguest::G((uint32_t)iStack_a0)) - 320.0);
    (*(float*)rtguest::G((uint32_t)pfVar4)) = ((*(float*)rtguest::G((uint32_t)pfVar4)) + ((float)(*(int32_t*)rtguest::G((uint32_t)iStack_9c)) - 224.0)) *
              ((fVar11 * fVar12 * (*(uint32_t*)rtguest::G(0x3331f8))) / 320.0 + 1.0) - ((float)(*(int32_t*)rtguest::G((uint32_t)iStack_9c)) - 224.0);
    pfVar4 = pfVar4 + 0x10;  // T* element step
  } while (-1 < iVar6);
  uiArenaRotQuads(param_1,iStack_a0,1,0,0,1);
  puVar1 = 0x0032f030;
  piVar2 = iStack_a0;
  do {
    puVar5 = (uint32_t)piVar2;
    puVar3 = puVar1;
    uVar9 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t)));
    uVar7 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint64_t)));
    uVar8 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(3) * sizeof(uint64_t)));
    (*(uint64_t*)rtguest::G((uint32_t)puVar5)) = (*(uint64_t*)rtguest::G((uint32_t)puVar3));
    (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(1) * sizeof(uint64_t))) = uVar9;
    (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(2) * sizeof(uint64_t))) = uVar7;
    (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(3) * sizeof(uint64_t))) = uVar8;
    puVar1 = puVar3 + 4;
    piVar2 = (uint32_t)(puVar5 + 4);
  } while (puVar3 + 4 != (uint32_t)0x0032f090);
  fVar12 = (float)(int)param_2 * (*(uint32_t*)rtguest::G(0x3331fc));
  uVar7 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(5) * sizeof(uint64_t)));
  (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(4) * sizeof(uint64_t))) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(5) * sizeof(uint64_t))) = uVar7;
  fVar12 = fVar12 * 3.0517578e-05;
  fVar10 = rtguest::fbits(cosf(rtguest::f2bits(fVar12)));
  fVar12 = rtguest::fbits(sinf(rtguest::f2bits(fVar12)));
  pfVar4 = afStack_80 + 1;
  iVar6 = 3;
  do {
    iVar6 = iVar6 + -1;
    fVar11 = (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(-1) * sizeof(float))) + ((float)(*(int32_t*)rtguest::G((uint32_t)iStack_a0)) - 320.0);
    (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(-1) * sizeof(float))) = fVar11 * fVar10 - ((float)(*(int32_t*)rtguest::G((uint32_t)iStack_a0)) - 320.0);
    (*(float*)rtguest::G((uint32_t)pfVar4)) = ((*(float*)rtguest::G((uint32_t)pfVar4)) + ((float)(*(int32_t*)rtguest::G((uint32_t)iStack_9c)) - 224.0)) *
              ((fVar11 * fVar12 * (*(uint32_t*)rtguest::G(0x333200))) / 320.0 + 1.0) - ((float)(*(int32_t*)rtguest::G((uint32_t)iStack_9c)) - 224.0);
    pfVar4 = pfVar4 + 0x10;  // T* element step
  } while (-1 < iVar6);
  uiArenaRotQuads(param_1,iStack_a0,1,0,0,1);
  return 0;
}


RT_GUEST_FN(0x00264d50, guest::drawRotatedQuadPair)

}  // namespace guest
