// guest:0x002283e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t MemcardReadFlushHandler(uint32_t param_1, ...) {
  long lVar1;
  int iVar2;
uint32_t auStack_30 = rtguest::allocFrame(4);
uint32_t auStack_2c = rtguest::allocFrame(12);
  
  lVar1 = sceMcSync(1,auStack_30,auStack_2c);
  if (lVar1 == 1) {
    iVar2 = (int)param_1;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) == '\0') {
      if ((int)(*(uint32_t*)rtguest::G((uint32_t)auStack_2c + (uint32_t)(0) * 4)) < 0) {
        ::printf((char *)rtguest::G((uint32_t)(0x2eb578u)));
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = (*(uint32_t*)rtguest::G((uint32_t)auStack_2c + (uint32_t)(0) * 4));
        gs_tasks_kill(param_1);
      }
      else if ((*(uint32_t*)rtguest::G((uint32_t)auStack_2c + (uint32_t)(0) * 4)) < 0x3448) {
        ::printf((char *)rtguest::G((uint32_t)(0x2eb588u)));
        sceMcClose((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x20))));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) = 2;
      }
      else {
        ::printf((char *)rtguest::G((uint32_t)(0x333628u)));
        sceMcClose((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x20))));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) = 1;
      }
    }
    else {
      if ((*(uint32_t*)rtguest::G((uint32_t)auStack_2c + (uint32_t)(0) * 4)) == 0) {
        if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) == '\x01') {
          ::printf((char *)rtguest::G((uint32_t)(0x333618u)));
          iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x18)));
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 1;
        }
        else {
          iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x18)));
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 0xfffffffd;
        }
      }
      else {
        ::printf((char *)rtguest::G((uint32_t)(0x333620u)));
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 0xffffffff;
      }
      gs_tasks_kill(param_1);
    }
  }
  return 0;
}


RT_GUEST_FN(0x002283e0, guest::MemcardReadFlushHandler)

}  // namespace guest
