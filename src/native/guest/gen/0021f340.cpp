// guest:0x0021f340 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t ComposeCameraMatrix(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(208);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0xc0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc4) = 0x00000000u;
  }
uint32_t auStack_d0 = rtguest::allocFrame(12);
    uint32_t uStack_c4 = rtguest::allocFrame(4);
uint32_t auStack_c0 = rtguest::allocFrame(16);
uint32_t auStack_b0 = rtguest::allocFrame(16);
uint32_t auStack_a0 = rtguest::allocFrame(32);
uint32_t auStack_80 = rtguest::allocFrame(16);
uint32_t auStack_70 = rtguest::allocFrame(16);
uint32_t auStack_60 = rtguest::allocFrame(32);
  int iVar1;
  
  RT_CALLRA(0x21f374, guest::TransformRectangle(auStack_d0,(int)param_2 + 0x14));
  RT_CALLRA(0x21f380, guest::vu0UnitMatrix(auStack_c0));
  RT_CALLRA(0x21f394, guest::vu0RotMatrixX((*(uint32_t*)rtguest::G((uint32_t)auStack_d0 + (uint32_t)(0) * 4)),auStack_80,auStack_c0));
  RT_CALLRA(0x21f3a4, guest::vu0RotMatrixY((*(uint32_t*)rtguest::G((uint32_t)uStack_c4)),auStack_c0,auStack_80));
  if (param_3 == 0) {
    RT_CALLRA(0x21f3f0, guest::vu0CopyVector(param_1,param_2,auStack_c0));
  }
  else {
    // fn/0021f340.c:27-28 as Ghidra wrote it. MIPS: jal 0x272758 @0x21f3bc delay `move $4,$18` => (c0, $5=$16 param_3,
    // $6=$18 c0 from 0x21f3b8); jal @0x21f3cc: $4=$19 (sp+0x50), $6=$4, delay `move $5,$21` => (80, param_4, 80).
    // Delay-slot instructions execute BEFORE the callee, so they define the args.
    RT_CALLRA(0x21f3bc, guest::vu0MulMatrix(auStack_c0,param_3,auStack_c0));
    RT_CALLRA(0x21f3cc, guest::vu0MulMatrix(auStack_80,param_4,auStack_80));
    RT_CALLRA(0x21f3dc, guest::vu0ApplyMatrix(param_1,auStack_c0,param_2));
  }
  iVar1 = (int)param_1;
  // fn/0021f340.c:35-38 (Ghidra was right): MIPS 0x21f3fc..0x21f424, each jal's delay slot sets $4 (dst) and the
  // preceding addiu sets $5: (+0x10,sp+0x20) (+0x20,sp+0x30) (+0x30,sp+0x60) (+0x40,sp+0x70); out+0x00 keeps
  // vu0ApplyMatrix's (or the copy's) result.
  RT_CALLRA(0x21f3fc, guest::vu0CopyVector(iVar1 + 0x10,auStack_b0));
  RT_CALLRA(0x21f408, guest::vu0CopyVector(iVar1 + 0x20,auStack_a0));
  RT_CALLRA(0x21f414, guest::vu0CopyVector(iVar1 + 0x30,auStack_70));
  RT_CALLRA(0x21f420, guest::vu0CopyVector(iVar1 + 0x40,auStack_60));
  return 0;
}


RT_GUEST_FN(0x0021f340, guest::ComposeCameraMatrix)

}  // namespace guest
