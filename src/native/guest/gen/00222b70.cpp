// guest:0x00222b70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawCarsAction(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(400);
uint32_t auStack_190 = rtguest::allocFrame(60);
    uint32_t fStack_154 = rtguest::allocFrame(4);
uint32_t acStack_150 = rtguest::allocFrame(32);
    uint32_t uStack_130 = rtguest::allocFrame(4);
    uint32_t uStack_12c = rtguest::allocFrame(4);
    uint32_t uStack_128 = rtguest::allocFrame(4);
    uint32_t uStack_124 = rtguest::allocFrame(4);
    uint32_t uStack_120 = rtguest::allocFrame(4);
    uint32_t uStack_11c = rtguest::allocFrame(4);
    uint32_t uStack_118 = rtguest::allocFrame(4);
    uint32_t uStack_114 = rtguest::allocFrame(4);
uint32_t auStack_110 = rtguest::allocFrame(4);
    uint32_t uStack_10c = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t uStack_fc = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t uStack_ec = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t uStack_dc = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_d0 = rtguest::allocFrame(64);
uint32_t auStack_90 = rtguest::allocFrame(16);
    uint32_t fStack_80 = rtguest::allocFrame(4);
    uint32_t fStack_7c = rtguest::allocFrame(4);
    uint32_t fStack_78 = rtguest::allocFrame(4);
  char cVar1;
  byte bVar2;
  int iVar3;
uint32_t pcVar4;
uint32_t puVar5;
  int iVar6;
  float fVar7;
  
  iVar6 = 0x17;
  vifUploadAt4();
  vifUploadAt16(0x177f5e0);
  vifDirect(0x29d290,4);
  puVar5 = 0x177ac50;
  do {
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x33))) != 0) {
      ToRelativeVec4(puVar5 + 6,puVar5 + 0x14,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x58))),
                     (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x5c))));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_130)) = 0x3f800000;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_124)) = 0x3f000000;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_12c)) = 0x3f800000;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_128)) = 0x3f800000;
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x194)));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_120)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x144)));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_11c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x154)));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_118)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x164)));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_114)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x174)));
      vifUploadColours(uStack_130,uStack_120);
      vu0CopyMatrix(auStack_110,puVar5);
      (*(uint32_t*)rtguest::G((uint32_t)uStack_ec)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_fc)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_10c)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_dc)) = 0;
      vifObjectMatrix(auStack_110);
      iVar3 = iVar3 * 0x44;
      vifBatchRegs((long)(iVar3 + 0x243e) | 0x1d308000U | (long)(iVar3 + 0x247e) << 0x25 |
                   0x20000005c0000000,7,0x1fc401fc40a,9);
      vifCallModel(0x290570);
    }
    iVar6 = iVar6 + -1;
    puVar5 = puVar5 + 0x270;  // T* element step
  } while (-1 < iVar6);
  vifDirect(0x29d240,5);
  vifUploadLightAt16(0x177f5e0);
  puVar5 = 0x177ac50;
  (*(uint32_t*)rtguest::G(0x3356e8)) = 0;
  iVar6 = 0;
  do {
    pcVar4 = acStack_150 + iVar6;
    (*(int8_t*)rtguest::G((uint32_t)pcVar4)) = -1;
    iVar6 = iVar6 + 1;
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x33))) != 0) {
      vu0MulMatrix(auStack_190,param_1,puVar5);
      fVar7 = (*(float*)rtguest::G((uint32_t)fStack_154)) / (*(float*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x18c)));
      if ((*(uint32_t*)rtguest::G(0x332cf8)) <= fVar7) {
        vu0CopyMatrix(auStack_d0,puVar5);
        (*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 4))) = -(*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 4)));
        (*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x14))) = -(*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x14)));
        (*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x24))) = -(*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x24)));
        (*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x34))) =
             ((*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x144))) + (*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x144)))) -
             (*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x34)));
        vu0CopyMatrix(auStack_90,0x177f5e0);
        (*(float*)rtguest::G((uint32_t)fStack_80)) = -(*(float*)rtguest::G((uint32_t)fStack_80));
        (*(float*)rtguest::G((uint32_t)fStack_7c)) = -(*(float*)rtguest::G((uint32_t)fStack_7c));
        (*(float*)rtguest::G((uint32_t)fStack_78)) = -(*(float*)rtguest::G((uint32_t)fStack_78));
        vifUploadLightAt16(auStack_90);
        drawCar((*(uint32_t*)rtguest::G(0x332cfc)),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x11c))),puVar5,
                (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x194))),(*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x19c))) >> 0xc & 3);
        vu0CopyMatrix(puVar5,auStack_d0);
        vifUploadLightAt16(0x177f5e0);
        cVar1 = drawCar(fVar7,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x11c))),puVar5,
                        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x194))),
                        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x19c))) >> 0xc & 3);
        (*(int8_t*)rtguest::G((uint32_t)pcVar4)) = cVar1;
      }
    }
    puVar5 = puVar5 + 0x270;  // T* element step
  } while (iVar6 < 0x18);
  vifUploadColours(0x29d2d0,0x29d2e0);
  puVar5 = 0x177ac50;
  iVar6 = 0;
  vifDirect(0x29d2f0,2);
  do {
    pcVar4 = acStack_150 + iVar6;
    iVar6 = iVar6 + 1;
    if (-1 < (*(int8_t*)rtguest::G((uint32_t)pcVar4))) {
      bVar2 = 2;
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x1fe))) == '\0') {
        bVar2 = 0;
      }
      drawCarExtraParts(puVar5,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x194))),
                        bVar2 | (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x1fc))) != '\0',(*(int8_t*)rtguest::G((uint32_t)pcVar4)));
    }
    puVar5 = puVar5 + 0x270;  // T* element step
  } while (iVar6 < 0x18);
  return 0;
}


RT_GUEST_FN(0x00222b70, guest::drawCarsAction)

}  // namespace guest
