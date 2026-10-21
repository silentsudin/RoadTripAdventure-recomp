// guest:0x00287ea8 hand-ported from Ghidra pseudocode — int* array indexing scaled x4
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifCallRpc(uint32_t param_1, uint32_t param_2, uint64_t param_3, uint64_t param_4, uint64_t param_5, uint64_t param_6, uint64_t param_7, uint64_t param_8, uint32_t param_9, ...) {
  rtguest::FrameGuard rt_fg(192);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0xa0, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x90, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x80, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x20, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xb0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xb4) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(2, (uint32_t)param_5);  // walk: = param_5
    rtguest::setS32(4, (uint32_t)param_6);  // walk: = param_6
    rtguest::setS32(5, (uint32_t)param_4);  // walk: = param_4
    rtguest::setS32(6, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(8, (uint32_t)param_3);  // walk: = param_3
  }
  int iVar1;
  long lVar2;
  long lVar3;
  int iVar4;
  uint32_t auStack_c0 = rtguest::allocFrame(12);
  uint32_t uStack_bc = auStack_c0 + 4;
  uint32_t uStack_b8 = auStack_c0 + 8;

  lVar2 = RT_CALLRA(0x287ef8, guest::sifAllocRpcPacket(0x1778840));
  if (lVar2 == 0) {
    return 0xffffffff;
  }
  iVar4 = (int)lVar2;
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
  (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(8) * 4)) = param_9;
  (*(int32_t*)rtguest::G((uint32_t)param_1)) = iVar4;
  (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * 4)) = iVar1;
  (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * 4)) = (int)param_8;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x20))) = param_2;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = (int)param_5;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x28))) = (int)param_6;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x2c))) = (int)param_7;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x14))) = iVar4;
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(9) * 4));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) = param_1;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x34))) = iVar1;
  if ((param_3 & 2) == 0) {
    if (param_4 == param_6) {
      if (param_7 <= param_5) {
        param_7 = param_5;
      }
      sceSifWriteBackDCache(param_4,param_7);
    }
    else {
      if (0 < param_5) {
        sceSifWriteBackDCache(param_4,param_5);
      }
      if (0 < param_7) {
        sceSifWriteBackDCache(param_6,param_7);
      }
    }
  }
  if ((param_3 & 1) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 1;
    lVar3 = RT_CALLRA(0x287fec, guest::CreateSema(auStack_c0));
    (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * 4)) = (int)lVar3;
    if (lVar3 < 0) {
      RT_CALLRA(0x287ffc, guest::sifFreeRpcPacket(lVar2));
      return 0xfffffffd;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x30))) = 1;
    lVar3 = RT_CALLRA(0x287fcc, guest::sceSifSendCmd(0xffffffff8000000a,lVar2,0x40,param_4,(*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * 4)),param_5));
    if (lVar3 != 0) {
      RT_CALLRA(0x288050, guest::WaitSema((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * 4))));
      RT_CALLRA(0x288038, guest::DeleteSema((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * 4))));
      return 0;
    }
    RT_CALLRA(0x288058, guest::DeleteSema((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * 4))));
  }
  else {
    if (param_8 == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x30))) = 0;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x30))) = 1;
    }
    (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * 4)) = -1;
    lVar3 = RT_CALLRA(0x288028, guest::sceSifSendCmd(0xffffffff8000000a,lVar2,0x40,param_4,(*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * 4)),param_5));
    if (lVar3 != 0) {
      return 0;
    }
  }
  RT_CALLRA(0x288040, guest::sifFreeRpcPacket(lVar2));
  return 0xfffffffe;
}


RT_GUEST_FN(0x00287ea8, guest::sceSifCallRpc)

}  // namespace guest
