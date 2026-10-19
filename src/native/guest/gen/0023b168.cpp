// guest:0x0023b168 transliterated from Ghidra pseudocode — DO NOT hand-edit
// dialogueTask: message-window task ('next msg #%d' script driver).
// String literal "next msg #%d\n" verified in ELF at vaddr 0x2ec7c8.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t dialogueTask(uint32_t param_1, uint32_t param_2, ...) {
  int8_t cVar2;
  uint16_t uVar3;
  int32_t iVar4;
  uint8_t bVar5;
  int32_t iVar6;
  uint32_t uVar7;
  uint32_t uVar8;
  uint32_t puVar9;
  uint32_t uVar10;
  int32_t lVar11;
  int32_t iVar12;
  uint32_t puVar13;
  uint32_t pcVar14;

  iVar12 = (int32_t)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)iVar12 + 8)) == 0) {
    uVar8 = (*(uint32_t*)rtguest::G((*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x1c)) + 4));
    (*(int32_t*)rtguest::G((uint32_t)iVar12 + 0x20)) =
         (*(int32_t*)rtguest::G((uint32_t)iVar12 + 0x1c));
    (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x28)) = uVar8;
    PlayUiSound(0x25);
  }
  cVar2 = (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x25));
  if (cVar2 == '\0') {
    uiSetTexa((*(int32_t*)rtguest::G((uint32_t)iVar12 + 8)) << 3);
    uiDrawDialogueFrameA(0x158);
    iVar6 = (int32_t)uiTextWidth(
         (*(uint32_t*)rtguest::G((*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x20)))));
    iVar6 = iVar6 * (*(int32_t*)rtguest::G((uint32_t)iVar12 + 8));
    if (iVar6 < 0) {
      iVar6 = iVar6 + 0xf;
    }
    uiDrawDialogueFrameB(0x158, iVar6 >> 4);
    if ((*(int32_t*)rtguest::G((uint32_t)iVar12 + 8)) == 0x10) {
      uVar7 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x25)) + 1;
      (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x25)) = (int8_t)uVar7;
      return uVar7;
    }
    return 0x10;
  }
  iVar6 = (int32_t)param_2;
  if (cVar2 == '\x03') {
    uiSetTexa((*(int32_t*)rtguest::G((uint32_t)iVar12 + 8)) * -8 + 0x80);
    uiDrawDialogueFrameA(0x158);
    uVar10 = uiTextWidth(
         (*(uint32_t*)rtguest::G((*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x20)))));
    uiDrawDialogueFrameB(0x158, uVar10);
    if ((*(int32_t*)rtguest::G((uint32_t)iVar12 + 8)) == 0x10) {
      if ((*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x27)) == '\x04') {
        uVar7 = (*(uint32_t*)rtguest::G((uint32_t)iVar6 + 0x28)) & 0xfffffbff;
      }
      else {
        uVar7 = (*(uint32_t*)rtguest::G((uint32_t)iVar6 + 0x28)) | 0x400;
      }
      (*(uint32_t*)rtguest::G((uint32_t)iVar6 + 0x28)) = uVar7;
      if ((*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x27)) != 0xff) {
        if ((*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x27)) == '\0') {
          iVar6 = (*(int32_t*)rtguest::G((uint32_t)iVar12 + 0x18));
          uVar7 = 5;
        }
        else {
          iVar6 = (*(int32_t*)rtguest::G((uint32_t)iVar12 + 0x18));
          uVar7 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x27));
        }
        (*(uint8_t*)rtguest::G((uint32_t)iVar6 + 0x24)) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)iVar6 + 4)) = uVar7;
      }
      uVar7 = gs_tasks_kill(param_1);
      return uVar7;
    }
    return 0x10;
  }
  if (cVar2 == '\x02') {
    bVar5 = (uint8_t)((int32_t)(*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x26)) + 2);
    (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x26)) = bVar5;
    if ((uint32_t)bVar5 % 0xc == 0) {
LAB_0023b404:
      (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x25)) = 1;
      goto LAB_0023b408;
    }
    uVar8 = (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x28));
  }
  else {
    if (cVar2 == '\x04') {
      if ((*(int8_t*)rtguest::G((uint32_t)iVar12 + 0x24)) != '\0') {
        (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x25)) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x28)) =
             (*(uint32_t*)rtguest::G((uint32_t)((*(int32_t*)rtguest::G((uint32_t)iVar12 + 4)) * 4) +
              (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x20))));
        printf(0x2ec7c8);  // "next msg #%d\n"
        (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x26)) = 0;
        (*(uint16_t*)rtguest::G((uint32_t)iVar12 + 2)) = 0;
      }
    }
    else {
      if (cVar2 != '\x05') {
        if (cVar2 == '\x06') {
          if ((*(int8_t*)rtguest::G((uint32_t)iVar12 + 0x24)) == '\0') {
            return 1;
          }
          goto LAB_0023b404;
        }
        uVar8 = (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x28));
        goto LAB_0023b40c;
      }
      if ((*(int8_t*)rtguest::G((uint32_t)iVar12 + 0x24)) != '\0') {
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)iVar12 + 4));
        if (iVar4 != -1) {
          iVar6 = (*(int32_t*)rtguest::G((uint32_t)iVar12 + 0x18));
          (*(uint8_t*)rtguest::G((uint32_t)iVar6 + 0x24)) = 1;
          (*(int32_t*)rtguest::G((uint32_t)iVar6 + 4)) = iVar4;
          (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x27)) = 0xff;
          (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x25)) = 3;
          (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 8)) = 0;
          uVar7 = PlayUiSound(0x26);
          return uVar7;
        }
        (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x25)) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x28)) =
             (*(uint32_t*)rtguest::G((uint32_t)((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x27))) * 4 +
              (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x20))));
        printf(0x2ec7c8);  // "next msg #%d\n"
        (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x26)) = 0;
        (*(uint16_t*)rtguest::G((uint32_t)iVar12 + 2)) = 0;
        (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x27)) = 0;
      }
    }
