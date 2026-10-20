// guest:0x00209cb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// Pseudocode carries the GIF register payloads as undefined8; guest_decls truncates them.

uint32_t RunMenuNavigationTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
  }
  ushort uVar1;
  int iVar2;
  undefined8 uVar3;
  long lVar4;
  byte bVar5;
  int iVar6;
  int iVar7;
  
  iVar7 = (int)param_2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x50))) = 0x80000000;
  iVar6 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x50))) = 0x80ffffff;
    RT_CALLRA(0x209d10, guest::gifBeginSpriteBatch(0x177a9e0,0x20043dce69329d8eULL));
    RT_CALLRA(0x209d1c, guest::gifSetSpriteCount(0x177aa40,1));
    RT_CALLRA(0x209d3c, guest::uiSpriteWriteFull(0x177aa50,0,0,0,0,0x280,0x1c0));
    (*(uint32_t*)rtguest::G(0x177aa94)) = 0xffff;
    RT_CALLRA(0x209d68, guest::gifSetTexRegs(0x177aaa0,0x20047b4e253221f2ULL,0));
    RT_CALLRA(0x209d74, guest::gifSetSpriteCount(0x177aac0,1));
    RT_CALLRA(0x209d94, guest::uiSpriteWrite(0x177aad0,0x10,0x18,0,0,0x1b8,0xdc));
    RT_CALLRA(0x209da0, guest::gifChainEndTag(0x177a9d0,0x14));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x27))) = (uint32_t)(*(uint32_t*)rtguest::G(0x333284));
  }
  RT_CALLRA(0x209db0, guest::sendTexturePackets(0x177a9d0));
  bVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xc))) != '\0';
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xd))) != '\0') {
    bVar5 = bVar5 | 2;
  }
  if (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) ||
     (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) == '\0' && ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) != bVar5)))) {
    RT_CALLRA(0x209dec, guest::DestroyTaskRecord(param_1));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = bVar5;
    uVar3 = RT_CALLRA(0x209df8, guest::uiMenuCreate(param_1));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xc))) == '\0') {
      RT_CALLRA(0x209e30, guest::uiMenuAddItem(uVar3,0x2ea360,0));
    }
    else {
      RT_CALLRA(0x209e54, guest::uiMenuAddItem(uVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ad20u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10));
      RT_CALLRA(0x209e8c, guest::uiMenuAddItem(uVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ad24u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10));
      if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xd))) != '\0') && (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) & 1) != 0)) {
        RT_CALLRA(0x209eb4, guest::uiMenuAddItem(uVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ad28u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10));
      }
      RT_CALLRA(0x209ed8, guest::uiMenuAddItem(uVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ad2cu + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10));
      RT_CALLRA(0x209efc, guest::uiMenuAddItem(uVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ad30u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar3 + 0xc))) = 2;
    }
    RT_CALLRA(0x209f08, guest::CenterRectangle(uVar3,10));
  }
  RT_CALLRA(0x209f10, guest::uiDrawHintBar(0));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    return 0;
  }
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
  if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) != '\0') && ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) == 1)) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xe))) & 0x7f;
  }
  lVar4 = RT_CALLRA(0x209f54, guest::uiMenuRun(iVar2,param_2));
  if (lVar4 != 1) {
    return 0;
  }
  switch((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd)))) {
  case 0:
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x30))) & 0x40) == 0) {
      PlayUiSound(0x1c);
      return 0;
    }
    PlayUiSound(0x1a);
    RT_CALLRA(0x209fa8, guest::gs_tasks_kill(param_1));
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 10))) = 0;
    goto LAB_0020a028;
  case 1:
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x30))) & 0x40) == 0) {
      PlayUiSound(0x1c);
      return 0;
    }
    PlayUiSound(0x1a);
    RT_CALLRA(0x20a008, guest::gs_tasks_kill(param_1));
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 10))) = 1;
LAB_0020a028:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) | 0x400000;
    return 0;
  case 2:
    if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) & 2) != 0) {
      PlayUiSound(0x1a);
      RT_CALLRA(0x20a070, guest::gs_tasks_kill(param_1));
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 2;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 10))) = 2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) | 0x400000;
      return 0;
    }
    PlayUiSound(0x1a);
    uVar3 = 0x2092d8;
    break;
  case 3:
    if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) & 2) == 0) {
      uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x30)));
      goto LAB_0020a0ec;
    }
    PlayUiSound(0x1a);
    uVar3 = 0x2092d8;
    break;
  case 4:
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x30)));
LAB_0020a0ec:
    if ((uVar1 & 0x40) == 0) {
      PlayUiSound(0x1c);
      return 0;
    }
    PlayUiSound(0x1a);
    uVar3 = 0x20a580;
    break;
  default:
    goto switchD_00209f88_default;
  }
  RT_CALLRA(0x20a10c, guest::gs_tasks_spawn(param_1,uVar3,0));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xe))) | 0x80;
switchD_00209f88_default:
  return 0;
}


RT_GUEST_FN(0x00209cb0, guest::RunMenuNavigationTask)

}  // namespace guest
