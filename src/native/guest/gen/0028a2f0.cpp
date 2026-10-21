// guest:0x0028a2f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceFormat(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(208);
rtguest::allocFrame(16);  // guest frame gap (sp+0x0..0xf)
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0xa0, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0xb0, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x70, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xc0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc4) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x90, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x80, 8);
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(1, (uint32_t)param_4);  // walk: = param_4
    rtguest::setS32(2, (uint32_t)param_2);  // walk: = param_2
    rtguest::gS[3] = 0x00000000017788c0ull;  // walk const
    rtguest::setS32(6, (uint32_t)param_3);  // walk: = param_3
    rtguest::gS[7] = 0x0000000001780000ull;  // walk const
  }
uint32_t auStack_c0 = rtguest::allocFrame(4);  // sp+0x10 (Ghidra entry-sp -0xc0, frame 0xd0)
    uint32_t uStack_bc = rtguest::allocFrame(4);  // sp+0x14 (Ghidra entry-sp -0xbc, frame 0xd0)
    uint32_t uStack_b8 = rtguest::allocFrame(4);  // sp+0x18 (Ghidra entry-sp -0xb8, frame 0xd0)
    rtguest::allocFrame(8);  // guest frame gap (sp+0x1c..0x23)
    uint32_t uStack_ac = rtguest::allocFrame(4);  // sp+0x24 (Ghidra entry-sp -0xac, frame 0xd0)
rtguest::allocFrame(8);  // guest frame gap (sp+0x28..0x2f)
uint32_t auStack_a0 = rtguest::allocFrame(16);  // sp+0x30 (Ghidra entry-sp -0xa0, frame 0xd0)
  char cVar1;
uint32_t puVar2;
  undefined8 uVar3;
  long lVar4;
uint32_t puVar5;
  int iVar6;
  
  RT_CALLRA(0x28a330, guest::_sceFsWaitS(0xe));
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    RT_CALLRA(0x28a348, guest::sceFsInit());
    (*(uint8_t*)rtguest::G((uint32_t)0x017788cc + 0)) = (*(int8_t*)rtguest::G((uint32_t)param_1));
  }
  else {
    (*(uint8_t*)rtguest::G((uint32_t)0x017788cc + 0)) = (*(int8_t*)rtguest::G((uint32_t)param_1));
  }
  iVar6 = 0;
  if ((char)(*(uint32_t*)rtguest::G(0x17788cc)) != '\0') {
    for (iVar6 = 1;
        (iVar6 < 0x400 &&
        (cVar1 = (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(iVar6) * sizeof(int8_t))), (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x017788cc + iVar6))) = cVar1, cVar1 != '\0'));
        iVar6 = iVar6 + 1) {
    }
  }
  if (iVar6 == 0x400) {
    (*(uint32_t*)rtguest::G(0x1778ccb)) = 0;
  }
  if (param_2 == 0) {
    (*(uint32_t*)rtguest::G(0x1778ccc)) = '\0';
  }
  else {
    (*(uint32_t*)rtguest::G(0x1778ccc)) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)param_2));
    iVar6 = 0;
    if ((*(uint32_t*)rtguest::G(0x1778ccc)) != '\0') {
      for (iVar6 = 1;
          (iVar6 < 0x400 &&
          (cVar1 = (*(int8_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(iVar6) * 1)), (*(uint32_t*)rtguest::G((uint32_t)0x1778ccc + (uint32_t)(iVar6) * 4)) = cVar1, cVar1 != '\0'));
          iVar6 = iVar6 + 1) {
      }
    }
    if (iVar6 == 0x400) {
      (*(uint32_t*)rtguest::G(0x17790cb)) = 0;
    }
  }
  if (param_4 < 0x401) {
    iVar6 = 0;
    if (0 < param_4) {
      do {
        puVar2 = (uint32_t)(param_3 + iVar6);
        puVar5 = (uint32_t)((int)0x017790cc + iVar6);
        iVar6 = iVar6 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)puVar5)) = (*(uint8_t*)rtguest::G((uint32_t)puVar2));
      } while (iVar6 < param_4);
    }
    (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0;
    (*(uint32_t*)rtguest::G(0x17794cc)) = param_4;
    uVar3 = RT_CALLRA(0x28a488, guest::CreateSema(auStack_c0));
    (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
    (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar3;
    (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_a0;
    sceSifWriteBackDCache(0x17788c0,0xc10);
    lVar4 = RT_CALLRA(0x28a4d0, guest::sceSifCallRpc(0x1779b80,0xe,0,0x17788c0,0xc10,0x1779500,4,0));
    iVar6 = (*(uint32_t*)rtguest::G(0x21779500));
    if (lVar4 < 0) {
      RT_CALLRA(0x28a4e0, guest::DeleteSema(uVar3));
      RT_CALLRA(0x28a410, guest::_sceFsSigSema());
      (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4)) = 0xfffffff5;
    }
    else {
      RT_CALLRA(0x28a4e8, guest::_sceFsSigSema());
      if (iVar6 == 0) {
        RT_CALLRA(0x28a50c, guest::DeleteSema(uVar3));
        (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4)) = 0xfffffff5;
      }
      else {
        RT_CALLRA(0x28a51c, guest::WaitSema(uVar3));
        RT_CALLRA(0x28a524, guest::DeleteSema(uVar3));
      }
    }
  }
  else {
    RT_CALLRA(0x28a4fc, guest::_sceFsSigSema());
    (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4)) = 0xfffffff9;
  }
  return (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4));;
}


RT_GUEST_FN(0x0028a2f0, guest::sceFormat)

}  // namespace guest
