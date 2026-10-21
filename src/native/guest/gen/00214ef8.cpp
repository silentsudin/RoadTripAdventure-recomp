// guest:0x00214ef8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdatePauseSubmenuTask(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x30, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x38) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x3c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_4);  // walk: = param_4
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(2, (uint32_t)param_3);  // walk: = param_3
  }
  long lVar1;
  int iVar2;
uint32_t puVar3;
uint32_t puVar4;
  int iVar5;
  int iVar6;
  
  iVar5 = (int)param_3;
  puVar3 = (uint32_t)param_4;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)puVar3)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
    RT_CALLRA(0x214f50, guest::InitMenuElementLayout(param_3,iVar5 + 0x820c,puVar3 + 7,(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(0x17) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t)))));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81d8))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81d4))) = 0xfffff0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81cc))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81d0))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81c8))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81c8))) | 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81dc))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81dc))) | 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x8208))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81e0))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81e4))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81e8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81ec))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81f0))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81f8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81fc))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x8200))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x8204))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81f4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81f4))) | 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81c0))) = 2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81b8))) = 0x1b8;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81bc))) = 0xa0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81b4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81b4))) | 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81a4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81a4))) | 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81b0))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81a8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x81ac))) = 0;
  }
  lVar1 = RT_CALLRA(0x215028, guest::UpdateVerticalMenuNavigation(param_1,param_2,param_3,param_4));
  if (lVar1 == 299) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xc))) = 1;
  }
  else if (lVar1 < 300) {
    if (lVar1 == 200) {
      iVar2 = (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)((*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) + 8) * sizeof(uint32_t)));
      iVar6 = (int)param_1;
      if (iVar2 < 0x16) {
        if (0xb < iVar2) {
          iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = iVar2 + -0xc;
          goto LAB_002150dc;
        }
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
      }
      else {
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
      }
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 0;
    }
  }
  else if (lVar1 == 399) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 0;
  }
LAB_002150dc:
  iVar2 = iVar5 + 0x20;
  puVar4 = (uint32_t)(iVar5 + 0x81dc);
  RT_CALLRA(0x2150e4, guest::uiArenaInit(iVar2,0x8000));
  RT_CALLRA(0x2150f4, guest::uiArenaBeginBatch(iVar2,0,0));
  iVar6 = 1;
  RT_CALLRA(0x215110, guest::DrawMenuGridBorder(param_3,iVar2,iVar5 + 0x81a4,1));
  RT_CALLRA(0x21512c, guest::DrawMenuListItems(param_3,iVar2,iVar5 + 0x81b4,1,param_2));
  RT_CALLRA(0x215148, guest::DrawMenuGridBackground(param_3,iVar2,iVar5 + 0x81c8,1));
  do {
    if (((((*(uint32_t*)rtguest::G((uint32_t)puVar4)) & 1) != 0) && ((*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(4) * sizeof(uint32_t))) != 0)) && (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) & 0x10) != 0)) {
      RT_CALLRA(0x215194, guest::uiArenaSetTexture(iVar2,0x29c030,0,0));
      RT_CALLRA(0x2151a0, guest::uiArenaDataTag(iVar2,1));
      RT_CALLRA(0x2151c4, guest::uiArenaSprites(iVar2,(*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(5) * sizeof(uint32_t))) * 0x20 + 0x29c300,1,(*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(3) * sizeof(uint32_t)))));
    }
    iVar6 = iVar6 + -1;
    puVar4 = puVar4 + 0x18;  // T* element step
  } while (iVar6 != -1);
  RT_CALLRA(0x2151e8, guest::DrawMenuCursorAndIcons(param_3,iVar2,iVar5 + 0x820c,0xb));
  uiArenaSubmit(iVar2);
  return 0;
}


RT_GUEST_FN(0x00214ef8, guest::UpdatePauseSubmenuTask)

}  // namespace guest
