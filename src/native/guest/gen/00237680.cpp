// guest:0x00237680 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawShopRow(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  int iVar1;
uint32_t puVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  iVar3 = (int)param_4;
  iVar4 = (int)param_1;
  iVar6 = iVar4 + 1;
  iVar5 = (int)param_2;
  puVar2 = 0x0177fd8c;
  if ((*(uint32_t*)rtguest::G((uint32_t)0x1780750 + (uint32_t)(param_3 + iVar3 * 0x1a) * 4)) != -1) {
    if (param_4 != 0) {
      puVar2 = (uint32_t)0x0;
      iVar1 = (param_3 * 2 + iVar3) * 2;
      if ((*(uint32_t*)rtguest::G((uint32_t)0x1782b1e + (uint32_t)(iVar1) * 4)) != '\0') {
        puVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((char)(*(uint32_t*)rtguest::G((uint32_t)0x1782b1f + (uint32_t)(iVar1) * 4)) * 0x10 +
                   (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 + (char)(*(uint32_t*)rtguest::G((uint32_t)0x1782b1e + (uint32_t)(iVar1) * 4)) * 4))) + 0xc)));
      }
    }
    uiDrawTextCell(param_1,param_2,puVar2,3);
    param_3 = param_3 + iVar3 * 0x1a;
    uiDrawTextCell(iVar6,iVar5 + 1,0x2ec4f0,3);
    iVar3 = param_3 * 4;
    iVar1 = iVar4 * 0x14 + 0xaa;
    uiDrawTextCell(iVar4 + 10,iVar5 + 1,(*(uint32_t*)rtguest::G((uint32_t)(0x29fd30u + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x01780750u + (uint32_t)(param_3) * 4))) * 4))),3);
    uiDrawTextCell(iVar6,iVar5 + 2,0x2ec500,3);
    uiDrawTime(iVar1,iVar5 * 0xc + 0x1d,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017807a0 + iVar3))),3);
    uiDrawTextCell(iVar6,iVar5 + 3,0x2ec510,3);
    uiDrawTime(iVar1,iVar5 * 0xc + 0x29,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017808d8 + iVar3))),3);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00237680, guest::DrawShopRow)

}  // namespace guest
