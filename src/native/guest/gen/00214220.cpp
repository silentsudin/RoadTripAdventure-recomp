// guest:0x00214220 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ResetControllerConfigDefaults(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(144);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x68, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x78, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x80) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x84) = 0x00000000u;
    rtguest::gS[0] = 0x0000000000000000ull;  // walk const
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
  }
  undefined4 uVar1;
  undefined4 uVar2;
  bool bVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  undefined8 uVar7;
uint32_t piVar8;
  int iVar9;
  undefined8 uVar10;
  undefined8 uVar11;
  uint uVar12;
  undefined8 uVar13;
  undefined8 uVar14;
  uint uVar15;
uint32_t puVar16;
uint32_t puVar17;
uint32_t piVar18;
  int iVar19;
  int iVar20;
uint32_t piVar21;
  
  bVar3 = false;
  iVar20 = (int)param_2;
  piVar21 = (uint32_t)(iVar20 + 0x10);
  RT_CALLRA(0x214244, guest::GetControllerButtonConfig());
  uVar4 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)piVar21));
  uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar20 + 0x18)));
  uVar10 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar20 + 0x20)));
  uVar13 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar20 + 0x28)));
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar20 + 0x30)));
  RT_CALLRA(0x214294, guest::InitTuningConstants(param_1));
  RT_CALLRA(0x2142a0, guest::GetControllerButtonConfig(param_1,param_2));
  puVar17 = (uint32_t)(iVar20 + 0x34);
  uVar15 = 0;
  iVar19 = 0;
  piVar18 = piVar21;
  puVar16 = puVar17;
  do {
    (*(uint32_t*)rtguest::G((uint32_t)puVar17)) = 0;
    if (uVar15 < 9) {
      uVar12 = 0;
      piVar8 = (uint32_t)(iVar20 + 0x10);
      iVar9 = 0;
      do {
        if ((uVar15 != uVar12) &&
           ((*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(iVar19 + (*(int32_t*)rtguest::G((uint32_t)piVar18))) * 4)) == (*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(iVar9 + (*(int32_t*)rtguest::G((uint32_t)piVar8))) * 4)))) {
          (*(uint32_t*)rtguest::G((uint32_t)puVar16)) = 1;
          bVar3 = true;
          break;
        }
        uVar12 = uVar12 + 1;
        piVar8 = piVar8 + 0x4;  // T* element step
        iVar9 = iVar9 + 0xf;
      } while (uVar12 < 9);
    }
    uVar15 = uVar15 + 1;
    piVar18 = piVar18 + 0x4;  // T* element step
    iVar19 = iVar19 + 0xf;
    puVar17 = puVar17 + 0x4;  // T* element step
    puVar16 = puVar16 + 0x4;  // T* element step
    if (0xb < uVar15) {
      if (bVar3) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar20 + 0x60))) = 1;
      }
      uVar14 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)piVar21));
      uVar5 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar20 + 0x18)));
      uVar7 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar20 + 0x20)));
      uVar11 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar20 + 0x28)));
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar20 + 0x30)));
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)piVar21)) = uVar4;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar20 + 0x18))) = uVar6;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar20 + 0x20))) = uVar10;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar20 + 0x28))) = uVar13;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar20 + 0x30))) = uVar1;
      RT_CALLRA(0x214400, guest::SetControllerButtonConfig(param_1,param_2));
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)piVar21)) = uVar14;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar20 + 0x18))) = uVar5;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar20 + 0x20))) = uVar7;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar20 + 0x28))) = uVar11;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar20 + 0x30))) = uVar2;
      return 0;
    }
  } while( true );
}


RT_GUEST_FN(0x00214220, guest::ResetControllerConfigDefaults)

}  // namespace guest
