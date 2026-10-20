// guest:0x00212ac8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateVerticalMenuNavigation(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  bool bVar1;
  ushort uVar2;
  int iVar3;
  int iVar4;
uint32_t piVar5;
uint32_t piVar6;
  int iVar7;
  uint uVar8;
  
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4));
  if (iVar3 == 0x66) {
    if ((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) != 0) {
      (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) + -1;
      piVar6 = (uint32_t)(param_3 + 0x10 + ((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t))) + 1) * 0x24 + 0x8200);
      (*(int32_t*)rtguest::G((uint32_t)piVar6)) = (*(int32_t*)rtguest::G((uint32_t)piVar6)) + -5;
      piVar6 = (uint32_t)(param_3 + 0x10 + ((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(4) * sizeof(int32_t))) + 1) * 0x24 + 0x8200);
      (*(int32_t*)rtguest::G((uint32_t)piVar6)) = (*(int32_t*)rtguest::G((uint32_t)piVar6)) + 5;
    }
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t)));
    uVar8 = 1;
    if (iVar3 < 0) {
      (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t))) = iVar3 + 1;
      iVar3 = 0x8224;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81cc))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81cc))) + 5;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81d0))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81d0))) + 0xe;
      do {
        piVar6 = (uint32_t)(param_3 + 0x10 + iVar3);
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + iVar3 + 0xc))) & 1) != 0) {
          piVar5 = (uint32_t)(param_3 + 0x14 + iVar3);
          (*(int32_t*)rtguest::G((uint32_t)piVar6)) = (*(int32_t*)rtguest::G((uint32_t)piVar6)) + 5;
          (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (*(int32_t*)rtguest::G((uint32_t)piVar5)) + 0xe;
        }
        uVar8 = uVar8 + 1;
        iVar3 = iVar3 + 0x24;
      } while (uVar8 < 0xb);
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t)));
    }
    else if (iVar3 < 1) {
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t)));
    }
    else {
      piVar6 = (uint32_t)(param_3 + 0x8238);
      (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t))) = iVar3 + -1;
      uVar8 = 1;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81cc))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81cc))) + -5;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81d0))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81d0))) + -0xe;
      do {
        uVar8 = uVar8 + 1;
        if (((*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(-2) * sizeof(int32_t))) & 1U) != 0) {
          (*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(-1) * sizeof(int32_t))) = (*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(-1) * sizeof(int32_t))) + -5;
          (*(int32_t*)rtguest::G((uint32_t)piVar6)) = (*(int32_t*)rtguest::G((uint32_t)piVar6)) + -0xe;
        }
        piVar6 = piVar6 + 0x24;  // T* element step
      } while (uVar8 < 0xb);
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t)));
    }
    if (iVar3 != 0) {
      return (*(int32_t*)rtguest::G((uint32_t)param_4));  // dropped *param_4 return
    }
    if ((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t))) != 0) {
      return (*(int32_t*)rtguest::G((uint32_t)param_4));  // dropped *param_4 return
    }
    (*(int32_t*)rtguest::G((uint32_t)param_4)) = 100;
    goto LAB_002135ac;
  }
  if (iVar3 < 0x67) {
    if (iVar3 == 3) {
      iVar3 = 0;
      if (0 < (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x17) * sizeof(int32_t))) + 1) {
        iVar4 = 0x8200;
        do {
          piVar6 = (uint32_t)(param_3 + 0x10 + iVar4);
          iVar3 = iVar3 + 1;
          (*(int32_t*)rtguest::G((uint32_t)piVar6)) = (*(int32_t*)rtguest::G((uint32_t)piVar6)) + -0x28;
          iVar4 = iVar4 + 0x24;
        } while (iVar3 < (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x17) * sizeof(int32_t))) + 1);
      }
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t)));
      (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = iVar3 + -1;
      if (iVar3 + -1 != 0) goto LAB_002135ac;
      iVar3 = 10;
      iVar4 = 4;
    }
    else {
      if (3 < iVar3) {
        if (iVar3 == 100) {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81ec))) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8204))) = 0;
          iVar3 = 0;
          if ((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(5) * sizeof(int32_t))) != 0) {
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81ec))) = 1;
            iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(5) * sizeof(int32_t)));
          }
          iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x17) * sizeof(int32_t))) - (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x18) * sizeof(int32_t)));
          if (iVar4 < 1) {
            iVar4 = 0;
          }
          if (iVar3 != iVar4) {
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8204))) = 1;
          }
          (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0x65;
          uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x30)));
        }
        else {
          if (iVar3 < 0x65) {
            if (iVar3 == 4) {
              iVar3 = 1;
              if ((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x17) * sizeof(int32_t))) + 1 < 2) {
                iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t)));
              }
              else {
                iVar4 = 0x8224;
                do {
                  piVar6 = (uint32_t)(param_3 + 0x10 + iVar4);
                  iVar3 = iVar3 + 1;
                  (*(int32_t*)rtguest::G((uint32_t)piVar6)) = (*(int32_t*)rtguest::G((uint32_t)piVar6)) + -0x28;
                  iVar4 = iVar4 + 0x24;
                } while (iVar3 < (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x17) * sizeof(int32_t))) + 1);
                iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t)));
              }
              (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = iVar3 + -1;
              if (iVar3 + -1 != 0) goto LAB_002135ac;
              iVar3 = 100;
              goto LAB_0021358c;
            }
            (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
            goto LAB_002135a8;
          }
          uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x30)));
        }
        if ((uVar2 & 0x40) == 0) {
          if ((uVar2 & 0x10) == 0) {
            if ((uVar2 & 0x800) == 0) {
              iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x17) * sizeof(int32_t)));
            }
            else {
              if ((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x19) * sizeof(int32_t))) != 0) goto LAB_00213014;
              iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x17) * sizeof(int32_t)));
            }
            iVar3 = RT_CALLRA(0x213050, guest::NavigateGridSelection(param_2,(*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t))),1,iVar3,0,1));
            if ((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t))) != iVar3) {
              iVar4 = (iVar3 + 1) * 0x24;
              iVar7 = iVar4 + 0x8210;
              (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(4) * sizeof(int32_t))) = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t)));
              (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t))) = iVar3;
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x10 + iVar7))) = 1;
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x14 + iVar7))) = 1;
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + param_3 + 0xc + 0x8220))) = 1;
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81c4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + iVar7 + 0x18)));
              iVar7 = ((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(4) * sizeof(int32_t))) + 1) * 0x24;
              iVar4 = iVar7 + 0x8210;
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x10 + iVar4))) = 0;
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x14 + iVar4))) = 0;
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + param_3 + 0xc + 0x8220))) = 0;
              (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 4;
              (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t))) = 0;
              if (iVar3 < (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(4) * sizeof(int32_t)))) {
                while( true ) {
                  iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(5) * sizeof(int32_t)));
                  if ((iVar3 == 0) || (iVar3 + 2 <= (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t))))) break;
                  (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(5) * sizeof(int32_t))) = iVar3 + -1;
                  (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t))) = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t))) + -4;
                }
              }
              else {
                iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x17) * sizeof(int32_t))) - (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x18) * sizeof(int32_t)));
                if (iVar3 < 1) {
                  iVar3 = 0;
                }
                while( true ) {
                  iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(5) * sizeof(int32_t)));
                  if ((iVar4 == iVar3) || ((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t))) < iVar4 + (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x18) * sizeof(int32_t))) + -2)) break;
                  (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(5) * sizeof(int32_t))) = iVar4 + 1;
                  (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t))) = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t))) + 4;
                }
              }
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81ec))) = 0;
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8204))) = 0;
              if ((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(5) * sizeof(int32_t))) == 0) {
                iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x17) * sizeof(int32_t)));
                iVar4 = 0;
              }
              else {
                (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81ec))) = 1;
                iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(5) * sizeof(int32_t)));
                iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x17) * sizeof(int32_t)));
              }
              iVar3 = iVar3 - (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x18) * sizeof(int32_t)));
              if (iVar3 < 1) {
                iVar3 = 0;
              }
              if (iVar4 == iVar3) {
                (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0x66;
              }
              else {
                (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8204))) = 1;
                (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0x66;
              }
            }
            goto LAB_002135ac;
          }
