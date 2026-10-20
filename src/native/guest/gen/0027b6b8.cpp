// guest:0x0027b6b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __do_global_ctors(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
uint32_t puVar1;
uint32_t piVar2;
uint32_t pcVar3;
  int iVar4;
uint32_t puVar5;
  int iVar6;
  
  __register_frame_info(0x2e99c0,0x1776140);
  iVar6 = (*(uint32_t*)rtguest::G(0x2e99b0));
  if ((*(uint32_t*)rtguest::G(0x2e99b0)) == -1) {
    iVar6 = 0;
    if ((*(uint32_t*)rtguest::G(0x2e99b4)) != 0) {
      iVar4 = 4;
      iVar6 = 0;
      do {
        piVar2 = (uint32_t)((int)0x002e99b4 + iVar4);
        iVar6 = iVar6 + 1;
        iVar4 = iVar4 + 4;
      } while ((*(int32_t*)rtguest::G((uint32_t)piVar2)) != 0);
    }
  }
  if (iVar6 != 0) {
    puVar5 = 0x002e99b0 + iVar6;
    pcVar3 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar5));
    while( true ) {
      puVar5 = puVar5 + -1;
      iVar6 = iVar6 + -1;
      rtCall(pcVar3);
      if (iVar6 == 0) break;
      pcVar3 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar5));
    }
  }
  puVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x002e8cc0)) + 0x148)));
  if (puVar5 == (uint32_t)0x0) {
    puVar5 = (uint32_t)((*(uint32_t*)rtguest::G(0x002e8cc0)) + 0x14c);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x002e8cc0)) + 0x148))) = puVar5;
  }
  if (0x1f < (int)(*(uint32_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(1) * sizeof(uint32_t)))) {
    puVar5 = malloc(0x88);
    puVar1 = (*(uint32_t*)rtguest::G(0x002e8cc0));
    if (puVar5 == (uint32_t)0x0) {
      return 0xffffffff;
    }
    (*(uint32_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x148)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x148))) = puVar5;
  }
  iVar6 = (*(uint32_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(1) * sizeof(uint32_t)));
  (*(uint32_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(1) * sizeof(uint32_t))) = iVar6 + 1;
  (*(uint32_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(iVar6 + 2) * sizeof(uint32_t))) = 0x27b628;
  return 0;
}


RT_GUEST_FN(0x0027b6b8, guest::__do_global_ctors)

}  // namespace guest
