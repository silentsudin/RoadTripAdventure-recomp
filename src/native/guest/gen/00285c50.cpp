// guest:0x00285c50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitAlarm(uint32_t q0, ...) {
  undefined4 uVar1;
  undefined8 uVar2;
uint32_t puVar3;
  uint uVar4;
  
  if ((REG_RCNT3_MODE & 0x100) == 0) {
    uVar4 = 2;
    puVar3 = 0x002e98c0;
    setSyscall((*(uint32_t*)rtguest::G(0x2e98b0)),(*(uint32_t*)rtguest::G(0x002e98b4)));
    syscallMemInstall(0xffffffff80076000,0x2e9148,0x740);
    syscallMemInstall(0x82000,0x2e9888,0x28);
    FlushCache(0);
    FlushCache(2);
    setSyscall((*(uint32_t*)rtguest::G(0x2e98b8)),(*(uint32_t*)rtguest::G(0x2e98bc)));
    uVar1 = (*(uint32_t*)rtguest::G(0x2e98c0));
    while( true ) {
      uVar4 = uVar4 + 1;
      uVar2 = getSyscall(uVar1);
      uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar3));
      puVar3 = puVar3 + 2;
      setSyscall(uVar1,uVar2);
      if (7 < uVar4) break;
      uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar3));
    }
  }
  return 0;
}


RT_GUEST_FN(0x00285c50, guest::InitAlarm)

}  // namespace guest