LAB_00213014:
          RT_CALLRA(0x212fdc, guest::PlayUiSound(0x1b));
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81ec))) = 0;
          iVar3 = 300;
        }
        else {
          RT_CALLRA(0x213014, guest::PlayUiSound(0x1a));
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81ec))) = 0;
          iVar3 = 200;
        }
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8204))) = 0;
        (*(int32_t*)rtguest::G((uint32_t)param_4)) = iVar3;
        goto LAB_002135ac;
      }
      if (iVar3 != 1) {
        if (1 < iVar3) {
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) + -0x28;
          iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t)));
          (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = iVar3 + -1;
          if (iVar3 + -1 == 0) {
            (*(int32_t*)rtguest::G((uint32_t)param_4)) = 3;
            (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 6;
          }
          goto LAB_002135ac;
        }
        if (iVar3 != 0) {
          (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
          goto LAB_002135a8;
        }
        piVar6 = (uint32_t)(param_3 + 0x10);
        uVar8 = 1;
        iVar7 = 0x8224;
        iVar3 = ((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t))) + 1) * 0x24;
        iVar4 = iVar3 + 0x8210;
        (*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t))) + 1) * 9 + 0x2084) * sizeof(int32_t))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x14 + iVar4))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + param_3 + 0x822c))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81c4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + iVar4 + 0x18)));
        (*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t))) + 1) * 9 + 0x2080) * sizeof(int32_t))) = (*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t))) + 1) * 9 + 0x2080) * sizeof(int32_t))) + -0x14;
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar6)) * -0x14;
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)piVar6)) * -0x38;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81cc))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81cc))) + iVar3;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81d0))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81d0))) + iVar4;
        do {
          if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + iVar7 + 0xc))) & 1) != 0) {
            piVar5 = (uint32_t)(param_3 + 0x14 + iVar7);
            (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar6 + iVar7))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar6 + iVar7))) + iVar3;
            (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (*(int32_t*)rtguest::G((uint32_t)piVar5)) + iVar4;
          }
          uVar8 = uVar8 + 1;
          iVar7 = iVar7 + 0x24;
        } while (uVar8 < 0xb);
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x10)));
        (*(int32_t*)rtguest::G((uint32_t)param_4)) = 1;
        (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t))) = ((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(5) * sizeof(int32_t))) - iVar3) * 4;
      }
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t)));
      if (iVar3 < 0) {
        uVar8 = 1;
        (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t))) = iVar3 + 1;
        iVar3 = 0x8224;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81cc))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81cc))) + 5;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81d0))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81d0))) + 0xe;
        do {
          piVar6 = (uint32_t)(param_3 + 0x10 + iVar3);
          if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + iVar3 + 0xc))) & 1) != 0) {
            piVar5 = (uint32_t)(param_3 + 0x14 + iVar3);
            (*(int32_t*)rtguest::G((uint32_t)piVar6)) = (*(int32_t*)rtguest::G((uint32_t)piVar6)) + 5;
            (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (*(int32_t*)rtguest::G((uint32_t)piVar5)) + 0xe;
          }
          uVar8 = uVar8 + 1;
          iVar3 = iVar3 + 0x24;
        } while (uVar8 < 0xb);
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t)));
      }
      else if (0 < iVar3) {
        piVar6 = (uint32_t)(param_3 + 0x8238);
        (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t))) = iVar3 + -1;
        uVar8 = 1;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81cc))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81cc))) + -5;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81d0))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x81d0))) + -0xe;
        do {
          uVar8 = uVar8 + 1;
          if (((*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(-2) * sizeof(int32_t))) & 1U) != 0) {
            (*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(-1) * sizeof(int32_t))) = (*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(-1) * sizeof(int32_t))) + -5;
            (*(int32_t*)rtguest::G((uint32_t)piVar6)) = (*(int32_t*)rtguest::G((uint32_t)piVar6)) + -0xe;
          }
          piVar6 = piVar6 + 0x24;  // T* element step
        } while (uVar8 < 0xb);
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t)));
      }
      if (iVar3 != 0) {

        return (*(int32_t*)rtguest::G((uint32_t)param_4));  // dropped *param_4 return

      }
      iVar3 = 10;
      iVar4 = 2;
    }
