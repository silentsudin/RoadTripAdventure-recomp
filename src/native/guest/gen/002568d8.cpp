// guest:0x002568d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ClosePaintEditorWindow(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[1] = 0x000000000175b570ull;  // walk const
  }
  short sVar1;
  int iVar2;
  
  iVar2 = (int)param_1;
  sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 2))) + -4;
  (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 2))) = sVar1;
  RT_CALLRA(0x25691c, guest::uiSpriteWrite(0x175b5e0,0x178,sVar1,0x120,0x140,0xd6,0x38));
  if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 2))) == -0x38) {
    RT_CALLRA(0x256934, guest::gs_tasks_kill(param_1));
  }
  vifDirect(0x175b570,0xc);
  return 0;
}


RT_GUEST_FN(0x002568d8, guest::ClosePaintEditorWindow)

}  // namespace guest
