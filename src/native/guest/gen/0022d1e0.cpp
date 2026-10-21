// guest:0x0022d1e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t OpenCourseSelectEntry(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  undefined4 uVar1;
  long lVar2;
  int iVar3;
  
  iVar3 = (int)param_1;
  if (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) != 0) && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0xf))) == '\0')) {
    lVar2 = RT_CALLRA(0x22d20c, guest::TestStateBitFc78(0x28));
    if (lVar2 == 0) {
      RT_CALLRA(0x22d21c, guest::gs_stamps_award_function(0x28));
      RT_CALLRA(0x22d22c, guest::SetFlaggedSlotBit(0xf,0,0));
      RT_CALLRA(0x22d234, guest::AppendQueuedByte(2));
      RT_CALLRA(0x22d23c, guest::AppendQueuedByte(1));
      RT_CALLRA(0x22d244, guest::AppendQueuedByte(0));
    }
    uVar1 = RT_CALLRA(0x22d24c, guest::stampBookBuild(param_2));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x20))) = uVar1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xc))) = 0x22cd38;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) = 0;
    RunCourseSelectHubTask(param_1,param_2);
  }
  return 0;
}


RT_GUEST_FN(0x0022d1e0, guest::OpenCourseSelectEntry)

}  // namespace guest