LAB_002134a8:
    (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = iVar3;
    (*(int32_t*)rtguest::G((uint32_t)param_4)) = iVar4;
    goto LAB_002135ac;
  }
  if (iVar3 == 0xcb) {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x17) * sizeof(int32_t)));
    iVar4 = 0;
    if (0 < iVar3 + 1) {
      iVar7 = 0x8200;
      do {
        piVar6 = (uint32_t)(param_3 + 0x10 + iVar7);
        if (iVar4 != (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t))) + 1) {
          (*(int32_t*)rtguest::G((uint32_t)piVar6)) = (*(int32_t*)rtguest::G((uint32_t)piVar6)) + -0x28;
          iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x17) * sizeof(int32_t)));
        }
        iVar4 = iVar4 + 1;
        iVar7 = iVar7 + 0x24;
      } while (iVar4 < iVar3 + 1);
    }
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t)));
    (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = iVar3 + -1;
    if (iVar3 + -1 != 0) goto LAB_002135ac;
    iVar3 = 0xcc;
  }
  else {
    if (iVar3 < 0xcc) {
      if (iVar3 != 0xc9) {
        if (0xc9 < iVar3) {
          iVar3 = 0;
          if (0 < (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x17) * sizeof(int32_t))) + 1) {
            iVar4 = 0x8200;
            do {
              piVar6 = (uint32_t)(param_3 + 0x10 + iVar4);
              iVar3 = iVar3 + 1;
              (*(int32_t*)rtguest::G((uint32_t)piVar6)) = (*(int32_t*)rtguest::G((uint32_t)piVar6)) + -0x28;
              iVar4 = iVar4 + 0x24;
            } while (iVar3 < (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x17) * sizeof(int32_t))) + 1);
          }
          iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t)));
          (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = iVar3 + -1;
          if (iVar3 + -1 != 0) goto LAB_002135ac;
          iVar3 = 10;
          iVar4 = 0xcb;
          goto LAB_002134a8;
        }
        if (iVar3 != 200) {
          (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
          goto LAB_002135a8;
        }
        (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0xc9;
        (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 10;
      }
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x17) * sizeof(int32_t)));
      iVar4 = 0;
      if (0 < iVar3 + 1) {
        iVar7 = 0x8200;
        do {
          bVar1 = iVar4 == (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t))) + 1;
          iVar4 = iVar4 + 1;
          if (bVar1) {
            piVar6 = (uint32_t)(param_3 + 0x10 + iVar7);
            (*(int32_t*)rtguest::G((uint32_t)piVar6)) = (*(int32_t*)rtguest::G((uint32_t)piVar6)) + -0x28;
            iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x17) * sizeof(int32_t)));
          }
          iVar7 = iVar7 + 0x24;
        } while (iVar4 < iVar3 + 1);
      }
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t)));
      (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = iVar3 + -1;
      if (iVar3 + -1 != 0) goto LAB_002135ac;
      iVar3 = 6;
      iVar4 = 0xca;
      goto LAB_002134a8;
    }
    if (iVar3 != 300) {
      if (iVar3 < 0x12d) {
        if (iVar3 == 0xcc) {
          iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(5) * sizeof(int32_t)));
          iVar4 = 299;
LAB_0021359c:
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x10))) = iVar3;
          (*(int32_t*)rtguest::G((uint32_t)param_4)) = iVar4;
          goto LAB_002135ac;
        }
        (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
      }
      else {
        if (iVar3 == 0x12d) goto LAB_00213538;
        if (iVar3 == 0x12e) {
          iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(5) * sizeof(int32_t)));
          iVar4 = 399;
          goto LAB_0021359c;
        }
        (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
      }
LAB_002135a8:
      (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0;
      goto LAB_002135ac;
    }
    (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
    (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0x12d;
LAB_00213538:
    iVar3 = 0;
    if (0 < (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x17) * sizeof(int32_t))) + 1) {
      iVar4 = 0x8200;
      do {
        piVar6 = (uint32_t)(param_3 + 0x10 + iVar4);
        iVar3 = iVar3 + 1;
        (*(int32_t*)rtguest::G((uint32_t)piVar6)) = (*(int32_t*)rtguest::G((uint32_t)piVar6)) + 0x28;
        iVar4 = iVar4 + 0x24;
      } while (iVar3 < (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(0x17) * sizeof(int32_t))) + 1);
    }
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t)));
    (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = iVar3 + -1;
    if (iVar3 + -1 != 0) goto LAB_002135ac;
    iVar3 = 0x12e;
  }
LAB_0021358c:
  (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
  (*(int32_t*)rtguest::G((uint32_t)param_4)) = iVar3;
LAB_002135ac:
  return (*(int32_t*)rtguest::G((uint32_t)param_4));
}


RT_GUEST_FN(0x00212ac8, guest::UpdateVerticalMenuNavigation)

}  // namespace guest
