// guest:0x002228a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawCarsField(uint32_t param_1, uint32_t param_2, ...) {
uint32_t auStack_110 = rtguest::allocFrame(60);
    uint32_t fStack_d4 = rtguest::allocFrame(4);
uint32_t acStack_d0 = rtguest::allocFrame(32);
    uint32_t uStack_b0 = rtguest::allocFrame(4);
    uint32_t uStack_ac = rtguest::allocFrame(4);
    uint32_t uStack_a8 = rtguest::allocFrame(4);
    uint32_t uStack_a4 = rtguest::allocFrame(4);
    uint32_t uStack_a0 = rtguest::allocFrame(4);
    uint32_t uStack_9c = rtguest::allocFrame(4);
    uint32_t uStack_98 = rtguest::allocFrame(4);
    uint32_t uStack_94 = rtguest::allocFrame(4);
uint32_t auStack_90 = rtguest::allocFrame(4);
    uint32_t uStack_8c = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t uStack_7c = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t uStack_6c = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t uStack_5c = rtguest::allocFrame(4);
  float fVar1;
  char cVar2;
  byte bVar3;
  int iVar4;
uint32_t pcVar5;
uint32_t puVar6;
  int iVar7;
  float fVar8;
  
  iVar7 = 0x17;
  vifUploadAt4();
  vifUploadAt16(0x177f5e0);
  vifDirect(0x29d290,4);
  puVar6 = 0x177ac50;
  do {
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x33))) != 0) {
      ToRelativeVec4(puVar6 + 6,puVar6 + 0x14,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x58))),
                     (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x5c))));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = 0x3f800000;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f000000;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0x3f800000;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0x3f800000;
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x194)));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x144)));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x154)));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_98)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x164)));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_94)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x174)));
      vifUploadColours(uStack_b0,uStack_a0);
      vu0CopyMatrix(auStack_90,puVar6);
      (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_7c)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = 0;
      vifObjectMatrix(auStack_90);
      iVar4 = iVar4 * 0x44;
      vifBatchRegs((long)(iVar4 + 0x243e) | 0x1d308000U | (long)(iVar4 + 0x247e) << 0x25 |
                   0x20000005c0000000,7,0x1fc401fc40a,9);
      vifCallModel(0x290570);
    }
    iVar7 = iVar7 + -1;
    puVar6 = puVar6 + 0x270;  // T* element step
  } while (-1 < iVar7);
  vifDirect(0x29d240,5);
  vifUploadLightAt16(0x177f5e0);
  fVar1 = (*(uint32_t*)rtguest::G(0x332cf4));
  (*(uint32_t*)rtguest::G(0x3356e8)) = 0;
  puVar6 = 0x177ac50;
  iVar7 = 0;
  do {
    pcVar5 = acStack_d0 + iVar7;
    (*(int8_t*)rtguest::G((uint32_t)pcVar5)) = -1;
    iVar7 = iVar7 + 1;
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x33))) != 0) {
      vu0MulMatrix(auStack_110,param_1,puVar6);
      fVar8 = (*(float*)rtguest::G((uint32_t)fStack_d4)) / (*(float*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x18c)));
      if (fVar1 <= fVar8) {
        cVar2 = drawCar(fVar8,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x11c))),puVar6,
                        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x194))),
                        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x19c))) >> 0xc & 3);
        (*(int8_t*)rtguest::G((uint32_t)pcVar5)) = cVar2;
      }
    }
    puVar6 = puVar6 + 0x270;  // T* element step
  } while (iVar7 < 0x18);
  vifUploadColours(0x29d2d0,0x29d2e0);
  puVar6 = 0x177ac50;
  iVar7 = 0;
  vifDirect(0x29d2f0,2);
  do {
    pcVar5 = acStack_d0 + iVar7;
    iVar7 = iVar7 + 1;
    if (-1 < (*(int8_t*)rtguest::G((uint32_t)pcVar5))) {
      bVar3 = 2;
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x1fe))) == '\0') {
        bVar3 = 0;
      }
      drawCarExtraParts(puVar6,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x194))),
                        bVar3 | (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x1fc))) != '\0',(*(int8_t*)rtguest::G((uint32_t)pcVar5)));
    }
    puVar6 = puVar6 + 0x270;  // T* element step
  } while (iVar7 < 0x18);
  return 0;
}


RT_GUEST_FN(0x002228a8, guest::drawCarsField)

}  // namespace guest
