// guest:0x002224f8 translated from Ghidra pseudocode and oracle MIPS decode
// (generated/sub_002224F8 comments): M = param_1 * (param_1+0x40) via
// vu0MulMatrix into 4 frame quads; two 21-vertex chains transform points with
// the column-major idiom lqc2 vf4..7 + vmulax/vmadday/vmaddaz/vmaddw -> vf9,
// qmfc2 + prot3w(rotate-left-1) extract .x/.y/.z (oracle PS2_PROT3W), stored
// into the ELF vertex chains; then vif upload + model call.
#include "../guest_vu0.h"
#include "guest_decls.inc"

namespace guest {
using namespace rtguest::vu0;

// one column-major 4x4 * vector transform, vf4..7 = columns, vf8 = point,
// result vf9 (oracle encodings 0x4be821bc..0x4be83a4b)
static inline void xform9() {
  rtguest::FrameGuard rt_fg(0);
  vmulabc(15,0,4,8); vmaddabc(15,1,5,8); vmaddabc(15,2,6,8); vmaddbc(15,3,9,7,8);
}

void drawArcRibbon(uint32_t param_1, long param_2, ...) {
  rtguest::FrameGuard rt_fg(208);
  int iVar1;
  int iVar2;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  uint32_t uVar10;
  uint32_t uVar11;
  uint32_t auStack_d0 = rtguest::allocFrame(0x40);  // 4 quads: auStack_d0/c0/b0/a0

  RT_CALLRA(0x222554, guest::vu0MulMatrix(auStack_d0,param_1,(uint32_t)((int)param_1 + 0x40)));
  uVar11 = *(uint32_t*)rtguest::G(0x00332ce4);
  fVar9 = 6.5;
  uVar10 = 0x41e80000;
  fVar6 = (*(float*)rtguest::G(0x00332cd0));
  fVar8 = (*(float*)rtguest::G(0x00332cd4));
  if (param_2 != 1) {
    if ((param_2 < 2) || (param_2 != 2)) {
      fVar9 = 4.0;
      uVar10 = 0x42480000;
      fVar6 = 26.0;
      fVar8 = -22.0;
    }
    else {
      uVar10 = 0x42ce0000;
      fVar6 = (*(float*)rtguest::G(0x00332cdc));
      fVar8 = (*(float*)rtguest::G(0x00332ce0));
      fVar9 = (*(float*)rtguest::G(0x00332cd8));
    }
  }
  iVar1 = 0;
  iVar2 = 0;
  fVar5 = (*(float*)rtguest::G(0x00332ce8));
  do {
    // quad (0.5, DAT_00332ce4, ram 0x29d318..) at 0x29d310
    (*(uint32_t*)rtguest::G(0x0029d310)) = 0x3f000000;
    (*(uint32_t*)rtguest::G(0x0029d314)) = uVar11;
    S.vf[8] = lqc2(0x0029d310);
    S.vf[4] = lqc2(auStack_d0);
    S.vf[5] = lqc2(auStack_d0 + 0x10);
    S.vf[6] = lqc2(auStack_d0 + 0x20);
    S.vf[7] = lqc2(auStack_d0 + 0x30);
    xform9();
    (*(uint32_t*)rtguest::G((uint32_t)(0x00299e54 + iVar2))) = rtguest::f2bits(S.vf[9].v[0]);
    (*(uint32_t*)rtguest::G((uint32_t)(0x00299e58 + iVar2))) = rtguest::f2bits(S.vf[9].v[1]);
    (*(uint32_t*)rtguest::G((uint32_t)(0x00299e5c + iVar2))) = rtguest::f2bits(S.vf[9].v[2]);
    fVar7 = rtguest::fdiv(((float)(iVar1 + -10) * fVar5), 10.0f);
    fVar4 = rtguest::fbits(RT_CALLRA(0x222678, guest::sinf(rtguest::f2bits(fVar7))));
    (*(float*)rtguest::G(0x0029d320)) = fVar4 * fVar6 + 0.5 + fVar9;
    fVar4 = rtguest::fbits(RT_CALLRA(0x222690, guest::cosf(rtguest::f2bits(fVar7))));
    (*(float*)rtguest::G(0x0029d324)) = fVar4 * fVar6 + fVar8;
    S.vf[8] = lqc2(0x0029d320);
    S.vf[4] = lqc2(auStack_d0);
    S.vf[5] = lqc2(auStack_d0 + 0x10);
    S.vf[6] = lqc2(auStack_d0 + 0x20);
    S.vf[7] = lqc2(auStack_d0 + 0x30);
    xform9();
    (*(uint32_t*)rtguest::G((uint32_t)(0x00299e90 + iVar2))) = rtguest::f2bits(S.vf[9].v[0]);
    (*(uint32_t*)rtguest::G((uint32_t)(0x00299e94 + iVar2))) = rtguest::f2bits(S.vf[9].v[1]);
    (*(uint32_t*)rtguest::G((uint32_t)(0x00299e98 + iVar2))) = rtguest::f2bits(S.vf[9].v[2]);
    iVar1 = iVar1 + 1;
    (*(int32_t*)rtguest::G((uint32_t)(0x00299e8c + iVar2))) = 0;
    (*(int32_t*)rtguest::G((uint32_t)(0x00299ec8 + iVar2))) = 0;
    fVar4 = (*(float*)rtguest::G(0x00332cf0));
    iVar2 = iVar2 + 0x78;
  } while (iVar1 < 0x15);
  iVar1 = 0;
  iVar2 = 0;
  uVar11 = *(uint32_t*)rtguest::G(0x00332cec);
  (*(uint32_t*)rtguest::G(0x0029d328)) = uVar10;
  do {
    (*(uint32_t*)rtguest::G(0x0029d310)) = 0xbf000000;
    (*(uint32_t*)rtguest::G(0x0029d314)) = uVar11;
    S.vf[8] = lqc2(0x0029d310);
    S.vf[4] = lqc2(auStack_d0);
    S.vf[5] = lqc2(auStack_d0 + 0x10);
    S.vf[6] = lqc2(auStack_d0 + 0x20);
    S.vf[7] = lqc2(auStack_d0 + 0x30);
    xform9();
    (*(uint32_t*)rtguest::G((uint32_t)(0x00299458 + iVar2))) = rtguest::f2bits(S.vf[9].v[0]);
    (*(uint32_t*)rtguest::G((uint32_t)(0x0029945c + iVar2))) = rtguest::f2bits(S.vf[9].v[1]);
    (*(uint32_t*)rtguest::G((uint32_t)(0x00299460 + iVar2))) = rtguest::f2bits(S.vf[9].v[2]);
    fVar7 = rtguest::fdiv(((float)(iVar1 + -10) * fVar4), 10.0f);
    fVar5 = rtguest::fbits(RT_CALLRA(0x2227b0, guest::sinf(rtguest::f2bits(fVar7))));
    (*(float*)rtguest::G(0x0029d320)) = (fVar5 * fVar6 - 0.5) - fVar9;
    fVar5 = rtguest::fbits(RT_CALLRA(0x2227c8, guest::cosf(rtguest::f2bits(fVar7))));
    (*(float*)rtguest::G(0x0029d324)) = fVar5 * fVar6 + fVar8;
    S.vf[8] = lqc2(0x0029d320);
    S.vf[4] = lqc2(auStack_d0);
    S.vf[5] = lqc2(auStack_d0 + 0x10);
    S.vf[6] = lqc2(auStack_d0 + 0x20);
    S.vf[7] = lqc2(auStack_d0 + 0x30);
    xform9();
    (*(uint32_t*)rtguest::G((uint32_t)(0x00299494 + iVar2))) = rtguest::f2bits(S.vf[9].v[0]);
    (*(uint32_t*)rtguest::G((uint32_t)(0x00299498 + iVar2))) = rtguest::f2bits(S.vf[9].v[1]);
    (*(uint32_t*)rtguest::G((uint32_t)(0x0029949c + iVar2))) = rtguest::f2bits(S.vf[9].v[2]);
    iVar1 = iVar1 + 1;
    (*(int32_t*)rtguest::G((uint32_t)(0x00299490 + iVar2))) = 0;
    (*(int32_t*)rtguest::G((uint32_t)(0x002994cc + iVar2))) = 0;
    iVar2 = iVar2 + 0x78;
  } while (iVar1 < 0x15);
  RT_CALLRA(0x222840, guest::vifUploadPacket(0x29d330,3));
  RT_CALLRA(0x22284c, guest::vifCallModel(0x299430));
  return;
}


RT_GUEST_FN(0x002224f8, guest::drawArcRibbon)

}  // namespace guest
