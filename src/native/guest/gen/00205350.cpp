// guest:0x00205350 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiMenuWindow(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  byte bVar1;
  char cVar2;
  short sVar3;
uint32_t psVar4;
  uint uVar5;
  undefined8 uVar6;
  ushort uVar7;
  int iVar8;
uint32_t pbVar9;
  int iVar10;
  long unaff_s5;
  int iVar11;
  
  if ((char)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * sizeof(int16_t))) == '\x01') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x13))) < 0x10) {
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x13))) + 2;
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x13))) = bVar1;
LAB_002053ec:
      uiSetTexa((uint32_t)bVar1 << 3);
      sVar3 = (*(int16_t*)rtguest::G((uint32_t)param_1));
    }
    else {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(7) * sizeof(int16_t)))) = 0;
      uiSetTexa(0x80);
      sVar3 = (*(int16_t*)rtguest::G((uint32_t)param_1));
    }
  }
  else {
    if ((char)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * sizeof(int16_t))) == '\x02') {
      cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x13)));
      bVar1 = cVar2 - 1;
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x13))) = bVar1;
      if (cVar2 != '\0') goto LAB_002053ec;
    }
    else {
      uiSetTexa(0x80);
    }
    sVar3 = (*(int16_t*)rtguest::G((uint32_t)param_1));
  }
  uiDrawPanel9(sVar3,(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int16_t))),(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int16_t))),(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int16_t))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x5f))));
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xb))) == '\0') {
    iVar10 = (int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int16_t)));
    goto LAB_002055e8;
  }
  if ((char)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * sizeof(int16_t))) == '\0') {
    cVar2 = NavigateGridSelection
                      (param_2,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xd))),1,
                       (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xb))) + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x11))),
                       (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(6) * sizeof(int16_t)))) & 3,((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(6) * sizeof(int16_t)))) >> 2 ^ 1) & 1);
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(8) * sizeof(int16_t))));
    uVar5 = (uint32_t)bVar1;
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xd))) = cVar2;
    if ((int)cVar2 < (int)uVar5) {
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(8) * sizeof(int16_t)))) = bVar1 - 1;
      iVar10 = 0;
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xb))) != '\0') {
        psVar4 = param_1 + (uint32_t)(10) * sizeof(int16_t);
        do {
          iVar8 = iVar10 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(8) * sizeof(int16_t))));
          iVar10 = iVar10 + 1;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)psVar4)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 * 4 + param_3)));
          psVar4 = psVar4 + (uint32_t)(2) * sizeof(int16_t);
        } while (iVar10 < (int)(uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xb))));
        cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xd)));
      }
      goto LAB_00205544;
    }
    cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xd)));
    if ((long)(int)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xb))) + uVar5) <= (long)cVar2) {
      iVar10 = 0;
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(8) * sizeof(int16_t)))) = bVar1 + 1;
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xb))) != 0) {
        psVar4 = param_1 + (uint32_t)(10) * sizeof(int16_t);
        do {
          iVar8 = iVar10 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(8) * sizeof(int16_t))));
          iVar10 = iVar10 + 1;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)psVar4)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 * 4 + param_3)));
          psVar4 = psVar4 + (uint32_t)(2) * sizeof(int16_t);
        } while (iVar10 < (int)(uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xb))));
        cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xd)));
      }
      goto LAB_00205544;
    }
  }
  else {
    cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xd)));
LAB_00205544:
    uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(8) * sizeof(int16_t))));
  }
  uiDrawBar3((int)(*(int16_t*)rtguest::G((uint32_t)param_1)) + (int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(int16_t))) + 0x14,
             ((int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int16_t))) + (((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(5) * sizeof(int16_t)))) + (int)cVar2) - uVar5) * 0xc) * 2,
             (int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int16_t))) + (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(int16_t))) * -2 + -0x14);
  if ((char)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * sizeof(int16_t))) == '\0') {
    uiDrawArrow((int)(*(int16_t*)rtguest::G((uint32_t)param_1)) + (int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(int16_t))) + -0x14,
                ((int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int16_t))) +
                (((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(5) * sizeof(int16_t)))) + (int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xd)))) -
                (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(8) * sizeof(int16_t))))) * 0xc) * 2);
    iVar10 = (int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int16_t)));
  }
  else {
    iVar10 = (int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int16_t)));
  }
