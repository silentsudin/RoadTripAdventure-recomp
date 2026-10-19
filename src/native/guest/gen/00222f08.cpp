// guest:0x00222f08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawRaceProps(uint32_t q0, ...) {
    uint32_t fStack_5c = rtguest::allocFrame(4);
uint32_t puVar1;
  undefined8 uVar2;
uint32_t pfVar3;
  uint uVar4;
  float fVar5;
uint32_t auStack_d0 = rtguest::allocFrame(64);
uint32_t auStack_90 = rtguest::allocFrame(52);
  
  uVar4 = 0;
  vifUploadAt4();
  uVar2 = GetSceneCamLookatIndex(0);
  fVar5 = 1.0;
  vu0DropShadowMatrix(0,0xbf800000,0,auStack_d0,uVar2,0);
  vifUploadLightAt16(0x177f5e0);
  vifUploadLightMatrix(0x29d1d0);
  puVar1 = 0x0177eb10;
  pfVar3 = (uint32_t)0x0177eb44;
  do {
    if ((*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(2) * sizeof(float))) != 0.0) {
      (*(float*)rtguest::G((uint32_t)pfVar3)) = (*(float*)rtguest::G((uint32_t)pfVar3)) - fVar5;
      vu0MulMatrix(auStack_90,auStack_d0,puVar1);
      (*(float*)rtguest::G((uint32_t)fStack_5c)) = (*(float*)rtguest::G((uint32_t)fStack_5c)) + 2.0;
      (*(float*)rtguest::G((uint32_t)pfVar3)) = (*(float*)rtguest::G((uint32_t)pfVar3)) + fVar5;
      vifObjectMatrix(auStack_90);
      vifUploadPacket(0x29d210,3);
      vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177f580 + (uint32_t)((uVar4 & 3) * 6) * 4)) + 0x10);
    }
    uVar4 = uVar4 + 1;
    pfVar3 = pfVar3 + 0x10;
    puVar1 = puVar1 + 0x40;
  } while ((int)uVar4 < 0x20);
  return 0;
}


RT_GUEST_FN(0x00222f08, guest::drawRaceProps)

}  // namespace guest
