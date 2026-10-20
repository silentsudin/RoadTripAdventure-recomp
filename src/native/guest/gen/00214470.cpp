// guest:0x00214470 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t HandleControllerConfigMenuInput(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(160);
    uint32_t uStack_a0 = rtguest::allocFrame(8);
    uint32_t uStack_98 = rtguest::allocFrame(8);
    uint32_t uStack_90 = rtguest::allocFrame(8);
    uint32_t uStack_88 = rtguest::allocFrame(8);
    uint32_t iStack_80 = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t uStack_70 = rtguest::allocFrame(8);
    uint32_t uStack_68 = rtguest::allocFrame(8);
    uint32_t uStack_60 = rtguest::allocFrame(8);
    uint32_t uStack_58 = rtguest::allocFrame(8);
    uint32_t iStack_50 = rtguest::allocFrame(4);
  ushort uVar1;
  bool bVar2;
  int iVar3;
  long lVar4;
uint32_t piVar5;
  int iVar6;
  uint uVar7;
uint32_t piVar8;
  uint uVar9;
uint32_t piVar10;
uint32_t piVar11;
  int iVar12;
uint32_t __s;
  int iVar13;
uint32_t puVar14;
  
  iVar13 = (int)param_1;
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1c)));
  __s = (uint32_t)param_4;
  iVar6 = (*(int32_t*)rtguest::G((uint32_t)__s));
  puVar14 = 0x177f760 + iVar3 * 0x689;
  if (iVar6 == 0x65) {
LAB_002145b8:
    lVar4 = NavigateGridSelection(param_2,(*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(2) * sizeof(int32_t))),1,0xc,iVar3,1);
    (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(2) * sizeof(int32_t))) = (int)lVar4;
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1c))) * 2 + (int)param_2 + 0x30)));
    if ((uVar1 & 0x10) != 0) {
      piVar10 = __s + 4;
      PlayUiSound(0x1b);
      (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(3) * sizeof(int32_t))) = 0;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)piVar10));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_98)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(__s + 6)));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(__s + 8)));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_88)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(__s + 10)));
      (*(int32_t*)rtguest::G((uint32_t)iStack_80)) = (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(0xc) * sizeof(int32_t)));
      GetControllerButtonConfig(puVar14,param_4);
      (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)piVar10));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(__s + 6)));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(__s + 8)));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(__s + 10)));
      (*(int32_t*)rtguest::G((uint32_t)iStack_50)) = (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(0xc) * sizeof(int32_t)));
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)piVar10)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_a0));
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(__s + 6))) = (*(uint64_t*)rtguest::G((uint32_t)uStack_98));
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(__s + 8))) = (*(uint64_t*)rtguest::G((uint32_t)uStack_90));
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(__s + 10))) = (*(uint64_t*)rtguest::G((uint32_t)uStack_88));
      (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(0xc) * sizeof(int32_t))) = (*(int32_t*)rtguest::G((uint32_t)iStack_80));
      iVar3 = ::memcmp((char *)rtguest::G((uint32_t)(uStack_a0)), (char *)rtguest::G((uint32_t)(uStack_70)), 0x24);
      if (iVar3 != 0) {
        gs_tasks_spawn(param_1,0x2173d0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1c))));
        (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(2) * sizeof(int32_t))) = 10;
        (*(int32_t*)rtguest::G((uint32_t)__s)) = 0x66;
        return 0x66;
      }
