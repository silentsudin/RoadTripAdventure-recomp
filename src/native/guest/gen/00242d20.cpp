// guest:0x00242d20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceMapMarkers(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t afStack_170 = rtguest::allocFrame(40);
    uint32_t fStack_148 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_140 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_138 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t afStack_130 = rtguest::allocFrame(40);
    uint32_t fStack_108 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_100 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_f8 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t afStack_f0 = rtguest::allocFrame(40);
    uint32_t fStack_c8 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_c0 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_b8 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t afStack_b0 = rtguest::allocFrame(40);
    uint32_t fStack_88 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_80 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_78 = rtguest::allocFrame(4);
uint32_t puVar1;
  uint uVar2;
  int iVar3;
  float fVar4;
  float fVar5;
  
  uVar2 = 1;
  fVar4 = 50.0 / (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a4058 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x22))) * 0x10)));
  fVar5 = (*(float*)rtguest::G(0x332f34));
  vifUploadColours(0x2a4370,0x2a4360);
  puVar1 = 0x0177aec0;
  iVar3 = (int)(*(uint32_t*)rtguest::G(0x177ae24));
  vu0UnitMatrix(afStack_170);
  (*(float*)rtguest::G((uint32_t)afStack_170 + (uint32_t)(0) * 4)) = fVar4;
  (*(float*)rtguest::G((uint32_t)fStack_148)) = fVar4;
  vu0RotMatrixY((float)iVar3 * fVar5 * 3.0517578e-05,afStack_170,afStack_170);
  (*(uint32_t*)rtguest::G((uint32_t)uStack_140)) = (*(uint32_t*)rtguest::G(0x177ace0));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_138)) = (*(uint32_t*)rtguest::G(0x177ace8));
  vifObjectMatrix(afStack_170);
  vifCallModel(0x290870);
  if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 10))) & 2) != 0) {
    vifUploadColours(0x2a4380,0x2a4360);
    iVar3 = (int)(*(uint32_t*)rtguest::G(0x177b094));
    uVar2 = 2;
    vu0UnitMatrix(afStack_130);
    puVar1 = 0x0177b130;
    (*(float*)rtguest::G((uint32_t)afStack_130 + (uint32_t)(0) * 4)) = fVar4;
    (*(float*)rtguest::G((uint32_t)fStack_108)) = fVar4;
    vu0RotMatrixY((float)iVar3 * fVar5 * 3.0517578e-05,afStack_130,afStack_130);
    (*(uint32_t*)rtguest::G((uint32_t)uStack_100)) = (*(uint32_t*)rtguest::G(0x177af50));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_f8)) = (*(uint32_t*)rtguest::G(0x177af58));
    vifObjectMatrix(afStack_130);
    vifCallModel(0x290870);
  }
  vifUploadColours(0x2a4390,0x2a4360);
  if (uVar2 < 0x18) {
    do {
      if (((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x198))) != 0) && (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x198))) & 0x30) == 0)) {
        vu0UnitMatrix(afStack_f0);
        (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x90)));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x98)));
        (*(float*)rtguest::G((uint32_t)afStack_f0 + (uint32_t)(0) * 4)) = fVar4;
        (*(float*)rtguest::G((uint32_t)fStack_c8)) = fVar4;
        vifObjectMatrix(afStack_f0);
        vifCallModel(0x290940);
      }
      uVar2 = uVar2 + 1;
      puVar1 = puVar1 + 0x270;
    } while ((int)uVar2 < 0x18);
  }
  puVar1 = 0x0177aec0;
  iVar3 = 5;
  do {
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x198))) & 0x30) != 0) {
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x198))) & 0x40) == 0) {
        vifUploadColours(0x2a4370,0x2a4360);
      }
      else {
        vifUploadColours(0x2a4380,0x2a4360);
      }
      vu0UnitMatrix(afStack_b0);
      (*(uint32_t*)rtguest::G((uint32_t)uStack_80)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x90)));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_78)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x98)));
      (*(float*)rtguest::G((uint32_t)afStack_b0 + (uint32_t)(0) * 4)) = fVar4;
      (*(float*)rtguest::G((uint32_t)fStack_88)) = fVar4;
      vifObjectMatrix(afStack_b0);
      vifCallModel(0x290940);
    }
    iVar3 = iVar3 + -1;
    puVar1 = puVar1 + 0x270;
  } while (-1 < iVar3);
  return 0;
}


RT_GUEST_FN(0x00242d20, guest::raceMapMarkers)

}  // namespace guest
