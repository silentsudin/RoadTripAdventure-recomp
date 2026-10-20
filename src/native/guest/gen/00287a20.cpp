// guest:0x00287a20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sifRpcOtherDataReply(uint32_t param_1, uint64_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  undefined4 uVar1;
  int iVar2;
  undefined8 uVar3;
  
  uVar3 = RT_CALLRA(0x287a30, guest::_sceRpcGetFPacket(param_2));
  iVar2 = (int)uVar3;
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x14))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14)));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1c))) = uVar1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x20))) = 0x8000000c;
  sifSendRpcReply(0xffffffff80000008,uVar3,0x40,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))),
                  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x24))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))));
  return 0;
}


RT_GUEST_FN(0x00287a20, guest::sifRpcOtherDataReply)

}  // namespace guest
