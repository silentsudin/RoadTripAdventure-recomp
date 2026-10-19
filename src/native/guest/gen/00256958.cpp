// guest:0x00256958 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t OpenPaintEditorScene(uint32_t param_1, uint32_t param_2, ...) {
  uint uVar1;
  int iVar2;
  int iVar3;
  bool bVar4;
  int iVar5;
  
  iVar5 = (int)param_1;
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c)));
  iVar2 = iVar3 * 0x1c;
  bVar4 = true;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) == 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x177f760 + iVar2 + 3))) =
         (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x177f760 + iVar2 + 5)));
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x177f760 + iVar2)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20))) = uVar1;
    if ((uVar1 & 0xf) == (uVar1 >> 0xc & 0xf)) {
      if ((uVar1 >> 4 & 0xf) == (uVar1 >> 0x10 & 0xf)) {
        bVar4 = ((uVar1 >> 0x14 ^ uVar1 >> 8) & 0xf) != 0;
        goto LAB_002569e8;
      }
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x27))) = 1;
    }
    else {
LAB_002569e8:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x27))) = bVar4;
    }
    BuildPaintEditorMenuSprites(0x175b570);
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c)));
  }
  if (iVar3 == 0) {
    if ((uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18))) + 0x27))) - 0xcU) < 2) goto LAB_00256a3c;
  }
  else if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18))) + 0x27))) == '\x04') {
LAB_00256a3c:
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) = 0xffc8;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xc))) = (*(uint8_t*)rtguest::G(0x256a78));
    SlideInPaintEditorWindow(param_1,param_2);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00256958, guest::OpenPaintEditorScene)

}  // namespace guest
