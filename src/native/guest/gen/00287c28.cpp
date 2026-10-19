// guest:0x00287c28 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sifRpcBindRequest(uint32_t param_1, uint32_t param_2, ...) {
  undefined4 uVar1;
  undefined8 uVar2;
  long lVar3;
  int iVar4;
  int iVar5;
  
  uVar2 = _sceRpcGetFPacket(param_2);
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14)));
  iVar5 = (int)uVar2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x14))) = uVar1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20))) = 0x80000009;
  lVar3 = sifFindRpcServer((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))),param_2);
  if (lVar3 == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x2c))) = 0;
  }
  else {
    iVar4 = (int)lVar3;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = iVar4;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x2c))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x14)));
  }
  sifSendRpcReply(0xffffffff80000008,uVar2,0x40,0,0,0);
  return 0;
}


RT_GUEST_FN(0x00287c28, guest::sifRpcBindRequest)

}  // namespace guest
