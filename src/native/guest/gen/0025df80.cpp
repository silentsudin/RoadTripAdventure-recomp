// guest:0x0025df80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t balloonNpcUpdate(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
    rtguest::setS32(3, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(4, (uint32_t)param_1);  // walk: = param_1
  }
  byte bVar1;
  byte bVar2;
uint32_t puVar3;
  bool bVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  int iVar7;
uint32_t puVar8;
uint32_t puVar9;
  float fVar10;
  float fVar11;
  undefined4 uVar12;
  float fVar13;
  float fVar14;
uint32_t auStack_50 = rtguest::allocFrame(16);
uint32_t auStack_40 = rtguest::allocFrame(4);
uint32_t auStack_3c = rtguest::allocFrame(12);
  
  fVar13 = rtguest::fbits(*(uint32_t*)rtguest::G(0x3330f0));  // DAT_003330f0 is float (fn:30, lwc1)
  puVar9 = (uint32_t)param_2;
  puVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
  bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 6)));
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar9 + 5))) & 0x8000) == 0) {
    bVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x19)));
    if (bVar2 == 1) {
      // puVar3[1]/[2]/[4] slots and DAT_003330e0..ec are float (lwc1/swc1, disasm 0x25dffc-0x25e05c; fn:37-42) — bits via fbits/float lvalues.
      fVar11 = rtguest::fbits(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t))) - rtguest::fbits(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t)));
      fVar10 = rtguest::fbits(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t))) + rtguest::fbits(*(uint32_t*)rtguest::G(0x3330ec));
      fVar14 = fVar11 - rtguest::fbits(*(uint32_t*)rtguest::G(0x3330e8));
      (*(float*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t))) = fVar11;
      (*(float*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t))) = fVar10;
      (*(float*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) = rtguest::fbits(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) - fVar13;
      if (fVar14 <= 78.0) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x19))) = 2;
      }
    }
    else if (bVar2 < 2) {
      if (bVar2 == 0) {
        // disasm 0x25e028-0x25e05c: compare first; the swc1 of [2] runs only on the true path (bc1f at 0x25e058 skips it).
        fVar14 = rtguest::fbits(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) - (*(float*)rtguest::G(0x3330e0));
        if (fVar14 < rtguest::fbits(*(uint32_t*)rtguest::G(0x3330e4))) {
          (*(float*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) = fVar14;
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x19))) = 1;
        }
      }
    }
    else if (bVar2 == 2) {
      RT_CALLRA(0x0025e084, rtCall((*(uint32_t*)rtguest::G(0x335710)), puVar3, auStack_40, auStack_3c, 0, param_2));
      // float domain (lwc1/swc1, disasm 0x25e0c8-0x25e0f8; fn:56-57)
      fVar13 = rtguest::fbits(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t))) + rtguest::fbits(*(uint32_t*)rtguest::G(0x3330f8));
      bVar4 = rtguest::fbits(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) - rtguest::fbits(*(uint32_t*)rtguest::G(0x3330f4)) < rtguest::fbits(*(uint32_t*)rtguest::G(0x3330fc));
      (*(float*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) = rtguest::fbits(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) - rtguest::fbits(*(uint32_t*)rtguest::G(0x3330f4));
      (*(float*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t))) = fVar13;
      if (bVar4) {
        iVar7 = RT_CALLRA(0x25e0c0, guest::__umoddi3((*(uint64_t*)rtguest::G((uint32_t)puVar9)),5));
        uVar6 = (*(uint32_t*)rtguest::G(0x333108));
        uVar5 = (*(uint32_t*)rtguest::G(0x333104));
        uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002c4bf0 + iVar7 * 4)));
        (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G(0x333100));
        (*(uint32_t*)rtguest::G((uint32_t)puVar3)) = uVar12;
        (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) = uVar5;
        (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t))) = uVar6;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x19))) = 0;
      }
    }
    RT_CALLRA(0x25e10c, guest::vu0SubVector(auStack_50,puVar3,0x177ace0));
    fVar13 = rtguest::fbits(RT_CALLRA(0x25e118, guest::vecDotProductVu0(auStack_50,auStack_50)));
    if (rtguest::fbits(*(uint32_t*)rtguest::G(0x33310c)) <= SQRT(fVar13)) {  // DAT_0033310c float (fn:74)
      fVar13 = rtguest::fbits(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t)));
      goto LAB_0025e178;
    }
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x19))) = 3;
  }
  fVar13 = rtguest::fbits(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t)));  // (float) cast on undefined4* = reinterpret (fn: "fVar13 = (float)puVar3[2]")
LAB_0025e178:
  if ((820.0 <= fVar13) || (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 1) != 0)) {
    (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(3) * sizeof(uint32_t))) = 0x3f800000;
  }
  puVar8 = 0x0177eb10 + (uint32_t)bVar1 * 0x40;
  RT_CALLRA(0x25e1c4, guest::vu0UnitMatrix(puVar8));
  if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x19))) != '\x03') && (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar9 + 5))) & 0x8000) == 0)) {
    RT_CALLRA(0x25e238, guest::vu0RotMatrixX(rtguest::fdiv(((float)(0x14 - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) % 0x28) * (*(float*)rtguest::G(0x333110))), 20.0),puVar8,puVar8
                 ));
  }
  RT_CALLRA(0x25e24c, guest::vu0CopyVector(0x0177eb40 + (uint32_t)bVar1 * 0x10,puVar3));
  return 0;
}


RT_GUEST_FN(0x0025df80, guest::balloonNpcUpdate)

}  // namespace guest
