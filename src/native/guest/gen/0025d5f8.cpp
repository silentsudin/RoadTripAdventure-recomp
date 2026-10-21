// guest:0x0025d5f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t drawFieldProp27Anim(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(160);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x88) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x8c) = 0x00000000u;
  }
  float fVar1;
  int iVar2;
  uint uVar3;
  float fVar4;
uint32_t auStack_a0 = rtguest::allocFrame(48);
uint32_t auStack_70 = rtguest::allocFrame(16);
  
  uVar3 = 0;
  RT_CALLRA(0x25d668, guest::vifFixedRegs(0x20079f05dd30bcb8,7,0,0xf));
  fVar1 = (*(float*)rtguest::G(0x3330dc));
  do {
    RT_CALLRA(0x25d678, guest::vu0UnitMatrix(auStack_a0));
    fVar4 = 180.0;
    if ((uVar3 & 3) == 0) {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) % 0x168 + -0xb4;
LAB_0025d6e8:
      RT_CALLRA(0x25d704, guest::vu0RotMatrixZ(rtguest::fdiv(((float)iVar2 * fVar1), fVar4),auStack_a0,auStack_a0));
    }
    else {
      if ((uVar3 & 3) == 1) {
        fVar4 = 720.0;
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) % 0x5a0 + -0x2d0;
        goto LAB_0025d6e8;
      }
      RT_CALLRA(0x25d74c, guest::vu0RotMatrixZ(rtguest::fdiv(((float)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) % 0x2d0 + -0x168) * fVar1), 360.0),auStack_a0,
                    auStack_a0));
    }
    iVar2 = uVar3 * 0x10;
    uVar3 = uVar3 + 1;
    RT_CALLRA(0x25d768, guest::tileSlotOrigin(auStack_70,iVar2 + 0x2c4250,3));
    RT_CALLRA(0x25d770, guest::vifObjectMatrix(auStack_a0));
    RT_CALLRA(0x25d780, guest::vifCallModel((*(int32_t*)rtguest::G(0x177f5c8)) + 0x10));
    if (0x15 < (int)uVar3) {
      return 0;
    }
  } while( true );
}


RT_GUEST_FN(0x0025d5f8, guest::drawFieldProp27Anim)

}  // namespace guest
