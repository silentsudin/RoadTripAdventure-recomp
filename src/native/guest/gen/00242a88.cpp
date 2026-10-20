// guest:0x00242a88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_map_draw_functions_scissored(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x50) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x54) = 0x00000000u;
  }
  char cVar1;
  ushort uVar2;
uint32_t pfVar3;
uint32_t auStack_60 = rtguest::allocFrame(48);
uint32_t auStack_30 = rtguest::allocFrame(16);
  
  RT_CALLRA(0x242aa4, guest::vifUploadAt16(0x177f5e0));
  RT_CALLRA(0x242ab8, guest::vifUploadColours(0x2a4350,0x2a4360));
  RT_CALLRA(0x242ac8, guest::vifDirect(0x2a43b0,3));
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x22))) == '\x19') {
    pfVar3 = (uint32_t)(0x2a42e0 + param_2 * 0x10);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xb)));
  }
  else {
    pfVar3 = (uint32_t)(0x2a4050 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x22))) * 0x10);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xb)));
  }
  if (cVar1 != '\0') {
    uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x2e)));
    if ((uVar2 & 0x2000) != 0) {
      (*(float*)rtguest::G((uint32_t)pfVar3)) = (*(float*)rtguest::G((uint32_t)pfVar3)) + 1.0;
    }
    if ((uVar2 & 0x8000) != 0) {
      (*(float*)rtguest::G((uint32_t)pfVar3)) = (*(float*)rtguest::G((uint32_t)pfVar3)) - 1.0;
    }
    if ((uVar2 & 0x1000) != 0) {
      (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(1) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(1) * sizeof(float))) + 1.0;
    }
    if ((uVar2 & 0x4000) != 0) {
      (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(1) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(1) * sizeof(float))) - 1.0;
    }
    if ((uVar2 & 2) != 0) {
      (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(2) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(2) * sizeof(float))) * (*(float*)rtguest::G(0x332f1c));
    }
    if ((uVar2 & 8) == 0) {
      uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x32)));
    }
    else {
      (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(2) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(2) * sizeof(float))) / (*(uint32_t*)rtguest::G(0x332f20));
      uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x32)));
    }
    if ((uVar2 & 4) != 0) {
      (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(3) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(3) * sizeof(float))) + (*(uint32_t*)rtguest::G(0x332f24));
    }
    if ((uVar2 & 1) != 0) {
      (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(3) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(3) * sizeof(float))) - (*(uint32_t*)rtguest::G(0x332f28));
    }
    RT_CALLRA(0x242c0c, guest::DrawMapCoordinateValues(pfVar3,0x1a));
  }
  if ((*(uint32_t*)rtguest::G(0x333288)) == 0) {
    RT_CALLRA(0x242c44, guest::mapViewMatrix((*(float*)rtguest::G((uint32_t)pfVar3)) + 1728.0,(*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(1) * sizeof(float))),(*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(2) * sizeof(float))),0x1752700));
  }
  else {
    RT_CALLRA(0x242c70, guest::mapViewMatrix((*(float*)rtguest::G((uint32_t)pfVar3)) * (*(uint32_t*)rtguest::G(0x332f2c)) + 1792.0,(*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(1) * sizeof(float))),(*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(2) * sizeof(float))),0x1752700));
  }
  RT_CALLRA(0x242c84, guest::vifDirect(0x2a4430,2));
  RT_CALLRA(0x242c8c, guest::vu0UnitMatrix(auStack_60));
  RT_CALLRA(0x242c9c, guest::vu0CopyVectorXYZ(auStack_30,0x2a43e0));
  RT_CALLRA(0x242cac, guest::vu0RotMatrixY((*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(3) * sizeof(float))),auStack_60,auStack_60));
  RT_CALLRA(0x242cbc, guest::vu0RotMatrixX((*(uint32_t*)rtguest::G(0x00332f30)),auStack_60,auStack_60));
  RT_CALLRA(0x242ccc, guest::vu0MulMatrix(0x1752700,0x1752700,auStack_60));
  RT_CALLRA(0x242cdc, guest::vu0MulMatrix(0x1752740,0x1752740,auStack_60));
  RT_CALLRA(0x242ce4, guest::vifUploadAt4(0x1752700));
  RT_CALLRA(0x242cec, guest::vu0UnitMatrix(auStack_60));
  RT_CALLRA(0x242cf4, guest::vifObjectMatrix(auStack_60));
  RT_CALLRA(0x242d04, guest::drawModelParts((*(uint32_t*)rtguest::G(0x177ab78)),2));
  return 0;
}


RT_GUEST_FN(0x00242a88, guest::gs_map_draw_functions_scissored)

}  // namespace guest
