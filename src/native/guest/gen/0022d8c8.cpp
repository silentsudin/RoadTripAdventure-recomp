// guest:0x0022d8c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void DrawRecordTableRows(uint32_t param_1, uint64_t param_2, uint32_t param_3, uint64_t param_4) {
  rtguest::FrameGuard rt_fg(48);
  char cVar1;
  char cVar2;
  undefined8 uVar3;
  int iVar4;
  int iVar5;
  ushort uVar6;
  
  uVar6 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x198))) & 1;
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x198))) & 0x200) == 0) {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x19b)));
    cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x19b)));
    if (cVar1 == '\x01') {
      uVar3 = 2;
      if ((*(int32_t*)rtguest::G((uint32_t)param_1)) == (*(int32_t*)rtguest::G(0x335700))) {
        uVar3 = 0;
      }
      SetStageConfig(param_4,uVar6,0,1,(*(int32_t*)rtguest::G((uint32_t)param_1)),uVar3);
      cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x19b)));
    }
    else if ('\x01' < cVar1) {
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(cVar1 < '\x03' ^ 1) * sizeof(int32_t)));
      uVar3 = 0;
      if (iVar4 != (*(int32_t*)rtguest::G(0x335700))) {
        uVar3 = 2;
      }
      SetStageConfig(param_4,uVar6,0,cVar1 + -1,iVar4,uVar3);
      iVar4 = (*(uint32_t*)rtguest::G(param_1 + (uint32_t)((cVar1 < '\x03') ^ 1) * sizeof(int32_t) + sizeof(int32_t)));
      uVar3 = 0;
      if (iVar4 != (*(int32_t*)rtguest::G(0x335700))) {
        uVar3 = 2;
      }
      SetStageConfig(param_4,uVar6,1,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x19b))),iVar4,uVar3);
      cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x19b)));
    }
    iVar5 = (int)cVar2;
    iVar4 = 2;
    if (iVar5 < 3) {
      iVar4 = iVar5;
    }
    SetStageConfig(param_4,uVar6,iVar4,iVar5 + 1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x260))),1);
    return;
  }
  iVar4 = 0;
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x19b)));
  if ('\x02' < (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x19b)))) {
    uVar3 = 2;
    if ((*(int32_t*)rtguest::G((uint32_t)param_1)) == (*(int32_t*)rtguest::G(0x335700))) {
      uVar3 = 0;
    }
    SetStageConfig(param_4,uVar6,0,(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x19b))) + -2,(*(int32_t*)rtguest::G((uint32_t)param_1)),uVar3);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x19b)));
    iVar4 = 1;
  }
  iVar5 = iVar4;
  if (1 < cVar1) {
    uVar3 = 2;
    iVar5 = iVar4 + 1;
    if ((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(iVar4) * sizeof(int32_t))) == (*(int32_t*)rtguest::G(0x335700))) {
      uVar3 = 0;
    }
    SetStageConfig(param_4,uVar6,iVar4,cVar1 + -1,(*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(iVar4) * sizeof(int32_t))),uVar3);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x19b)));
  }
  uVar3 = 2;
  if ((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(iVar5) * sizeof(int32_t))) == (*(int32_t*)rtguest::G(0x335700))) {
    uVar3 = 0;
  }
  SetStageConfig(param_4,uVar6,iVar5,cVar1,(*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(iVar5) * sizeof(int32_t))),uVar3);
  return;
}


RT_GUEST_FN(0x0022d8c8, guest::DrawRecordTableRows)

}  // namespace guest
