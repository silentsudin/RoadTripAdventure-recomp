// guest:0x00273468 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _send_to_iop(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(368);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x160) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x164) = 0x00000000u;
  }
    uint32_t iStack_16c = rtguest::allocFrame(4);
    uint32_t uStack_168 = rtguest::allocFrame(4);
    uint32_t uStack_164 = rtguest::allocFrame(4);
uint32_t puVar1;
  uint uVar2;
uint32_t puVar3;
  long lVar4;
  int iVar5;
uint32_t puStack_170;
  
  iVar5 = param_2 * 0x1c + param_1 * 0x70;
  puVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01773214 + iVar5)));
  lVar4 = RT_CALLRA(0x2734b8, guest::sceSifDmaStat((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177321c + iVar5)))));
  puVar3 = 0x002e0000;
  if (lVar4 < 0) {
    uVar2 = (*(uint32_t*)rtguest::G((uint32_t)puVar1));
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x01773218 + iVar5)));
    (*(uint32_t*)rtguest::G((uint32_t)puVar1)) = uVar2 + 1;
    RT_CALLRA(0x273508, guest::sceSifWriteBackDCache(puVar1,puVar1 + 8));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_168)) = 0x20;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_164)) = 0;
    puStack_170 = puVar1;
    (*(int32_t*)rtguest::G((uint32_t)iStack_16c)) = iVar5 + (uVar2 + 1 & 1) * 0x20;
    lVar4 = RT_CALLRA(0x273528, guest::sceSifSetDma(rtguest::allocFrame(0x20),1));
    if ((lVar4 == 0) && ((*(int32_t*)rtguest::G(0x2e6ec4)) != 0)) {
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x3311a0u)));
    }
    iVar5 = param_2 * 0x1c + param_1 * 0x70;
    puVar3 = 0x01773210 + iVar5;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177321c + iVar5))) = (int)lVar4;
  }
  else if ((*(int32_t*)rtguest::G(0x2e6ec4)) != 0) {
    puVar3 = (uint32_t)rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x3311a0u)));
  }
  return puVar3;
}


RT_GUEST_FN(0x00273468, guest::_send_to_iop)

}  // namespace guest
