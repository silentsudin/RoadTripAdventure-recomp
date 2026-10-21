// guest:0x00203240 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawHintBar(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x30, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x38) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x3c) = 0x00000000u;
  }
  byte bVar1;
  int iVar2;
uint32_t puVar3;
  undefined8 uVar4;
uint32_t pbVar5;
  
  if (param_1 == 0) {
    RT_CALLRA(0x20327c, guest::uiDrawPanel9(0x1ae,0x1d,0xb4,0xc,0));
  }
  else {
    RT_CALLRA(0x2032a0, guest::uiDrawPanel9(0x1ae,0x1d,0xb4,0x18,0));
  }
  if (param_1 == 3) {
    uVar4 = 0x1b6;
    pbVar5 = (*(uint32_t*)rtguest::G((uint32_t)(0x29a8e0u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4)));
    if (((*(uint32_t*)rtguest::G(0x335688)) != 1) && ((*(uint32_t*)rtguest::G(0x335688)) = 1, (*(uint32_t*)rtguest::G(0x335684)) < 0xa00)) {
      iVar2 = (*(uint32_t*)rtguest::G(0x335684)) * 0x10;
      (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 1;
      puVar3 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + iVar2 + 0x70);
      (*(uint64_t*)rtguest::G((uint32_t)puVar3)) = 0x113780001400000;
      (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
    }
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
    while (bVar1 != 0) {
      uVar4 = RT_CALLRA(0x203358, guest::uiDrawGlyph(uVar4,0x3a,0,pbVar5));
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
      if (bVar1 == 0x2f) {
        pbVar5 = pbVar5 + 2;
      }
      else if (bVar1 == 0x60) {
        pbVar5 = pbVar5 + 2;
      }
      else if (bVar1 == 0x5e) {
        pbVar5 = pbVar5 + 2;
      }
      else if (bVar1 == 0x7e) {
        pbVar5 = pbVar5 + 2;
      }
      else if ((int)((uint32_t)bVar1 << 0x18) < 0) {
        pbVar5 = pbVar5 + 2;
      }
      else {
        pbVar5 = pbVar5 + 1;
      }
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
    }
  }
  else {
    uVar4 = 0x1b6;
    pbVar5 = (*(uint32_t*)rtguest::G((uint32_t)(0x29a8e4u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4)));
    if (((*(uint32_t*)rtguest::G(0x335688)) != 1) && ((*(uint32_t*)rtguest::G(0x335688)) = 1, (*(uint32_t*)rtguest::G(0x335684)) < 0xa00)) {
      iVar2 = (*(uint32_t*)rtguest::G(0x335684)) * 0x10;
      (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 1;
      puVar3 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + iVar2 + 0x70);
      (*(uint64_t*)rtguest::G((uint32_t)puVar3)) = 0x113780001400000;
      (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
    }
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
    while (bVar1 != 0) {
      uVar4 = RT_CALLRA(0x203460, guest::uiDrawGlyph(uVar4,0x3a,0,pbVar5));
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
      if (bVar1 == 0x2f) {
        pbVar5 = pbVar5 + 2;
      }
      else if (bVar1 == 0x60) {
        pbVar5 = pbVar5 + 2;
      }
      else if (bVar1 == 0x5e) {
        pbVar5 = pbVar5 + 2;
      }
      else if (bVar1 == 0x7e) {
        pbVar5 = pbVar5 + 2;
      }
      else if ((int)((uint32_t)bVar1 << 0x18) < 0) {
        pbVar5 = pbVar5 + 2;
      }
      else {
        pbVar5 = pbVar5 + 1;
      }
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
    }
  }
  if (param_1 < 4) {
    if (param_1 < 2) {
      if (param_1 == 1) {
        uVar4 = 0x1b6;
        pbVar5 = (*(uint32_t*)rtguest::G((uint32_t)(0x29a8e8u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4)));
        if (((*(uint32_t*)rtguest::G(0x335688)) != 1) && ((*(uint32_t*)rtguest::G(0x335688)) = param_1, (*(uint32_t*)rtguest::G(0x335684)) < 0xa00)) {
          iVar2 = (*(uint32_t*)rtguest::G(0x335684)) * 0x10;
          (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 1;
          puVar3 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + iVar2 + 0x70);
          (*(uint64_t*)rtguest::G((uint32_t)puVar3)) = 0x113780001400000;
          (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
        }
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
        while (bVar1 != 0) {
          uVar4 = RT_CALLRA(0x203590, guest::uiDrawGlyph(uVar4,0x52,0,pbVar5));
          bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
          if (bVar1 == 0x2f) {
            pbVar5 = pbVar5 + 2;
          }
          else if (bVar1 == 0x60) {
            pbVar5 = pbVar5 + 2;
          }
          else if (bVar1 == 0x5e) {
            pbVar5 = pbVar5 + 2;
          }
          else if (bVar1 == 0x7e) {
            pbVar5 = pbVar5 + 2;
          }
          else if ((int)((uint32_t)bVar1 << 0x18) < 0) {
            pbVar5 = pbVar5 + 2;
          }
          else {
            pbVar5 = pbVar5 + 1;
          }
          bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
        }
      }
    }
    else {
      uVar4 = 0x1b6;
      pbVar5 = (*(uint32_t*)rtguest::G((uint32_t)(0x29a8ecu + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4)));
      if (((*(uint32_t*)rtguest::G(0x335688)) != 1) && ((*(uint32_t*)rtguest::G(0x335688)) = 1, (*(uint32_t*)rtguest::G(0x335684)) < 0xa00)) {
        iVar2 = (*(uint32_t*)rtguest::G(0x335684)) * 0x10;
        (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 1;
        puVar3 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + iVar2 + 0x70);
        (*(uint64_t*)rtguest::G((uint32_t)puVar3)) = 0x113780001400000;
        (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
      }
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
      while (bVar1 != 0) {
        uVar4 = RT_CALLRA(0x203690, guest::uiDrawGlyph(uVar4,0x52,0,pbVar5));
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
        if (bVar1 == 0x2f) {
          pbVar5 = pbVar5 + 2;
        }
        else if (bVar1 == 0x60) {
          pbVar5 = pbVar5 + 2;
        }
        else if (bVar1 == 0x5e) {
          pbVar5 = pbVar5 + 2;
        }
        else if (bVar1 == 0x7e) {
          pbVar5 = pbVar5 + 2;
        }
        else if ((int)((uint32_t)bVar1 << 0x18) < 0) {
          pbVar5 = pbVar5 + 2;
        }
        else {
          pbVar5 = pbVar5 + 1;
        }
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
      }
    }
  }
  else if (param_1 == 4) {
    uVar4 = 0x1b6;
    pbVar5 = (*(uint32_t*)rtguest::G((uint32_t)(0x29a8f0u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4)));
    if (((*(uint32_t*)rtguest::G(0x335688)) != 1) && ((*(uint32_t*)rtguest::G(0x335688)) = 1, (*(uint32_t*)rtguest::G(0x335684)) < 0xa00)) {
      iVar2 = (*(uint32_t*)rtguest::G(0x335684)) * 0x10;
      (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 1;
      puVar3 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + iVar2 + 0x70);
      (*(uint64_t*)rtguest::G((uint32_t)puVar3)) = 0x113780001400000;
      (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
    }
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
    while (bVar1 != 0) {
      uVar4 = RT_CALLRA(0x203790, guest::uiDrawGlyph(uVar4,0x52,0,pbVar5));
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
      if (bVar1 == 0x2f) {
        pbVar5 = pbVar5 + 2;
      }
      else if (bVar1 == 0x60) {
        pbVar5 = pbVar5 + 2;
      }
      else if (bVar1 == 0x5e) {
        pbVar5 = pbVar5 + 2;
      }
      else if (bVar1 == 0x7e) {
        pbVar5 = pbVar5 + 2;
      }
      else if ((int)((uint32_t)bVar1 << 0x18) < 0) {
        pbVar5 = pbVar5 + 2;
      }
      else {
        pbVar5 = pbVar5 + 1;
      }
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
    }
  }
  return 0;
}


RT_GUEST_FN(0x00203240, guest::uiDrawHintBar)

}  // namespace guest
