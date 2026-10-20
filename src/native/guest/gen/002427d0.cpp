// guest:0x002427d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_map_draw_functions_race(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(96);
  ushort uVar1;
  int iVar2;
uint32_t pfVar4;
uint32_t auStack_60 = rtguest::allocFrame(48);
uint32_t auStack_30 = rtguest::allocFrame(16);
  int iVar3;
  
  vifUploadAt16(0x177f5e0);
  vifUploadColours(0x2a4350,0x2a4360);
  vifDirect(0x2a43b0,3);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xb))) != '\0') {
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x2e)));
    iVar3 = (int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x22)));
    iVar2 = iVar3 * 0x10;
    pfVar4 = (uint32_t)(0x2a4050 + iVar2);
    if ((uVar1 & 0x2000) != 0) {
      (*(float*)rtguest::G((uint32_t)pfVar4)) = (*(float*)rtguest::G((uint32_t)pfVar4)) + 1.0;
    }
    if ((uVar1 & 0x8000) != 0) {
      (*(float*)rtguest::G((uint32_t)pfVar4)) = (*(float*)rtguest::G((uint32_t)pfVar4)) - 1.0;
    }
    if ((uVar1 & 0x1000) != 0) {
      (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a4054 + iVar2))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a4054 + iVar2))) + 1.0;
    }
    if ((uVar1 & 0x4000) != 0) {
      (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a4054 + iVar2))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a4054 + iVar2))) - 1.0;
    }
    if ((uVar1 & 2) != 0) {
      (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a4058 + iVar2))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a4058 + iVar2))) * (*(float*)rtguest::G(0x332f04));
    }
    if ((uVar1 & 8) == 0) {
      uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x32)));
    }
    else {
      (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a4058 + iVar2))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a4058 + iVar2))) / (*(uint32_t*)rtguest::G(0x332f08));
      uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x32)));
    }
    if ((uVar1 & 4) != 0) {
      (*(uint32_t*)rtguest::G((uint32_t)(0x2a405cu + (uint32_t)(iVar3 * 4) * 4))) =
           (uint32_t)((float)(*(uint32_t*)rtguest::G((uint32_t)(0x2a405cu + (uint32_t)(iVar3 * 4) * 4))) + (*(uint32_t*)rtguest::G(0x332f0c)));
    }
    if ((uVar1 & 1) != 0) {
      (*(uint32_t*)rtguest::G((uint32_t)(0x2a405cu + (uint32_t)(iVar3 * 4) * 4))) =
           (uint32_t)((float)(*(uint32_t*)rtguest::G((uint32_t)(0x2a405cu + (uint32_t)(iVar3 * 4) * 4))) - (*(uint32_t*)rtguest::G(0x332f10)));
    }
    DrawMapCoordinateValues(pfVar4,0x1a);
  }
  if ((*(uint32_t*)rtguest::G(0x333288)) == 0) {
    iVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x22))) * 0x10;
    mapViewMatrix((*(float*)rtguest::G((uint32_t)(uint32_t)(0x2a4050 + iVar2))) + 1728.0,
                  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a4054 + iVar2))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a4058 + iVar2))),
                  0x1752700);
  }
  else {
    iVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x22))) * 0x10;
    mapViewMatrix((*(float*)rtguest::G((uint32_t)(uint32_t)(0x2a4050 + iVar2))) * (*(uint32_t*)rtguest::G(0x332f14)) + 1792.0,
                  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a4054 + iVar2))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a4058 + iVar2))),
                  0x1752700);
  }
  vu0UnitMatrix(auStack_60);
  vu0CopyVectorXYZ(auStack_30,0x2a43e0);
  vu0RotMatrixY((*(uint32_t*)rtguest::G((uint32_t)(0x2a405cu + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x22))) * 4) * 4))),auStack_60,auStack_60);
  vu0RotMatrixX((*(uint32_t*)rtguest::G(0x00332f18)),auStack_60,auStack_60);
  vu0MulMatrix(0x1752700,0x1752700,auStack_60);
  vu0MulMatrix(0x1752740,0x1752740,auStack_60);
  vifUploadAt4(0x1752700);
  vu0UnitMatrix(auStack_60);
  vifObjectMatrix(auStack_60);
  drawModelParts((*(uint32_t*)rtguest::G(0x177ab78)),2);
  return 0;
}


RT_GUEST_FN(0x002427d0, guest::gs_map_draw_functions_race)

}  // namespace guest
