// guest:0x0022d620 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateZoneFromRects(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg;
  bool bVar1;
  undefined1 uVar2;
uint32_t pfVar3;
  int iVar4;
  
  iVar4 = (int)param_2;
  pfVar3 = (uint32_t)(iVar4 + 0x90);
  bVar1 = false;
  if (((((*(float*)rtguest::G((uint32_t)param_4)) < (*(float*)rtguest::G((uint32_t)pfVar3))) && ((*(float*)rtguest::G((uint32_t)pfVar3)) < (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(float))))) && ((*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(float))) < (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x98)))))
     && ((*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x98))) < (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(float))))) {
    bVar1 = true;
  }
  if (bVar1) {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x19a))) == '\x04') {
      RecordLapSplitTimes(param_1,param_2);
      uVar2 = 2;
    }
    else {
      uVar2 = 2;
    }
  }
  else {
    bVar1 = false;
    if ((((*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(4) * sizeof(float))) < (*(float*)rtguest::G((uint32_t)pfVar3))) && ((*(float*)rtguest::G((uint32_t)pfVar3)) < (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(6) * sizeof(float))))) &&
       (((*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(5) * sizeof(float))) < (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x98))) && ((*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x98))) < (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(7) * sizeof(float))))))) {
      bVar1 = true;
    }
    uVar2 = 3;
    if (!bVar1) {
      bVar1 = false;
      if ((((*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(8) * sizeof(float))) < (*(float*)rtguest::G((uint32_t)pfVar3))) && ((*(float*)rtguest::G((uint32_t)pfVar3)) < (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(10) * sizeof(float))))) &&
         (((*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(9) * sizeof(float))) < (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x98))) && ((*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x98))) < (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(0xb) * sizeof(float))))))) {
        bVar1 = true;
      }
      if (bVar1) {
        if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x19a))) != '\x03') {
          return 0;
        }
        uVar2 = 4;
      }
      else {
        if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x19a))) != '\x02') {
          return 0;
        }
        uVar2 = 1;
      }
    }
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x19a))) = uVar2;
  return 0;
}


RT_GUEST_FN(0x0022d620, guest::UpdateZoneFromRects)

}  // namespace guest
