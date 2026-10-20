// guest:0x002826c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
// fflush
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t fflush(uint32_t __stream, ...) {
  rtguest::FrameGuard rt_fg;
  ushort uVar1;
  uint32_t puVar2;
  int iVar3;
  long lVar4;
  uint32_t puVar5;
  uint32_t pcVar6;
  uint32_t pcVar7;

  // Ghidra FILE-type offsets: _flags=+0x00, _IO_read_end=+0x08, _IO_read_base=+0x0c,
  // _IO_write_base=+0x10, _IO_write_ptr=+0x14, _IO_buf_base=+0x1c, _IO_save_base=+0x24, _offset+4=+0x54
  puVar5 = (*(uint32_t*)rtguest::G(0x002e8cc0));
  if (__stream == (uint32_t)0x0) {
    iVar3 = _fwalk((*(uint32_t*)rtguest::G(0x002e8cc0)),0x2826c0);
    return iVar3;
  }
  puVar2 = (*(uint32_t*)rtguest::G(__stream + 0x54));
  if (puVar2 == (uint32_t)0x0) {
    (*(uint32_t*)rtguest::G(__stream + 0x54)) = (*(uint32_t*)rtguest::G(0x002e8cc0));
    iVar3 = (*(int32_t*)rtguest::G(puVar5 + 0x38));
  }
  else {
    iVar3 = (*(int32_t*)rtguest::G(puVar2 + 0x38));
    puVar5 = puVar2;
  }
  if (iVar3 == 0) {
    __sinit(puVar5);
    uVar1 = (*(uint16_t*)rtguest::G(__stream + 0xc));
  }
  else {
    uVar1 = (*(uint16_t*)rtguest::G(__stream + 0xc));
  }
  if ((uVar1 & 8) != 0) {
    pcVar7 = (*(uint32_t*)rtguest::G(__stream + 0x10));
    pcVar6 = (uint32_t)0x0;
    if (pcVar7 != (uint32_t)0x0) {
      iVar3 = (*(int32_t*)rtguest::G(__stream));
      (*(int32_t*)rtguest::G(__stream)) = (int)pcVar7;
      iVar3 = iVar3 - (int)pcVar7;
      if ((uVar1 & 3) == 0) {
        pcVar6 = (*(uint32_t*)rtguest::G(__stream + 0x14));
      }
      (*(uint32_t*)rtguest::G(__stream + 8)) = pcVar6;
      while (0 < iVar3) {
        lVar4 = (long)rtCall((*(uint32_t*)rtguest::G(__stream + 0x24)),(*(uint32_t*)rtguest::G(__stream + 0x1c)),pcVar7,iVar3);
        iVar3 = iVar3 - (int)lVar4;
        if (lVar4 < 1) {
          (*(uint16_t*)rtguest::G(__stream + 0xc)) = (*(uint16_t*)rtguest::G(__stream + 0xc)) | 0x40;
          return -1;
        }
        pcVar7 = pcVar7 + (int)lVar4;
      }
    }
  }
  return 0;
}


RT_GUEST_FN(0x002826c0, guest::fflush)

}  // namespace guest
