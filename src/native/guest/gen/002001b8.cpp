// guest:0x002001b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t Ps2InitSystem(uint32_t q0, ...) {
  long lVar1;
  
  ::printf((char *)rtguest::G((uint32_t)(0x2e9a18u)));
  sceSifInitRpc(0);
  ::printf((char *)rtguest::G((uint32_t)(0x2e9a38u)), 0x2e9a48);
  sceCdInit(0);
  do {
    lVar1 = sceSifRebootIop(0x2e9a58);
  } while (lVar1 == 0);
  do {
    lVar1 = sceSifSyncIop();
  } while (lVar1 == 0);
  sceSifInitRpc(0);
  sceSifInitIopHeap();
  sceSifLoadFileReset();
  sceCdInit(0);
  sceCdMmode(1);
  sceFsReset();
  do {
    lVar1 = sceSifLoadModule(0x2e9a70,0,0);
  } while (lVar1 < 0);
  do {
    lVar1 = sceSifLoadModule(0x2e9a88,0,0);
  } while (lVar1 < 0);
  do {
    lVar1 = sceSifLoadModule(0x2e9aa0,0,0);
  } while (lVar1 < 0);
  do {
    lVar1 = sceSifLoadModule(0x2e9ab8,0,0);
  } while (lVar1 < 0);
  do {
    lVar1 = sceSifLoadModule(0x2e9ad0,0,0);
  } while (lVar1 < 0);
  do {
    lVar1 = sceSifLoadModule(0x2e9ae8,0,0);
  } while (lVar1 < 0);
  do {
    lVar1 = sceSifLoadModule(0x2e9b00,0,0);
  } while (lVar1 < 0);
  ::printf((char *)rtguest::G((uint32_t)(0x2e9b18u)));
  sceDmaReset(1);
  lVar1 = sceMcInit();
  if (lVar1 != 0) {
    ::printf((char *)rtguest::G((uint32_t)(0x2e9b30u)));
  }
  while (lVar1 = scePadInit(0), lVar1 != 1) {
    ::printf((char *)rtguest::G((uint32_t)(0x2e9b40u)));
  }
  scePadPortOpen(0,0,0x335a80);
  scePadPortOpen(1,0,0x335b80);
  KickVifPacket();
  return 0;
}


RT_GUEST_FN(0x002001b8, guest::Ps2InitSystem)

}  // namespace guest