LAB_00214894:
      (*(int32_t*)rtguest::G((uint32_t)__s)) = 300;
      return 300;
    }
    if (lVar4 == 9) {
      if ((uVar1 & 0x40) == 0) {
        return (*(int32_t*)rtguest::G((uint32_t)__s));  // dropped *__s return
      }
      PlayUiSound(0x1a);
      ResetControllerConfigDefaults(puVar14,param_4);
      return (*(int32_t*)rtguest::G((uint32_t)__s));  // dropped *__s return
    }
    if (lVar4 < 9) {
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(2) * sizeof(int32_t)));
    }
    else {
      if (lVar4 == 10) {
        if ((uVar1 & 0x40) != 0) {
          bVar2 = false;
          PlayUiSound(0x1a);
          GetControllerButtonConfig(puVar14,param_4);
          piVar8 = __s + 0xd;
          uVar9 = 0;
          piVar11 = __s + 4;
          iVar3 = 0;
          piVar10 = piVar8;
          do {
            (*(int32_t*)rtguest::G((uint32_t)piVar8)) = 0;
            if (uVar9 < 9) {
              uVar7 = 0;
              piVar5 = __s + 4;
              iVar6 = 0;
              do {
                if ((uVar9 != uVar7) &&
                   ((*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(iVar3 + (*(int32_t*)rtguest::G((uint32_t)piVar11))) * 4)) == (*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(iVar6 + (*(int32_t*)rtguest::G((uint32_t)piVar5))) * 4)))) {
                  (*(int32_t*)rtguest::G((uint32_t)piVar10)) = 1;
                  bVar2 = true;
                  break;
                }
                uVar7 = uVar7 + 1;
                piVar5 = piVar5 + 0x4;  // T* element step
                iVar6 = iVar6 + 0xf;
              } while (uVar7 < 9);
            }
            uVar9 = uVar9 + 1;
            piVar11 = piVar11 + 0x4;  // T* element step
            iVar3 = iVar3 + 0xf;
            piVar8 = piVar8 + 0x4;  // T* element step
            piVar10 = piVar10 + 0x4;  // T* element step
          } while (uVar9 < 0xc);
          if (!bVar2) goto LAB_00214aa0;
          iVar3 = (*(int32_t*)rtguest::G((uint32_t)__s));
          goto LAB_002149f8;
        }
        goto LAB_00214aa0;
      }
      if (lVar4 == 0xb) {
        if ((uVar1 & 0x40) == 0) {
          return (*(int32_t*)rtguest::G((uint32_t)__s));  // dropped *__s return
        }
        if ((*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(0x18) * sizeof(int32_t))) != 0) {
          PlayUiSound(0x1e);
          return (*(int32_t*)rtguest::G((uint32_t)__s));  // dropped *__s return
        }
        PlayUiSound(0x1a);
        SetControllerButtonConfig(puVar14,param_4);
        (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(3) * sizeof(int32_t))) = 0;
        goto LAB_00214894;
      }
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(2) * sizeof(int32_t)));
    }
    iVar6 = 0;
    if ((*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(iVar3 * 0xf) * 4)) != -1) {
      piVar10 = 0x0029c620 + iVar3 * 0xf;
      do {
        piVar10 = piVar10 + 0x4;  // T* element step
        iVar6 = iVar6 + 1;
      } while ((*(int32_t*)rtguest::G((uint32_t)piVar10)) != -1);
    }
    bVar2 = false;
    iVar12 = 0;
    iVar3 = NavigateGridSelection(param_2,(*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(iVar3 + 4) * sizeof(int32_t))),iVar6,1,(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1c))),1);
    piVar8 = __s + 0xd;
    uVar9 = 0;
    (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(2) * sizeof(int32_t))) + 4) * sizeof(int32_t))) = iVar3;
    piVar11 = __s + 4;
    piVar10 = piVar8;
    do {
      (*(int32_t*)rtguest::G((uint32_t)piVar8)) = 0;
      if (uVar9 < 9) {
        uVar7 = 0;
        piVar5 = __s + 4;
        iVar3 = 0;
        do {
          if ((uVar9 != uVar7) &&
             ((*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(iVar12 + (*(int32_t*)rtguest::G((uint32_t)piVar11))) * 4)) == (*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(iVar3 + (*(int32_t*)rtguest::G((uint32_t)piVar5))) * 4)))) {
            (*(int32_t*)rtguest::G((uint32_t)piVar10)) = 1;
            bVar2 = true;
            break;
          }
          uVar7 = uVar7 + 1;
          piVar5 = piVar5 + 0x4;  // T* element step
          iVar3 = iVar3 + 0xf;
        } while (uVar7 < 9);
      }
      uVar9 = uVar9 + 1;
      piVar11 = piVar11 + 0x4;  // T* element step
      iVar12 = iVar12 + 0xf;
      piVar8 = piVar8 + 0x4;  // T* element step
      piVar10 = piVar10 + 0x4;  // T* element step
    } while (uVar9 < 0xc);
    if (bVar2) {
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)__s));
LAB_002149f8:
      (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(0x18) * sizeof(int32_t))) = 1;
      return iVar3;
    }
    goto LAB_00214aa0;
  }
  if (0x65 < iVar6) {
    if (iVar6 == 200) {
      return 200;
    }
    if (iVar6 < 0xc9) {
      if (iVar6 != 0x66) {
        (*(int32_t*)rtguest::G((uint32_t)__s)) = 0;
LAB_00214a90:
        (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(1) * sizeof(int32_t))) = 0;
        return 0;
      }
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x24))) == '\0') {
        return 0x66;
      }
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 4))) == 0) {
        PlayUiSound(0x1a);
        (*(int32_t*)rtguest::G((uint32_t)__s)) = 300;
        return 300;
      }
      PlayUiSound(0x1b);
      goto LAB_00214a3c;
    }
    if (iVar6 == 300) {
      (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
      (*(int32_t*)rtguest::G((uint32_t)__s)) = 0x12d;
    }
    else if (iVar6 != 0x12d) {
      (*(int32_t*)rtguest::G((uint32_t)__s)) = 0;
      goto LAB_00214a90;
    }
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) + 0x28;
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(1) * sizeof(int32_t)));
    (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(1) * sizeof(int32_t))) = iVar3 + -1;
    if (iVar3 + -1 == 0) {
      (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(1) * sizeof(int32_t))) = 0;
      (*(int32_t*)rtguest::G((uint32_t)__s)) = 399;
      return 399;
    }
    goto LAB_00214aa0;
  }
  if (iVar6 == 1) {
LAB_0021456c:
    (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
    (*(int32_t*)rtguest::G((uint32_t)__s)) = 2;
  }
  else {
    if (iVar6 < 2) {
      if (iVar6 != 0) {
        (*(int32_t*)rtguest::G((uint32_t)__s)) = 0;
        goto LAB_00214a90;
      }
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x6c));
      GetControllerButtonConfig(puVar14,param_4);
      (*(int32_t*)rtguest::G((uint32_t)__s)) = 1;
      goto LAB_0021456c;
    }
    if (iVar6 != 2) {
      if (iVar6 != 100) {
        (*(int32_t*)rtguest::G((uint32_t)__s)) = 0;
        goto LAB_00214a90;
      }
      (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(3) * sizeof(int32_t))) = 1;
      (*(int32_t*)rtguest::G((uint32_t)__s)) = 0x65;
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1c)));
      goto LAB_002145b8;
    }
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) + -0x28;
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(1) * sizeof(int32_t)));
  (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(1) * sizeof(int32_t))) = iVar3 + -1;
  if (iVar3 + -1 == 0) {
    (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(1) * sizeof(int32_t))) = 0;
LAB_00214a3c:
    (*(int32_t*)rtguest::G((uint32_t)__s)) = 100;
    return 100;
  }
LAB_00214aa0:
  return (*(int32_t*)rtguest::G((uint32_t)__s));  // dropped *__s return
}


RT_GUEST_FN(0x00214470, guest::HandleControllerConfigMenuInput)

}  // namespace guest
