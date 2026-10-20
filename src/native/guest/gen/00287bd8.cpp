// guest:0x00287bd8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sifFindRpcServer(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
uint32_t piVar2;
  int iVar3;
  
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
  if (iVar3 != 0) {
    piVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8)));
    while( true ) {
      if (piVar2 == (uint32_t)0x0) {
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x14)));
      }
      else {
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar2));
        while( true ) {
          if (iVar1 == param_1) {
            return piVar2;
          }
          piVar2 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)piVar2 + (uint32_t)(0xe) * sizeof(int32_t)));
          if (piVar2 == (uint32_t)0x0) break;
          iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar2));
        }
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x14)));
      }
      if (iVar3 == 0) break;
      piVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8)));
    }
  }
  return (uint32_t)0x0;
}


RT_GUEST_FN(0x00287bd8, guest::sifFindRpcServer)

}  // namespace guest
