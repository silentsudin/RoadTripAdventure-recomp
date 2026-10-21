// guest:0x00211f68 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void DrawMenuListItems(uint64_t param_1, uint64_t param_2, uint32_t param_3, uint64_t param_4, uint32_t param_5) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x40, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(3, (uint32_t)param_2);  // walk: = param_2
    rtguest::gS[4] = 0x000000000029c1a0ull;  // walk const
    rtguest::setS32(5, (uint32_t)param_5);  // walk: = param_5
    rtguest::gS[6] = 0x0000000000330000ull;  // walk const
    rtguest::gS[7] = 0x00000000002f0000ull;  // walk const
    rtguest::gS[8] = 0x0000000000330000ull;  // walk const
  }
  int iVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  
  iVar5 = (int)param_4 + -1;
  if (param_4 != 0) {
    do {
      if (((*(uint32_t*)rtguest::G((uint32_t)param_3)) & 1) != 0) {
        RT_CALLRA(0x211fdc, guest::uiArenaSetTexture(param_2,0x29c030,0,0));
        RT_CALLRA(0x211fe8, guest::uiArenaDataTag(param_2,7));
        RT_CALLRA(0x212008, guest::uiArenaSprites(param_2,0x29c0c0,7,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t)))));
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_5 + 0x28))) & 0x1000) == 0) {
          uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(4) * sizeof(uint32_t)));
          iVar4 = 0;
        }
        else {
          uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(4) * sizeof(uint32_t)));
          if ((((((uVar3 == 1) || (uVar3 == 0xc)) || (uVar3 == 0xd)) ||
               ((uVar3 == 0xe || (uVar3 == 0xf)))) ||
              ((uVar3 == 0x10 || ((uVar3 == 0x11 || (uVar3 == 0x12)))))) ||
             ((uVar3 == 0x13 || ((uVar3 == 0x14 || (iVar4 = 0, uVar3 == 0x15)))))) {
            RT_CALLRA(0x212094, guest::DrawTextHalfY((*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))) + 0x1e,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))) + 0x1c,0x2eaae8,6));
            RT_CALLRA(0x2120b0, guest::DrawTextHalfY((*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))) + 0x1e,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))) + 0x34,0x333548,0));
            RT_CALLRA(0x2120cc, guest::DrawTextHalfY((*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))) + 0x1e,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))) + 0x4c,0x2eaaf8,0));
            RT_CALLRA(0x2120e8, guest::DrawTextHalfY((*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))) + 0x1e,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))) + 100,0x333550,0));
            goto LAB_00212150;
          }
        }
        while( true ) {
          iVar2 = uVar3 * 4 + iVar4;
          iVar1 = iVar4 * 0x18;
          iVar4 = iVar4 + 1;
          RT_CALLRA(0x212138, guest::DrawTextHalfY((*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))) + 0x1e,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))) + iVar1 + 0x28,(*(uint32_t*)rtguest::G((uint32_t)(0x29c1a0u + (uint32_t)(iVar2) * 4))),0
                       ));
          if (3 < iVar4) break;
          uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(4) * sizeof(uint32_t)));
        }
      }
LAB_00212150:
      iVar5 = iVar5 + -1;
      param_3 = param_3 + 5;
    } while (iVar5 != -1);
  }
  return;
}


RT_GUEST_FN(0x00211f68, guest::DrawMenuListItems)

}  // namespace guest
