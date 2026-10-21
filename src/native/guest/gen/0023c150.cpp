// guest:0x0023c150 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CountFlaggedSlots(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x58, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x50, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x68) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x6c) = 0x00000000u;
    rtguest::setS32(3, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(7, (uint32_t)param_4);  // walk: = param_4
    rtguest::gS[8] = 0x0000000000000000ull;  // walk const
  }
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  long lVar8;
uint32_t puVar9;
uint32_t puVar10;
  uint uVar11;
  uint uVar12;
  
  iVar6 = (int)param_1;
  iVar2 = 0;
  puVar9 = 0x01780390 + iVar6 * 5 + param_3 * 0xd12;
  if ((param_1 == 0) || (uVar12 = 0, param_1 == 0xf)) {
    if (param_2 == 0xffffffff) {
      iVar2 = RT_CALLRA(0x23c1d0, guest::CountSetBits((*(uint32_t*)rtguest::G((uint32_t)puVar9))));
      iVar3 = RT_CALLRA(0x23c1e0, guest::CountSetBits((*(uint32_t*)rtguest::G((uint32_t)0x17803a0 + (uint32_t)(iVar6 * 5 + param_3 * 0xd12) * 4))));
      iVar4 = RT_CALLRA(0x23c1f4, guest::CountSetBits((*(uint32_t*)rtguest::G((uint32_t)0x1780398 + (uint32_t)(iVar6 * 5 + param_3 * 0xd12) * 4))));
      iVar5 = RT_CALLRA(0x23c204, guest::CountSetBits((*(uint32_t*)rtguest::G((uint32_t)0x1780394 + (uint32_t)(iVar6 * 5 + param_3 * 0xd12) * 4))));
      iVar6 = RT_CALLRA(0x23c21c, guest::CountSetBits((*(uint32_t*)rtguest::G((uint32_t)0x178039c + (uint32_t)(iVar6 * 5 + param_3 * 0xd12) * 4))));
      return iVar2 + iVar3 + iVar4 + iVar5 + iVar6;
    }
    if (((*(uint32_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)((int)param_2 >> 5) * sizeof(uint32_t))) & 1 << (param_2 & 0x1f)) != 0) {
      while (param_2 = param_2 - 1, param_2 != 0xffffffff) {
        if (((*(uint32_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)((int)param_2 >> 5) * sizeof(uint32_t))) & 1 << (param_2 & 0x1f)) != 0) {
          iVar2 = iVar2 + 1;
        }
      }
      return iVar2;
    }
  }
  else {
    while ((int)uVar12 < 0x10) {
      uVar11 = 0;
      if ((uVar12 != 0) || (bVar1 = true, param_1 < 8)) {
        if ((param_4 & 1) != 0) {
          uVar11 = (uint32_t)((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x177f76c + (uint32_t)(param_3 * 0x3448 + iVar6) * 1)) == uVar12);
        }
        if ((((param_4 & 2) != 0) && ((*(int8_t*)rtguest::G((uint32_t)0x177fdd4 + (uint32_t)(param_3 * 0x3448) * 1)) != '\0')) &&
           ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177f788 + iVar6 + param_3 * 0x3448))) == uVar12)) {
          uVar11 = uVar11 + 1;
        }
        if ((((param_4 & 4) != 0) && ((*(int8_t*)rtguest::G((uint32_t)0x177fdd6 + (uint32_t)(param_3 * 0x3448) * 1)) != '\0')) &&
           ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177f7a0 + iVar6 + param_3 * 0x3448 + 4))) == uVar12)) {
          uVar11 = uVar11 + 1;
        }
        bVar1 = uVar11 < 5;
      }
      if (bVar1) {
        puVar10 = puVar9 + uVar11;
        uVar7 = (*(uint32_t*)rtguest::G((uint32_t)puVar10));
        while( true ) {
          puVar10 = puVar10 + 0x4;  // T* element step
          iVar3 = iVar2;
          if ((((uVar7 & 1 << (uVar12 & 0x1f)) != 0) &&
              (((param_4 & 8) == 0 || (lVar8 = RT_CALLRA(0x23c3a0, guest::DispatchByIndex15(param_1,uVar12)), lVar8 != 0)))) &&
             (iVar3 = iVar2 + 1, uVar12 == param_2)) {
            return iVar2;
          }
          iVar2 = iVar3;
          uVar11 = uVar11 + 1;
          if (4 < (int)uVar11) break;
          uVar7 = (*(uint32_t*)rtguest::G((uint32_t)puVar10));
        }
        uVar12 = uVar12 + 1;
      }
      else {
        uVar12 = uVar12 + 1;
      }
    }
  }
  iVar6 = -1;
  if (param_2 == 0xffffffff) {
    iVar6 = iVar2;
  }
  return iVar6;
}


RT_GUEST_FN(0x0023c150, guest::CountFlaggedSlots)

}  // namespace guest