LAB_0023b408:
    uVar8 = (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x28));
  }
LAB_0023b40c:
  uVar8 = InterpretScriptCommands(param_1, param_2, uVar8);
  (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x28)) = uVar8;
  uiDrawDialogueFrameA(0x158);
  lVar11 = (int32_t)RenderEscapeText(param_1, (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x28)),
                                     param_2);
  uVar10 = uiTextWidth(
       (*(uint32_t*)rtguest::G((*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x20)))));
  uiDrawDialogueFrameB(0x158, uVar10);
  uiDrawText((uint32_t)(0x23e - (int32_t)uVar10), 0xa0,
             (*(uint32_t*)rtguest::G((*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x20)))), 0);
  if ((*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x25)) != '\x01') {
    return 1;
  }
  if (lVar11 == 0) {
    if (((*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x26)) == 0) ||
        (((*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x26)) / 0xc) % 3 == 0)) {
      if ((*(uint32_t*)rtguest::G((uint32_t)iVar12 + 8) & 0x10) != 0) {
        uiDrawIcon20(0x136, 0x1a4, 7);
      }
      if ((*(uint16_t*)rtguest::G((uint32_t)iVar6 + 0x30) & 0x50) == 0) {
        return 2;
      }
      PlayUiSound(0x1a);
    }
    (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x25)) = 2;
    return 2;
  }
  puVar13 = (uint32_t)lVar11;
  switch (*(uint8_t*)rtguest::G(puVar13)) {
  case 1:
    uVar10 = 0x239260;
    break;
  case 2:
    uVar10 = 0x23bc90;
    break;
  case 3:
    if ((*(uint16_t*)rtguest::G((uint32_t)iVar6 + 0x30) & 0x50) == 0) {
      return 4;
    }
    (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 8)) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x25)) = 3;
    PlayUiSound(0x26);
    (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x27)) = 4;
    bVar5 = (*(uint8_t*)rtguest::G(puVar13 + 1));
    if (bVar5 != 0) {
      (*(uint8_t*)rtguest::G((uint32_t)iVar6 + 0x20)) = bVar5;
      return (uint32_t)bVar5;
    }
    return 0;
  case 4:
    uVar8 = 9;
    goto LAB_0023b708;
  case 5:
    uVar10 = 0x23a320;
    break;
  case 6:
    uVar8 = 8;
