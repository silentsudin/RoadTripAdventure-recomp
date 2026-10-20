// guest:0x00242438 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_map_draw_functions_town(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(368);
  long lVar1;
  int iVar2;
uint32_t auStack_170 = rtguest::allocFrame(48);
uint32_t auStack_140 = rtguest::allocFrame(16);
uint32_t auStack_130 = rtguest::allocFrame(48);
uint32_t auStack_100 = rtguest::allocFrame(16);
uint32_t auStack_f0 = rtguest::allocFrame(48);
uint32_t auStack_c0 = rtguest::allocFrame(16);
uint32_t auStack_b0 = rtguest::allocFrame(48);
uint32_t auStack_80 = rtguest::allocFrame(16);
uint32_t auStack_70 = rtguest::allocFrame(48);
uint32_t auStack_40 = rtguest::allocFrame(16);
  
  vifUploadAt16(0x177f5e0);
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x14))) - 0xe484U < 0x1944d) {
    vifUploadColours(0x2a4340,0x2a4360);
  }
  else {
    vifUploadColours(0x2a4330,0x2a4360);
  }
  vifDirect(0x2a43b0,3);
  if ((*(uint32_t*)rtguest::G(0x333288)) == 0) {
    vifDirect(0x2a43f0,2);
    iVar2 = param_2 * 0x10;
    mapViewMatrix(((*(float*)rtguest::G((uint32_t)(uint32_t)(0x2a42c0 + iVar2))) + 1728.0) -
                  ((*(float*)rtguest::G((uint32_t)param_1)) * (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a42c8 + iVar2)))) / 200.0,
                  (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x2a42c4))) + 1936.0 +
                  ((*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(float))) * (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a42c8 + iVar2))) * (*(uint32_t*)rtguest::G(0x332efc))) / 200.0,0x1752700)
    ;
  }
  else {
    vifDirect(0x2a4410,2);
    iVar2 = param_2 * 0x10;
    mapViewMatrix(((*(float*)rtguest::G((uint32_t)(uint32_t)(0x2a42c0 + iVar2))) * (*(float*)rtguest::G(0x332ef4)) + 1792.0) -
                  ((*(float*)rtguest::G((uint32_t)param_1)) * (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a42c8 + iVar2))) * (*(float*)rtguest::G(0x332ef4))) / 200.0,
                  (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x2a42c4))) + 1920.0 +
                  ((*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(float))) * (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a42c8 + iVar2))) * (*(uint32_t*)rtguest::G(0x332ef8))) / 200.0,0x1752700)
    ;
  }
  (*(uint32_t*)rtguest::G(0x1752740)) = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a42c8 + param_2 * 0x10))) / 768.0;
  (*(uint32_t*)rtguest::G(0x1752754)) = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a42c8 + param_2 * 0x10))) * 0.001953125;
  vu0UnitMatrix(auStack_170);
  vu0CopyVectorXYZ(auStack_140,0x2a43e0);
  vu0RotMatrixX((*(uint32_t*)rtguest::G(0x00332f00)),auStack_170,auStack_170);
  vu0MulMatrix(0x1752700,0x1752700,auStack_170);
  vu0MulMatrix(0x1752740,0x1752740,auStack_170);
  vifUploadAt4(0x1752700);
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x60))) < 0) {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 100)));
  }
  else {
    vu0UnitMatrix(auStack_130);
    lVar1 = tileSlotOrigin(auStack_100,auStack_100,0);
    if (lVar1 == 0) {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 100)));
    }
    else {
      vifObjectMatrix(auStack_130);
      drawModelParts((*(uint32_t*)rtguest::G(0x177ab70)),2);
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 100)));
    }
  }
  if (iVar2 < 0) {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x68)));
  }
  else {
    vu0UnitMatrix(auStack_f0);
    lVar1 = tileSlotOrigin(auStack_c0,auStack_c0,1);
    if (lVar1 == 0) {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x68)));
    }
    else {
      vifObjectMatrix(auStack_f0);
      drawModelParts((*(uint32_t*)rtguest::G(0x177ab74)),2);
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x68)));
    }
  }
  if (iVar2 < 0) {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x6c)));
  }
  else {
    vu0UnitMatrix(auStack_b0);
    lVar1 = tileSlotOrigin(auStack_80,auStack_80,2);
    if (lVar1 == 0) {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x6c)));
    }
    else {
      vifObjectMatrix(auStack_b0);
      drawModelParts((*(uint32_t*)rtguest::G(0x177ab78)),2);
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x6c)));
    }
  }
  if (-1 < iVar2) {
    vu0UnitMatrix(auStack_70);
    lVar1 = tileSlotOrigin(auStack_40,auStack_40,3);
    if (lVar1 != 0) {
      vifObjectMatrix(auStack_70);
      drawModelParts((*(uint32_t*)rtguest::G(0x177ab7c)),2);
    }
  }
  return 0;
}


RT_GUEST_FN(0x00242438, guest::gs_map_draw_functions_town)

}  // namespace guest
