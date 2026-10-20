// guest:0x0021f340 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

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
    // Machine args (disasm 0x21f3bc/0x21f3c8 + delay slots): jal #1 carries
    // $4=$17 (param_1), $5=$16 (param_3), delay $6=auStack_c0; jal #2's $5 is
    // still $16 (param_3, not param_4). Ghidra dropped the register state.
    RT_CALLRA(0x21f3bc, guest::vu0MulMatrix(param_1,param_3,auStack_c0));
    RT_CALLRA(0x21f3cc, guest::vu0MulMatrix(auStack_80,param_3,auStack_80));
    RT_CALLRA(0x21f3dc, guest::vu0ApplyMatrix(param_1,auStack_c0,param_2));
  }
  iVar1 = (int)param_1;
  // Machine-exact tail copies (full disasm 0x21f3fc-0x21f424, delay-slot args
  // run BEFORE the callee so they ARE the callee's args):
  //   jal#2 21f3fc: $4=$17+0x00, $5=+0x20 (b-delay 21f3e8)
  //   jal#3 21f408: $4=$17+0x10 (21f404), $5=+0x30 (21f40c delay)
  //   jal#4 21f414: $4=$17+0x30 (21f410), $5=+0x60 (21f418 delay)
  //   jal#5 21f420: $4=$17+0x40 (21f41c), $5=+0x70 (21f424 delay)
  // dsts are +0x00/+0x10/+0x30/+0x40: column +0x20 keeps the mulMatrix/
  // applyMatrix write. Attract V is stack feedback through these aliased
  // slots, not live camera math.
  RT_CALLRA(0x21f3fc, guest::vu0CopyVector(iVar1,auStack_b0));
  RT_CALLRA(0x21f408, guest::vu0CopyVector(iVar1 + 0x10,auStack_a0));
  RT_CALLRA(0x21f414, guest::vu0CopyVector(iVar1 + 0x30,auStack_70));
  RT_CALLRA(0x21f420, guest::vu0CopyVector(iVar1 + 0x40,auStack_60));
  return 0;
}


RT_GUEST_FN(0x0021f340, guest::ComposeCameraMatrix)

}  // namespace guest