LAB_0023b708:
    gs_tasks_spawn(param_1, 0x239900, uVar8);
    (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x25)) = 5;
    bVar5 = (*(uint8_t*)rtguest::G(puVar13 + 1));
    (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x27)) = bVar5;
    return (uint32_t)bVar5;
  case 7:
    if ((*(uint32_t*)rtguest::G((uint32_t)iVar12 + 8) & 0x10) != 0) {
      uiDrawIcon20(0x136, 0x1a4, 7);
    }
    if ((*(uint16_t*)rtguest::G((uint32_t)iVar6 + 0x30) & 0x50) == 0) {
      return 0;
    }
    PlayUiSound(0x1a);
    uVar10 = 0x239f68;
    break;
  case 8:
    uVar10 = 0x239a80;
    break;
  case 9:
    uVar10 = 0x23bcd0;
    break;
  case 10:
    uVar7 = (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 8));
    goto LAB_0023ba9c;
  case 0xb:
    uVar10 = 0x23be88;
    break;
  case 0xc:
    if ((*(uint32_t*)rtguest::G((uint32_t)iVar12 + 8) & 0x10) != 0) {
      uiDrawIcon20(0x136, 0x1a4, 7);
    }
    if ((*(uint16_t*)rtguest::G((uint32_t)iVar6 + 0x30) & 0x50) == 0) {
      return 0;
    }
    PlayUiSound(0x1a);
    puVar9 = (uint32_t)((uint32_t)(*(uint8_t*)rtguest::G(puVar13 + 1)) * 4 +
              (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x20)));
    goto LAB_0023b9ac;
  case 0xd: {
    bool bVar1 = ((*(uint16_t*)rtguest::G((uint32_t)iVar6 + 0x30) & 0x50) != 0);
    if (bVar1) {
      (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 8)) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x25)) = 3;
      PlayUiSound(0x26);
    }
    pcVar14 = puVar13 + 1;
    if (bVar1) {
      if ((*(int8_t*)rtguest::G(pcVar14)) != '\0') {
        cVar2 = (*(int8_t*)rtguest::G(pcVar14));
        while (true) {
          pcVar14 = pcVar14 + 1;
          uVar7 = gs_stamps_award_function((uint32_t)cVar2);
          if ((*(int8_t*)rtguest::G(pcVar14)) == '\0') break;
          cVar2 = (*(int8_t*)rtguest::G(pcVar14));
        }
        return uVar7;
      }
      return 0;
    }
    return 0;
  }
  case 0xe:
    (*(uint8_t*)rtguest::G((uint32_t)0x1780a3c)) =
         (*(uint8_t*)rtguest::G((uint32_t)iVar6 + 0x23));
    gs_tasks_spawn(param_1, 0x229538, 0);
    puVar9 = (uint32_t)((uint32_t)(*(uint8_t*)rtguest::G(puVar13 + 1)) * 4 +
              (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x20)));
    goto LAB_0023b878;
  case 0xf:
    (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x27)) = 6;
    // fall through (pseudocode case-fall structure)
  case 0:
    if ((*(uint16_t*)rtguest::G((uint32_t)iVar6 + 0x30) & 0x50) != 0) {
      (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 8)) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x25)) = 3;
      uVar7 = PlayUiSound(0x26);
      return uVar7;
    }
    return 3;
  case 0x10:
    uVar10 = 0x239748;
    break;
  case 0x11:
    if ((*(uint32_t*)rtguest::G((uint32_t)iVar12 + 8) & 0x10) != 0) {
      uiDrawIcon20(0x136, 0x1a4, 7);
    }
    if ((*(uint16_t*)rtguest::G((uint32_t)iVar6 + 0x30) & 0x50) == 0) {
      return 0;
    }
    PlayUiSound(0x1a);
    gs_photos_take_function((uint32_t)(*(uint8_t*)rtguest::G(puVar13 + 1)));
    gs_tasks_spawn(param_1, 0x22bfd0,
                   (uint32_t)((*(uint8_t*)rtguest::G(puVar13 + 1)) - 1 | 0x10000));
    (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x28)) =
         (*(uint32_t*)rtguest::G((uint32_t)((uint32_t)(*(uint8_t*)rtguest::G(puVar13 + 2)) * 4) +
          (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x20))));
    printf(0x2ec7c8);  // "next msg #%d\n"
    (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x26)) = 0;
    (*(uint16_t*)rtguest::G((uint32_t)iVar12 + 2)) = 0;
    PlayUiSound(0x306);
    (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x25)) = 6;
    return 6;
  case 0x12:
    uVar10 = 0x23be18;
    break;
  case 0x13:
    if ((*(uint32_t*)rtguest::G((uint32_t)iVar12 + 8) & 0x10) != 0) {
      uiDrawIcon20(0x136, 0x1a4, 7);
    }
    if ((*(uint16_t*)rtguest::G((uint32_t)iVar6 + 0x30) & 0x50) == 0) {
      return 0;
    }
    PlayUiSound(0x1a);
    gs_tasks_spawn(param_1, 0x269e88, 0);
    puVar9 = (uint32_t)((uint32_t)(*(uint8_t*)rtguest::G(puVar13 + 1)) * 4 +
              (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x20)));
