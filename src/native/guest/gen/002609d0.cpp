// guest:0x002609d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t rouletteTablePoseUpdate(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(176);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0xa0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xa4) = 0x00000000u;
  }
uint32_t auStack_b0 = rtguest::allocFrame(4);
    uint32_t uStack_ac = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_a0 = rtguest::allocFrame(8);
    uint32_t uStack_98 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_90 = rtguest::allocFrame(8);
    uint32_t uStack_88 = rtguest::allocFrame(8);
uint32_t auStack_80 = rtguest::allocFrame(4);
    uint32_t fStack_7c = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_70 = rtguest::allocFrame(64);
  float fVar1;
  short sVar2;
  
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_b0)), 0, 0x10));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0x3f800000;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_a0)), 0, 0x10));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_98)) = 0x3f800000;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = 0x43c80000;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_88)) = 0x43c80000;
  RT_CALLRA(0x260a48, guest::vu0ScaleVector(0,auStack_80,auStack_80));
  fVar1 = (*(uint32_t*)rtguest::G(0x333178));
  sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xe)));
  (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = sVar2;
  (*(float*)rtguest::G((uint32_t)fStack_7c)) = (float)(int)sVar2 * fVar1 * 3.0517578e-05;
  RT_CALLRA(0x260a90, guest::vu0UnitMatrix(auStack_70));
  RT_CALLRA(0x260aa0, guest::vu0RotMatrix(auStack_70,auStack_70,auStack_80));
  RT_CALLRA(0x260ab0, guest::vu0ApplyMatrix(auStack_a0,auStack_70,auStack_a0));
  RT_CALLRA(0x260ac0, guest::vu0ApplyMatrix(auStack_b0,auStack_70,auStack_b0));
  RT_CALLRA(0x260ad0, guest::vecNormalizeVu0(auStack_a0,auStack_a0));
  RT_CALLRA(0x260ae0, guest::vecNormalizeVu0(auStack_b0,auStack_b0));
  RT_CALLRA(0x260af0, guest::BuildTransformMatrix(0x177eb10,auStack_b0,auStack_a0));
  RT_CALLRA(0x260afc, guest::vu0CopyVectorXYZ(0x177eb40,uStack_90));
  RT_CALLRA(0x260b08, guest::vu0CopyMatrix(0x177eb50,0x177eb10));
  RT_CALLRA(0x260b14, guest::vu0CopyMatrix(0x177eb90,0x177eb10));
  RT_CALLRA(0x260b20, guest::vu0CopyMatrix(0x177ebd0,0x177eb10));
  return 0;
}


RT_GUEST_FN(0x002609d0, guest::rouletteTablePoseUpdate)

}  // namespace guest
