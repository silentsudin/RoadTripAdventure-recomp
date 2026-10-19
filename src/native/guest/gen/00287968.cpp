// guest:0x00287968 transliterated from Ghidra pseudocode — DO NOT hand-edit
// sifRpcReplyHandler: RPC end/session-end server handler (iSignalSema + packet free).
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sifRpcReplyHandler(uint32_t param_1, ...) {
  uint32_t uVar1;
  int32_t iVar2;
  uint32_t puVar3;

  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + 0x20));
  if (uVar1 == 0x8000000a) {
    puVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + 0x1c));
    if ((*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(7) * 4)) == 0) {
      iVar2 = (int32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * 4));
      goto LAB_002879ec;
    }
    rtCall((*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(7) * 4)),
           (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(8) * 4)));
    puVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + 0x1c));
  }
  else if (uVar1 < 0x8000000b) {
    if (uVar1 == 0x80000009) {
      puVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + 0x1c));
      (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(9) * 4)) =
           (*(uint32_t*)rtguest::G((uint32_t)param_1 + 0x24));
      (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(5) * 4)) =
           (*(uint32_t*)rtguest::G((uint32_t)param_1 + 0x28));
      (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(6) * 4)) =
           (*(uint32_t*)rtguest::G((uint32_t)param_1 + 0x2c));
    }
    else {
      puVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + 0x1c));
    }
  }
  else {
    puVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + 0x1c));
  }
  iVar2 = (int32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * 4));
LAB_002879ec:
  if (-1 < iVar2) {
    iSignalSema();  // pseudo argless call (W12 arg-forwarding item)
  }
  sifFreeRpcPacket((*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(0) * 4)));
  (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(0) * 4)) = 0;
  return 0;
}


RT_GUEST_FN(0x00287968, guest::sifRpcReplyHandler)

}  // namespace guest
