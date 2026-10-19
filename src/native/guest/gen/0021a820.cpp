// guest:0x0021a820 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_controls_player_control(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  char cVar1;
  ushort uVar2;
  ushort uVar3;
  ushort uVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  ushort uVar8;
  
  iVar7 = param_2 + 0xc;
  uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x198)));
  uVar6 = uVar2 & 1;
  iVar5 = uVar6 * 0x3448;
  uVar3 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(uVar6 * 2 + iVar7 + 0x20)));
  uVar8 = uVar3 & 0xa000;
  if ((uVar3 & (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x1780a48 + iVar5)))) != 0) {
    uVar8 = uVar3 & 0xa000 | 1;
  }
  if ((uVar3 & (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01780a4a + iVar5)))) != 0) {
    uVar8 = uVar8 | 2;
  }
  if ((uVar3 & (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01780a4c + iVar5)))) != 0) {
    uVar8 = uVar8 | 5;
  }
  uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(uVar6 * 2 + param_2 + 0x30)));
  if ((uVar3 & (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01780a4e + iVar5)))) != 0) {
    uVar8 = uVar8 | 8;
  }
  if ((uVar3 & (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01780a58 + iVar5)))) != 0) {
    uVar8 = uVar8 | 0x10;
  }
  if ((uVar4 & (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01780a50 + iVar5)))) != 0) {
    uVar8 = uVar8 | 0x20;
  }
  if ((uVar4 & (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01780a52 + iVar5)))) != 0) {
    uVar8 = uVar8 | 0x40;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + uVar6))) == '\a') {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(uVar6 * 4 + iVar7 + 0x32)));
    if (cVar1 < '\0') {
      uVar8 = uVar8 | 0x8000;
    }
    if ('\0' < cVar1) {
      uVar8 = uVar8 | 0x2000;
    }
  }
  uVar3 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01780a54 + iVar5)));
  if ((uVar4 & uVar3) != 0) {
    PlayUiSound((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x245))));
    uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x198)));
    uVar3 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01780a54 + iVar5)));
  }
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((uVar2 & 1) * 2 + param_2 + 0x38))) & uVar3) != 0) {
    PlayUiSound((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x245))) + 0x8000);
  }
}


RT_GUEST_FN(0x0021a820, guest::gs_controls_player_control)

}  // namespace guest
