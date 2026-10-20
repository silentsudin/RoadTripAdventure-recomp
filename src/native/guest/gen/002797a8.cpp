// guest:0x002797a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceMcInit(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(176);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0xa0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xa4) = 0x00000000u;
  }
rtguest::allocFrame(16);  // guest frame gap (sp+0x0..0xf)
uint32_t auStack_a0 = rtguest::allocFrame(4);  // sp+0x10 (Ghidra entry-sp -0xa0, frame 0xb0)
    uint32_t uStack_9c = rtguest::allocFrame(4);  // sp+0x14 (Ghidra entry-sp -0x9c, frame 0xb0)
    uint32_t uStack_98 = rtguest::allocFrame(4);  // sp+0x18 (Ghidra entry-sp -0x98, frame 0xb0)
    rtguest::allocFrame(8);  // guest frame gap (sp+0x1c..0x23)
    uint32_t uStack_8c = rtguest::allocFrame(4);  // sp+0x24 (Ghidra entry-sp -0x8c, frame 0xb0)
  int iVar1;
  long lVar2;
  
  if ((*(int32_t*)rtguest::G(0x2e89bc)) < 0) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_98)) = 1;
    (*(int32_t*)rtguest::G(0x2e89bc)) = RT_CALLRA(0x2797e8, guest::CreateSema(auStack_a0));
  }
  RT_CALLRA(0x2797fc, guest::sceMcSync(0,0,0));
  RT_CALLRA(0x279804, guest::WaitSema((*(int32_t*)rtguest::G(0x2e89bc))));
  RT_CALLRA(0x27980c, guest::sceSifInitRpc(0));
  while( true ) {
    lVar2 = RT_CALLRA(0x279858, guest::sceSifBindRpc(0x1774b40,0xffffffff80000400,0));
    if (lVar2 < 0) {
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x331e08u)));
      do {
                    /* WARNING: Do nothing block with infinite loop */
      } while( true );
    }
    if ((*(uint32_t*)rtguest::G(0x1774b64)) != 0) break;
    iVar1 = 0x100000;
    do {
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
  }
  lVar2 = RT_CALLRA(0x2798c4, guest::sceSifCallRpc(0x1774b40,0xfe,0,0x1774bc0,0x30,0x1776100,0xc,0));
  RT_CALLRA(0x2798d0, guest::SignalSema((*(int32_t*)rtguest::G(0x2e89bc))));
  if (lVar2 < 0) {
    (*(uint32_t*)rtguest::G(0x1774b64)) = 0;
    iVar1 = (int)lVar2 + -100;
  }
  else if ((*(uint32_t*)rtguest::G(0x1776104)) < 0x20a) {
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x331e20u)));
    (*(uint32_t*)rtguest::G(0x1774b64)) = 0;
    iVar1 = -0x78;
  }
  else {
    iVar1 = (*(uint32_t*)rtguest::G(0x1776100));
    if ((*(uint32_t*)rtguest::G(0x1776108)) < 0x20e) {
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x331e48u)));
      (*(uint32_t*)rtguest::G(0x1774b64)) = 0;
      iVar1 = -0x79;
    }
  }
  return iVar1;
}


RT_GUEST_FN(0x002797a8, guest::sceMcInit)

}  // namespace guest
