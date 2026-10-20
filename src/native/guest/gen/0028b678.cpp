// guest:0x0028b678 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDevctl(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(224);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0xd0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xd4) = 0x00000000u;
  }
uint32_t auStack_d0 = rtguest::allocFrame(4);
    uint32_t uStack_cc = rtguest::allocFrame(4);
    uint32_t uStack_c8 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_bc = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_b0 = rtguest::allocFrame(16);
  char cVar1;
uint32_t puVar2;
  undefined8 uVar3;
  long lVar4;
uint32_t puVar5;
  int iVar6;
  uint uVar7;
  
  RT_CALLRA(0x28b6bc, guest::_sceFsWaitS(0x17));
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    RT_CALLRA(0x28b6dc, guest::sceFsInit());
    (*(uint8_t*)rtguest::G((uint32_t)0x017788cc + 0)) = (*(int8_t*)rtguest::G((uint32_t)param_1));
  }
  else {
    (*(uint8_t*)rtguest::G((uint32_t)0x017788cc + 0)) = (*(int8_t*)rtguest::G((uint32_t)param_1));
  }
  iVar6 = 0;
  if ((char)(*(uint32_t*)rtguest::G(0x17788cc)) != '\0') {
    for (iVar6 = 1;
        (iVar6 < 0x400 &&
        (cVar1 = (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(iVar6) * sizeof(int8_t))), (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x017788cc + iVar6))) = cVar1, cVar1 != '\0'));
        iVar6 = iVar6 + 1) {
    }
  }
  if (iVar6 == 0x400) {
    (*(uint32_t*)rtguest::G(0x1778ccb)) = 0;
  }
  if ((param_4 < 0x401) && (param_6 < 0x401)) {
    uVar7 = 0;
    if (param_4 != 0) {
      do {
        puVar2 = (uint32_t)(param_3 + uVar7);
        puVar5 = 0x01778ccc + uVar7;
        uVar7 = uVar7 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)puVar5)) = (*(uint8_t*)rtguest::G((uint32_t)puVar2));
      } while (uVar7 < param_4);
    }
    (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0;
    (*(uint32_t*)rtguest::G(0x17790cc)) = param_2;
    (*(uint32_t*)rtguest::G(0x17790d0)) = param_4;
    uVar3 = RT_CALLRA(0x28b7c8, guest::CreateSema(auStack_d0));
    (*(uint32_t*)rtguest::G(0x17790d8)) = (uint32_t)param_6;
    (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
    (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar3;
    (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_b0;
    (*(uint32_t*)rtguest::G(0x17790d4)) = param_5;
    sceSifWriteBackDCache(0x17788c0,0x81c);
    lVar4 = RT_CALLRA(0x28b818, guest::sceSifCallRpc(0x1779b80,0x17,0,0x17788c0,0x81c,0x1779500,4,0));
    iVar6 = (*(uint32_t*)rtguest::G(0x21779500));
    if (lVar4 < 0) {
      RT_CALLRA(0x28b828, guest::DeleteSema(uVar3));
      RT_CALLRA(0x28b830, guest::_sceFsSigSema());
      (*(uint32_t*)rtguest::G((uint32_t)auStack_b0 + (uint32_t)(0) * 4)) = 0xfffffff5;
    }
    else {
      RT_CALLRA(0x28b844, guest::_sceFsSigSema());
      if (iVar6 == 0) {
        RT_CALLRA(0x28b854, guest::DeleteSema(uVar3));
        (*(uint32_t*)rtguest::G((uint32_t)auStack_b0 + (uint32_t)(0) * 4)) = 0xfffffff5;
      }
      else {
        RT_CALLRA(0x28b864, guest::WaitSema(uVar3));
        RT_CALLRA(0x28b86c, guest::DeleteSema(uVar3));
      }
    }
  }
  else {
    (*(uint32_t*)rtguest::G((uint32_t)auStack_b0 + (uint32_t)(0) * 4)) = 0xffffffea;
  }
  return (*(uint32_t*)rtguest::G((uint32_t)auStack_b0 + (uint32_t)(0) * 4));;
}


RT_GUEST_FN(0x0028b678, guest::sceDevctl)

}  // namespace guest
