// guest:0x00274198 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadSetActDirect(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
  }
  int iVar1;
uint32_t puVar2;
  undefined4 uVar3;
uint32_t puVar4;
  int iVar5;
  
  iVar1 = RT_CALLRA(0x2741b4, guest::scePadGetDmaStr());
  uVar3 = 0;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x72))) == '\x01') {
    iVar5 = 0;
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x01773214 + (int)param_2 * 0x1c + (int)param_1 * 0x70)));
    do {
      puVar2 = (uint32_t)(param_3 + iVar5);
      puVar4 = (uint32_t)(iVar1 + 0xc + iVar5);
      iVar5 = iVar5 + 1;
      (*(uint8_t*)rtguest::G((uint32_t)puVar4)) = (*(uint8_t*)rtguest::G((uint32_t)puVar2));
    } while (iVar5 < 6);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 4))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) = 6;
    RT_CALLRA(0x27422c, guest::_send_to_iop(param_1,param_2));
    uVar3 = 1;
  }
  return uVar3;
}


RT_GUEST_FN(0x00274198, guest::scePadSetActDirect)

}  // namespace guest
