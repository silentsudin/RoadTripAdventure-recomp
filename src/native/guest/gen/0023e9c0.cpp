// guest:0x0023e9c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t SubmitOscillatingTextureLayout(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
  }
uint32_t puVar1;
  short sVar2;
  short sVar3;
  int iVar4;
uint32_t puVar5;
uint32_t puVar6;
  int iVar7;
uint32_t pfVar8;
uint32_t pfVar9;
uint32_t ppuVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  float fVar13;
  
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) == 1) {
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0;
    puVar6 = 0x017526f8;
    iVar7 = 2;
    do {
      iVar7 = iVar7 + -1;
      (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = 0x43340000;
      puVar6 = puVar6 - sizeof(uint32_t);  // W13: fn/0023e9c0.c:31 `puVar6 + -1` on undefined4* (was -1 byte)
    } while (-1 < iVar7);
  }
  ppuVar10 = 0x2a3766u;
  pfVar9 = (uint32_t)0x002a3770;
  puVar6 = 0x002a3780;
  pfVar8 = (uint32_t)0x017526f0;
  iVar7 = 0;
  puVar5 = 0x0177eb10;
  do {
    iVar4 = iVar7 * 0x40;
    fVar13 = rtguest::fdiv(((float)((int)(short)(int)(*(float*)rtguest::G((uint32_t)pfVar8)) % 0x168 + -0xb4) * (*(float*)rtguest::G(0x332e68))), 180.0);
    iVar7 = iVar7 + 1;
    RT_CALLRA(0x23eac8, guest::vu0UnitMatrix(puVar5));
    RT_CALLRA(0x23ead8, guest::vu0RotMatrixX(fVar13,puVar5,puVar5));
    uVar11 = (*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(1) * sizeof(uint32_t)));
    uVar12 = (*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(2) * sizeof(uint32_t)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x30))) = (*(uint32_t*)rtguest::G((uint32_t)puVar6));
    iVar4 = iVar4 + 0x177ec10;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x34))) = uVar11;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x38))) = uVar12;
    RT_CALLRA(0x23eb08, guest::vu0UnitMatrix(iVar4));
    RT_CALLRA(0x23eb18, guest::vu0RotMatrixY((*(uint32_t*)rtguest::G(0x00332e6c)),iVar4,iVar4));
    RT_CALLRA(0x23eb28, guest::vu0RotMatrixZ(fVar13,iVar4,iVar4));
    sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)ppuVar10 + -6)));
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
    puVar1 = puVar6 + 9;
    uVar12 = (*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(10) * sizeof(uint32_t)));
    uVar11 = (*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(0xb) * sizeof(uint32_t)));
    puVar6 = puVar6 + 0xc;  // T* element step
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x130))) = (*(uint32_t*)rtguest::G((uint32_t)puVar1));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x134))) = uVar12;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x138))) = uVar11;
    puVar5 = puVar5 + 0x40;
    if (sVar2 == 0) {
      trap(7);
    }
    sVar3 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)ppuVar10));
    ppuVar10 = (uint32_t)((int)ppuVar10 + 2);
    if ((long)(iVar4 % (int)sVar2) < (long)sVar3) {
      fVar13 = (*(float*)rtguest::G((uint32_t)pfVar8)) + (*(float*)rtguest::G((uint32_t)pfVar9));
    }
    else {
      fVar13 = (*(float*)rtguest::G((uint32_t)pfVar8)) - (*(float*)rtguest::G((uint32_t)pfVar9));
    }
    (*(float*)rtguest::G((uint32_t)pfVar8)) = fVar13;
    pfVar9 = pfVar9 + 0x4;  // T* element step
    pfVar8 = pfVar8 + 0x4;  // T* element step
  } while (iVar7 < 3);
  iVar7 = (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) * (int)(*(uint32_t*)rtguest::G(0x2a2e0a));
  if (iVar7 < 0) {
    iVar7 = iVar7 + 0xf;
  }
  (*(uint32_t*)rtguest::G(0x2a34b0)) = (long)(*(uint32_t*)rtguest::G(0x2a2df0)) | (long)(*(uint32_t*)rtguest::G(0x2a2dfc)) << 0x38 |
                 (long)(*(uint32_t*)rtguest::G(0x2a2df4)) << 0x20 | (long)(*(uint32_t*)rtguest::G(0x2a2dfc)) << 0x18 |
                 (long)(*(uint32_t*)rtguest::G(0x2a2df8)) << 0x10 | (long)(*(uint32_t*)rtguest::G(0x2a2df8)) << 0x30;
  (*(uint32_t*)rtguest::G(0x2a34d0)) = (long)(*(uint32_t*)rtguest::G(0x2a2e00)) | (long)(*(uint32_t*)rtguest::G(0x2a2e04)) << 0x20;
  (*(uint32_t*)rtguest::G(0x2a34c0)) = ((long)(iVar7 >> 4) & (uint32_t)(*(uint32_t*)rtguest::G(0x2a2e0e))) << 0x10;
  RT_CALLRA(0x23ec3c, guest::AdvanceOscillatingValue(param_1 + 2));
  (*(uint64_t*)rtguest::G(0x2a34a0)) = (*(uint64_t*)rtguest::G(0x2a34a0)) & 0xffffffffffff8000 | 4;
  RT_CALLRA(0x23ec60, guest::gifChainEndTag(0x2a3490,5));
  sendTexturePackets(0x2a3490);
  return 0;
}


RT_GUEST_FN(0x0023e9c0, guest::SubmitOscillatingTextureLayout)

}  // namespace guest
