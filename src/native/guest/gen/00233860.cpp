// guest:0x00233860 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceHudDraw(uint32_t param_1, ...) {
  int iVar1;
uint32_t piVar2;
  int iVar3;
  int iVar4;
  
  uiArenaInit(param_1,0x8000);
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8000))) == 0) {
    uiArenaBeginBatch(param_1,0,0);
    raceHudItemPanel(param_1,param_1 + 0x800c,1);
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8078))) != 0) {
      uiArenaSetTexture(param_1,0x29f5f8,0,1);
      uiArenaDataTag(param_1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8088))));
      uiArenaSprites(param_1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8084))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8088))),
                     (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x807c))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8080))),0);
    }
    raceHudElementC(param_1,param_1 + 0x808c,1);
    raceHudElementA(param_1,param_1 + 0x80a4,0x14);
    raceHudElementE(param_1,param_1 + 0x8414,1);
    raceHudElementF(param_1,param_1 + 0x8438,1);
    raceHudElementG(param_1,param_1 + 0x844c,1);
    raceHudElementH(param_1,param_1 + 0x8470,1);
    raceHudElementI(param_1,param_1 + 0x8480,1);
    piVar2 = (uint32_t)(param_1 + 0x84a0);
    iVar4 = 1;
    raceHudElementJ(param_1,param_1 + 0x8490,1);
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar2));
    iVar3 = param_1;
    while( true ) {
      if (iVar1 != 0) {
        uiArenaSetTexture(param_1,0x29f5f8,0,1);
        uiArenaDataTag(param_1,(*(int32_t*)rtguest::G((uint32_t)piVar2 + (uint32_t)(4) * sizeof(int32_t))));
        uiArenaSprites(param_1,(*(int32_t*)rtguest::G((uint32_t)piVar2 + (uint32_t)(3) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)piVar2 + (uint32_t)(4) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)piVar2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)piVar2 + (uint32_t)(2) * sizeof(int32_t))),0);
      }
      iVar4 = iVar4 + -1;
      raceHudElementC(param_1,iVar3 + 0x84b4,3);
      raceHudElementD(param_1,iVar3 + 0x84fc,3);
      iVar1 = iVar3 + 0x8538;
      iVar3 = iVar3 + 0x408;
      piVar2 = piVar2 + 0x408;  // T* element step
      raceHudElementA(param_1,iVar1,0x14);
      if (iVar4 < 0) break;
      iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar2));
    }
  }
  uiArenaSubmit(param_1);
  return 0;
}


RT_GUEST_FN(0x00233860, guest::raceHudDraw)

}  // namespace guest
