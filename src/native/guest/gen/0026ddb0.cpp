// guest:0x0026ddb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
// 4 rows, stride 0x30 (undefined4* +1 = +4 per row base, cols at +0/+10/+20/+30).
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void PrintXyzTable(uint32_t param_1, int param_2, ...) {
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  uint32_t uVar8;
  int iVar6;
  int iVar7;
  uint32_t auStack_80 = rtguest::allocFrame(64);

  iVar7 = 3;
  uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(param_1)));
  iVar6 = param_2 << 3;
  while (true) {
    iVar7 = iVar7 + -1;
    uVar2 = fptodp(uVar8);
    uVar3 = fptodp((*(uint32_t*)rtguest::G((uint32_t)(param_1 + 0x10))));
    uVar4 = fptodp((*(uint32_t*)rtguest::G((uint32_t)(param_1 + 0x20))));
    uint32_t puVar1 = param_1 + 0x30;
    param_1 = param_1 + 4;
    uVar5 = fptodp((*(uint32_t*)rtguest::G((uint32_t)(puVar1))));
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_80)), (char *)rtguest::G((uint32_t)(0x32fba8)),
              uVar2, uVar3, uVar4, uVar5);
    uiDebugText8(0x10, iVar6, auStack_80, 0xf);
    if (iVar7 < 0) break;
    uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(param_1)));
    iVar6 = iVar6 + 8;
  }
  return;
}


RT_GUEST_FN(0x0026ddb0, guest::PrintXyzTable)

}  // namespace guest
