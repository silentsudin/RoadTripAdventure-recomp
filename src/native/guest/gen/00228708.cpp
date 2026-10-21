// guest:0x00228708 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t MemcardCreateSlotTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x18, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
  }
  char cVar1;
  long lVar2;
uint32_t __format;
  int iVar3;
uint32_t auStack_30 = rtguest::allocFrame(4);
uint32_t aiStack_2c = rtguest::allocFrame(12);
  
  RT_CALLRA(0x228730, guest::uiDrawPanelCells(9,8,0x15,10,0));
  RT_CALLRA(0x228748, guest::uiDrawTextCell(10,9,0x2eb660,0));
  iVar3 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) == 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x27))) = (char)((uint32_t)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c))) >> 8);
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb670u)), (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c))) + 1, ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c))) >> 8 & 0xffU) + 1);
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x333630u)));
    lVar2 = RT_CALLRA(0x2287a4, guest::sceMcMkdir((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26))),0,0x2eb690));
    if (lVar2 != 0) {
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb53eu)), lVar2);
    }
    RT_CALLRA(0x2287cc, guest::gs_tasks_spawn(param_1,0x20dc50,0x3d0000));
    cVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25)));
  }
  else {
    cVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25)));
  }
  if ((cVar1 == '\x03') && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) != '\0')) {
    return 0;
  }
  lVar2 = RT_CALLRA(0x2287f8, guest::sceMcSync(1,auStack_30,aiStack_2c));
  if (lVar2 != 1) {
    return 0;
  }
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb6a0u)), (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))), (*(int32_t*)rtguest::G((uint32_t)aiStack_2c + (uint32_t)(0) * 4)));
  switch((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25)))) {
  case 0:
    if (((*(int32_t*)rtguest::G((uint32_t)aiStack_2c + (uint32_t)(0) * 4)) == 0) || ((*(int32_t*)rtguest::G((uint32_t)aiStack_2c + (uint32_t)(0) * 4)) == -4)) {
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb6b8u)));
      lVar2 = RT_CALLRA(0x228878, guest::sceMcOpen((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26))),0,0x2eb6d0,0x200));
      if (lVar2 == 0) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 1;
        return 0;
      }
      __format = 0x2eb6e8u;
      goto LAB_00228a50;
    }
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
    break;
  case 1:
    if (-1 < (*(int32_t*)rtguest::G((uint32_t)aiStack_2c + (uint32_t)(0) * 4))) {
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x20))) = (*(int32_t*)rtguest::G((uint32_t)aiStack_2c + (uint32_t)(0) * 4));
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb708u)));
      lVar2 = RT_CALLRA(0x2288cc, guest::sceMcWrite((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x20))),0x29ed20,0x3c4));
      if (lVar2 == 0) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 2;
        return 0;
      }
LAB_00228a48:
      __format = 0x2eb718u;
LAB_00228a50:
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(__format)), lVar2);
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0xffffffff;
      RT_CALLRA(0x228a6c, guest::gs_tasks_kill(param_1));
      return 0;
    }
    goto LAB_00228a88;
  case 2:
  case 5:
  case 8:
    if (-1 < (*(int32_t*)rtguest::G((uint32_t)aiStack_2c + (uint32_t)(0) * 4))) {
      RT_CALLRA(0x2288f4, guest::sceMcClose((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x20)))));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) + '\x01';
      return 0;
    }
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
    break;
  case 3:
    if ((*(int32_t*)rtguest::G((uint32_t)aiStack_2c + (uint32_t)(0) * 4)) == 0) {
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb738u)));
      lVar2 = RT_CALLRA(0x228938, guest::sceMcOpen((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26))),0,0x2eb750,0x200));
      if (lVar2 == 0) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 4;
        return 0;
      }
      __format = 0x2eb6e8u;
      goto LAB_00228a50;
    }
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
    break;
  case 4:
    if (-1 < (*(int32_t*)rtguest::G((uint32_t)aiStack_2c + (uint32_t)(0) * 4))) {
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x20))) = (*(int32_t*)rtguest::G((uint32_t)aiStack_2c + (uint32_t)(0) * 4));
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb708u)));
      lVar2 = RT_CALLRA(0x228988, guest::sceMcWrite((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x20))),0x100000,0x8158));
      if (lVar2 == 0) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 5;
        return 0;
      }
      goto LAB_00228a48;
    }
    goto LAB_00228a88;
  case 6:
    if ((*(int32_t*)rtguest::G((uint32_t)aiStack_2c + (uint32_t)(0) * 4)) == 0) {
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb770u)));
      lVar2 = RT_CALLRA(0x2289d0, guest::sceMcOpen((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26))),0,0x2eb5b0,0x200));
      if (lVar2 == 0) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 7;
        return 0;
      }
      __format = 0x2eb6e8u;
      goto LAB_00228a50;
    }
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
    break;
  case 7:
    if (-1 < (*(int32_t*)rtguest::G((uint32_t)aiStack_2c + (uint32_t)(0) * 4))) {
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x20))) = (*(int32_t*)rtguest::G((uint32_t)aiStack_2c + (uint32_t)(0) * 4));
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eb708u)));
      lVar2 = RT_CALLRA(0x228a34, guest::sceMcWrite((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x20))),
                         0x177f760 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x27))) * 0x689,0x3448));
      if (lVar2 == 0) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 8;
        return 0;
      }
      goto LAB_00228a48;
    }
LAB_00228a88:
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
    break;
  case 9:
    if ((*(int32_t*)rtguest::G((uint32_t)aiStack_2c + (uint32_t)(0) * 4)) == 0) {
      RT_CALLRA(0x228abc, guest::PlayUiSound(0x303));
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 1;
    }
    else {
      RT_CALLRA(0x228ad8, guest::PlayUiSound(0x1e));
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0xffffffff;
    }
    RT_CALLRA(0x228a9c, guest::gs_tasks_kill(param_1));
  default:
    goto switchD_00228840_default;
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0xffffffff;
  RT_CALLRA(0x228af4, guest::gs_tasks_kill(param_1));
switchD_00228840_default:
  return 0;
}


RT_GUEST_FN(0x00228708, guest::MemcardCreateSlotTask)

}  // namespace guest