LAB_0023b878:
    (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x28)) = (*(uint32_t*)rtguest::G(puVar9));
    printf(0x2ec7c8);  // "next msg #%d\n"
    (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x26)) = 0;
    (*(uint16_t*)rtguest::G((uint32_t)iVar12 + 2)) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x25)) = 6;
    return 6;
  case 0x14:
    if ((*(uint16_t*)rtguest::G((uint32_t)iVar6 + 0x30) & 0x50) == 0) {
      return 0;
    }
    (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 8)) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x25)) = 3;
    PlayUiSound(0x26);
    ReleaseStampFlag(param_2);
    (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x27)) = 0xb;
    (*(uint8_t*)rtguest::G((uint32_t)iVar6 + 0x23)) = (*(uint8_t*)rtguest::G(puVar13 + 1));
    bVar5 = (*(uint8_t*)rtguest::G(puVar13 + 2));
    (*(uint8_t*)rtguest::G((uint32_t)iVar6 + 0x21)) = bVar5;
    return (uint32_t)bVar5;
  case 0x15:
    if ((*(uint32_t*)rtguest::G((uint32_t)iVar12 + 8) & 0x10) != 0) {
      uiDrawIcon20(0x136, 0x1a4, 7);
    }
    if ((*(uint16_t*)rtguest::G((uint32_t)iVar6 + 0x30) & 0x50) == 0) {
      return 0;
    }
    PlayUiSound(0x1a);
    SetObjectAttribute(0x177f760, (uint32_t)(*(uint8_t*)rtguest::G(puVar13 + 1)),
                       (uint32_t)(*(uint8_t*)rtguest::G(puVar13 + 2)));
    PlayUiSound(0x303);
    puVar9 = (uint32_t)((uint32_t)(*(uint8_t*)rtguest::G(puVar13 + 3)) * 4 +
              (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x20)));
LAB_0023b9ac:
    (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 0x28)) = (*(uint32_t*)rtguest::G(puVar9));
    uVar7 = printf(0x2ec7c8);  // "next msg #%d\n"
    (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x26)) = 0;
    (*(uint16_t*)rtguest::G((uint32_t)iVar12 + 2)) = 0;
    return uVar7;
  case 0x16:
    uVar10 = 0x23a140;
    break;
  case 0x17:
    uVar10 = 0x23a460;
    break;
  case 0x18:
    if ((*(uint16_t*)rtguest::G((uint32_t)iVar6 + 0x30) & 0x50) == 0) {
      return 0xf;
    }
    (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 8)) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x25)) = 3;
    PlayUiSound(0x26);
    (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x27)) = 0xf;
    return 0xf;
  default:
    uVar7 = (*(uint32_t*)rtguest::G((uint32_t)iVar12 + 8));
LAB_0023ba9c:
    if ((uVar7 & 1) == 0) {
      (*(int16_t*)rtguest::G((uint32_t)iVar12 + 2)) =
           (int16_t)((*(int16_t*)rtguest::G((uint32_t)iVar12 + 2)) + 1);
      if ((uVar7 & 3) == 0) {
        PlayUiSound(0x23);
        (*(uint32_t*)rtguest::G((uint32_t)iVar6 + 0x18)) = 2;
      }
      uVar3 = (*(uint16_t*)rtguest::G((uint32_t)iVar6 + 0x30));
    }
    else {
      uVar3 = (*(uint16_t*)rtguest::G((uint32_t)iVar6 + 0x30));
    }
    if ((uVar3 & 0x50) != 0) {
      (*(uint16_t*)rtguest::G((uint32_t)iVar12 + 2)) = 0x7fff;
      return 0x7fff;
    }
    return 0x7fff;
  }
  uVar7 = gs_tasks_spawn(param_1, uVar10, puVar13 + 1);
  (*(uint8_t*)rtguest::G((uint32_t)iVar12 + 0x25)) = 4;
  return uVar7;
}


RT_GUEST_FN(0x0023b168, guest::dialogueTask)

}  // namespace guest
