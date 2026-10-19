// guest:0x0022d1e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t OpenCourseSelectEntry(uint32_t param_1, uint32_t param_2, ...) {
  undefined4 uVar1;
  long lVar2;
  int iVar3;
  
  iVar3 = (int)param_1;
  if (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) != 0) && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0xf))) == '\0')) {
    lVar2 = TestStateBitFc78(0x28);
    if (lVar2 == 0) {
      gs_stamps_award_function(0x28);
      SetFlaggedSlotBit(0xf,0,0);
      AppendQueuedByte(2);
      AppendQueuedByte(1);
      AppendQueuedByte(0);
    }
    uVar1 = stampBookBuild(param_2);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x20))) = uVar1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xc))) = (*(uint8_t*)rtguest::G(0x22cd38));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) = 0;
    RunCourseSelectHubTask(param_1,param_2);
  }
  return 0;
}


RT_GUEST_FN(0x0022d1e0, guest::OpenCourseSelectEntry)

}  // namespace guest
