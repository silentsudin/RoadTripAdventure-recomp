// guest:0x00211530 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SpawnTasksFromCarTable(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x48, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x58) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x5c) = 0x00000000u;
    *(uint32_t*)rtguest::G(rt_sv+0x0) = (uint32_t)(int32_t)param_1;
    rtguest::gS[2] = 0x0000000000000000ull;  // walk const
    rtguest::setS32(4, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(8, (uint32_t)param_3);  // walk: = param_3
  }
  char cVar1;
  int iVar2;
  long lVar3;
uint32_t piVar4;
  long lVar5;
  ulong uVar6;
  
  lVar5 = 0;
  iVar2 = (int)param_2 * 8;
  uVar6 = (uint32_t)(char)(*(uint32_t*)rtguest::G((uint32_t)0x2be43f + (uint32_t)(iVar2) * 4));
  cVar1 = (*(uint8_t*)rtguest::G((uint32_t)0x2be43e + (uint32_t)(iVar2) * 1));
  if (0 < (long)uVar6) {
    piVar4 = 0x0177ea60;
    do {
      iVar2 = (int)cVar1 + (int)lVar5;
      lVar5 = (long)((int)lVar5 + 1);
      lVar3 = RT_CALLRA(0x2115bc, guest::MatchSlotPairState(param_2,iVar2));
      if (lVar3 == 0) {
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 + (int)param_2 * 4))) + iVar2 * 0x10;
      }
      else {
        iVar2 = (int)0x177f760 + (int)lVar3 * 0x1c;
      }
      (*(int32_t*)rtguest::G((uint32_t)piVar4)) = iVar2;
      piVar4 = piVar4 + 0x4;  // T* element step
    } while (lVar5 < (long)uVar6);
  }
  gs_tasks_spawn(param_1,0x20dc50,uVar6 | param_3 | 0x70400);
  return 0;
}


RT_GUEST_FN(0x00211530, guest::SpawnTasksFromCarTable)

}  // namespace guest
