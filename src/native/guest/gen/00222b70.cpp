// guest:0x00222b70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t drawCarsAction(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(400);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x170, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x148, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x150, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x158, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x160, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x168, 8);
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0x188);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x140, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x178, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x180) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x184) = 0x00000000u;
    rtguest::gS[2] = 0x0000000000000017ull;  // walk const
    rtguest::setS32(3, (uint32_t)param_2);  // walk: = param_2
    rtguest::gS[4] = 0x0000000000290000ull;  // walk const
    rtguest::gS[6] = 0x0000000001780000ull;  // walk const
  }
uint32_t auStack_190 = rtguest::allocFrame(60);
    uint32_t fStack_154 = rtguest::allocFrame(4);
uint32_t acStack_150 = rtguest::allocFrame(32);
    uint32_t uStack_130 = rtguest::allocFrame(4);
    uint32_t uStack_12c = rtguest::allocFrame(4);
    uint32_t uStack_128 = rtguest::allocFrame(4);
    uint32_t uStack_124 = rtguest::allocFrame(4);
    uint32_t uStack_120 = rtguest::allocFrame(4);
    uint32_t uStack_11c = rtguest::allocFrame(4);
    uint32_t uStack_118 = rtguest::allocFrame(4);
    uint32_t uStack_114 = rtguest::allocFrame(4);
uint32_t auStack_110 = rtguest::allocFrame(4);
    uint32_t uStack_10c = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t uStack_fc = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t uStack_ec = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t uStack_dc = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_d0 = rtguest::allocFrame(64);
uint32_t auStack_90 = rtguest::allocFrame(16);
    uint32_t fStack_80 = rtguest::allocFrame(4);
    uint32_t fStack_7c = rtguest::allocFrame(4);
    uint32_t fStack_78 = rtguest::allocFrame(4);
  char cVar1;
  byte bVar2;
  int iVar3;
uint32_t pcVar4;
uint32_t puVar5;
  int iVar6;
  float fVar7;
  
  iVar6 = 0x17;
  RT_CALLRA(0x222bac, guest::vifUploadAt4());
  RT_CALLRA(0x222bbc, guest::vifUploadAt16(0x177f5e0));
  RT_CALLRA(0x222bcc, guest::vifDirect(0x29d290,4));
  puVar5 = 0x177ac50;
  do {
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x33))) != 0) {
      RT_CALLRA(0x222bf8, guest::ToRelativeVec4(puVar5 + 6,puVar5 + 0x14,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x58))),
                     (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x5c)))));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_130)) = 0x3f800000;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_124)) = 0x3f000000;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_12c)) = 0x3f800000;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_128)) = 0x3f800000;
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x194)));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_120)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x144)));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_11c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x154)));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_118)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x164)));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_114)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x174)));
      RT_CALLRA(0x222c44, guest::vifUploadColours(uStack_130,uStack_120));
      RT_CALLRA(0x222c50, guest::vu0CopyMatrix(auStack_110,puVar5));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_ec)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_fc)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_10c)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_dc)) = 0;
      RT_CALLRA(0x222c68, guest::vifObjectMatrix(auStack_110));
      iVar3 = iVar3 * 0x44;
      RT_CALLRA(0x222cc8, guest::vifBatchRegs((long)(iVar3 + 0x243e) | 0x1d308000U | (long)(iVar3 + 0x247e) << 0x25 |
                   0x20000005c0000000,7,0x1fc401fc40a,9));
      RT_CALLRA(0x222cd0, guest::vifCallModel(0x290570));
    }
    iVar6 = iVar6 + -1;
    puVar5 = puVar5 + 0x270;  // T* element step
  } while (-1 < iVar6);
  RT_CALLRA(0x222cf0, guest::vifDirect(0x29d240,5));
  RT_CALLRA(0x222cfc, guest::vifUploadLightAt16(0x177f5e0));
  puVar5 = 0x177ac50;
  (*(uint32_t*)rtguest::G(0x3356e8)) = 0;
  iVar6 = 0;
  do {
    pcVar4 = acStack_150 + iVar6;
    (*(int8_t*)rtguest::G((uint32_t)pcVar4)) = -1;
    iVar6 = iVar6 + 1;
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x33))) != 0) {
      RT_CALLRA(0x222d48, guest::vu0MulMatrix(auStack_190,param_1,puVar5));
      fVar7 = rtguest::fdiv((*(float*)rtguest::G((uint32_t)fStack_154)), (*(float*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x18c))));
      if ((*(float*)rtguest::G(0x332cf8)) <= fVar7) {
        RT_CALLRA(0x222d7c, guest::vu0CopyMatrix(auStack_d0,puVar5));
        (*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 4))) = -(*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 4)));
        (*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x14))) = -(*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x14)));
        (*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x24))) = -(*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x24)));
        (*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x34))) =
             ((*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x144))) + (*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x144)))) -
             (*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x34)));
        RT_CALLRA(0x222dc0, guest::vu0CopyMatrix(auStack_90,0x177f5e0));
        (*(float*)rtguest::G((uint32_t)fStack_80)) = -(*(float*)rtguest::G((uint32_t)fStack_80));
        (*(float*)rtguest::G((uint32_t)fStack_7c)) = -(*(float*)rtguest::G((uint32_t)fStack_7c));
        (*(float*)rtguest::G((uint32_t)fStack_78)) = -(*(float*)rtguest::G((uint32_t)fStack_78));
        RT_CALLRA(0x222dec, guest::vifUploadLightAt16(auStack_90));
        RT_CALLRA(0x222e0c, guest::drawCar((*(uint32_t*)rtguest::G(0x332cfc)),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x11c))),puVar5,
                (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x194))),(*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x19c))) >> 0xc & 3));
        RT_CALLRA(0x222e18, guest::vu0CopyMatrix(puVar5,auStack_d0));
        RT_CALLRA(0x222e20, guest::vifUploadLightAt16(0x177f5e0));
        cVar1 = RT_CALLRA(0x222e40, guest::drawCar(fVar7,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x11c))),puVar5,
                        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x194))),
                        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x19c))) >> 0xc & 3));
        (*(int8_t*)rtguest::G((uint32_t)pcVar4)) = cVar1;
      }
    }
    puVar5 = puVar5 + 0x270;  // T* element step
  } while (iVar6 < 0x18);
  RT_CALLRA(0x222e64, guest::vifUploadColours(0x29d2d0,0x29d2e0));
  puVar5 = 0x177ac50;
  iVar6 = 0;
  RT_CALLRA(0x222e88, guest::vifDirect(0x29d2f0,2));
  do {
    pcVar4 = acStack_150 + iVar6;
    iVar6 = iVar6 + 1;
    if (-1 < (*(int8_t*)rtguest::G((uint32_t)pcVar4))) {
      bVar2 = 2;
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x1fe))) == '\0') {
        bVar2 = 0;
      }
      RT_CALLRA(0x222ec0, guest::drawCarExtraParts(puVar5,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x194))),
                        bVar2 | (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x1fc))) != '\0',(*(int8_t*)rtguest::G((uint32_t)pcVar4))));
    }
    puVar5 = puVar5 + 0x270;  // T* element step
  } while (iVar6 < 0x18);
  return 0;
}


RT_GUEST_FN(0x00222b70, guest::drawCarsAction)

}  // namespace guest
