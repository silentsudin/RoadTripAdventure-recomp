// guest:0x002001b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t Ps2InitSystem(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  long lVar1;
  
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2e9a18u)));
  RT_CALLRA(0x2001d8, guest::sceSifInitRpc(0));
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2e9a38u)), 0x2e9a48);
  RT_CALLRA(0x2001f4, guest::sceCdInit(0));
  do {
    lVar1 = RT_CALLRA(0x200200, guest::sceSifRebootIop(0x2e9a58));
  } while (lVar1 == 0);
  do {
    lVar1 = RT_CALLRA(0x200210, guest::sceSifSyncIop());
  } while (lVar1 == 0);
  RT_CALLRA(0x200220, guest::sceSifInitRpc(0));
  RT_CALLRA(0x200228, guest::sceSifInitIopHeap());
  RT_CALLRA(0x200230, guest::sceSifLoadFileReset());
  RT_CALLRA(0x200238, guest::sceCdInit(0));
  RT_CALLRA(0x200240, guest::sceCdMmode(1));
  RT_CALLRA(0x200248, guest::sceFsReset());
  do {
    lVar1 = RT_CALLRA(0x20025c, guest::sceSifLoadModule(0x2e9a70,0,0));
  } while (lVar1 < 0);
  do {
    lVar1 = RT_CALLRA(0x20027c, guest::sceSifLoadModule(0x2e9a88,0,0));
  } while (lVar1 < 0);
  do {
    lVar1 = RT_CALLRA(0x20029c, guest::sceSifLoadModule(0x2e9aa0,0,0));
  } while (lVar1 < 0);
  do {
    lVar1 = RT_CALLRA(0x2002bc, guest::sceSifLoadModule(0x2e9ab8,0,0));
  } while (lVar1 < 0);
  do {
    lVar1 = RT_CALLRA(0x2002dc, guest::sceSifLoadModule(0x2e9ad0,0,0));
  } while (lVar1 < 0);
  do {
    lVar1 = RT_CALLRA(0x2002fc, guest::sceSifLoadModule(0x2e9ae8,0,0));
  } while (lVar1 < 0);
  do {
    lVar1 = RT_CALLRA(0x200318, guest::sceSifLoadModule(0x2e9b00,0,0));
  } while (lVar1 < 0);
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2e9b18u)));
  RT_CALLRA(0x200330, guest::sceDmaReset(1));
  lVar1 = RT_CALLRA(0x200338, guest::sceMcInit());
  if (lVar1 != 0) {
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2e9b30u)));
  }
  while (lVar1 = scePadInit(0), lVar1 != 1) {
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2e9b40u)));
  }
  RT_CALLRA(0x200388, guest::scePadPortOpen(0,0,0x335a80));
  RT_CALLRA(0x200398, guest::scePadPortOpen(1,0,0x335b80));
  KickVifPacket();
  return 0;
}


RT_GUEST_FN(0x002001b8, guest::Ps2InitSystem)

}  // namespace guest