LAB_002055e8:
  iVar11 = -1;
  iVar8 = 0;
  iVar10 = iVar10 + -0xb;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xf))) != '\0') {
    psVar4 = param_1 + (uint32_t)(10) * sizeof(int16_t);
    pbVar9 = (uint32_t)(param_1 + (uint32_t)(0x28) * sizeof(int16_t));
    do {
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar9));
      if ((bVar1 & 0x80) == 0) {
        if ((bVar1 & 8) == 0) {
          if ((bVar1 & 0x10) == 0) {
            unaff_s5 = (long)(*(int16_t*)rtguest::G((uint32_t)param_1));
          }
          else {
            unaff_s5 = (long)((int)(*(int16_t*)rtguest::G((uint32_t)param_1)) + (int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(int16_t))) + 0x14);
          }
        }
        else {
          unaff_s5 = (long)((int)(*(int16_t*)rtguest::G((uint32_t)param_1)) + (int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int16_t))));
        }
        iVar11 = iVar11 + 1;
        iVar10 = iVar10 + 0xc;
        cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xb)));
      }
      else {
        cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xb)));
      }
      if (cVar2 == '\0') {
        cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x5f)));
LAB_002056a4:
        uVar6 = 10;
        if (cVar2 != '\x02') {
          uVar6 = 0;
        }
LAB_002056b0:
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar9));
      }
      else {
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar9));
        uVar6 = 6;
        if (((bVar1 & 1) == 0) && (uVar6 = 5, (bVar1 & 2) == 0)) {
          uVar6 = 0;
          if (iVar11 != ((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(5) * sizeof(int16_t)))) + (int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xd)))) -
                        (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(8) * sizeof(int16_t))))) {
            cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x5f)));
            goto LAB_002056a4;
          }
          goto LAB_002056b0;
        }
      }
      if ((bVar1 & 0x40) == 0) {
        if ((bVar1 & 8) == 0) {
          unaff_s5 = uiDrawText(unaff_s5,iVar10,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)psVar4)),uVar6);
        }
        else {
          unaff_s5 = uiDrawTextRight((int)(*(int16_t*)rtguest::G((uint32_t)param_1)) + (int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int16_t))),iVar10);
        }
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xf)));
      }
      else {
        uiDrawNumberAuto(unaff_s5,iVar10,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)psVar4)));
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xf)));
      }
      iVar8 = iVar8 + 1;
      psVar4 = psVar4 + (uint32_t)(2) * sizeof(int16_t);
      pbVar9 = pbVar9 + 1;
    } while (iVar8 < (int)(uint32_t)bVar1);
  }
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(6) * sizeof(int16_t)))) & 3;
  iVar10 = (int)param_2;
  if (bVar1 == 1) {
    uVar7 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x32)));
  }
  else if (bVar1 < 2) {
    if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(6) * sizeof(int16_t)))) & 3) == 0) {
      uVar7 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x30)));
    }
    else {
      uVar7 = 0;
    }
  }
  else if (bVar1 == 2) {
    uVar7 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x32))) | (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x30)));
  }
  else {
    uVar7 = 0;
  }
  if ((char)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * sizeof(int16_t))) == '\0') {
    bVar1 = (char)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(9) * sizeof(int16_t))) + 1;
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(9) * sizeof(int16_t)))) = bVar1;
    if ((bVar1 & 0x10) != 0) {
      bVar1 = 0;
      if ((char)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(8) * sizeof(int16_t))) != '\0') {
        uiDrawIcon20((int)(*(int16_t*)rtguest::G((uint32_t)param_1)) +
                     ((int)((int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int16_t))) + (uint32_t)((uint32_t)(uint16_t)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int16_t))) >> 0xf)) >> 1) + -10,
                     (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int16_t))) * 2 + -0x14,2);
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(8) * sizeof(int16_t))));
      }
      if (bVar1 < (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x11)))) {
        uiDrawIcon20((int)(*(int16_t*)rtguest::G((uint32_t)param_1)) +
                     ((int)((int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int16_t))) + (uint32_t)((uint32_t)(uint16_t)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int16_t))) >> 0xf)) >> 1) + -10,
                     ((int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int16_t))) + (int)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int16_t)))) * 2,3);
      }
    }
    if ((uVar7 & 0x40) != 0) {
      return 1;
    }
    if ((((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(6) * sizeof(int16_t)))) & 8) != 0) && ((uVar7 & 0x800) != 0)) {
      return 1;
    }
    if ((uVar7 & 0x10) != 0) {
      return 0xffffffff;
    }
  }
  else {
    if ((char)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * sizeof(int16_t))) != '\x02') {
      return 0;
    }
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x13))) == '\0') {
      return 0xfffffffe;
    }
  }
  return 0;
}


RT_GUEST_FN(0x00205350, guest::uiMenuWindow)

}  // namespace guest
