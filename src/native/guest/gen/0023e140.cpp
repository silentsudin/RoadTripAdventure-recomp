// guest:0x0023e140 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t texAnimCopyA(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(272);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0xc0, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0xc8, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0xd0, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0xd8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xe8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xec) = 0x00000000u;
    rtguest::rtSpillF(rt_fg, 7, rt_sv, 0x108);
    rtguest::rtSpillF(rt_fg, 6, rt_sv, 0x100);
    rtguest::rtSpillF(rt_fg, 5, rt_sv, 0xf8);
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0xf0);
    *(uint32_t*)rtguest::G(rt_sv+0x0) = (uint32_t)(int32_t)param_1;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0xa0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0xa8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0xb0, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0xb8, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0xe0, 8);
    rtguest::gS[5] = 0x00000000002a2c30ull;  // walk const
  }
  undefined4 uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined4 uVar8;
  float fVar9;
  undefined4 uVar10;
  
  uVar1 = (*(uint32_t*)rtguest::G(0x332e48));
  uVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) >> 3 & 3;
  iVar3 = (int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2a36e0u + uVar2)));
  iVar4 = iVar3 * (*(uint32_t*)rtguest::G(0x2a2c2a));
  iVar3 = iVar3 * (*(uint32_t*)rtguest::G(0x2a2c0a));
  iVar6 = (int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2a36d8u + uVar2)));
  iVar5 = iVar6 * (*(uint32_t*)rtguest::G(0x2a2c08));
  iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) * (int)(*(uint32_t*)rtguest::G(0x2a2c4a));
  iVar6 = iVar6 * (*(uint32_t*)rtguest::G(0x2a2c28));
  if (iVar4 < 0) {
    iVar4 = iVar4 + 0xf;
  }
  if (iVar3 < 0) {
    iVar3 = iVar3 + 0xf;
  }
  if (iVar5 < 0) {
    iVar5 = iVar5 + 0xf;
  }
  if (iVar6 < 0) {
    iVar6 = iVar6 + 0xf;
  }
  if (iVar7 < 0) {
    iVar7 = iVar7 + 0xf;
  }
  (*(uint32_t*)rtguest::G(0x2a34d0)) = (long)(*(uint32_t*)rtguest::G(0x2a2c00)) | (long)(*(uint32_t*)rtguest::G(0x2a2c04)) << 0x20;
  (*(uint32_t*)rtguest::G(0x2a3530)) = (long)(*(uint32_t*)rtguest::G(0x2a2c30)) | (long)(*(uint32_t*)rtguest::G(0x2a2c3c)) << 0x38 |
                 (long)(*(uint32_t*)rtguest::G(0x2a2c34)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2a2c3c)) << 0x18 |
                 (long)(*(uint32_t*)rtguest::G(0x2a2c38)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2a2c38)) << 0x30;
  (*(uint32_t*)rtguest::G(0x2a3500)) = (long)(iVar6 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2a2c2c)) |
                 (long)(int)(iVar4 >> 4 & (uint32_t)(*(uint32_t*)rtguest::G(0x2a2c2e))) << 0x10;
  (*(uint32_t*)rtguest::G(0x2a34f0)) = (long)(*(uint32_t*)rtguest::G(0x2a2c10)) | (long)(*(uint32_t*)rtguest::G(0x2a2c1c)) << 0x38 |
                 (long)(*(uint32_t*)rtguest::G(0x2a2c14)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2a2c1c)) << 0x18 |
                 (long)(*(uint32_t*)rtguest::G(0x2a2c18)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2a2c18)) << 0x30;
  (*(uint32_t*)rtguest::G(0x2a3540)) = (long)(int)(iVar7 >> 4 & (uint32_t)(*(uint32_t*)rtguest::G(0x2a2c4e))) << 0x10;
  (*(uint32_t*)rtguest::G(0x2a34c0)) = (long)(iVar5 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2a2c0c)) |
                 (long)(int)(iVar3 >> 4 & (uint32_t)(*(uint32_t*)rtguest::G(0x2a2c0e))) << 0x10;
  (*(uint32_t*)rtguest::G(0x2a34b0)) = (long)(*(uint32_t*)rtguest::G(0x2a2bf0)) | (long)(*(uint32_t*)rtguest::G(0x2a2bfc)) << 0x38 |
                 (long)(*(uint32_t*)rtguest::G(0x2a2bf4)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2a2bfc)) << 0x18 |
                 (long)(*(uint32_t*)rtguest::G(0x2a2bf8)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2a2bf8)) << 0x30;
  (*(uint32_t*)rtguest::G(0x2a3510)) = (long)(*(uint32_t*)rtguest::G(0x2a2c20)) | (long)(*(uint32_t*)rtguest::G(0x2a2c24)) << 0x20;
  (*(uint32_t*)rtguest::G(0x2a3550)) = (long)(*(uint32_t*)rtguest::G(0x2a2c40)) | (long)(*(uint32_t*)rtguest::G(0x2a2c44)) << 0x20;
  (*(uint64_t*)rtguest::G(0x2a34a0)) = (*(uint64_t*)rtguest::G(0x2a34a0)) & 0xffffffffffff8000 | 0xc;
  uVar8 = (*(uint32_t*)rtguest::G(0x332e4c));
  uVar10 = (*(uint32_t*)rtguest::G(0x332e44));
  RT_CALLRA(0x23e440, guest::gifChainEndTag(0x2a3490,0xd));
  RT_CALLRA(0x23e448, guest::sendTexturePackets(0x2a3490));
  fVar9 = (float)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) % 0x168 + -0xb4) * (*(float*)rtguest::G(0x332e50));
  RT_CALLRA(0x23e4a4, guest::vu0UnitMatrix(0x177eb10));
  RT_CALLRA(0x23e4b4, guest::vu0RotMatrixX(rtguest::fdiv(fVar9, 180.0),0x177eb10,0x177eb10));
  (*(uint32_t*)rtguest::G(0x177eb40)) = uVar10;
  (*(uint32_t*)rtguest::G(0x177eb48)) = uVar8;
  (*(uint32_t*)rtguest::G(0x177eb44)) = uVar1;
  return 0;
}


RT_GUEST_FN(0x0023e140, guest::texAnimCopyA)

}  // namespace guest
