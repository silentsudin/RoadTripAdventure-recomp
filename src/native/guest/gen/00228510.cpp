// guest:0x00228510 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t MemcardReadSlotTask(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  int iVar1;
  long lVar2;
  int iVar3;
uint32_t piVar4;
uint32_t auStack_30 = rtguest::allocFrame(16);
  
  iVar3 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) == 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x27))) = (char)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c))) >> 8);
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb5a0u)));
    lVar2 = sceMcOpen((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26))),0,0x2eb5b0,1);
    if (lVar2 != 0) {
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb53eu)), lVar2);
    }
  }
  piVar4 = (uint32_t)(iVar3 + 0x20);
  lVar2 = sceMcSync(1,auStack_30,piVar4);
  if (lVar2 == 1) {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar4));
    if (iVar1 < 0) {
      if (iVar1 == -4) {
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb618u)), 0xfffffffffffffffc);
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0;
      }
      else if (((iVar1 + 3U < 2) || (iVar1 == -5)) || (iVar1 == -7)) {
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb628u)), (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x20))));
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0xfffffffe;
      }
      else {
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb618u)));
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0xffffffff;
      }
      gs_tasks_kill(param_1);
    }
    else {
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb5e8u)));
      lVar2 = sceMcRead((*(int32_t*)rtguest::G((uint32_t)piVar4)),0x177f760 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x27))) * 0x689,
                        0x3448);
      if (lVar2 == 0) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xc))) = 0x2283e0;
      }
      else {
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb5f8u)), lVar2);
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0xffffffff;
        gs_tasks_kill(param_1);
      }
    }
  }
  else if (0x78 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8)))) {
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb648u)));
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0xfffffffe;
    gs_tasks_kill(param_1);
  }
  return 0;
}


RT_GUEST_FN(0x00228510, guest::MemcardReadSlotTask)

}  // namespace guest
