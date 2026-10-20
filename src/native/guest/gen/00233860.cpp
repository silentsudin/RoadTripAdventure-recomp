// guest:0x00233860 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceHudDraw(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
  }
  int iVar1;
uint32_t piVar2;
  int iVar3;
  int iVar4;
  
  RT_CALLRA(0x233880, guest::uiArenaInit(param_1,0x8000));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8000))) == 0) {
    RT_CALLRA(0x2338ac, guest::uiArenaBeginBatch(param_1,0,0));
    RT_CALLRA(0x2338c0, guest::raceHudItemPanel(param_1,param_1 + 0x800c,1));
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8078))) != 0) {
      RT_CALLRA(0x2338e0, guest::uiArenaSetTexture(param_1,0x29f5f8,0,1));
      RT_CALLRA(0x2338ec, guest::uiArenaDataTag(param_1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8088)))));
      RT_CALLRA(0x233908, guest::uiArenaSprites(param_1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8084))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8088))),
                     (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x807c))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8080))),0));
    }
    RT_CALLRA(0x233920, guest::raceHudElementC(param_1,param_1 + 0x808c,1));
    RT_CALLRA(0x233934, guest::raceHudElementA(param_1,param_1 + 0x80a4,0x14));
    RT_CALLRA(0x233948, guest::raceHudElementE(param_1,param_1 + 0x8414,1));
    RT_CALLRA(0x23395c, guest::raceHudElementF(param_1,param_1 + 0x8438,1));
    RT_CALLRA(0x233970, guest::raceHudElementG(param_1,param_1 + 0x844c,1));
    RT_CALLRA(0x233984, guest::raceHudElementH(param_1,param_1 + 0x8470,1));
    RT_CALLRA(0x233998, guest::raceHudElementI(param_1,param_1 + 0x8480,1));
    piVar2 = (uint32_t)(param_1 + 0x84a0);
    iVar4 = 1;
    RT_CALLRA(0x2339bc, guest::raceHudElementJ(param_1,param_1 + 0x8490,1));
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar2));
    iVar3 = param_1;
    while( true ) {
      if (iVar1 != 0) {
        RT_CALLRA(0x2339d8, guest::uiArenaSetTexture(param_1,0x29f5f8,0,1));
        RT_CALLRA(0x2339e4, guest::uiArenaDataTag(param_1,(*(int32_t*)rtguest::G((uint32_t)piVar2 + (uint32_t)(4) * sizeof(int32_t)))));
        RT_CALLRA(0x233a00, guest::uiArenaSprites(param_1,(*(int32_t*)rtguest::G((uint32_t)piVar2 + (uint32_t)(3) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)piVar2 + (uint32_t)(4) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)piVar2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)piVar2 + (uint32_t)(2) * sizeof(int32_t))),0));
      }
      iVar4 = iVar4 + -1;
      RT_CALLRA(0x233a18, guest::raceHudElementC(param_1,iVar3 + 0x84b4,3));
      RT_CALLRA(0x233a2c, guest::raceHudElementD(param_1,iVar3 + 0x84fc,3));
      iVar1 = iVar3 + 0x8538;
      iVar3 = iVar3 + 0x408;
      piVar2 = piVar2 + 0x408;  // T* element step
      RT_CALLRA(0x233a48, guest::raceHudElementA(param_1,iVar1,0x14));
      if (iVar4 < 0) break;
      iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar2));
    }
  }
  uiArenaSubmit(param_1);
  return 0;
}


RT_GUEST_FN(0x00233860, guest::raceHudDraw)

}  // namespace guest
