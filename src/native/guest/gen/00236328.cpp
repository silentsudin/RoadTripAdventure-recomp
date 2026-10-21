// guest:0x00236328 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawLicenseList(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
    rtguest::gS[1] = 0x0000000000000000ull;  // walk const
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[3] = 0x0000000000000001ull;  // walk const
    rtguest::gS[4] = 0x000000000000012aull;  // walk const
    rtguest::gS[5] = 0x000000000000000dull;  // walk const
    rtguest::gS[6] = 0x000000000029fd30ull;  // walk const
    rtguest::gS[7] = 0x0000000000330000ull;  // walk const
    rtguest::gS[8] = 0x0000000001787008ull;  // walk const
  }
  byte bVar1;
  uint uVar2;
uint32_t pbVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
uint32_t ppuVar9;
  
  uVar5 = 0;
  iVar6 = 1;
  iVar7 = 0x12a;
  iVar8 = 0xd;
  ppuVar9 = 0x29fd30u;
  if ((*(int8_t*)rtguest::G(0x177fdd4)) != '\0') {
    iVar6 = 2;
  }
  if ((*(int8_t*)rtguest::G(0x177fdd6)) != '\0') {
    iVar6 = iVar6 + 1;
  }
  do {
    uVar2 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) + 1)));
    iVar4 = 0;
    if (uVar2 != 0) {
      pbVar3 = 0x0178700a;
      bVar1 = (*(uint8_t*)rtguest::G(0x178700a));
      while ((pbVar3 = pbVar3 + 4, bVar1 != uVar5 && (iVar4 = iVar4 + 1, iVar4 < (int)uVar2))) {
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar3));
      }
    }
    if (iVar4 < iVar6) {
      iVar4 = iVar8 + 1;
      RT_CALLRA(0x236404, guest::uiDrawTextCell(0xb,iVar8,(*(uint32_t*)rtguest::G((uint32_t)ppuVar9)),0));
      iVar7 = iVar7 + 0x18;
      RT_CALLRA(0x23641c, guest::uiDrawTextCell(0xd,iVar8,0x333838,0));
      iVar8 = iVar4;
      if ((int)uVar5 < 6) {
        RT_CALLRA(0x236464, guest::uiDrawNumber10(0x19a,iVar7,
                       (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0029fcb8 +
                        ((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) + 3))) * 6 + uVar5) * 4))),0));
      }
    }
    uVar5 = uVar5 + 1;
    ppuVar9 = ppuVar9 + 1;
  } while ((int)uVar5 < 6);
  return 0;
}


RT_GUEST_FN(0x00236328, guest::DrawLicenseList)

}  // namespace guest
