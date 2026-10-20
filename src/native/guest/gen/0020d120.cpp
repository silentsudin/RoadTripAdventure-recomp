// guest:0x0020d120 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// 2026-10-19 MIPS cross-check 0x0020d120: Ghidra halved chunk walk (qword +0x10 / stride size*8+8); real MIPS qword +0x20, stride size*16+0x10 (see gen/0020dde0.cpp).
uint32_t loadStep(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  char cVar1;
  ushort uVar2;
uint32_t puVar3;
  uint uVar4;
  long lVar5;
  ulong uVar6;
  undefined8 uVar7;
uint32_t puVar8;
  long lVar9;
  int iVar10;
  int iVar11;
  
  lVar9 = 0;
  iVar11 = (int)param_1;
  iVar10 = (int)param_2;
  if (((*(uint32_t*)rtguest::G(0x3356c0)) & 0x8000) != 0) {
    lVar5 = RT_CALLRA(0x20d15c, guest::cdReadPoll());
    if (lVar5 != 0) {
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x1c))) & 0x100000;
      goto LAB_0020dbd8;
    }
    (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) & 0xffff7fff;
  }
  uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x1c)));
  puVar8 = (uint32_t)(uVar4 & 0xf0000);
  if (puVar8 == (uint32_t)0x60000) {
    if ((*(uint32_t*)rtguest::G(0x3356c0)) == 1) {
      puVar3 = 0x00100000;
      cVar1 = (*(int8_t*)rtguest::G(0x100003));
      while (cVar1 != 'p') {
        if (cVar1 == '\x10') {
          uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
        }
        else {
          RT_CALLRA(0x20d370, guest::fatal(0x2ea490));
          uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
        }
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20))) =
             uVar6 & 0xffffc000ffffffff |
             ((long)(int)(((uint32_t)(uVar6 >> 0x20) & 0x3fff) + 0x237a) & 0x3fffU) << 0x20;
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
        puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x10;
      }
      goto LAB_0020db38;
    }
    if (1 < (int)(*(uint32_t*)rtguest::G(0x3356c0))) goto LAB_0020da68;
    lVar9 = 0;
    if ((*(uint32_t*)rtguest::G(0x3356c0)) == 0) {
      RT_CALLRA(0x20d8a0, guest::cdReadEntryPart(0x29b084,0x100000,(uVar4 & 0xff) * 0xc000,0xc000));
      (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
      goto switchD_0020d2bc_caseD_2;
    }
  }
  else if (puVar8 < (uint32_t)0x60001) {
    if (puVar8 == (uint32_t)0x20000) {
      if ((*(uint32_t*)rtguest::G(0x3356c0)) == 1) {
        (*(uint32_t*)rtguest::G(0x177f310)) = 0x707b80;
        RT_CALLRA(0x20d31c, guest::cdReadEntry(0x29b174,0x100000));
        (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
switchD_0020d2bc_caseD_2:
        lVar9 = 0;
      }
      else if ((int)(*(uint32_t*)rtguest::G(0x3356c0)) < 2) {
        lVar9 = 0;
        if ((*(uint32_t*)rtguest::G(0x3356c0)) == 0) {
          RT_CALLRA(0x20d404, guest::cdReadEntry(0x29b15c,0x707b80));
          (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
          goto switchD_0020d2bc_caseD_2;
        }
      }
      else {
        if ((*(uint32_t*)rtguest::G(0x3356c0)) == 2) {
          puVar3 = 0x00100000;
          cVar1 = (*(int8_t*)rtguest::G(0x100003));
          while (cVar1 != 'p') {
            if (cVar1 == '\x10') {
              uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
            }
            else {
              RT_CALLRA(0x20d458, guest::fatal(0x2ea490));
              uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
            }
            (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20))) =
                 uVar6 & 0xffffc000ffffffff |
                 ((long)(int)(((uint32_t)(uVar6 >> 0x20) & 0x3fff) + 0x1d8e) & 0x3fffU) << 0x20;
            cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
            puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x10;
          }
          goto LAB_0020db38;
        }
        lVar9 = 0;
        if ((*(uint32_t*)rtguest::G(0x3356c0)) == 3) {
LAB_0020db60:
          (*(uint32_t*)rtguest::G(0x3356c0)) = 0xffffffff;
          goto switchD_0020d2bc_caseD_2;
        }
      }
    }
    else if (puVar8 < (uint32_t)0x20001) {
      if (puVar8 == (uint32_t)0x0) {
        switch(uVar4 & 0xf) {
        case 0:
          lVar9 = RT_CALLRA(0x20d2cc, guest::loadCarPartsSet(param_1,param_2));
          break;
        case 1:
          lVar9 = RT_CALLRA(0x20d2e4, guest::loadPoseSet(param_1,param_2));
          break;
        case 2:
          goto switchD_0020d2bc_caseD_2;
        case 3:
          if ((*(uint32_t*)rtguest::G(0x3356c0)) == 1) {
            puVar3 = 0x00100000;
            cVar1 = (*(int8_t*)rtguest::G(0x100003));
            while (cVar1 != 'p') {
              if (cVar1 == '\x10') {
                uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
              }
              else {
                RT_CALLRA(0x20d580, guest::fatal(0x2ea490));
                uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
              }
              (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20))) =
                   uVar6 & 0xffffc000ffffffff |
                   ((long)(int)(((uint32_t)(uVar6 >> 0x20) & 0x3fff) + 0x237a) & 0x3fffU) << 0x20;
              cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
              puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x10;
            }
LAB_0020db38:
            uVar7 = RT_CALLRA(0x20db38, guest::sceDmaGetChan(1));
            RT_CALLRA(0x20db48, guest::sceDmaSync(uVar7,0,0));
            RT_CALLRA(0x20db50, guest::sendTexturePackets(0x100000));
            lVar9 = 0;
          }
          else if ((int)(*(uint32_t*)rtguest::G(0x3356c0)) < 2) {
            lVar9 = 0;
            if ((*(uint32_t*)rtguest::G(0x3356c0)) == 0) {
              RT_CALLRA(0x20d4fc, guest::cdReadEntry(0x29b410,0x100000));
              (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
              goto switchD_0020d2bc_caseD_2;
            }
          }
          else {
LAB_0020da68:
            lVar9 = 0;
            if ((*(uint32_t*)rtguest::G(0x3356c0)) == 2) goto LAB_0020db60;
          }
          break;
        case 4:
        case 5:
          lVar9 = RT_CALLRA(0x20d3cc, guest::loadShopSet(param_1,param_2));
          break;
        case 6:
          if ((*(uint32_t*)rtguest::G(0x3356c0)) == 1) {
            puVar3 = 0x00100000;
            cVar1 = (*(int8_t*)rtguest::G(0x100003));
            while (cVar1 != 'p') {
              if (cVar1 == '\x10') {
                uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
              }
              else {
                RT_CALLRA(0x20d688, guest::fatal(0x2ea490));
                uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
              }
              (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20))) =
                   uVar6 & 0xffffc000ffffffff |
                   ((long)(int)(((uint32_t)(uVar6 >> 0x20) & 0x3fff) + 0x285e) & 0x3fffU) << 0x20;
              cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
              puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x10;
            }
            goto LAB_0020db38;
          }
          if (1 < (int)(*(uint32_t*)rtguest::G(0x3356c0))) goto LAB_0020da68;
          lVar9 = 0;
          if ((*(uint32_t*)rtguest::G(0x3356c0)) == 0) {
            RT_CALLRA(0x20d528, guest::cdReadEntry(0x29b054,0x100000));
            (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
            goto switchD_0020d2bc_caseD_2;
          }
        }
      }
      else if (puVar8 == (uint32_t)0x10000) {
        if ((*(uint32_t*)rtguest::G(0x3356c0)) == 1) {
          puVar3 = 0x00100000;
          cVar1 = (*(int8_t*)rtguest::G(0x100003));
          while (cVar1 != 'p') {
            if (cVar1 == '\x10') {
              uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
            }
            else {
              RT_CALLRA(0x20d800, guest::fatal(0x2ea490));
              uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
            }
            (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20))) =
                 uVar6 & 0xffffc000ffffffff |
                 ((long)(int)(((uint32_t)(uVar6 >> 0x20) & 0x3fff) + 0x1d8e) & 0x3fffU) << 0x20;
            cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
            puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x10;
          }
          goto LAB_0020db38;
        }
        if (1 < (int)(*(uint32_t*)rtguest::G(0x3356c0))) goto LAB_0020da68;
        lVar9 = 0;
        if ((*(uint32_t*)rtguest::G(0x3356c0)) == 0) {
          RT_CALLRA(0x20d5f8, guest::cdReadEntry(0x29afa0,0x100000));
          (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
          goto switchD_0020d2bc_caseD_2;
        }
      }
    }
    else if (puVar8 == (uint32_t)0x40000) {
      if ((*(uint32_t*)rtguest::G(0x3356c0)) == 1) {
        puVar3 = 0x00100000;
        cVar1 = (*(int8_t*)rtguest::G(0x100003));
        while (cVar1 != 'p') {
          if (cVar1 == '\x10') {
            uVar2 = (*(uint16_t*)rtguest::G((uint32_t)puVar3));
          }
          else {
            RT_CALLRA(0x20d8f8, guest::fatal(0x2ea490));
            uVar2 = (*(uint16_t*)rtguest::G((uint32_t)puVar3));
          }
          (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
          cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)uVar2 * 0x10 + 0x13)));
          puVar3 = puVar3 + (uint32_t)uVar2 * 0x10 + 0x10;
        }
        goto LAB_0020db38;
      }
      if (1 < (int)(*(uint32_t*)rtguest::G(0x3356c0))) goto LAB_0020da68;
      lVar9 = 0;
      if ((*(uint32_t*)rtguest::G(0x3356c0)) == 0) {
        RT_CALLRA(0x20d630, guest::cdReadEntry(0x29af58 + (uVar4 & 0xff) * 0xc,0x100000)  /* W13: fn/0020d120.c:239 `&sysFileTable + idx*3` on undefined4[] = 12-byte entries (symbols: sysFileTable 12-byte {..lsn,sectors}) */);
        (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
        goto switchD_0020d2bc_caseD_2;
      }
    }
    else if (puVar8 < (uint32_t)0x40001) {
      if (puVar8 == (uint32_t)0x30000) {
        lVar9 = RT_CALLRA(0x20d858, guest::DispatchInputMapping(param_1,param_2));
      }
    }
    else if (puVar8 == (uint32_t)0x50000) {
      lVar9 = RT_CALLRA(0x20d75c, guest::loadCourseTile(param_1,param_2,0x29be50 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x22))) * 0xc,
                             (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x22))),2));
    }
  }
  else if (puVar8 == 0x000a0000) {
    if ((*(uint32_t*)rtguest::G(0x3356c0)) == 1) {
      puVar3 = 0x00100000;
      cVar1 = (*(int8_t*)rtguest::G(0x100003));
      while (cVar1 != 'p') {
        if (cVar1 == '\x10') {
          uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
        }
        else {
          RT_CALLRA(0x20d9f8, guest::fatal(0x2ea490));
          uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
        }
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20))) =
             uVar6 & 0xffffc000ffffffff |
             ((long)(int)(((uint32_t)(uVar6 >> 0x20) & 0x3fff) + 0x237a) & 0x3fffU) << 0x20;
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
        puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x10;
      }
      goto LAB_0020db38;
    }
    if (1 < (int)(*(uint32_t*)rtguest::G(0x3356c0))) goto LAB_0020da68;
    lVar9 = 0;
    if ((*(uint32_t*)rtguest::G(0x3356c0)) == 0) {
      RT_CALLRA(0x20d7a8, guest::cdReadEntry((uVar4 & 0xff) * 0xc + 0x29b398,0x100000));
      (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
      goto switchD_0020d2bc_caseD_2;
    }
  }
  else if (0x000a0000 < puVar8) {
    if (puVar8 == (uint32_t)0xc0000) {
      lVar9 = RT_CALLRA(0x20d954, guest::loadCarModelAt(param_1,param_2));
    }
    else if (puVar8 < (uint32_t)0xc0001) {
      if (puVar8 == (uint32_t)0xb0000) {
        lVar9 = RT_CALLRA(0x20d6f0, guest::CheckInputMapping(param_1,param_2));
      }
    }
    else if (puVar8 == (uint32_t)0xd0000) {
      if ((*(uint32_t*)rtguest::G(0x3356c0)) == 0) {
        RT_CALLRA(0x20d9a0, guest::cdReadEntry(0x29b168,0x100000));
        (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
        goto switchD_0020d2bc_caseD_2;
      }
      lVar9 = 0;
      if ((*(uint32_t*)rtguest::G(0x3356c0)) == 1) goto LAB_0020db60;
    }
  }
  else if (puVar8 == (uint32_t)0x80000) {
    if ((*(uint32_t*)rtguest::G(0x3356c0)) == 1) {
      puVar3 = 0x00100000;
      cVar1 = (*(int8_t*)rtguest::G(0x100003));
      while (cVar1 != 'p') {
        if (cVar1 == '\x10') {
          uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
        }
        else {
          RT_CALLRA(0x20dae8, guest::fatal(0x2ea490));
          uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
        }
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20))) =
             uVar6 & 0xffffc000ffffffff |
             ((long)(int)(((uint32_t)(uVar6 >> 0x20) & 0x3fff) + 0x2c7e) & 0x3fffU) << 0x20;
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
        puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x10;
      }
      goto LAB_0020db38;
    }
    if (1 < (int)(*(uint32_t*)rtguest::G(0x3356c0))) goto LAB_0020da68;
    lVar9 = 0;
    if ((*(uint32_t*)rtguest::G(0x3356c0)) == 0) {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x22))) == -1) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x68))) = 0xffffffff;
      }
      RT_CALLRA(0x20da90, guest::cdReadEntry(0x29b150,0x100000));
      (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
      goto switchD_0020d2bc_caseD_2;
    }
  }
  else if (puVar8 < (uint32_t)0x80001) {
    if (puVar8 == (uint32_t)0x70000) {
      lVar9 = RT_CALLRA(0x20d6e0, guest::UpdateInputMapping(param_1,param_2));
    }
  }
  else if (puVar8 == (uint32_t)0x90000) {
    lVar9 = RT_CALLRA(0x20d724, guest::loadFieldTile(param_1,param_2,0x29bb50 + (uVar4 & 0xff) * 0xc,uVar4 & 0xff,
                          uVar4 & 3));
  }
  if (lVar9 == 0) {
    if ((*(uint32_t*)rtguest::G(0x3356c0)) == 0xffffffff) {
      if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x1c))) & 0x400000) != 0) {
        RT_CALLRA(0x20dba8, guest::ClearTaskPoolFlag());
      }
      (*(uint32_t*)rtguest::G(0x333370)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0xc))) = 0x20df98;
      return 0;
    }
    (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) + 1;
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x1c))) & 0x100000;
  }
  else {
    (*(uint32_t*)rtguest::G(0x3356c0)) = 0;
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x1c))) & 0x100000;
  }
LAB_0020dbd8:
  if (uVar4 == 0) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 8))) < 0x40) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) | 0x400000;
      RT_CALLRA(0x20dc00, guest::uiFade(0x6000000));
    }
    RT_CALLRA(0x20dc18, guest::uiDrawTextCell(0xc,10,0x2ea6b0,0));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x50))) = 0x80000000;
  }
  return 0;
}


RT_GUEST_FN(0x0020d120, guest::loadStep)

}  // namespace guest
