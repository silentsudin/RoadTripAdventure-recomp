// guest:0x0022d390 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RecordLapSplitTimes(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(2, (uint32_t)param_3);  // walk: = param_3
  }
  undefined4 uVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  char cVar5;
  long lVar6;
  ushort uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x28))) & 0x40) != 0) goto LAB_0022d4bc;
  uVar7 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x198)));
  if ((uVar7 & 0x73) == 0) goto LAB_0022d4bc;
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x260)));
  iVar3 = (uint32_t)((uVar7 & 0x40) != 0) * 3 + ((uVar7 & 0x30) >> 4);
  if (iVar4 < (int)(*(uint32_t*)rtguest::G((uint32_t)0x1787074 + (uint32_t)(iVar3 * 4) * 4))) {
    if ((uVar7 & 3) != 0) {
      if (iVar4 < (*(int32_t*)rtguest::G(0x335700))) {
        if ((*(int32_t*)rtguest::G(0x335700)) == 0x7fffffff) {
          RT_CALLRA(0x22d42c, guest::PlayUiSound(0x2e));
          iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x260)));
          (*(int32_t*)rtguest::G(0x335700)) = iVar4;
        }
        else {
          RT_CALLRA(0x22d440, guest::PlayUiSound(0x2f));
          iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x260)));
          (*(int32_t*)rtguest::G(0x335700)) = iVar4;
        }
      }
      else {
        RT_CALLRA(0x22d458, guest::PlayUiSound(0x2e));
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x260)));
      }
    }
    (*(uint32_t*)rtguest::G((uint32_t)0x1787074 + (uint32_t)(iVar3 * 4) * 4)) = iVar4;
LAB_0022d480:
    cVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x19b)));
  }
  else {
    if ((uVar7 & 3) != 0) {
      RT_CALLRA(0x22d478, guest::PlayUiSound(0x2e));
      goto LAB_0022d480;
    }
    cVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x19b)));
  }
  if (cVar5 < '\x03') {
    (*(uint32_t*)rtguest::G((uint32_t)(0x1787068u + iVar3 * 4) + (uint32_t)(cVar5) * 4)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x260)));
  }
  else {
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01787070 + iVar3 * 0x10)));
    (*(uint32_t*)rtguest::G((uint32_t)0x1787068 + (uint32_t)(iVar3 * 4) * 4)) = (*(uint32_t*)rtguest::G((uint32_t)0x178706c + (uint32_t)(iVar3 * 4) * 4));
    (*(uint32_t*)rtguest::G((uint32_t)0x178706c + (uint32_t)(iVar3 * 4) * 4)) = uVar1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01787070 + iVar3 * 0x10))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x260)));
  }
LAB_0022d4bc:
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x260))) = 0;
  cVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x19b))) + '\x01';
  (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x19b))) = cVar5;
  uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x28)));
  if ((uVar8 & 0x800) == 0) {
    lVar6 = (long)(int)cVar5;
    cVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x10))) + 2)));
    if (lVar6 == cVar5) {
      if ((uVar8 & 0x40) == 0) {
        cVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26)));
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = cVar5 + '\x01';
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x247))) = cVar5;
        uVar7 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x198)));
        if ((uVar7 & 3) != 0) {
          RT_CALLRA(0x22d52c, guest::PlayUiSound((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x245))) + 0x8000));
          uVar7 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x198)));
        }
      }
      else {
        uVar7 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x198)));
      }
      if ((uVar7 & 0x73) != 0) {
        uVar10 = (uVar7 & 0x30) >> 4;
        bVar2 = (uVar7 & 0x40) != 0;
        uVar8 = (uint32_t)bVar2;
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x28))) & 0x40) == 0) {
          uVar9 = (uint32_t)bVar2;
          (*(uint32_t*)rtguest::G((uint32_t)0x1786ff0 + (uint32_t)(uVar9 * 3 + uVar10) * 4)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20)));
          iVar4 = uVar9 * 2 + uVar8;
        }
        else {
          iVar4 = uVar8 * 3;
        }
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) | (uint32_t)(1 << uVar10 + iVar4);
        uVar7 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x198)));
      }
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x246))) = 0x28;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x198))) = uVar7 | 0x280;
    }
    else if ((((lVar6 == cVar5 + -1) && (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x198))) & 0x3ff) == 2)) &&
             ((uVar8 & 0x40) == 0)) &&
            ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x28))) = uVar8 | 0x10, ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 10))) & 2) == 0)) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 0x80;
    }
  }
  return 0;
}


RT_GUEST_FN(0x0022d390, guest::RecordLapSplitTimes)

}  // namespace guest
