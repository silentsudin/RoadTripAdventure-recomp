// guest:0x00273598 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadInit(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
  }
  ulong uVar1;
  undefined8 uVar2;
  int iVar3;
  
  (*(uint32_t*)rtguest::G(0x002e6ec0)) = 1;
  while( true ) {
    RT_CALLRA(0x2735fc, guest::sceSifBindRpc(0x17731c0,0xffffffff80000100,0));
    iVar3 = 0x10000;
    if ((*(uint32_t*)rtguest::G(0x17731e4)) != 0) break;
    do {
      iVar3 = iVar3 + -1;
    } while (iVar3 != -1);
  }
  while( true ) {
    RT_CALLRA(0x273650, guest::sceSifBindRpc(0x17731e8,0xffffffff80000101,0));
    iVar3 = 0x10000;
    if ((*(uint32_t*)rtguest::G(0x177320c)) != 0) break;
    do {
      iVar3 = iVar3 + -1;
    } while (iVar3 != -1);
  }
  uVar1 = RT_CALLRA(0x273664, guest::scePadGetModVersion());
  iVar3 = (int)uVar1 >> 8;
  if (iVar3 == 4) {
    uVar2 = RT_CALLRA(0x2736b8, guest::scePadInit2(param_1));
  }
  else {
    if ((*(int32_t*)rtguest::G(0x2e6ec4)) != 0) {
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x3311e8u)));
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x331210u)), 4, 0, iVar3, uVar1 & 0xff);
    }
    uVar2 = 0;
  }
  return uVar2;
}


RT_GUEST_FN(0x00273598, guest::scePadInit)

}  // namespace guest
