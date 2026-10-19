// guest:0x00278008 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t cmd_sem_init(uint32_t q0, ...) {
    uint32_t uStack_3c = rtguest::allocFrame(4);
    uint32_t uStack_48 = rtguest::allocFrame(4);
    uint32_t uStack_4c = rtguest::allocFrame(4);
uint32_t auStack_50 = rtguest::allocFrame(4);
  
  if (((*(int32_t*)rtguest::G(0x2e6f28)) == -1) || ((*(int32_t*)rtguest::G(0x2e6f2c)) == -1)) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_3c)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_4c)) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_48)) = 1;
    (*(int32_t*)rtguest::G(0x2e6f28)) = CreateSema(auStack_50);
    (*(int32_t*)rtguest::G(0x2e6f2c)) = CreateSema(auStack_50);
    (*(uint32_t*)rtguest::G((uint32_t)uStack_48)) = 0;
    (*(uint32_t*)rtguest::G(0x2e6f20)) = CreateSema(auStack_50);
    (*(uint32_t*)rtguest::G(0x2e6f30)) = 0;
  }
  return 0;
}


RT_GUEST_FN(0x00278008, guest::cmd_sem_init)

}  // namespace guest
