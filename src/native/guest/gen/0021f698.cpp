// guest:0x0021f698 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

// Machine-level fix (disasm 0x21f748 + delay slot): jal ComposeCameraMatrix's
// delay slot move $4,$sp sets the callee's $a0 to the caller slot, but $a1..$a3
// carry param_2/3/4 live — Ghidra's 1-arg call site dropped them. Same clobber
// at jal 0x21f6f4: vu0CopyVector #1 copies param_2 -> auStack_1d0.
uint32_t buildCamera(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(464);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x198) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x19c) = 0x00000000u;
  }
uint32_t auStack_1d0 = rtguest::allocFrame(16);
uint32_t auStack_1c0 = rtguest::allocFrame(16);
uint32_t auStack_1b0 = rtguest::allocFrame(16);
uint32_t auStack_1a0 = rtguest::allocFrame(16);
uint32_t auStack_190 = rtguest::allocFrame(16);
uint32_t afStack_180 = rtguest::allocFrame(20);
    uint32_t fStack_16c = rtguest::allocFrame(4);
    rtguest::allocFrame(16);  // guest frame gap
    uint32_t uStack_158 = rtguest::allocFrame(4);
    uint32_t uStack_154 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_148 = rtguest::allocFrame(4);
    uint32_t uStack_144 = rtguest::allocFrame(4);
uint32_t afStack_140 = rtguest::allocFrame(20);
    uint32_t fStack_12c = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_120 = rtguest::allocFrame(4);
    uint32_t uStack_11c = rtguest::allocFrame(4);
    uint32_t uStack_118 = rtguest::allocFrame(4);
    uint32_t uStack_114 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_108 = rtguest::allocFrame(4);
    uint32_t uStack_104 = rtguest::allocFrame(4);
