// guest:0x0021a990 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AcquireVehicleInput(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
  }
uint32_t psVar1;
  ulong uVar2;
  int iVar3;
uint32_t psVar4;
  uint uVar5;
uint32_t puVar6;
uint32_t puVar7;
  ushort uVar8;
  ushort uVar9;
uint32_t puVar10;
  ulong uVar11;
  int iVar12;
  ulong uVar13;
  undefined8 uVar14;
  
  uVar13 = 0;
  uVar14 = 0;
  iVar12 = (int)param_3;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x250))) != 0) {
    uVar14 = RT_CALLRA(0x21a9cc, guest::UpdateVehicleWaypointProgress(param_2,param_3));
  }
  iVar3 = (int)param_2;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x20))) == '\x18') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x24a))) != 'f') {
      uVar2 = (uint32_t)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x198)));
      goto LAB_0021aa98;
    }
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x19a))) == '\0') {
      uVar2 = (uint32_t)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x198)));
      goto LAB_0021aa98;
    }
    uVar8 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x198)));
    uVar2 = (uint32_t)(short)uVar8;
    if ((long)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x10))) + 2))) + -1) <= (long)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x19b))))
    goto LAB_0021aa98;
    if (uVar2 == 2) {
      RT_CALLRA(0x21aa38, guest::PlayUiSound((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x245))) + 0x8000));
      RT_CALLRA(0x21aa40, guest::PlayUiSound(0x309));
      uVar8 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x198)));
    }
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x198))) = uVar8 | 0x400;
    uVar13 = rtCall((*(uint32_t*)rtguest::G(0x3356dc)), param_2, param_3, uVar14, 0);
    uVar13 = uVar13 & 0xfffffffffffffff7;
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x23c))) + 0x200;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x23c))) = iVar3;
    if (0x40000 < iVar3) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x23c))) = 0x40000;
    }
  }
  else {
    uVar2 = (uint32_t)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x198)));
LAB_0021aa98:
    if (uVar2 == 0x402) {
      RT_CALLRA(0x21aaa4, guest::PlayUiSound(0x8309));
      uVar2 = (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x198)));
    }
    uVar11 = uVar2 & 0xfffffffffffffbff;
    uVar8 = (uint32_t)uVar11;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x198))) = uVar8;
    if ((uVar2 & 0x33) == 0) {
      if ((uVar2 & 0x80) != 0) {
        uVar13 = rtCall((*(uint32_t*)rtguest::G(0x3356dc)), param_2, param_3, uVar14, 0);
        uVar11 = (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x198)));
      }
      goto LAB_0021ac54;
    }
    uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28)));
    if ((uVar5 & 0x40) == 0) {
      if ((uVar2 & 0x80) == 0) {
        if ((uVar2 & 3) != 0) {
          uVar13 = RT_CALLRA(0x21ab98, guest::gs_controls_player_control(param_1,param_2,param_3));
          uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28)));
        }
      }
      else {
        uVar9 = uVar8 & 0x31;
        if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xb))) == '\0') {
          uVar9 = uVar8 & 0x30;
        }
        uVar13 = rtCall((*(uint32_t*)rtguest::G(0x3356dc)), param_2, param_3, uVar14, uVar9);
        uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28)));
      }
    }
    else {
      puVar7 = (uint32_t)((int)param_1 + 0x20);
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)puVar7));
      puVar10 = (uint32_t)((((uint32_t)uVar11 & 0x30) >> 2) + iVar3 + 0x44);
      puVar6 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar10));
      (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = uVar5 + 1;
      uVar13 = 0;
      if ((*(uint16_t*)rtguest::G((uint32_t)puVar6)) == uVar5) {
        puVar6 = puVar6 + 0xc;  // T* element step
        (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)puVar10)) = puVar6;
      }
      if ((int)puVar6 < 0x1ffff9) {
        uVar13 = (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(1) * sizeof(uint16_t)));
      }
      else {
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x198))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x198))) | 0x200;
      }
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28)));
    }
    uVar11 = (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x198)));
    if ((uVar5 & 0x20) == 0) goto LAB_0021ac54;
    puVar10 = (uint32_t)((((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x198))) & 0x30) >> 2) + iVar3 + 0x44);
    psVar1 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar10));
    if ((uint32_t)(*(int16_t*)rtguest::G((uint32_t)psVar1 + (uint32_t)(1) * sizeof(int16_t))) == uVar13) {
      (*(int16_t*)rtguest::G((uint32_t)psVar1)) = (*(int16_t*)rtguest::G((uint32_t)psVar1)) + 1;
    }
    else {
      psVar4 = psVar1 + 6;
      (*(uint32_t*)rtguest::G((uint32_t)puVar10)) = psVar4;
      if (0x1ffff8 < (int)psVar4) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))) = uVar5 & 0xffffffdf;
        goto LAB_0021ac54;
      }
      (*(int16_t*)rtguest::G((uint32_t)psVar1 + (uint32_t)(7) * sizeof(int16_t))) = (short)uVar13;
      (*(int16_t*)rtguest::G((uint32_t)psVar4)) = 1;
    }
  }
  uVar11 = (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x198)));
LAB_0021ac54:
  if ((uVar11 & 0x800) == 0) {
    if ((uVar11 & 0x100) == 0) {
      return uVar13;
    }
    uVar2 = uVar13 & 0xff00;
    if (0x3f < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 8)))) {
      return uVar2;
    }
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x1b8))) < 0x1000) {
      return uVar2 | 1;
    }
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x1b8))) < 0x1801) {
      return uVar2;
    }
  }
  return uVar13 & 0xff00 | 2;
}


RT_GUEST_FN(0x0021a990, guest::AcquireVehicleInput)

}  // namespace guest
