// guest:0x00204a00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UiDestroyRemovedNodes(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
  }
uint32_t pbVar1;
uint32_t pcVar2;
uint32_t puVar3;
  int iVar4;
uint32_t pbVar5;
uint32_t pcVar6;
uint32_t pbVar7;
  
  pbVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
  while( true ) {
    if (pbVar1 == (uint32_t)0x0) {
      return 0;
    }
    if (((*(int8_t*)rtguest::G((uint32_t)pbVar1)) & 2) == 0) break;
    if ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x2c))) == (uint32_t)0x0) {
      puVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x10)));
    }
    else {
      RT_CALLRA(0x00204a44, rtCall((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x2c))), pbVar1));  // MIPS 0x204a38/a44: lw+jalr, node in $a0 delay; Ghidra (**code**) mistranslation (cf. 0x205028)
      puVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x10)));
    }
    (*(int8_t*)rtguest::G((uint32_t)pbVar1)) = 0;
    (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x28) * sizeof(int8_t))) = 0;
    (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x29) * sizeof(int8_t))) = 0;
    (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x2a) * sizeof(int8_t))) = 0;
    (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x2b) * sizeof(int8_t))) = 0;
    if (puVar3 != (uint32_t)0x0) {
      if ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x2c))) == (uint32_t)0x0) {
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10)));
      }
      else {
        RT_CALLRA(0x00204a68, rtCall((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x2c))), puVar3));  // MIPS 0x204a5c/a68: direct handler ptr (cf. 0x205028)
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10)));
      }
      (*(uint8_t*)rtguest::G((uint32_t)puVar3)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x28))) = 0;
      if (iVar4 != 0) {
        RT_CALLRA(0x204a80, guest::DestroyTaskTree(iVar4)); // arg-drop fix: $4=iVar4 at the jal
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10))) = 0;
      }
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x14))) == 0) {
        (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x10) * sizeof(int8_t))) = 0;
        (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x11) * sizeof(int8_t))) = 0;
        (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x12) * sizeof(int8_t))) = 0;
        (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x13) * sizeof(int8_t))) = 0;
      }
      else {
        RT_CALLRA(0x204a98, guest::DestroyTaskTree((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x14))))); // arg-drop fix: $4=*(puVar3+0x14) at the jal
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x14))) = 0;
        (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x10) * sizeof(int8_t))) = 0;
        (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x11) * sizeof(int8_t))) = 0;
        (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x12) * sizeof(int8_t))) = 0;
        (*(int8_t*)rtguest::G((uint32_t)pbVar1 + (uint32_t)(0x13) * sizeof(int8_t))) = 0;
      }
    }
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x18))) == 0) {
      pbVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
    }
    else {
      pcVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x18))) + 0x10)));
      pcVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar6 + 0x14)));
      if ((*(int8_t*)rtguest::G((uint32_t)pcVar6)) == '\0') {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar6 + 0x18))) + 0x10))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar6 + 0x14)));
      }
      else {
        for (; (*(int8_t*)rtguest::G((uint32_t)pcVar2)) != '\0'; pcVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar2 + 0x14)))) {
          pcVar6 = pcVar2;
        }
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar6 + 0x14))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar2 + 0x14)));
      }
      pbVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
    }
  }
  pbVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x14)));
  do {
    while( true ) {
      pbVar7 = pbVar5;
      if (pbVar7 == (uint32_t)0x0) {
        return 0;
      }
      if (((*(int8_t*)rtguest::G((uint32_t)pbVar7)) & 2) != 0) break;
LAB_00204bd4:
      pbVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pbVar7 + 0x14)));
      pbVar1 = pbVar7;
    }
    if ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pbVar7 + 0x2c))) == (uint32_t)0x0) {
      puVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pbVar7 + 0x10)));
    }
    else {
      RT_CALLRA(0x00204b24, rtCall((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pbVar7 + 0x2c))), pbVar7));  // MIPS 0x204b18/b24: direct handler ptr (cf. 0x205028)
      puVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pbVar7 + 0x10)));
    }
    (*(int8_t*)rtguest::G((uint32_t)pbVar7)) = 0;
    (*(int8_t*)rtguest::G((uint32_t)pbVar7 + (uint32_t)(0x28) * sizeof(int8_t))) = 0;
    (*(int8_t*)rtguest::G((uint32_t)pbVar7 + (uint32_t)(0x29) * sizeof(int8_t))) = 0;
    (*(int8_t*)rtguest::G((uint32_t)pbVar7 + (uint32_t)(0x2a) * sizeof(int8_t))) = 0;
    (*(int8_t*)rtguest::G((uint32_t)pbVar7 + (uint32_t)(0x2b) * sizeof(int8_t))) = 0;
    if (puVar3 != (uint32_t)0x0) {
      if ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x2c))) == (uint32_t)0x0) {
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10)));
      }
      else {
        RT_CALLRA(0x00204b48, rtCall((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x2c))), puVar3));  // MIPS 0x204a5c/a68: direct handler ptr (cf. 0x205028)
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10)));
      }
      (*(uint8_t*)rtguest::G((uint32_t)puVar3)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x28))) = 0;
      if (iVar4 != 0) {
        RT_CALLRA(0x204b60, guest::DestroyTaskTree(iVar4)); // arg-drop fix: $4=iVar4 at the jal
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10))) = 0;
      }
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x14))) == 0) {
        (*(int8_t*)rtguest::G((uint32_t)pbVar7 + (uint32_t)(0x10) * sizeof(int8_t))) = 0;
        (*(int8_t*)rtguest::G((uint32_t)pbVar7 + (uint32_t)(0x11) * sizeof(int8_t))) = 0;
        (*(int8_t*)rtguest::G((uint32_t)pbVar7 + (uint32_t)(0x12) * sizeof(int8_t))) = 0;
        (*(int8_t*)rtguest::G((uint32_t)pbVar7 + (uint32_t)(0x13) * sizeof(int8_t))) = 0;
      }
      else {
        RT_CALLRA(0x204b78, guest::DestroyTaskTree((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x14))))); // arg-drop fix: $4=*(puVar3+0x14) at the jal
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x14))) = 0;
        (*(int8_t*)rtguest::G((uint32_t)pbVar7 + (uint32_t)(0x10) * sizeof(int8_t))) = 0;
        (*(int8_t*)rtguest::G((uint32_t)pbVar7 + (uint32_t)(0x11) * sizeof(int8_t))) = 0;
        (*(int8_t*)rtguest::G((uint32_t)pbVar7 + (uint32_t)(0x12) * sizeof(int8_t))) = 0;
        (*(int8_t*)rtguest::G((uint32_t)pbVar7 + (uint32_t)(0x13) * sizeof(int8_t))) = 0;
      }
    }
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(pbVar7 + 0x18))) != 0) {
      pcVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(pbVar7 + 0x18))) + 0x10)));
      pcVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar6 + 0x14)));
      pbVar7 = pbVar1;
      if ((*(int8_t*)rtguest::G((uint32_t)pcVar6)) == '\0') {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar6 + 0x18))) + 0x10))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar6 + 0x14)));
      }
      else {
        for (; (*(int8_t*)rtguest::G((uint32_t)pcVar2)) != '\0'; pcVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar2 + 0x14)))) {
          pcVar6 = pcVar2;
        }
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar6 + 0x14))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar2 + 0x14)));
      }
      goto LAB_00204bd4;
    }
    pbVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x14)));
  } while( true );
}


RT_GUEST_FN(0x00204a00, guest::UiDestroyRemovedNodes)

}  // namespace guest