uint32_t auStack_100 = rtguest::allocFrame(64);
uint32_t auStack_c0 = rtguest::allocFrame(64);
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 uVar5;
  float fVar6;
  
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1c))) & 0x1000) == 0) {
    RT_CALLRA(0x21f748, guest::ComposeCameraMatrix(auStack_1d0,param_2,param_3,param_4));
  }
  else {
    RT_CALLRA(0x21f6f4, guest::vu0CopyVector(auStack_1d0,param_2));
    RT_CALLRA(0x21f704, guest::vu0CopyVector(auStack_1c0,param_3));
    iVar4 = (int)param_3;
    RT_CALLRA(0x21f714, guest::vu0CopyVector(auStack_1b0,iVar4 + 0x10));
    RT_CALLRA(0x21f724, guest::vu0CopyVector(auStack_1a0,iVar4 + 0x20));
    RT_CALLRA(0x21f734, guest::vu0CopyVector(auStack_190,iVar4 + 0x30));
  }
  iVar4 = (int)param_1;
  fVar6 = (*(float*)rtguest::G(0x332c84));
  { // Callee-saved s0..s7 live in this frame's registers at jal 0x21f784; BuildCameraViewMatrices' prologue
    // `sd $s0..$s7,0x60..0x98($sp)` (words 0xffb00060 0xffb10068 0xffb20070 0xffb30078 0xffb40080 0xffb50088
    // 0xffb60090 0xffb70098 @0x21f454..0x21f484; opcode 0x3f = SD, plain MIPS, NOT a PQ quad op) stores them into the
    // callee frame (callee sp = frameTop-0xd0). The gen callee has no register model, so the caller seeds the slots.
    // Values from this fn's prologue: s0=a2 (0x21f6a0), s1=a0 (0x21f6f0), s2=a1 (0x21f6ac), s3=sp+0x10, s6=sp+0x20,
    // s7=sp+0x30, s5=sp+0x40, s4=sp+0xd0 (0x21f700..0x21f740 / 0x21f750..0x21f760, both branches).
    uint32_t sp = rtguest::frameTop(), csp = sp - 208;
    const uint32_t sv[8] = {param_3, param_1, param_2, sp + 0x10, sp + 0xd0, sp + 0x40, sp + 0x20, sp + 0x30};
    for (int i = 0; i < 8; i++) {
      *(uint32_t*)rtguest::G(csp + 0x60 + 8 * i) = sv[i];
      *(uint32_t*)rtguest::G(csp + 0x64 + 8 * i) = 0;
    }
  }
  RT_CALLRA(0x21f784, guest::BuildCameraViewMatrices((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10))),auStack_100,auStack_c0,auStack_1d0,auStack_1b0,
             auStack_1c0,iVar4 + 0x100));
  uVar3 = (*(uint32_t*)rtguest::G(0x3356e4));
  (*(float*)rtguest::G((uint32_t)fStack_12c)) = (*(float*)rtguest::G(0x332c88));
  (*(float*)rtguest::G((uint32_t)afStack_180 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10)));
  if ((*(uint32_t*)rtguest::G(0x333288)) == 0) {
    fVar6 = 1.0;
  }
  uVar5 = (*(uint32_t*)rtguest::G(0x3356e0));
  RT_CALLRA(0x21f7b0, guest::vu0UnitMatrix(afStack_140));
  // MIPS 0x21f7b8 mul.s $f20,$f21,$f20: fov * DAT_00332c88 held in REGISTERS across vu0UnitMatrix(afStack_140)
  // (Ghidra's fStack_12c/afStack_180[0] memory form reads values the unit matrix just overwrote).
  (*(float*)rtguest::G((uint32_t)fStack_12c)) = (*(float*)rtguest::G((uint32_t)(param_2 + 0x10))) * (*(float*)rtguest::G(0x332c88));
  (*(float*)rtguest::G((uint32_t)afStack_140 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_180 + (uint32_t)(0) * 4)) * fVar6;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_118)) = (*(uint32_t*)rtguest::G(0x332c8c));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_108)) = (*(uint32_t*)rtguest::G(0x332c90));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_11c)) = uVar3;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_114)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_104)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_120)) = uVar5;
  RT_CALLRA(0x21f7f0, guest::vu0UnitMatrix(afStack_180));
  // MIPS 0x21f808 mul.s $f21,$f21,$f1 ($f21 = fov from 0x21f794, $f1 = 1/512), stored to afStack_180[0] and [5] after
  // vu0UnitMatrix(afStack_180): fov/512, not unit[0]/512.
  (*(float*)rtguest::G((uint32_t)afStack_180 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)(param_2 + 0x10))) * 0.001953125f;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_158)) = (*(uint32_t*)rtguest::G(0x332c94));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_148)) = (*(uint32_t*)rtguest::G(0x332c98));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_154)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_144)) = 0;
  (*(float*)rtguest::G((uint32_t)fStack_16c)) = (*(float*)rtguest::G((uint32_t)afStack_180 + (uint32_t)(0) * 4));
  RT_CALLRA(0x21f824, guest::vu0UnitMatrix((uint32_t)(iVar4 + 0x80)));
  uVar2 = (*(uint32_t*)rtguest::G(0x332ca4));
  uVar1 = (*(uint32_t*)rtguest::G(0x332ca0));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x94))) = (*(uint32_t*)rtguest::G(0x332c9c));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xa8))) = uVar1;
  (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x80))) = fVar6 * 512.0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xb8))) = uVar2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xbc))) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xb0))) = uVar5;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xb4))) = uVar3;
  RT_CALLRA(0x21f868, guest::vu0MulMatrix(param_1,afStack_140,auStack_100));
  RT_CALLRA(0x21f87c, guest::vu0MulMatrix(iVar4 + 0x40,afStack_180,auStack_100));
  RT_CALLRA(0x21f89c, guest::BuildCameraViewMatrices(0,auStack_100,auStack_c0,auStack_1d0,auStack_190,auStack_1a0,0));
  RT_CALLRA(0x21f8ac, guest::vu0MulMatrix(iVar4 + 0xc0,afStack_180,auStack_100));
  RT_CALLRA(0x21f8b8, guest::vu0CopyVector(iVar4 + 0x180,auStack_1b0));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18c))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10)));
  return 0;
}


RT_GUEST_FN(0x0021f698, guest::buildCamera)

}  // namespace guest
