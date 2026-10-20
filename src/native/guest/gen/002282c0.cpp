// guest:0x002282c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t MemcardGetInfoTask(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  uint uVar1;
  long lVar2;
  undefined4 uVar3;
  int iVar4;
uint32_t auStack_30 = rtguest::allocFrame(4);
uint32_t auStack_2c = rtguest::allocFrame(12);
  
  iVar4 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) == 0) {
    lVar2 = sceMcGetInfo((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))),0,iVar4 + 0x20,0x3356fc,0x3356f8);
    if (lVar2 == 0) {
      return 0;
    }
    uVar1 = rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb530u)), lVar2);
    return uVar1;
  }
  lVar2 = sceMcSync(1,auStack_30,auStack_2c);
  if (lVar2 != 1) {
    uVar1 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) < 0x79);
    if (uVar1 != 0) {
      return uVar1;
    }
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb558u)));
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = 0xfffffffe;
    uVar1 = gs_tasks_kill(param_1);
    return uVar1;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x20))) == 2) {
    if ((*(int32_t*)rtguest::G(0x3356f8)) == 0) {
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = 0;
      goto LAB_00228388;
    }
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
    uVar3 = 2;
    if (0x3b < (*(uint32_t*)rtguest::G(0x3356fc))) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = 1;
      goto LAB_00228388;
    }
  }
  else {
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
    uVar3 = 0xffffffff;
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = uVar3;
LAB_00228388:
  uVar1 = gs_tasks_kill(param_1);
  return uVar1;
}


RT_GUEST_FN(0x002282c0, guest::MemcardGetInfoTask)

}  // namespace guest
