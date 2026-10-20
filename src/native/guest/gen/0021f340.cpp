// guest:0x0021f340 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ComposeCameraMatrix(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(208);
uint32_t auStack_d0 = rtguest::allocFrame(12);
    uint32_t uStack_c4 = rtguest::allocFrame(4);
uint32_t auStack_c0 = rtguest::allocFrame(16);
uint32_t auStack_b0 = rtguest::allocFrame(16);
uint32_t auStack_a0 = rtguest::allocFrame(32);
uint32_t auStack_80 = rtguest::allocFrame(16);
uint32_t auStack_70 = rtguest::allocFrame(16);
uint32_t auStack_60 = rtguest::allocFrame(32);
  int iVar1;
  
  TransformRectangle(auStack_d0,(int)param_2 + 0x14);
  vu0UnitMatrix(auStack_c0);
  vu0RotMatrixX((*(uint32_t*)rtguest::G((uint32_t)auStack_d0 + (uint32_t)(0) * 4)),auStack_80,auStack_c0);
  vu0RotMatrixY((*(uint32_t*)rtguest::G((uint32_t)uStack_c4)),auStack_c0,auStack_80);
  if (param_3 == 0) {
    vu0CopyVector(param_1,param_2,auStack_c0);
  }
  else {
    // Machine args (disasm 0x21f3bc/0x21f3c8 + delay slots): jal #1 carries
    // $4=$17 (param_1), $5=$16 (param_3), delay $6=auStack_c0; jal #2's $5 is
    // still $16 (param_3, not param_4). Ghidra dropped the register state.
    vu0MulMatrix(param_1,param_3,auStack_c0);
    vu0MulMatrix(auStack_80,param_3,auStack_80);
    vu0ApplyMatrix(param_1,auStack_c0,param_2);
  }
  iVar1 = (int)param_1;
  // Machine-exact dsts (disasm 0x21f3fc-0x21f424): the addiu $4,$17,+N in each
  // delay slot feeds the NEXT jal; $4 at jal 0x21f3fc is still $17 (+0x00).
  // Ghidra shifted every dst one quad high. The applyMatrix/mulMatrix writes
  // to param_1+0..0x30 are therefore overwritten here: attract V is stack
  // feedback state through these aliased slots, not live camera math.
  vu0CopyVector(iVar1,auStack_b0);
  vu0CopyVector(iVar1 + 0x10,auStack_a0);
  vu0CopyVector(iVar1 + 0x20,auStack_70);
  vu0CopyVector(iVar1 + 0x30,auStack_60);
  return 0;
}


RT_GUEST_FN(0x0021f340, guest::ComposeCameraMatrix)

}  // namespace guest
