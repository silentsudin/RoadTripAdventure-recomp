// guest:0x00288c30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceFsInit(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x60) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x64) = 0x00000000u;
  }
  int iVar1;
  undefined4 uVar2;
  long lVar3;
uint32_t puVar4;
  
  RT_CALLRA(0x288c54, guest::sceSifInitRpc(0));
  (*(uint32_t*)rtguest::G(0x1779c00)) = 0;
  (*(uint32_t*)rtguest::G(0x1779c04)) = 0;
  RT_CALLRA(0x288c60, guest::DIntr());
  RT_CALLRA(0x288c7c, guest::sceSifAddCmdHandler(0xffffffff80000011,0x288720,0x1779bc0));
  RT_CALLRA(0x288c94, guest::sceSifAddCmdHandler(0xffffffff80000013,0x288c00,0x1779c00));
  RT_CALLRA(0x288c9c, guest::EIntr());
  while( true ) {
    lVar3 = RT_CALLRA(0x288ce4, guest::sceSifBindRpc(0x1779b80,0xffffffff80000001,0));
    if (lVar3 < 0) {
      return 0xffffffff;
    }
    iVar1 = 0x100000;
    if ((*(uint32_t*)rtguest::G(0x1779ba4)) != 0) break;
    do {
      iVar1 = iVar1 + -1;
    } while (iVar1 != -1);
  }
  RT_CALLRA(0x288d00, guest::fsFdSemInit());
  RT_CALLRA(0x288d08, guest::WaitSema((*(uint32_t*)rtguest::G(0x2e9990))));
  puVar4 = (uint32_t)0x1779980;
  do {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar4 + 4))) = 0;
    puVar4 = puVar4 + 0x10;
  } while (puVar4 < 0x01779b80);
  RT_CALLRA(0x288d60, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e9990))));
  (*(uint32_t*)rtguest::G(0x1778880)) = 0x01779540;
  lVar3 = RT_CALLRA(0x288d90, guest::sceSifCallRpc(0x1779b80,0xff,0,0x1778880,4,0x1779500,4,0));
  if (lVar3 < 0) {
    uVar2 = 0xfffeffff;
  }
  else {
    (*(uint32_t*)rtguest::G(0x1779ba8)) = (*(uint32_t*)rtguest::G(0x21779500));
    (*(int32_t*)rtguest::G(0x2e9988)) = 1;
    uVar2 = 0;
  }
                    /* WARNING: Read-only address (ram,0x21779500) is written */
  return uVar2;
}


RT_GUEST_FN(0x00288c30, guest::sceFsInit)

}  // namespace guest
